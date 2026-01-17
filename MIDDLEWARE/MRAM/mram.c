#include "mram.h"
#include "SPI0_wrapper.h"

// Be advised that the CS pin is automatically switch HIGH and LOW 
void MRAMwrite_cmd_addr_data(uint8_t cmd, uint32_t addr, const uint8_t data, int dataSize) 
{
	WREN_cmd();
	spi_m_sync_enable(&SPI0_desc);
	
	// Retrieve the 24 bite address into array size of 3
	uint8_t addr_byte[] = {(addr & 0xFF0000) >> 16, (addr & 0xFF00) >> 8, (addr & 0xFF)};	
	
	//Send the cmd
	const uint8_t *cmd_buffer = &cmd;
	SPI0_transferCustom(cmd_buffer, NULL, 1, 0);
	
	//Send the address
	const uint8_t *addr_byte_buffer = addr_byte;
	SPI0_transferCustom(addr_byte, NULL, 3, 0);
	
	//Send the data
	SPI0_transferCustom(data, NULL, dataSize, 0);
	
	spi_m_sync_disable(&SPI0_desc);
	WRDI_cmd();
}

void MRAMwrite_rollover_helper(uint8_t cmd, uint32_t addrStart1, uint32_t addrStart2, const void* data, int size, int sizeSplit) 
{
	WREN_cmd();
	spi_m_sync_enable(&SPI0_desc);
	
	uint8_t addr1_byte[] = {(addrStart1 & 0xFF0000) >> 16, (addrStart1 & 0xFF00) >> 8, (addrStart1 & 0xFF)};
	uint8_t addr2_byte[] = {(addrStart2 & 0xFF0000) >> 16, (addrStart2 & 0xFF00) >> 8, (addrStart2 & 0xFF)};

	//Send the cmd
	const uint8_t *cmd_buffer = &cmd;
	SPI0_transferCustom(cmd_buffer, NULL, 1, 0);
	
	//Send the address
	const uint8_t *addr_byte_buffer = addr1_byte;
	SPI0_transferCustom(addr1_byte, NULL, 3, 0);
	
	//Send the data
	SPI0_transferCustom(data, NULL, sizeSplit, 0);

	//Send the cmd
	const uint8_t *cmd_buffer = &cmd;
	SPI0_transferCustom(cmd_buffer, NULL, 1, 0);
	
	//Send the address
	const uint8_t *addr_byte_buffer = addr1_byte;
	SPI0_transferCustom(addr2_byte, NULL, 3, 0);
	
	//Send the data
	SPI0_sectionWrite(data, sizeSplit, size);
	
	spi_m_sync_enable(&SPI0_desc);
}

void MRAMread_cmd_addr(uint8_t cmd, uint32_t addr, void* data, int dataSize) 
{
	/*
	Read MRAM data where command does require a specified address
	Retrieve 24 LSB
	*/
	uint8_t addr_byte[] = { (addr & 0xFF0000) >> 16, (addr & 0xFF00) >> 8, (addr & 0xFF) };
		
	spi_m_sync_enable(&SPI0_desc);
	
	const uint8_t *cmd_buffer = &cmd;
	SPI0_transferCustom(cmd_buffer, NULL, 1, 0);
	
	//Send the address
	const uint8_t *addr_byte_buffer = addr_byte;
	SPI0_transferCustom(addr_byte, NULL, 3, 0);
	
	//Send the data
	SPI0_transferCustom(data, NULL, dataSize, 0);
	
	spi_m_sync_enable(&SPI0_desc);
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
	
// To complete
void WREN_cmd(void) 
{
	spi_m_sync_enable(&SPI0_desc);
	
	SPI0_transferCustom(Write_EN, 0, sizeof(Write_EN));
	
	spi_m_sync_disable(&SPI0_desc);
}
	
// To complete
void WRDI_cmd(void) 
{
	spi_m_sync_enable(&SPI0_desc);
	
	SPI0_transferCustom(Write_EN, 0, sizeof(Write_DIS));
	
	spi_m_sync_disable(&SPI0_desc);
}

// No need because it should already be initialize on the SPI wrapper
/*
void initMRAM(void) 
{
	
}
*/