/**
 * @file MSP430_I2C.h
 * @brief I2C driver header for MSP430. This header defines configurations, command IDs,
 * state machine variables, and functions used for I2C communication between an MSP430
 * device (slave) and a master device.
 *
 * @requirements:
 * - Compatible with MSP430FR59xx series running at 16 MHz.
 * - Configures I2C in slave mode with specific commands and response structures.
 */

#ifndef _I2C_
#define _I2C_

#include <msp430.h> 
#include <stdint.h>
#include <stddef.h>

//*******************************************************************************
// General I2C State Machine ***************************************************
//*******************************************************************************

#define MAX_BUFFER_SIZE             20    /**< Maximum buffer size for I2C data transmission */

typedef struct sI2cConfigCb
{
    void (*Rx_Proc_Data)(uint8_t data);
    uint8_t slave_addr;
} sI2cConfigCb_t;

/**
 * @brief Initializes the I2C module in slave mode with the specified slave address.
 */
void initI2C(sI2cConfigCb_t* cb_config);

/** 
* @brief Write 
*/
int16_t transmitI2C(const uint8_t* data, uint8_t size) ;

#endif /* I2C */
