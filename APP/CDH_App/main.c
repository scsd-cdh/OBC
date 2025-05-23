#include <atmel_start.h>
#include "hal_delay.h"
#include "atmel_start_pins.h"

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	/* Replace with your application code */
	while (1) {
		gpio_toggle_pin_level(LED0);
		delay_ms(100);
	}
}
