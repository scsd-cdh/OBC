/**
 * @file MSP430_I2C.h
 * @brief I2C slave driver header for MSP430.
 */

#ifndef _I2C_
#define _I2C_

#include <msp430.h>
#include <stdint.h>
#include <stddef.h>
#include <include/timer_a.h>
#include <lfp.h>

#define I2C_IS_TRANSMITTING (UCB0CTLW0 & UCTR)

typedef enum {
    I2C_MODULE_NONE = 0,
    I2C_MODULE_UCB0,
    I2C_MODULE_UCB1
} i2c_module_t;

/** Called on start condition, module identifies which triggered it */
typedef void (*i2c_stt_cb_t)(i2c_module_t module);
/** Called on tx interrupt, module identifies which triggered it */
typedef void (*i2c_tx_cb_t)(i2c_module_t module);
/** Called on receive, module doesn't matter */
typedef void (*i2c_rx_cb_t)(uint8_t byte);

typedef struct {
    i2c_stt_cb_t i2c_stt_cb; // on start condition, any module
    i2c_tx_cb_t i2c_tx_cb; // on tx interrupt, any module

    i2c_rx_cb_t i2c_rx_cb; // on receive (module doesn't matter)
    uint8_t slave_addr;
} i2c_ctx_t;

/**
 * Configures i2c module (usci_b) in slave mode
 *
 * @param module_mask bitmask of which modules to initialize
 * @param slave_addr our i2c slave address
 */
void i2c_init_registers(uint8_t module_mask, uint8_t slave_addr);

/**
 * Sets config data and configures i2c module (usci_b) in slave mode
 *
 * @param module_mask bitmask of which modules to initialize
 * @param cb_config context with callback and slave address
 */
void i2c_init(uint8_t module_mask, i2c_ctx_t* cb_config);

/**
 *  Reassigns the transmit buffer pointer to be sent on next transmit.
 *  @param data Pointer to data buffer which we will be sending directly over i2c when asked
 *  @param size How much data is available to be sent
 */
int16_t i2c_set_txbuf(volatile uint8_t* data, uint8_t size);

/**
 * Acks the last received byte
 * @param module which usci_b module to ack on
 */
void i2c_ack(i2c_module_t module);

/**
 * Nacks the last received byte
 * @param module which usci_b module to nack on
 */
void i2c_nack(i2c_module_t module);

/**
 * Writes a byte to the tx buffer register
 * @param module which usci_b module to write to
 * @param byte data to send
 */
void i2c_write(i2c_module_t module, uint8_t byte);

#endif /* I2C */
