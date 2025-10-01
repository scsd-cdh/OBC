/*
 * MRAM_peripheral.c
 *
 * Created: 10/1/2025 7:11:28 PM
 *  Author: space
 */ 

void MRAM_readDeviceID(volatile uint8_t* buffer, size_t len)
{
	SPI_masterTransfer(MRAM_RDID)
	int i;
	for (i = 0; i < len; ++i) {
		buffer[i] = SPI_masterReceive();
	}
}

void MRAM_readStatusRegister(volatile uint8_t* buffer, uint8_t len)
{
	SPI_masterTransfer(MRAM_RDSR);
	int i;
	for (i = 0; i < len; ++i) {
		buffer[i] = SPI_masterReceive();
	}
}
