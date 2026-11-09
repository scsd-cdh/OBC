#include "pins.h"
#include "cdh_comms.h"
#include "uart/uart_transport.h"
#include "zephyr/kernel.h"
#include "zephyr/logging/log.h"

LOG_MODULE_REGISTER(cdh, LOG_LEVEL_DBG);

int main(void)
{
    int err = pins_init();
    if (err != 0) {
        LOG_ERR("Pin initialization failed");
        return err;
    }

    int ret = uart_transport_open(transmitter);
    if (ret != 0) {
        LOG_ERR("Failed to open UART2: %d", ret);
        return ret;
    }
    return 0;
}
