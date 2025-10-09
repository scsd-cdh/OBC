#include <atmel_start.h>
#include "hal_delay.h"
#include "atmel_start_pins.h"
#include "SPI0_wrapper.h"

#define MRAM_RDSR 0x05
#define MRAM_RDID 0x9F

#define MRAM_STATUS_REGISTER_ID_DATA_BYTES 1
#define MRAM_DEVICE_ID_DATA_BYTES 4

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	
	atmel_start_init();
	
	spi_m_sync_enable(&SPI0_desc);
	
	/* Replace with your application code */
	uint8_t tx = 0x9F;
	const uint8_t *txbuf = &tx; 
	uint8_t rxbuf[MRAM_DEVICE_ID_DATA_BYTES];
	while (1) {
		SPI0_transferCustom(txbuf, rxbuf, 1, sizeof(rxbuf));
		delay_ms(250);
	}
}
