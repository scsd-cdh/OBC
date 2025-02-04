#ifndef _MRAM_SPI_
#define _MRAM_SPI_

#include <stdint.h>
#include <stddef.h>

#define MRAM_MAX_ADDRESS 0x3FFFFF 

// Store port in the first byte and pin in the second. 
// This is fine for now, however there are 3 byte+ pins defined in driverlib. 
// I don't personally see them on the launchboard 
// TODO: Support all GPIO pins
#define GPIO_PIN_13 ((GPIO_PORT_P1 << 8) | GPIO_PIN3)
#define GPIO_PIN_14 ((GPIO_PORT_P1 << 8) | GPIO_PIN4)
#define GPIO_PIN_15 ((GPIO_PORT_P1 << 8) | GPIO_PIN5)
#define GPIO_PIN_16 ((GPIO_PORT_P1 << 8) | GPIO_PIN6)
#define GPIO_PIN_17 ((GPIO_PORT_P1 << 8) | GPIO_PIN7)
#define GPIO_PIN_22 ((GPIO_PORT_P2 << 8) | GPIO_PIN2)

#define DEFAULT_CS_PIN GPIO_PIN_13
#define DEFAULT_MOSI_PIN GPIO_PIN_16
#define DEFAULT_MISO_PIN GPIO_PIN_17 
#define DEFAULT_SCLK_PIN GPIO_PIN_22

typedef uint8_t MRAM_ErrorCode;
enum {
    MRAM_ERR_OK = 0,
    MRAM_ERR_BAD_PARAM,
    MRAM_ERR_WRITE_BLOCK_PROTECTION_ENABLED,
    MRAM_ERR_WRITE_PROTECTION_ENABLED,
    MRAM_ERR_OUT_OF_BOUNDS,
    MRAM_ERR_BAD_CRC,
};


typedef uint8_t SPI_Mode;
enum {
    SPI_MODE_ZERO = 0,
    SPI_MODE_ONE = 1,
    SPI_MODE_TWO = 2,
};

void CS_LOW();
void CS_HIGH();

// SPI-specific functions:
void SPI_init(uint32_t clockSpeed, SPI_Mode mode, uint16_t CS_pin, uint16_t SCLK_pin, uint16_t MOSI_pin, uint16_t MISO_pin);
uint8_t SPI_transfer(uint8_t byte);

// MRAM-specific functions:
MRAM_ErrorCode MRAM_readStatusRegister(uint8_t* status);
void MRAM_writeStatusRegister(uint8_t status);
void MRAM_readDeviceId(uint8_t deviceId[4]);
MRAM_ErrorCode MRAM_readMemoryArray(uint32_t addr, uint8_t* buffer, size_t length);
MRAM_ErrorCode MRAM_writeMemoryArray(uint32_t addr, uint8_t* buffer, size_t length);
void MRAM_writeMemoryEn();
void MRAM_writeMemoryDisable();

#endif  // _MRAM_SPI_
