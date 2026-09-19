#include "lfp_i2c.h"

// we might want to keep more than one buffer, one for main, one for redundant
static volatile uint8_t* p_txbuf = NULL;
static uint16_t s_txbuf_len = 0;
static uint16_t s_txidx = 0;

// state
static lfp_i2c_state_t s_state;

// lfp stream context 
static lfp_stream_ctx_t* p_ctx;

// lfp callback predefines
static void i2c_tx_cb(i2c_module_t module);
static void i2c_stt_cb(i2c_module_t module);
static void i2c_tx_cb(i2c_module_t module);

static void i2c_stt_cb(i2c_module_t module) {
    if (s_state == I2C_SLAVE_STATE_PROCESSING) {
        i2c_nack(module);
    }

    // Roll back 1 byte on start to transmit the byte that was missed during the last transmission
    // USCI_B triggers TXIFG0 while previous byte being clocked out (as soon as UCB0TXBUF is empty). If master asks for a header, the first byte of the following body will
    // therefore loaded into UCB0TXBUF and s_tx_id will be incremented. Master sending stop condition discards this byte in UCB0TXBUF
    // https://e2e.ti.com/support/microcontrollers/msp-low-power-microcontrollers-group/msp430/f/msp-low-power-microcontroller-forum/209820/msp430-i2c-slave-transmit
    if (s_txidx > 0 && s_txidx <= s_txbuf_len) {
        s_txidx--;
    }
}

static void i2c_tx_cb(i2c_module_t module) {
    if (s_state != I2C_SLAVE_STATE_RESPONSE) { // Unless we are in response state, just return 0xFF
        i2c_write(module, 0xFF);
    } else if (!p_txbuf || s_txidx >= s_txbuf_len) { // if we are asked for more bytes than we have, send 0xFF. This should really never happen
        // If we are at the byte after the last byte, go one over to make sure we dont roll back on start
        // (by now the controller has received atleast one 0xFF) -- why?
        if (s_txidx == s_txbuf_len) {
            s_txidx++;
        }
        i2c_write(module, 0xFF);
    } else {
        i2c_write(module, p_txbuf[s_txidx++]);
    }
}

static void i2c_rx_cb(uint8_t data) {
    lfp_stream_update(p_ctx, data);
}

void lfp_i2c_init(lfp_stream_ctx_t * p_lfp_ctx, i2c_module_t module_mask, uint8_t slave_addr) {

    p_ctx = p_lfp_ctx;

    i2c_ctx_t i2c_ctx = {
        .i2c_stt_cb = i2c_stt_cb,
        .i2c_tx_cb = i2c_tx_cb,

        .i2c_rx_cb = i2c_rx_cb,
        .slave_addr = slave_addr,
    };
    i2c_init(module_mask, &i2c_ctx);
}

void lfp_i2c_transition(lfp_i2c_state_t state) {
    // naive for now
    if (state == I2C_SLAVE_STATE_REQUEST) {
        // discard tx buffer and reset
        p_txbuf = NULL;
        s_txidx  = 0;
        s_txbuf_len  = 0;
    }

    // NACK if we are in processing stage
    if (state == I2C_SLAVE_STATE_PROCESSING) {
        i2c_nack(I2C_MODULE_UCB0);
#if defined (__MSP430FR5989__) || (__MSP430FR6989__)
        i2c_nack(I2C_MODULE_UCB1);
#endif
    } else {
        i2c_ack(I2C_MODULE_UCB0);
#if defined (__MSP430FR5989__) || (__MSP430FR6989__)
        i2c_ack(I2C_MODULE_UCB1);
#endif
    }

    s_state = state;
}

// Which buffer is to be sent over i2c
int16_t lfp_i2c_set_txbuf(volatile uint8_t* data, uint8_t size) {
    int res = i2c_set_txbuf(data, size);
    if (res != 0) 
        return res;
    // We're ready to start transmitting data
    lfp_i2c_transition(I2C_SLAVE_STATE_RESPONSE);
    return res; 
}

lfp_i2c_state_t lfp_i2c_state() {
    return s_state;
}
