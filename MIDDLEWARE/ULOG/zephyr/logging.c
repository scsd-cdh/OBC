#include "logging.h"

#include <ulog/ulog.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>

ULOG_SYSTEM_ID(CONFIG_ULOG_SYSTEM_ID);

// Protects the device handle
static K_MUTEX_DEFINE(logging_uart_dev_mutex);
// Needed because a mutex doesn't protect against reentrancy
static bool logging_locked = false;
// Device to output to
static const struct device *p_logging_uart_dev;

int ulog_logging_init(void)
{
    p_logging_uart_dev = DEVICE_DT_GET(DT_CHOSEN(ulog_usart));

    return 0;
}

bool ulog_external_acquire(void)
{
    // Lock the mutex (we can lock it multiple times in a single thread, careful!)
    const int ret = k_mutex_lock(&logging_uart_dev_mutex, K_NO_WAIT);
    // Couldnt lock the mutex
    if (ret != 0) {
        return false;
    }
    // Logging is locked by the current thread
    if (logging_locked) {
        return false;
    }

    // Set the reentrancy lock
    logging_locked = true;

    return true;
}
void ulog_external_release(void)
{
    // Unlock both locks
    logging_locked = false;
    k_mutex_unlock(&logging_uart_dev_mutex);
}
void ulog_external_write(const uint8_t byte)
{
    uart_poll_out(p_logging_uart_dev, byte);
}
void ulog_external_flush(void)
{
}

void logging_force_reset(void)
{
    // There's nothing to reset
    if (!logging_uart_dev_mutex.lock_count || !logging_locked) {
        return;
    }

    // Reset the flag and destroy the mutex
    logging_locked = false;
    k_mutex_init(&logging_uart_dev_mutex);

    // Force ulog to flush. This may corrupt a frame, and is unsupported.
    ulog__flush();
}

SYS_INIT(ulog_logging_init, APPLICATION, 30);