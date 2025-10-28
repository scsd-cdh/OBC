#ifndef SPI0_WRAPPER_H
#define SPI0_WRAPPER_H

#include <hal_spi_m_sync.h>

#define SPI_BAUD_HZ 100000u

extern struct spi_m_sync_descriptor SPI0_desc;

void SPI0_init(void);
void SPI0_transfer(const uint8_t* txbuf, uint8_t* rxbuf, size_t size);
void SPI0_transferCustom(const uint8_t* txbuf, uint8_t* rxbuf, size_t txsize, size_t rxsize);

#endif // SPI0_WRAPPER_H