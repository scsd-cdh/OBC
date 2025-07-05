#include <atmel_start.h>
#include <atmel_start_pins.h>

#include "hal_delay.h"
#include "commsdrive.h"

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	CommsDrive_Init(0xA);

	/* Replace with your application code */
	while (1) {
		gpio_toggle_pin_level(LED0);
		CommsDrive_Process();
		
		delay_ms(100);	
	}
}
