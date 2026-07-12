
#include "i2c.h"
#include "utils.h"

static i2c_ctx_t s_ctx;

static volatile uint8_t* p_txbuf = NULL; // this is redirected to a static buffer in whichever slave application
static volatile uint8_t s_tx_idx = 0;
static volatile uint16_t s_msg_len = 0;

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
    s_ctx.i2c_tx_byte_cb = ctx->i2c_tx_byte_cb;
    i2c_init_registers(ctx->slave_addr);
}

// NOTE: this is a tad dangerous since we trust the user to pass in memory that is allocated and will stay allocated
int16_t i2c_tx_own_buf(volatile uint8_t* data, uint8_t size)
{
    // We are not allowed to call this function if we have registered a per byte tx callback
    if (s_ctx.i2c_tx_byte_cb != NULL) {
        return -1;     
    }
    s_tx_idx = 0;
    s_msg_len = size;
    p_txbuf = data; 

    return 0; // TODO switch to project defined error flags
}

void i2c_send_nack(void) 
{
    UCB0CTLW0 |= UCTXNACK;
}

void i2c_clear_nack(void)
{
    UCB0CTLW0 &= ~UCTXNACK;
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
        s_ctx.i2c_start_cond_cb();
        break;
    case USCI_I2C_UCSTPIFG:  break;         // Vector 8: STPIFG
    case USCI_I2C_UCRXIFG3:  break;         // Vector 10: RXIFG3
    case USCI_I2C_UCTXIFG3:  break;         // Vector 12: TXIFG3
    case USCI_I2C_UCRXIFG2:  break;         // Vector 14: RXIFG2
    case USCI_I2C_UCTXIFG2:  break;         // Vector 16: TXIFG2
    case USCI_I2C_UCRXIFG1:  break;         // Vector 18: RXIFG1
    case USCI_I2C_UCTXIFG1:  break;         // Vector 20: TXIFG1
    case USCI_I2C_UCRXIFG0:                 // Vector 22: RXIFG0  -> Receive one byte from MASTER (SAMV71)
        // callback for processing incoming packet from master. This cannot be inlined as it is
        s_ctx.i2c_rx_cb(UCB0RXBUF);
        break;
    case USCI_I2C_UCTXIFG0:                 // Vector 24: TXIFG0  -> Send one byte to MASTER (SAMV71)
        if (s_ctx.i2c_tx_byte_cb != NULL) {
            s_ctx.i2c_tx_byte_cb(&UCB0TXBUF);
        } else {
            s_tx_idx = s_tx_idx % s_msg_len;
            UCB0TXBUF = p_txbuf[s_tx_idx]; 
        }
        break;
                          
    default: 
        break;
  }
}

