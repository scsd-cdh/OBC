#include "main.h"

#include <stdio.h>
#include <ulog/ulog.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/led.h>

static const struct led_dt_spec dev_led = LED_DT_SPEC_GET(DT_ALIAS(led0));

ULOG_SYSTEM_ID(0x1234)

int main(void)
{
    int value = 0;
    bool status = false;

    // ReSharper disable once CppDFAEndlessLoop
    while (1) {
        status = !status;
        led_set_brightness_dt(&dev_led, status ? LED_BRIGHTNESS_MAX : 0);
        ULOG_INFO("Blink number {}", value++);
        k_msleep(2000);
    }
}
