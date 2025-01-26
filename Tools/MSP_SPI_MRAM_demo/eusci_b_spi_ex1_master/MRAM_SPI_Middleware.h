#include <stdint.h>

#ifndef _MRAM_SPI_
#define _MRAM_SPI_

void CS_LOW();
void CS_HIGH();
void initSPI();
uint8_t spiTransfer(uint8_t cmd);
void readUniqueId(uint8_t* uniqueId);
uint8_t readMemoryArray();
void writeMemoryArray();
void writeMemoryEn();

#endif  //_MRAM_SPI_
