/*  

NOTE: This driver is configured to be used with the MSP430FR5969 and was written using interrupts without taking into 
  account also using I2C on the MSP. If you want to use I2C and SPI at the same time, you will either need to modify the I2C ISR
  or (reccomened) use the USCI_B0_ISR for I2C and a different ISR and vector for SPI. Note that changing the ISR and vector will require
  you to use different pins for the SPI interface. Defined below are some of the pins that can be used for the SPI interface for the MSP430FR5969.

  Note that if you try to use two seperate B0 or A0 ISRs, the program will not compile and give you some confusing errors (__TI_<something> already defined or something).
  
  Look up the datasheet for your specific SPI device to find what vectors correspond to what pins and so on. From there, you can modify this driver
  to work with those pins. For example EUSCI_B0_BASE -> EUSCI_<vector>_BASE. Be sure to double check what API you are using to make sure the transition
  is this simple. 

  Also to make sure we don't run into any build issues the ISR in MRAM_SPI.c will be commented out to avoid any build issues. Use this driver as reference.

  Had to leave in a rush because this project was abandoned and we have bigger priorities right now.
*/


#ifndef _MRAM_SPI_
#define _MRAM_SPI_

#include "gpio.h"
#include <stdint.h>
#include <stddef.h>

extern volatile uint8_t rxData;

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

#define GPIO_PIN_23 ((GPIO_PORT_P2 << 8) | GPIO_PIN3)
#define GPIO_PIN_24 ((GPIO_PORT_P2 << 8) | GPIO_PIN4)
#define GPIO_PIN_25 ((GPIO_PORT_P2 << 8) | GPIO_PIN5)
#define GPIO_PIN_26 ((GPIO_PORT_P2 << 8) | GPIO_PIN6)
#define GPIO_PIN_17 ((GPIO_PORT_P1 << 8) | GPIO_PIN7)
#define GPIO_PIN_22 ((GPIO_PORT_P2 << 8) | GPIO_PIN2)


// B0 vector pins 
#define DEFAULT_B0_CS_PIN GPIO_PIN_13
#define DEFAULT_B0_MOSI_PIN GPIO_PIN_16
#define DEFAULT_B0_MISO_PIN GPIO_PIN_17
#define DEFAULT_B0_SCLK_PIN GPIO_PIN_22

// A1 vector pins
#define DEFAULT_A1_CS_PIN GPIO_PIN_23
#define DEFAULT_A1_SCLK_PIN GPIO_PIN_24
#define DEFAULT_A1_MOSI_PIN GPIO_PIN_25
#define DEFAULT_A1_MISO_PIN GPIO_PIN_26

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

extern void CS_LOW();
extern void CS_HIGH();

// SPI-specific functions:
extern void SPI_init(uint32_t clockSpeed, SPI_Mode mode, uint16_t CS_pin, uint16_t SCLK_pin, uint16_t MOSI_pin, uint16_t MISO_pin);
extern uint8_t SPI_transfer(uint8_t byte);

// MRAM-specific functions:
extern MRAM_ErrorCode MRAM_readStatusRegister(uint8_t* status);
extern void MRAM_writeStatusRegister(uint8_t status);
extern void MRAM_readDeviceId(uint8_t deviceId[4]);
extern MRAM_ErrorCode MRAM_readMemoryArray(uint32_t addr, uint8_t* buffer, size_t length);
extern MRAM_ErrorCode MRAM_writeMemoryArray(uint32_t addr, const uint8_t* buffer, size_t length);
extern void MRAM_writeMemoryEn();
extern void MRAM_writeMemoryDisable();

#endif  // _MRAM_SPI_
