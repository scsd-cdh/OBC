#include "main.h"

#include "cdh_bms.h"
#include "cdh_comms.h"
#include "zephyr/kernel.h"
#include "zephyr/logging/log.h"

LOG_MODULE_REGISTER(cdh);

int main(void)
{
    static uint8_t uart_rx_buffer[256];
    int ret = cdh_comms_uart_open(transmitter);
    if (ret != 0) {
        LOG_ERR("Failed to open UART2 RX: %d", ret);
        return ret;
    }

    LOG_INF("Starting BMS I2C to UART2 loopback test");

    while (1) {
        get_bms_power_status();

        ret = cdh_comms_uart_read(
            uart_rx_buffer,
            sizeof(uart_rx_buffer),
            K_MSEC(250)
        );
        if (ret > 0) {
            LOG_HEXDUMP_INF(uart_rx_buffer, ret, "UART2 loopback RX:");
        }

        k_msleep(1000);
    }
}
