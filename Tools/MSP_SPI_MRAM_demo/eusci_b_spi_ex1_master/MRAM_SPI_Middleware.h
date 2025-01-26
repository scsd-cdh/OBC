#include <stdint.h>

#ifndef _MRAM_SPI_
#define _MRAM_SPI_

void CS_LOW();
void CS_HIGH();
void initSPI();
void spi_read(uint8_t cmd, uint8_t * rx_data, uint8_t rx_size);
void readUniqueId(uint8_t id_buffer[4]);
void writeMemoryEn();

#endif  //_MRAM_SPI_
