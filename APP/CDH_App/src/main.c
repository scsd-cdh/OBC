#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <ulog/ulog.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/led.h>
#include <zephyr/drivers/i2c.h>

static const struct i2c_dt_spec dev_test = I2C_DT_SPEC_GET(DT_NODELABEL(demo_i2c));
void demo_i2c(void)
{
    char * msg = "Hello!";

    // Write out message via i2c
    i2c_write_dt(&dev_test, (uint8_t *)msg, strlen(msg) + 1);
    (void)dev_test.addr;

    // Allocate buffer for response
    uint8_t buf[128];

    // Read the response into the buffer
    i2c_read_dt(&dev_test, buf, strlen(msg));

    __ASSERT(strcmp(buf, msg) == 0, "I2C demo mismatch");
}
static const struct led_dt_spec dev_led = LED_DT_SPEC_GET(DT_ALIAS(led0));

int main(void)
{
    int value = 0;
    bool status = false;

    // ReSharper disable once CppDFAEndlessLoop
    while (1) {
        demo_i2c();
        status = !status;
        led_set_brightness_dt(&dev_led, status ? LED_BRIGHTNESS_MAX : 0);
        ULOG_INFO("Blink number {}", value++);
        k_msleep(2000);
    }
}
