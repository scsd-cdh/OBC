#include "main.h"
#include "cdh_comms.h"
#include "zephyr/kernel.h"
#include "zephyr/logging/log.h"

LOG_MODULE_REGISTER(cdh);


int main(void)
{
    while (1) {
        cdh_comms_local_loopback_test(transmitter);
        k_msleep(200);
    }
}
