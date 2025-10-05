#include <atmel_start.h>
#include "hal_delay.h"
#include "atmel_start_pins.h"
#include "driver_init.h"

#define MISOpin   20u   // PD20 = SPI0_MISO
#define MOSIpin   21u   // PD21 = SPI0_MOSI
#define SPCKpin   22u   // PD22 = SPI0_SPCK

#define PD_MASK   (PD20 | PD21 | PD22)

#define LED_ON(void) (PIOA->PIO_CODR = (1u << 23))
#define LED_OFF(void) (PIOA->PIO_SODR = (1u << 23))
#define CS_LOW(void) (PIOD->PIO_CODR = (1u << 25))
#define CS_HIGH(void) (PIOD->PIO_SODR = (1u << 25))


static inline void busy_delay(volatile uint32_t n) { while (n--) __asm__("nop"); }

static inline uint32_t spi_scbr(uint32_t mck, uint32_t baud)
{
	uint32_t div = (mck + baud - 1u) / baud;
	if (div < 1u)   div = 1u;
	if (div > 255u) div = 255u;
	return div;
}


int main(void)
{
	init_mcu();
	// Enable PIO clocks for A and D
	PMC->PMC_PCER0 = (1u << ID_PIOA) | (1u << ID_PIOD) | (1 << ID_SPI0);
	
	PIOD->PIO_PDR = PD_MASK;
	PIOD->PIO_PUDR = PD_MASK;
	
	// Make sure SPI0 owns pins we want
	// Note SAMV71 apparently puts the pins we want (PD20, PD21, PD22) on peripheral C where SAME70 is on B
	PIOD->PIO_ABCDSR[0] &= ~PD_MASK;   
	PIOD->PIO_ABCDSR[1] |=  PD_MASK; 
	
	// LED0 = PA23
	PIOA->PIO_PER  = (0x1 << 23);   // Take control from peripheral
	PIOA->PIO_OER  = (0x1 << 23);   // Output enable

	PIOD->PIO_PER  = (0x1 << 25);
	PIOD->PIO_OER  = (0x1 << 25);
	
	CS_HIGH();

	SPI0->SPI_CR = SPI_CR_SWRST; // sw reset
	SPI0->SPI_CR = SPI_CR_SPIDIS; // disable while configuring
	
	SPI0->SPI_MR = SPI_MR_MSTR | SPI_MR_PS | SPI_MR_MODFDIS; // Master mode, variable chip select
	uint32_t scbr = spi_scbr(SystemCoreClock, 100000u);
	SPI0->SPI_CSR[1] = SPI_CSR_BITS_8_BIT | SPI_CSR_SCBR(scbr) | SPI_CSR_NCPHA; // set baud rate and data per whatever
	SPI0->SPI_CSR[1] &= ~SPI_CSR_CPOL;
	SPI0->SPI_WPMR = SPI_WPMR_WPKEY(0x535049);
	// write to wpsr to clear wp
	volatile uint32_t wpsr = SPI0->SPI_WPSR; 
	(void)wpsr;
	SPI0->SPI_CR = SPI_CR_SPIEN; // SPI Enable

	// Tell us if we we're at least able to flash and initialize
	LED_ON();
	delay_ms(500);
	LED_OFF();
	delay_ms(500);
	
	volatile uint8_t rx;
	volatile uint32_t spi_sr;
	for (;;) {
		CS_HIGH();

		spi_sr = SPI0->SPI_SR;
		if ( (spi_sr & (SPI_SR_SPIENS)) == 0 ) {
			int i;
			for (i = 0; i < 10; ++i) {
				LED_ON();
				delay_ms(50);
				LED_OFF();
				delay_ms(50);
			}
			SPI0->SPI_CR = SPI_CR_SPIEN;
		}
		LED_ON();
		//delay_ms(250); // Leave LED on for 1/4 of a second for debugging
		while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0);
		CS_LOW();
		SPI0->SPI_TDR = SPI_TDR_PCS_NPCS1 | SPI_TDR_TD(0xAB);
		while ((SPI0->SPI_SR & SPI_SR_TXEMPTY) == 0);
		
		(void)SPI0->SPI_RDR; // Drain rx just in case this does something
		// Pull MOSI down for sanity
		while (!(SPI0->SPI_SR & SPI_SR_TDRE));
		SPI0->SPI_TDR = SPI_TDR_PCS_NPCS1 | SPI_TDR_TD(0x00);
		while (!(SPI0->SPI_SR & SPI_SR_TXEMPTY));
		CS_HIGH();
		(void)SPI0->SPI_RDR; // Drain rx just in case this does something
		LED_OFF(); // If LED is off for a full second we we're able to transfer data or something
		delay_ms(1000);
		
		//busy_delay(3000000);
	}
}
