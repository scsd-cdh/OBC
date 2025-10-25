#include "mram.h"

void MRAMwrite_cmd_addr_data(uint8_t cmd, uint32_t addr, const void* data, int dataSize) 
{
	
	spi_m_sync_enable(&SPI0_desc);
	
	// Retrieve the 24 bite address into array size of 3
	uint8_t addr_byte[] = {(addr & 0xFF0000) >> 16, (addr & 0xFF00) >> 8, (addr & 0xFF)};
	
	WREN_cmd();
	
	//Send the cmd
	const uint8_t *cmd_buffer = &cmd;
	SPI0_transferCustom(cmd_buffer, NULL, 1, 0);
	
	//Send the address
	const uint8_t *addr_byte_buffer = addr_byte;
	SPI0_transferCustom(addr_byte, NULL, 3, 0);
	
	//Send the data
	SPI0_transferCustom(data, NULL, dataSize, 0);
	
	WRDI_cmd();
	
	spi_m_sync_disable(&SPI0_desc);
}

void MRAMwrite_rollover_helper(uint8_t, uint32_t, uint32_t, const void*, int, int) {}

void MRAMread_cmd_addr(uint8_t, uint32_t, void*, int) 
{
	
}
	
void configWP(void) {}
	
void WREN_cmd(void) 
{
	spi_m_sync_enable(&SPI0_desc);
	//CS low
	
	SPI0_transferCustom(Write_EN)
	
	//CS High
	spi_m_sync_disable(&SPI0_desc);
}
	
void WRDI_cmd(void) 
{
	spi_m_sync_enable(&SPI0_desc);
	//CS low
	
	SPI0_transferCustom(Write_DIS)
	
	//CS High
	spi_m_sync_disable(&SPI0_desc);
}

void initMRAM(void) 
{
	
}