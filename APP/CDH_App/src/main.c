#include "main.h"

#include <stdio.h>
//#include <ulog/ulog.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/led.h>

//#include "logging.h"
#include "zephyr/drivers/i2c.h"

// static const struct led_dt_spec dev_led = LED_DT_SPEC_GET(DT_ALIAS(led0));
static const struct i2c_dt_spec dev_test = I2C_DT_SPEC_GET(DT_NODELABEL(i2c_test));


int main(void)
{


    int ret;
   // logging_init();

    // int value = 0;

    // ReSharper disable once CppDFAEndlessLoop
    // while (1) {
        // led_on_dt(&dev_led);
        // ULOG_INFO("Blink number {}", value++);
        // k_msleep(2000);
    // }

    while (false)
    {
        k_msleep(1000);

        //i2c_configure_dt(&dev_test, I2C_SPEED_SET(I2C_SPEED_STANDARD) | I2C_MODE_CONTROLLER);

        ret = i2c_is_ready_dt(&dev_test);
        if (!ret) {
            printk("Not ready yet!\n");
            continue;
        }


        printk("Hi!\n");
        ret = i2c_write_dt(&dev_test, (uint8_t[]){'A','C'}, 2);
        printk("i2c_write_dt: %d\n", ret);
        uint8_t buf[16];
        ret = i2c_read_dt(&dev_test, buf, 2);
        printk("i2c_read_dt: %d\n", ret);
        printk("Buf is:\n");
        for (int i=0; i<16; i++) {
            printk("%02x ", buf[i]);
            if ((i+1)%16 == 0) printk("\n");
        }
    }

}
