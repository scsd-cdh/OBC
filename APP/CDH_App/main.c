#include "main.h"

#include <hal_delay.h>
#include <stdio.h>
#include <CDH/driver_init.h>
#include <CDH/pins.h>
#include <commsdrive.h>

#define SLAVE_ADDR 0x08

#include "logging.h"
#include "ulog/ulog.h"

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	system_init();
	commsdrive_init();

	init_logging();

	/* Replace with your application code */
	static const uint8_t size = 5;
	uint8_t rx_buf[size];
	volatile int32_t ret = 0;
	while (1) {
		gpio_toggle_pin_level(LED0);
		ret = commsdrive_process_telerequest(SLAVE_ADDR, 0x01, rx_buf, size);
		delay_ms(100);
	}
}
