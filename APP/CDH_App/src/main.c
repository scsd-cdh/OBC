#include "main.h"

#include <stdio.h>
#include <ulog/ulog.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/led.h>

#include "logging.h"

static const struct led_dt_spec dev_led = LED_DT_SPEC_GET(DT_ALIAS(led0));


int main(void)
{
    logging_init();

    int value = 0;

    // ReSharper disable once CppDFAEndlessLoop
    while (1) {
        led_on_dt(&dev_led);
        ULOG_INFO("Blink number {}", value++);
        k_msleep(2000);
    }

}
