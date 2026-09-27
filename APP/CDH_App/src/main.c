#include "main.h"
#include "cdh_bms_example.h"
#include "zephyr/kernel.h"

int main(void)
{
    while (1) {
        // get_bms_system_status();
        get_bms_power_status();
        k_msleep(500);
    }
}
