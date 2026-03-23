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

extern uint8_t I2C_CLOCK_HANG;

typedef void (*i2c_proc_byte_cb_t)(uint8_t byte);
typedef struct {
    i2c_proc_byte_cb_t i2c_rx_cb;
    uint8_t slave_addr;
} i2c_ctx_t;

typedef enum {
    I2C_SLAVE_STATE_REQUEST,
    I2C_SLAVE_STATE_PROCESSING,
    I2C_SLAVE_STATE_RESPONSE
} i2c_slave_state_t;
    
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

void i2c_transition(i2c_slave_state_t slave);

i2c_slave_state_t i2c_slave_current_state(); 


/** 
 *  Reassigns the transmit buffer pointer. Basically assumes data is a static global buffer. 
 *  @param data Static data buffer which we will be sending directly over i2c when asked
 *  @param size How much data is available to be sent
 */
int16_t i2c_reassign_txbuf(volatile uint8_t* data, uint8_t size);

#endif /* I2C */
