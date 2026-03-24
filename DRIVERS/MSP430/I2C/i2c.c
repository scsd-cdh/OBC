
#include "i2c.h"
#include "utils.h"
#include "lfp.h"
#include "lfp/stream.h"

static i2c_ctx_t s_ctx;

static volatile uint8_t* p_txbuf; // this is redirected to a static buffer in whichever slave application
static volatile uint8_t s_tx_idx = 0;
static volatile uint16_t s_msg_len = 0;
static volatile i2c_slave_state_t s_current_state = I2C_SLAVE_STATE_REQUEST;

void i2c_init_registers(uint8_t slave_addr) 
{
    UCB0CTLW0 = UCSWRST;                         // Software reset enabled
    UCB0CTLW0 |= UCMODE_3 | UCSYNC;              // I2C mode, sync mode
    UCB0I2COA0 = slave_addr | UCOAEN;            // Own Address and enable
    UCB0CTLW0 &= ~UCSWRST;                       // clear reset register

    UCB0IE |= UCSTTIE;                           // Enable START interrupt
    UCB0IE |= UCRXIE;                            // Enable RX interrupt
    UCB0IE |= UCTXIE;                            // Enable TX interrupt
}

void i2c_init(i2c_ctx_t* ctx)
{
    s_ctx.slave_addr = ctx->slave_addr;
    s_ctx.i2c_rx_cb = ctx->i2c_rx_cb;
    i2c_init_registers(ctx->slave_addr);
}

i2c_slave_state_t i2c_slave_current_state() 
{
    return s_current_state;
}

// FIXME: We should only ever transition REQUEST -> PROCESSING -> RESPONSE -> REQUEST ... 
void i2c_transition(i2c_slave_state_t state)
{
    // naive for now
    if (state == I2C_SLAVE_STATE_REQUEST) {
        // discard tx buffer and reset 
        p_txbuf = NULL;
        s_tx_idx = 0;
        s_msg_len = 0;
    }

    // NACK if we are in processing stage
    if (state == I2C_SLAVE_STATE_PROCESSING) {
        UCB0CTLW0 |= UCTXNACK;
    } else {
        UCB0CTLW0 &= ~UCTXNACK;
    }
    
    s_current_state = state;
}

// NOTE: this is a tad dangerous since we trust the user to pass in memory that is allocated and will stay allocated
int16_t i2c_reassign_txbuf(volatile uint8_t* data, uint8_t size)
{
    s_tx_idx = 0;
    s_msg_len = size;
    p_txbuf = data; 
    // We're ready to start transmitting data
    i2c_transition(I2C_SLAVE_STATE_RESPONSE);

    return 0; // TODO switch to project defined error flags
}

//******************************************************************************
// I2C Interrupt ***************************************************************
//******************************************************************************

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  //Must read from UCB0RXBUF
  switch(__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG))
  {
    case USCI_NONE:          break;         // Vector 0: No interrupts
    case USCI_I2C_UCALIFG:   break;         // Vector 2: ALIFG
    case USCI_I2C_UCNACKIFG: break;         // Vector 4: NACKIFG
    case USCI_I2C_UCSTTIFG:                 // Vector 6: STTIFG
        if (s_current_state == I2C_SLAVE_STATE_PROCESSING) { 
            UCB0CTLW0 |= UCTXNACK; // NACK master writes while we're processing
        }

        // Roll back 1 byte on start to transmit the byte that was missed during the last transmission
        if (s_tx_idx > 0 && s_tx_idx <= s_msg_len) { 
            s_tx_idx--;
        }
        break;
    case USCI_I2C_UCSTPIFG:  break;         // Vector 8: STPIFG
    case USCI_I2C_UCRXIFG3:  break;         // Vector 10: RXIFG3
    case USCI_I2C_UCTXIFG3:  break;         // Vector 12: TXIFG3
    case USCI_I2C_UCRXIFG2:  break;         // Vector 14: RXIFG2
    case USCI_I2C_UCTXIFG2:  break;         // Vector 16: TXIFG2
    case USCI_I2C_UCRXIFG1:  break;         // Vector 18: RXIFG1
    case USCI_I2C_UCTXIFG1:  break;         // Vector 20: TXIFG1
    case USCI_I2C_UCRXIFG0:                 // Vector 22: RXIFG0  -> Receive one byte from MASTER (SAMV71)
        s_ctx.i2c_rx_cb(UCB0RXBUF);
        break;
    case USCI_I2C_UCTXIFG0:                 // Vector 24: TXIFG0  -> Send one byte to MASTER (SAMV71)
        if (s_current_state != I2C_SLAVE_STATE_RESPONSE) { // Unless we are in response state, just return 0xFF
            UCB0TXBUF = 0xFF;
        } else if (!p_txbuf || s_tx_idx >= s_msg_len) { // if we are asked for more bytes than we have, send 0xFF
            // If we are at the byte after the last byte, go one over to make sure we dont roll back on start 
            // (by now the controller has received atleast one 0xFF)
            if (s_tx_idx == s_msg_len) {
                s_tx_idx++;
            }
            UCB0TXBUF = 0xFF; 
        } else {
            UCB0TXBUF = p_txbuf[s_tx_idx++]; 
        }
        break;
                          
    default: 
        break;
  }
}

