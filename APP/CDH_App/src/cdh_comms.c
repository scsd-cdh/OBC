#include "cdh_comms.h"
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/ring_buffer.h>

#include "lfp/stream.h"

LOG_MODULE_REGISTER(cdh_comms);

#define NOBAD_BYTES(byte) ((byte) >= 0x20 && (byte) <= 0x7e) ? (byte) : '.'

#define UART2_TX 0 // used for loopback transmitter
#define UART2_RX 1 // used for loopback receiver
#define DUMMY_ENDPOINT 2
#define CDH_COMMS_UART_RX_BUFFER_SIZE 512

RING_BUF_DECLARE(uart_rx_ring, CDH_COMMS_UART_RX_BUFFER_SIZE);
static struct k_sem uart_rx_available;
static const struct device *uart_rx_dev;

static void cdh_comms_uart_irq_handler(const struct device *dev, void *user_data)
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

        // when we get an interrupt we update the semaphore once the ring buffer is filled with the number of bytes
        uint32_t written = ring_buf_put(&uart_rx_ring, fifo_buffer, received);
        // use counting semaphore to tolerate partial reads
        for (uint32_t i = 0; i < written; ++i) {
            k_sem_give(&uart_rx_available);
        }

        if (written < (uint32_t)received) {
            LOG_WRN("UART RX buffer full; dropped %d bytes", received - written);
        }
    }
}

void cdh_comms_local_loopback_test(const struct device *lp_uart)
{
    cdh_comms_d2d_loopback_test(lp_uart, lp_uart);
}

void cdh_comms_d2d_loopback_test(const struct device *lp_txuart, const struct device* lp_rxuart)
{
    static const uint8_t test_data[] = "D2D loopback\r\n";
    static bool ready_logged;

    if (!device_is_ready(lp_txuart)) {
        LOG_ERR("D2D loopback TX device %s not ready", lp_txuart->name);
        return;
    }

    if (!device_is_ready(lp_rxuart)) {
        LOG_ERR("D2D loopback RX device %s not ready", lp_rxuart->name);
        return;
    }

    if (!ready_logged) {
        LOG_INF("D2D loopback is ready");
        ready_logged = true;
    }
    unsigned char byte;
    while (uart_poll_in(lp_rxuart, &byte) == 0) {}

    for (size_t i = 0; i < sizeof(test_data) - 1; i++) {
        uart_poll_out(lp_txuart, test_data[i]);

        int got = -1;
        for (int retry = 0; retry < 1000; retry++)
        {
            if (uart_poll_in(lp_rxuart, &byte) == 0) {
                got = byte;
                break;
            }
            k_busy_wait(100);
        }
        if (got != test_data[i]) {
            LOG_INF("%s -> %s: D2D loopback, Got the wrong data: %c, expected: %c", lp_txuart->name, lp_rxuart->name, NOBAD_BYTES(got), NOBAD_BYTES(test_data[i]));
            continue;
        }
        LOG_INF("%s -> %s: D2D loopback received: 0x%02x ('%c')",
            lp_txuart->name, lp_rxuart->name, got, NOBAD_BYTES(got));
    }
}

static bool on_header(const lfp_header_t * p_header, void * p_ctx)
{
    ARG_UNUSED(p_ctx);
    LOG_INF("{ length: %d, initiater: %d, target: %d, endpoint: %d }",
        p_header->encoded_length,
        p_header->initiator,
        p_header->target,
        p_header->endpoint);
    return true;
}

static bool on_message(const lfp_header_t * p_header,
    const uint8_t * p_body,
    uint16_t body_length,
    void * p_ctx
)
{
    ARG_UNUSED(body_length);
    ARG_UNUSED(p_ctx);
    LOG_INF("{ length: %d, initiater: %d, target: %d, endpoint: %d } { body: %s }",
        p_header->encoded_length,
        p_header->initiator,
        p_header->target,
        p_header->endpoint,
        p_body);
    return true;
}


void cdh_comms_lfp_uart_lp(
    const struct device *lp_txuart,
    const struct device *lp_rxuart,
    const uint8_t *txdata,
    size_t txlen,
    uint8_t *rxdata,
    size_t rxlen
)
{
    if (!device_is_ready(lp_txuart)) {
        LOG_ERR("LFP UART device %s not ready", lp_txuart->name);
        return;
    }

    if (!device_is_ready(lp_rxuart)) {
        LOG_ERR("LFP UART device %s not ready", lp_rxuart->name);
        return;
    }

    static uint8_t lfp_encoded_tx_buffer[256]; // Extra space for LFP overhead
    int encode_result = lfp_encode(UART2_TX,
        UART2_RX,
        DUMMY_ENDPOINT,
        LFP_DIRECTION_TARGET,
        txdata,
        txlen,
        lfp_encoded_tx_buffer,
        sizeof(lfp_encoded_tx_buffer));
    if (encode_result < 0) {
        LOG_ERR("Failed to encode LFP message: %d", encode_result);
        if (encode_result == LFP_EOVERFLOW) {
            LOG_ERR("LFP message is too large to fit in the buffer %d > %d", txlen, sizeof(lfp_encoded_tx_buffer));
        }
        return;
    }

    lfp_stream_ctx_t input_stream;
    lfp_stream_init(&input_stream, rxdata, rxlen, on_header, on_message, NULL, NULL);
    for (size_t i = 0; i < encode_result; i++) {
        uart_poll_out(lp_txuart, lfp_encoded_tx_buffer[i]);
        uint8_t byte;
        int got = -1;
        for (int retry = 0; retry < 1000; retry++)
        {
            if (uart_poll_in(lp_rxuart, &byte) == 0) {
                got = byte;
                break;
            }
            k_busy_wait(100);

        }
        if (got != lfp_encoded_tx_buffer[i]) {
            LOG_INF("LFP UART loopback, Got the wrong data: %c, expected: %c", NOBAD_BYTES(got), NOBAD_BYTES(lfp_encoded_tx_buffer[i]));
            return;
        }
        lfp_stream_update(&input_stream, got);
    }
}

int cdh_comms_encode_lfp_msg(
    const asn1_lfp_decode_data_t *data,
    uint8_t *send_buf,
    size_t send_buf_size
)
{
    return lfp_encode(
        data->p_header->initiator,
        data->p_header->target,
        data->p_header->endpoint,
        data->p_header->direction,
        data->p_body,
        data->body_length,
        send_buf,
        send_buf_size
    );
}

void cdh_comms_send_msg(
    const struct device *tx_dev,
    const uint8_t *txdata,
    size_t txlen
)
{
    if (!device_is_ready(tx_dev)) {
        LOG_ERR("LFP UART device %s not ready", tx_dev->name);
        return;
    }

    for (size_t i = 0; i < txlen; i++) {
        uart_poll_out(tx_dev, txdata[i]);
    }
}

int cdh_comms_uart_open(const struct device *rx_dev)
{
    if (!device_is_ready(rx_dev)) {
        LOG_ERR("UART RX device %s not ready", rx_dev->name);
        return -ENODEV;
    }

    if (uart_rx_dev != NULL) {
        LOG_ERR("UART RX port already open on %s", uart_rx_dev->name);
        return -EALREADY;
    }

    k_sem_init(&uart_rx_available, 0, CDH_COMMS_UART_RX_BUFFER_SIZE);

    int ret = uart_irq_callback_user_data_set(rx_dev, cdh_comms_uart_irq_handler, NULL);
    if (ret != 0) {
        LOG_ERR("Failed to install UART RX callback on %s: %d", rx_dev->name, ret);
        return ret;
    }

    uart_rx_dev = rx_dev;
    uart_irq_rx_enable(rx_dev);
    return 0;
}


// Non blocking interrupt driven UART read
int cdh_comms_uart_read(
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

    // Allow main thread to sleep until we get an interrupt or timeout
    if (k_sem_take(&uart_rx_available, timeout) != 0) {
        return 0;
    }

    size_t read = ring_buf_get(&uart_rx_ring, rxdata, rxlen);
    // Drain the semaphore for any additional bytes that were received in the ring buffer
    // If for whateve reason we don't drain the full amount of bytes in the ring buffer, we will just block on the next read until we get an interrupt
    for (size_t i = 1; i < read; ++i) {
        (void)k_sem_take(&uart_rx_available, K_NO_WAIT);
    }

    return (int)read;
}
