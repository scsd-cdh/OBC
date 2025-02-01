#include <stdint.h>

#ifndef _MRAM_SPI_
#define _MRAM_SPI_

typedef uint8_t SPI_Mode;
enum {
    SPI_MODE_ZERO = 0,
    SPI_MODE_ONE = 1,
    SPI_MODE_TWO = 2,
};

void CS_LOW();
void CS_HIGH();

// SPI-specific functions:
void SPI_init(uint32_t clockSpeed, SPI_Mode mode);
uint8_t SPI_transfer(uint8_t cmd);

// MRAM-specific functions:
void MRAM_readDeviceId(uint8_t deviceId[4]);
uint8_t MRAM_readMemoryArray(uint8_t addr[3]);
void MRAM_writeMemoryArray(uint8_t addr[3], uint8_t value);
void MRAM_writeMemoryEn();

#endif  // _MRAM_SPI_
