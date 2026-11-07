#include "include/main.h"
#include "include/pins.h"
#include "include/cdh_pds_example.h"
#include "zephyr/kernel.h"
  
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

LOG_MODULE_REGISTER(main);


int main(void)
{
    int err = pins_init();
    if (err != 0) {
        LOG_ERR("Pin initilization failed");
        return err;
    }
    while (1) {
        // get_pds_system_status();
        // get_pds_health_check();
        get_pds_converter();
        k_msleep(500);
    }
}
