#include "main.h"
#include "cdh_pds_example.h"
#include "zephyr/kernel.h"

int main(void)
{
    while (1) {
        // get_pds_system_status();
        // get_pds_health_check();
        get_pds_converter();
        k_msleep(500);
    }
}
