#include "main.h"

#include <hal_delay.h>
#include <stdio.h>
#include <CDH/driver_init.h>
#include <CDH/pins.h>

#include "logging.h"
#include "ulog/ulog.h"

int main(void)
{
	system_init();

	init_logging();

	int value = 0;

	/* Replace with your application code */
	while (1) {
		gpio_toggle_pin_level(LED0);
		delay_ms(2000);
		ULOG_INFO("Blink number {}", value++);
	}

}
