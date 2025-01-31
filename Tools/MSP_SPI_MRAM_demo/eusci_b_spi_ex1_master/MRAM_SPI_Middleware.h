#include <stdint.h>

#ifndef _MRAM_SPI_
#define _MRAM_SPI_

void CS_LOW();
void CS_HIGH();
void initSPI();
uint8_t spiTransfer(uint8_t cmd);
void readDeviceId(uint8_t deviceId[4]);
uint8_t readMemoryArray(uint8_t addr[3]);
void writeMemoryArray(uint8_t addr[3], uint8_t value);
void writeMemoryEn();

#endif  //_MRAM_SPI_
