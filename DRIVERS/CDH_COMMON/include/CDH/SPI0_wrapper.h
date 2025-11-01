#pragma once

#include <stddef.h>
#include <stdint.h>

void SPI0_init(void);
void SPI0_transfer(const uint8_t* txbuf, uint8_t* rxbuf, size_t size);
void SPI0_transferCustom(const uint8_t* txbuf, uint8_t* rxbuf, size_t txsize, size_t rxsize);
