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

#include "SPI_Wrapper.h"

int main(void)
{
    SystemInit();

	SPI_init();
	
	
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
