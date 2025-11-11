#include "main.h"

#include <hal_delay.h>
#include <CDH/driver_init.h>
#include <CDH/pins.h>

int main(void)
{
	system_init();

	/* Replace with your application code */
	while (1) {
		gpio_toggle_pin_level(LED0);
		delay_ms(100);
	}
}
