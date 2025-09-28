/*
 * SPI0 master on SAM E70/S70/V70/V71
 * - MISO  = PD20 (Peripheral B)
 * - MOSI  = PD21 (Peripheral B)
 * - SPCK  = PD22 (Peripheral B)
 * - NPCS1 = PD25 (Peripheral B)  <-- hardware CS
 *
 * Notes:
 * - Fixed Peripheral Select (PS=0): SPI_MR.PCS selects the device; TDR.PCS is ignored.
 * - We configure CSR[1] because we use NPCS1.
 */

#include "sam.h"
#include <stdint.h>

typedef uint16_t size_t;

#define MISOpin   20u   // PD20 = SPI0_MISO
#define MOSIpin   21u   // PD21 = SPI0_MOSI
#define SPCKpin   22u   // PD22 = SPI0_SPCK
#define NPCS1pin  25u   // PD25 = SPI0_NPCS1

#define PD_MASK   ((1u<<MISOpin)|(1u<<MOSIpin)|(1u<<SPCKpin)|(1u<<NPCS1pin))

// Target SPI bit rate (adjust as needed)
#define SPI_BAUD_HZ   100000u

#define MRAM_RDSR 0x05
#define MRAM_RDID 0x9F

#define MRAM_STATUS_REGISTER_ID_DATA_BYTES 1
#define MRAM_DEVICE_ID_DATA_BYTES 4


// Helper to compute SCBR = MCK / SPI_BAUD (clamped to 1..255)
static inline uint32_t spi_scbr(uint32_t mck, uint32_t baud)
{
    uint32_t div = (mck + baud - 1u) / baud;
    if (div < 1u)   div = 1u;
    if (div > 255u) div = 255u;
    return div;
}

static void SPI_masterTransfer(const uint8_t cmdId)
{
	 while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0) { /* wait */ }
	 // In fixed select mode, TDR.PCS is ignored; only write data
	 SPI0->SPI_TDR = SPI_TDR_TD(cmdId);
}

static inline uint8_t SPI_masterReceive()
{
	while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0) {}      // wait data ready
	return (uint8_t)SPI0->SPI_RDR;
}

static inline void SPI_masterReceiveBulk(volatile uint8_t* buffer, size_t len)
{

}

static void SPI_masterSendAndReceive(const uint8_t cmdId, volatile uint8_t* buffer, size_t len)
{
	// Optionally wait for TXEMPTY to ensure CS deasserts between frames
	while ((SPI0->SPI_SR & SPI_SR_TXEMPTY) == 0) { /* wait */ }
	SPI_masterTransfer(cmdId);

	// Clear any received data (optional but tidy if MISO is connected)
	if (SPI0->SPI_SR & SPI_SR_RDRF) {
		(void)SPI0->SPI_RDR;
	}
	
	SPI_masterReceiveBulk(buffer, len);

	/* optional: after last transfer */
	while ((SPI0->SPI_SR & SPI_SR_TXEMPTY) == 0) {}   // last bit shifted out
}

//NOTE: Produces big gaps in between bytes in the SCK and in the CS line that seem to produce some weirdness 
//		when I tried to factor this into different functions. Seemed to make some difference in the gap but not enough
//	    to make the weirdness go away when inlining the functions. Further debugging required.
static void MRAM_readDeviceID(volatile uint8_t* buffer, size_t len)
{
	uint8_t cmdId = MRAM_RDID;
	// Optionally wait for TXEMPTY to ensure CS deasserts between frames
	while ((SPI0->SPI_SR & SPI_SR_TXEMPTY) == 0) { /* wait */ }
	// In fixed select mode, TDR.PCS is ignored; only write data
	SPI0->SPI_TDR = SPI_TDR_TD(cmdId);
	// Clear any received data (optional but tidy if MISO is connected)
	if (SPI0->SPI_SR & SPI_SR_RDRF) {
		(void)SPI0->SPI_RDR;
	}
	
	 int i;
	 for (i = 0; i < len; ++i) {
		 while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0) { /* wait */ }
		// In fixed select mode, TDR.PCS is ignored; only write data
		 SPI0->SPI_TDR = SPI_TDR_TD(0x00);
	
	 	while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0) {}      // wait data ready
		 buffer[i] = (uint8_t)SPI0->SPI_RDR;
	 }
	 
	/* optional: after last transfer */
	while ((SPI0->SPI_SR & SPI_SR_TXEMPTY) == 0) {}   // last bit shifted out
}

static void MRAM_readStatusRegister(volatile uint8_t* buffer, uint8_t len)
{
	SPI_masterSendAndReceive(MRAM_RDSR, buffer, len);
}

int main(void)
{
    SystemInit();

    // --- Enable peripheral clocks: SPI0 and PIOD
    PMC->PMC_PCER0 = (1u << ID_SPI0) | (1u << ID_PIOD);

    // --- Hand PD20/21/22/25 to Peripheral B (SPI0)
    // Disable PIO control on those pins so the peripheral owns them
    PIOD->PIO_PDR = PD_MASK;

    // Select Peripheral B: ABCDSR = 01 (ABCDSR[0]=1, ABCDSR[1]=0)
    PIOD->PIO_ABCDSR[0] |=  PD_MASK;   // set bit -> 1
    PIOD->PIO_ABCDSR[1] &= ~PD_MASK;   // clear bit -> 0

    // Optional: disable pull-ups on these lines
    PIOD->PIO_PUDR = PD_MASK;

    // --- Configure SPI0 in master, fixed peripheral select, NPCS1
    SPI0->SPI_CR = SPI_CR_SWRST;      // reset
    SPI0->SPI_CR = SPI_CR_SPIDIS;     // disable while configuring

    // Master | Mode Fault Detect disabled | PS=0 (fixed) | PCS = 0xD (assert NPCS1)
    // PCS is active-low mask: 0xE->NPCS0, 0xD->NPCS1, 0xB->NPCS2, 0x7->NPCS3
    SPI0->SPI_MR = SPI_MR_MSTR
                 | SPI_MR_MODFDIS
                 | SPI_MR_PCS(0xDu);   // select NPCS1

    // SPI mode: CPOL=0, NCPHA=1 (i.e., Mode 1); change to your target mode as needed.
    // Set baud = MCK/SCBR. Use SystemCoreClock as MCK baseline.
    uint32_t scbr = spi_scbr(SystemCoreClock, SPI_BAUD_HZ);

    // IMPORTANT: use CSR[1] because we’re talking to NPCS1
    SPI0->SPI_CSR[1] = SPI_CSR_NCPHA            // CPHA=1 (Mode 1). Omit for CPHA=0.
                     | SPI_CSR_BITS_8_BIT       // 8-bit frames
                     | SPI_CSR_SCBR(scbr)       // serial clock baud rate divider
                     | SPI_CSR_DLYBS(0)         // optional setup delay
                     | SPI_CSR_DLYBCT(0);       // optional inter-transfer delay

    SPI0->SPI_CR = SPI_CR_SPIEN;                // enable SPI

    // --- Simple transmit loop (polling)
	volatile uint8_t MRAM_deviceIdBuffer[MRAM_DEVICE_ID_DATA_BYTES] = {};
	volatile uint8_t MRAM_statusRegisterBuffer[MRAM_STATUS_REGISTER_ID_DATA_BYTES] = {};
    for (;;)
    {
        // Wait for TDRE (TDR empty) then write a byte.
        while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0) { /* wait */ }

		MRAM_readDeviceID(MRAM_deviceIdBuffer, MRAM_DEVICE_ID_DATA_BYTES);

        // Change data pattern for visibility on a logic analyzer
        for (volatile uint32_t i = 0; i < 100000; ++i) { __NOP(); }
    }
	
	return 0;
}
