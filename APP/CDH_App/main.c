#include <atmel_start.h>
#include <atmel_start_pins.h>

#include "hal_delay.h"
#include "commsdrive.h"
#include "driver_examples.h"

#define SLAVE_ADDR 0x08

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	CommsDrive_Init(SLAVE_ADDR);

	/* Replace with your application code */
	static const uint8_t size = 5;
	uint8_t rx_buf[size];
	uint8_t pwm_buff[5] = {99, 99, 99, 99};
	while (1) {
		gpio_toggle_pin_level(LED0);
		CommsDrive_ProcessTeleCommand(SLAVE_ADDR, 0x15, pwm_buff, size);
		delay_ms(100);	
	}
}
