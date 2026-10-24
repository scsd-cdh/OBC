#include "cdh_comms.h"
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(cdh_comms);


#define NOBAD_BYTES(byte) ((byte) >= 0x20 && (byte) <= 0x7e) ? (byte) : '.'

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

