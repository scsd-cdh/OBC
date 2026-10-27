#include "main.h"
#include "cdh_comms.h"
#include "zephyr/kernel.h"
#include "zephyr/logging/log.h"

LOG_MODULE_REGISTER(cdh);


int main(void)
{
    static const uint8_t test_data[] = "LFP UART loopback";
    static uint8_t lfp_encoded_rx_buffer[256]; // Extra space for LFP overhead

    while (1) {
        cdh_comms_lfp_uart_lp(transmitter,
            transmitter,
            test_data,
            strlen(test_data),
            lfp_encoded_rx_buffer,
            sizeof(lfp_encoded_rx_buffer));
        LOG_INF("LFP UART loopback full parsed message body: %s", lfp_encoded_rx_buffer);
        k_msleep(200);
    }
}
