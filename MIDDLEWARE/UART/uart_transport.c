#include "uart/uart_transport.h"

#include <errno.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/irq.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/ring_buffer.h>

LOG_MODULE_REGISTER(uart_transport, LOG_LEVEL_DBG);

#define UART_TRANSPORT_RX_BUFFER_SIZE 1024
#define UART_TRANSPORT_DEBUG_HISTORY_SIZE 128

RING_BUF_DECLARE(uart_rx_ring, UART_TRANSPORT_RX_BUFFER_SIZE);
K_MUTEX_DEFINE(uart_tx_lock);
static struct k_sem uart_rx_available;
static const struct device *uart_rx_dev;
static uint8_t uart_debug_history[UART_TRANSPORT_DEBUG_HISTORY_SIZE];
static size_t uart_debug_history_next;
static size_t uart_debug_history_count;
static uint32_t uart_debug_dropped;

static void uart_transport_irq_handler(const struct device *dev, void *user_data)
{
    ARG_UNUSED(user_data);

    if (!uart_irq_update(dev)) {
        return;
    }

    while (uart_irq_rx_ready(dev)) {
        uint8_t fifo_buffer[32];
        int received = uart_fifo_read(dev, fifo_buffer, sizeof(fifo_buffer));
        if (received <= 0) {
            break;
        }

        // FIXME: This is a (potentially) temporary debug measure to help see what is being received over UART. It may be removed in the future or wrapped in a debug flag.
        for (int i = 0; i < received; ++i) {
            uart_debug_history[uart_debug_history_next] = fifo_buffer[i];
            uart_debug_history_next =
                (uart_debug_history_next + 1) % UART_TRANSPORT_DEBUG_HISTORY_SIZE;
            if (uart_debug_history_count < UART_TRANSPORT_DEBUG_HISTORY_SIZE) {
                uart_debug_history_count++;
            }
        }

        uint32_t written = ring_buf_put(&uart_rx_ring, fifo_buffer, received);
        // Increment the semaphore for each byte written to the ring buffer.
        for (uint32_t i = 0; i < written; ++i) {
            k_sem_give(&uart_rx_available);
        }

        if (written < (uint32_t)received) {
            uart_debug_dropped += received - written;
        }
    }
}

size_t uart_transport_debug_snapshot(uint8_t *buffer, size_t buffer_size)
{
    if (buffer == NULL || buffer_size == 0) {
        return 0;
    }

    unsigned int key = irq_lock();
    const size_t count = MIN(uart_debug_history_count, buffer_size);
    const size_t start =
        (uart_debug_history_next + UART_TRANSPORT_DEBUG_HISTORY_SIZE
         - count) % UART_TRANSPORT_DEBUG_HISTORY_SIZE;

    for (size_t i = 0; i < count; ++i) {
        buffer[i] = uart_debug_history[(start + i) % UART_TRANSPORT_DEBUG_HISTORY_SIZE];
    }
    irq_unlock(key);
    return count;
}

uint32_t uart_transport_debug_dropped(void)
{
    unsigned int key = irq_lock();
    const uint32_t dropped = uart_debug_dropped;
    irq_unlock(key);
    return dropped;
}

int uart_transport_open(const struct device *rx_dev)
{
    if (rx_dev == NULL || !device_is_ready(rx_dev)) {
        LOG_ERR("UART RX device is not ready");
        return -ENODEV;
    }

    if (uart_rx_dev != NULL) {
        LOG_ERR("UART RX port already open on %s", uart_rx_dev->name);
        return -EALREADY;
    }

    k_sem_init(&uart_rx_available, 0, UART_TRANSPORT_RX_BUFFER_SIZE);

    int ret = uart_irq_callback_user_data_set(
        rx_dev, uart_transport_irq_handler, NULL);
    if (ret != 0) {
        LOG_ERR("Failed to install UART RX callback on %s: %d",
            rx_dev->name, ret);
        return ret;
    }

    uart_rx_dev = rx_dev;
    uart_irq_rx_enable(rx_dev);
    return 0;
}

int uart_transport_read(
    uint8_t *rxdata,
    size_t rxlen,
    k_timeout_t timeout
)
{
    if (uart_rx_dev == NULL) {
        return -ENODEV;
    }

    if (rxdata == NULL || rxlen == 0) {
        return -EINVAL;
    }

    // Wait for data to be available or timeout. Non blocking, puts thread to sleep. usecase for semaphore
    if (k_sem_take(&uart_rx_available, timeout) != 0) {
        return 0;
    }

    size_t read = ring_buf_get(&uart_rx_ring, rxdata, rxlen);
    // decrement the semaphore for each byte read from the ring buffer.
    for (size_t i = 1; i < read; ++i) {
        (void)k_sem_take(&uart_rx_available, K_NO_WAIT);
    }

    return (int)read;
}

int uart_transport_write(
    const struct device *tx_dev,
    const uint8_t *txdata,
    size_t txlen
)
{
    if (tx_dev == NULL || !device_is_ready(tx_dev)) {
        LOG_ERR("UART TX device is not ready");
        return -ENODEV;
    }

    if (txdata == NULL && txlen != 0) {
        return -EINVAL;
    }

    int ret = k_mutex_lock(&uart_tx_lock, K_FOREVER);
    if (ret != 0) {
        return ret;
    }

    for (size_t i = 0; i < txlen; ++i) {
        uart_poll_out(tx_dev, txdata[i]);
    }

    k_mutex_unlock(&uart_tx_lock);
    return (int)txlen;
}
