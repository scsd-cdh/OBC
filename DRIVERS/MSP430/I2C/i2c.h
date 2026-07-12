/**
 * @file MSP430_I2C.h
 * @brief I2C slave driver header for MSP430. 
 */

#ifndef _I2C_
#define _I2C_

#include <msp430.h> 
#include <stdint.h>
#include <stddef.h>

#define I2C_IS_TRANSMITTING (UCB0CTLW0 & UCTR)

extern uint8_t I2C_CLOCK_HANG;

typedef void (*i2c_rx_byte_cb_t)(uint8_t byte);
typedef void (*i2c_tx_byte_cb_t)(volatile uint8_t* byte);
typedef void (*i2c_start_cond_cb_t)(void);

typedef struct {
    i2c_rx_byte_cb_t i2c_rx_cb;
    i2c_tx_byte_cb_t i2c_tx_byte_cb;
    i2c_start_cond_cb_t i2c_start_cond_cb;
    uint8_t slave_addr;
} i2c_ctx_t;
    
/**
 * Configures i2c module (usci_b) in slave mode
 * 
 * @param slave_addr our i2c slave address 
 */
void i2c_init_registers(uint8_t slave_addr);
/**
 * Sets config data and configures i2c module (usci_b) in slave mode
 * 
 * @param cb_config context with callback and slave address 
 */
void i2c_init(i2c_ctx_t* cb_config);

/** 
 *  Reassigns the transmit buffer pointer, does not take ownership (user responsible for managing buffer). Transmitting a buffer and registering a per byte tx callback is not allowed
 *
 *  @param  data Static data buffer which we will be sending directly over i2c when asked
 *  @param  size How much data is available to be sent
 *  @return -1 if user has registered a i2c_tx_byte_cb and 0 otherwise. Data will not be sent in this case
 */
int16_t i2c_tx_own_buf(volatile uint8_t* data, uint8_t size);

/**
 * Sends NACK
 */
void i2c_send_nack(void);

/**
 * Clears NACK
 */
void i2c_clear_nack(void);

#endif /* I2C */
