#include "SPI0_wrapper.h"
#include <hpl_spi_base.h>
#include <hpl_pmc.h>
#include <atmel_start_pins.h>

struct spi_m_sync_descriptor SPI0_desc;

#define SPI_DEACTIVATE_NEXT 0x8000

static inline void SPI0_pinInit(void)
{
	gpio_set_pin_function(PD20, MUX_PD20B_SPI0_MISO);
	gpio_set_pin_function(PD21, MUX_PD21B_SPI0_MOSI);
	gpio_set_pin_function(PD22, MUX_PD22B_SPI0_SPCK);
	gpio_set_pin_function(PD25, MUX_PD25B_SPI0_NPCS1);
}

static inline void SPI0_clockInit(void)
{
	_pmc_enable_periph_clock(ID_SPI0);
}

// Helper to compute SCBR = MCK / SPI_BAUD (clamped to 1..255)
static inline uint32_t spi_scbr(uint32_t mck, uint32_t baud)
{
	uint32_t div = (mck + baud - 1u) / baud;
	if (div < 1u)   div = 1u;
	if (div > 255u) div = 255u;
	return div;
}

// Need a custom hardware register initializer because I can't figure out how to generate atmel start spi code that supports NPCS1
static void SPI0_initHardwareRegisters(void)
{
	hri_spi_write_CR_reg(SPI0, SPI_CR_SWRST); // software reset
	// set NPCS1 disable error handling and set SPI to master. NPCS1 allows us to use PD25 instead of default PB02. PD25 is what CDH board says our CS pin will be and is exposed on our dev boards
	hri_spi_write_MR_reg(SPI0, SPI_MR_MSTR | SPI_MR_PCS(0x0D) | SPI_MR_MODFDIS ); 
	uint32_t scbr = spi_scbr(SystemCoreClock, SPI_BAUD_HZ);
	hri_spi_write_CSR_reg(SPI0, 1, SPI_CSR_BITS_8_BIT | SPI_CSR_SCBR(scbr) | SPI_CSR_NCPHA | SPI_CSR_DLYBS(0) | SPI_CSR_DLYBCT(0) | SPI_CSR_CSAAT);
}

static inline uint8_t SPI0_transferByte(uint8_t tx)
{
	while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0);
	SPI0->SPI_TDR = SPI_TDR_TD(tx);
	while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0);
	return (uint8_t)SPI0->SPI_RDR;
}

void SPI0_init(void)
{
	// Disable pull up on MISO MOSI and SPCK
	PIOD->PIO_PUDR = (PIO_PUDR_P20 | PIO_PUDR_P21 | PIO_PUDR_P22);
	SPI0_clockInit();
	// spi_m_sync_transfer uses this for something
	SPI0_desc.dev.prvt = SPI0;
	SPI0_initHardwareRegisters();
	SPI0_pinInit();
}

void SPI0_transferCustom(const uint8_t* txbuf, uint8_t* rxbuf, size_t txsize, size_t rxsize)
{
	int i;
	for (i = 0; i < txsize; ++i) {
		SPI0_transferByte(txbuf[i]);
	}
	for (i = 0; i < rxsize; ++i) {
		rxbuf[i] = SPI0_transferByte(0x00);
	}
	
	// This ensures CS only goes high after we're done transferring (last transfer)
	hri_spi_write_CR_reg(SPI0, SPI_CR_LASTXFER);
}

// Function to write starting at a specific index and end on a specific index
void SPI0_sectionWrite(const uint8_t* txbuf, size_t startWrite, size_t endWrite)
{
	int i;
	for (i = startWrite; i < endWrite; ++i) {
		SPI0_transferByte(txbuf[i]);
	}
	
	// This ensures CS only goes high after we're done transferring (last transfer)
	hri_spi_write_CR_reg(SPI0, SPI_CR_LASTXFER);
}

// FIXME: This behaves weirdly hence the custom transfer function. We would like to be able to use atmel drivers as much as possible
void SPI0_transfer(const uint8_t* txbuf, uint8_t* rxbuf, size_t size)
{
	struct spi_xfer xfer;
	xfer.txbuf = txbuf;
	xfer.rxbuf = rxbuf;
	xfer.size = size;
	spi_m_sync_transfer(&SPI0_desc, &xfer);
	// This ensures CS only goes high after we're done transferring (last transfer)
	hri_spi_write_CR_reg(SPI0, SPI_CR_LASTXFER);
}
