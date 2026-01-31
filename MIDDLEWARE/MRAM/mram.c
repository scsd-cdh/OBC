#include "MRAM.h"
#include "SPI0_wrapper.h"

// Be advised that the CS pin is automatically switch HIGH and LOW on the SAMV71 and the SAME70
void MRAM_write_cmd_addr_data(uint8_t cmd, uint32_t addr, const uint8_t data, int data_size) 
{
	// Send the write enable command
	WREN_cmd();
	// Start the SPI communications
	spi_m_sync_enable(&SPI0_desc);
	
	// Retrieve the 24 bite address into array size of 3
	uint8_t addr_byte[] = {(addr & 0xFF0000) >> 16, (addr & 0xFF00) >> 8, (addr & 0xFF)};	
	
	// Send the write command
	const uint8_t *cmd_buffer = &cmd;
	SPI0_transferCustom(cmd_buffer, NULL, 1, 0);
	
	// Send the address
	const uint8_t *addr_byte_buffer = addr_byte;
	SPI0_transferCustom(addr_byte, NULL, 3, 0);
	
	// Send the data
	SPI0_transferCustom(data, NULL, data_size, 0);
	
	// Stops the SPI communications
	spi_m_sync_disable(&SPI0_desc);
	// Send the write disable command
	WRDI_cmd();
}

void MRAM_write_rollover_helper(uint8_t cmd, uint32_t addr_start1, uint32_t addr_start2, const void* data, int size, int size_split) 
{
	// Send the write enable command
	WREN_cmd();
	// Start the SPI communications
	spi_m_sync_enable(&SPI0_desc);
	
	// Bit shift start address 1 and 2 into 3 bytes each
	uint8_t addr1_byte[] = {(addr_start1 & 0xFF0000) >> 16, (addr_start1 & 0xFF00) >> 8, (addr_start1 & 0xFF)};
	uint8_t addr2_byte[] = {(addr_start2 & 0xFF0000) >> 16, (addr_start2 & 0xFF00) >> 8, (addr_start2 & 0xFF)};

	// Send the write command
	const uint8_t *cmd_buffer = &cmd;
	SPI0_transferCustom(cmd_buffer, NULL, 1, 0);
	
	// Send the address 1
	const uint8_t *addr1_byte_buffer = addr1_byte;
	SPI0_transferCustom(addr1_byte_buffer, NULL, 3, 0);
	
	// Send the data
	SPI0_transferCustom(data, NULL, size_split, 0);

	// Send the write command
	const uint8_t *cmd_buffer = &cmd;
	SPI0_transferCustom(cmd_buffer, NULL, 1, 0);
	
	// Send the address 2
	const uint8_t *addr2_byte_buffer = addr2_byte;
	SPI0_transferCustom(addr2_byte_buffer, NULL, 3, 0);
	
	// Send the data
	SPI0_sectionWrite(data, size_split, size);
	
	// Stops the SPI communications
	spi_m_sync_disable(&SPI0_desc);
	// Send the write disable command
	WRDI_cmd();
}

/*
Read MRAM data where command does require a specified address
Retrieve 24 LSB
*/
void MRAM_read_cmd_addr(uint8_t cmd, uint32_t addr, void* data, int data_size) 
{
	// Bit shift start the read address into 3 bytes
	uint8_t addr_byte[] = { (addr & 0xFF0000) >> 16, (addr & 0xFF00) >> 8, (addr & 0xFF) };
		
	// Start the SPI communications
	spi_m_sync_enable(&SPI0_desc);
	
	// Sending the read command
	const uint8_t *cmd_buffer = &cmd;
	SPI0_transferCustom(cmd_buffer, NULL, 1, 0);
	
	// Send the address
	const uint8_t *addr_byte_buffer = addr_byte;
	SPI0_transferCustom(addr_byte, NULL, 3, 0);
	
	// Send the data
	SPI0_transferCustom(data, NULL, data_size, 0);
	
	// Stops the SPI communications
	spi_m_sync_disable(&SPI0_desc);
}
	
// Configure Write Protect
void configWP(void) 
{
	/*
	1. Send WREN command to MRAM
	2. Turn Modify status/config register to liking
	3. Protect status/config register (either state 1 or 2 in table 14)
	*/	
	
	WREN_cmd();
	
	/*
	Don't need to do anything with status register, but implementing a function would be necessary if it needs to be.
	
	Don't need to do anything with config register, but implementing a function would be necessary if it needs to be.
	*/
}
	
// Write enable
void WREN_cmd(void) 
{
	// Start the SPI communication
	spi_m_sync_enable(&SPI0_desc);
	
	// Transfers the write enable command
	SPI0_transferCustom(Write_EN, 0, sizeof(Write_EN));
	
	// Stops SPI communications
	spi_m_sync_disable(&SPI0_desc);
}
	
// Write Disable
void WRDI_cmd(void) 
{
	// Start the SPI communication
	spi_m_sync_enable(&SPI0_desc);
	
	// Transfers the write disable command
	SPI0_transferCustom(Write_EN, 0, sizeof(Write_DIS));
	
	// Stops SPI communications
	spi_m_sync_disable(&SPI0_desc);
}

// No need because it should already be initialize on the SPI wrapper
/*
void initMRAM(void) 
{
	
}
*/