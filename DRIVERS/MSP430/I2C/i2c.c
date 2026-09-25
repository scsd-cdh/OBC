
#include "i2c.h"
#include "utils.h"
#include "lfp.h"
#include "lfp/stream.h"

static i2c_ctx_t s_ctx;

// FIXME: We may want two seperate buffers for main and redundant
static volatile uint8_t* p_txbuf; // this is redirected to a static buffer in whichever slave application
static volatile uint8_t s_tx_idx = 0;
static volatile uint16_t s_msg_len = 0;

void i2c_init_registers(uint8_t module_mask, uint8_t slave_addr)
{
    if (module_mask & I2C_MODULE_UCB0) {
        UCB0CTLW0 = UCSWRST;                         // Software reset enabled
        UCB0CTLW0 |= UCMODE_3 | UCSYNC;              // I2C mode, sync mode
        UCB0I2COA0 = slave_addr | UCOAEN;            // Own Address and enable
        UCB0CTLW0 &= ~UCSWRST;                       // clear reset register

        UCB0IE |= UCSTTIE;                           // Enable START interrupt
        UCB0IE |= UCRXIE;                            // Enable RX interrupt
        UCB0IE |= UCTXIE;                            // Enable TX interrupt
    }

    // Redundant i2c
#if defined (__MSP430FR5989__) || (__MSP430FR6989__)
    if (module_mask & I2C_MODULE_UCB1) {
        UCB1CTLW0 = UCSWRST;
        UCB1CTLW0 |= UCMODE_3 | UCSYNC;
        UCB1I2COA0 = slave_addr | UCOAEN;
        UCB1CTLW0 &= ~UCSWRST;

        UCB1IE |= UCSTTIE;
        UCB1IE |= UCRXIE;
        UCB1IE |= UCTXIE;
    }
#endif
}

void i2c_init(uint8_t module_mask, i2c_ctx_t* ctx)
{
    s_ctx.i2c_rx_cb = ctx->i2c_rx_cb;
    s_ctx.i2c_tx_cb = ctx->i2c_tx_cb;
    s_ctx.i2c_stt_cb = ctx->i2c_stt_cb;
    s_ctx.slave_addr = ctx->slave_addr;

    i2c_init_registers(module_mask, ctx->slave_addr);
}

// NOTE: this is a tad dangerous since we trust the user to pass in memory that is allocated and will stay allocated
int16_t i2c_set_txbuf(volatile uint8_t* data, uint8_t size)
{
    if (s_ctx.i2c_tx_cb != NULL ) {
        return -1;
    }

    s_tx_idx = 0;
    s_msg_len = size;
    p_txbuf = data;

    return 0; // TODO switch to project defined error flags
}

void i2c_ack(i2c_module_t module) {
    if (module == I2C_MODULE_NONE)
        return;
    if (module == I2C_MODULE_UCB0)
        UCB0CTLW0 &= ~UCTXNACK;
#if defined (__MSP430FR5989__) || (__MSP430FR6989__)
    if (module == I2C_MODULE_UCB1)
        UCB1CTLW0 &= ~UCTXNACK;
#endif
}

void i2c_nack(i2c_module_t module) {
    if (module == I2C_MODULE_NONE)
        return;
    if (module == I2C_MODULE_UCB0)
        UCB0CTLW0 |= UCTXNACK;
#if defined (__MSP430FR5989__) || (__MSP430FR6989__)
    if (module == I2C_MODULE_UCB1)
        UCB1CTLW0 |= UCTXNACK;
#endif
}

void i2c_write(i2c_module_t module, uint8_t byte) {
    if (module == I2C_MODULE_NONE)
        return;
    if (module == I2C_MODULE_UCB0)
        UCB0TXBUF = byte;
#if defined (__MSP430FR5989__) || (__MSP430FR6989__)
    if (module == I2C_MODULE_UCB1)
        UCB1TXBUF = byte;
#endif
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
  switch(__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG))
  {
    case USCI_I2C_UCSTTIFG:                 // Vector 6: STTIFG
        if (s_ctx.i2c_stt_cb != NULL)
            s_ctx.i2c_stt_cb(I2C_MODULE_UCB0);
        break;
    case USCI_I2C_UCRXIFG0:                 // Vector 22: RXIFG0  -> Receive one byte from MASTER (SAMV71)
        // callback for processing incoming packet from master. Calls lfp_stream_update. This cannot be inlined as it is
        s_ctx.i2c_rx_cb(UCB0RXBUF);
        break;
    case USCI_I2C_UCTXIFG0:                 // Vector 24: TXIFG0  -> Send one byte to MASTER (SAMV71)
        if (s_ctx.i2c_tx_cb != NULL) {
            s_ctx.i2c_tx_cb(I2C_MODULE_UCB0);
        } else {
            UCB0TXBUF = p_txbuf[s_tx_idx];
            s_tx_idx = (s_tx_idx + 1) % s_msg_len;
        }
        break;

    default:
        break;
  }
}

#if defined (__MSP430FR5989__) || (__MSP430FR6989__)
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B1_VECTOR
__interrupt void USCI_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B1_VECTOR))) USCI_B1_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCB1IV, USCI_I2C_UCBIT9IFG))
  {
    case USCI_I2C_UCSTTIFG:                 // Vector 6: STTIFG
        if (s_ctx.i2c_stt_cb != NULL)
            s_ctx.i2c_stt_cb(I2C_MODULE_UCB1);
        break;
    case USCI_I2C_UCRXIFG0:                 // Vector 22: RXIFG0  -> Receive one byte from MASTER (SAMV71)
        // callback for processing incoming packet from master. Calls lfp_stream_update. This cannot be inlined as it is
        s_ctx.i2c_rx_cb(UCB1RXBUF);
        break;
    case USCI_I2C_UCTXIFG0:                 // Vector 24: TXIFG0  -> Send one byte to MASTER (SAMV71)
        if (s_ctx.i2c_tx_cb != NULL) {
            s_ctx.i2c_tx_cb(I2C_MODULE_UCB1);
        } else {
            UCB1TXBUF = p_txbuf[s_tx_idx];
            s_tx_idx = (s_tx_idx + 1) % s_msg_len;
        }
        break;

    default:
        break;
  }
}
#endif
