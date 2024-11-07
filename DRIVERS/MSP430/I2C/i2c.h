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

/**
 * @enum I2C_Mode
 * @brief Defines the various modes for the I2C state machine. Used to track
 *        the current mode of the I2C communication.
 */
typedef enum I2C_ModeEnum {
    IDLE_MODE,               /**< I2C is idle */
    RX_REG_ADDRESS_MODE,     /**< Receiving register address */
    RX_DATA_MODE,            /**< Receiving data from master */
    I2C_MODE_MAX
} I2C_Mode;

typedef struct sI2cConfigCb
{
    uint8_t (*Rx_Proc_Cmd)(uint8_t cmd);
    uint8_t (*Rx_Proc_Data)(uint8_t cmd);
} sI2cConfigCb_t;

/**
 * Buffers and counters for data transmission and reception
 * - ReceiveBuffer: Buffer to hold data received from the master
 * - RXByteCtr: Counter for bytes left to receive
 * - ReceiveIndex: Index of the next byte to be received in ReceiveBuffer
 * - TransmitBuffer: Buffer to hold data to send to the master
 * - TXByteCtr: Counter for bytes left to transmit
 * - TransmitIndex: Index of the next byte to be transmitted in TransmitBuffer
 */
uint8_t ReceiveBuffer[MAX_BUFFER_SIZE] = {0};
uint8_t RXByteCtr = 0;
uint8_t ReceiveIndex = 0;
uint8_t TransmitBuffer[MAX_BUFFER_SIZE] = {0};
uint8_t TXByteCtr = 0;
uint8_t TransmitIndex = 0;

/** I2C SlaveMode - Tracks the current mode of the I2C software state machine */
static I2C_Mode SlaveMode = IDLE_MODE;
static sI2cConfigCb_t slaveFuncsCb = {
    .Rx_Proc_Cmd = NULL,
    .Rx_Proc_Data = NULL
};

/**
 * @brief Initializes the I2C module in slave mode with the specified slave address.
 */
void initI2C(sI2cConfigCb_t* cb_config);

#endif /* I2C */
