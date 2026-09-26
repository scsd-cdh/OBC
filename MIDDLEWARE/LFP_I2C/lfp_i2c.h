#ifndef LFP_I2C
#define LFP_I2C

#include "i2c.h"
#include "lfp/stream.h"

typedef enum {
    I2C_SLAVE_STATE_IDLE,
    I2C_SLAVE_STATE_REQUEST,
    I2C_SLAVE_STATE_PROCESSING,
    I2C_SLAVE_STATE_RESPONSE
} lfp_i2c_state_t;


/**
 * Initializes i2c to necessary lfp specific callbacks
 *
 * @param p_lfp_ctx LFP Context struct. Passed to LFP 
 * @param module_mask Which i2c module to initialize
 * @param slave_addr i2c slave address  
 */ 
void lfp_i2c_init(lfp_stream_ctx_t * p_lfp_ctx, i2c_module_t module_mask, uint8_t slave_addr);

/** 
 * Sets state to given lfp_i2c state. NACKS the master if I2C_SLAVE_STATE_PROCESSING and discards tx buffer and resets if I2C_SLAVE_STATE_REQUEST
 * NOTE: this is currently does not enforce strict state machine logic, user can transition from any state to any other state. Not set in stone 
 *
 * @param state LFP state to transition to
 */
void lfp_i2c_transition(lfp_i2c_state_t state);

/**
 * Sets the buffer to be sent on next i2c read  
 *
 * @param data pointer to buffer to be sent
 * @param size size of buffer (number of bytes to send)
 */
int16_t lfp_i2c_set_txbuf(volatile uint8_t* data, uint8_t size);

/**
 * Get the current state
 * 
 * @return the current lfp state 
 */
lfp_i2c_state_t lfp_i2c_state();

#endif // LFP_I2C
