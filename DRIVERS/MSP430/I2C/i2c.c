
#include "i2c.h"
#include "utils.h"

typedef enum eI2C_Mode {
    I2C_IDLE_MODE,          /**< I2C is idle */
    I2C_RX_MODE,            /**< Receiving Data */
    I2C_TX_MODE,            /**< Transmiting Data */
    I2C_MODE_MAX
} eI2C_Mode_t;

typedef struct sI2cCtxPriv
{
    void (*Rx_Proc_Data)(uint8_t data);
    eI2C_Mode_t i2c_mode;
} sI2cCtxPriv;

/** I2C SlaveMode - Tracks the current mode of the I2C software state machine */
static sI2cCtxPriv i2cSlaveCtx = {
    .Rx_Proc_Data = NULL,
    .i2c_mode = I2C_IDLE_MODE
};

uint8_t ReceiveBuffer[MAX_BUFFER_SIZE] = {0};
uint8_t ReceiveIndex = 0;

uint8_t TransmitBuffer[MAX_BUFFER_SIZE] = {0};
uint8_t TransmitIndex = 0;

void initI2C(sI2cConfigCb_t* cb_config)
{
    UCB0CTLW0 = UCSWRST;                      // Software reset enabled
    UCB0CTLW0 |= UCMODE_3 | UCSYNC;           // I2C mode, sync mode
    UCB0I2COA0 = cb_config->slave_addr | UCOAEN; // Own Address and enable
    UCB0CTLW0 &= ~UCSWRST;                    // clear reset register

    UCB0IE |= UCSTPIE;                         // Enable STOP interrupt
    UCB0IE |= UCRXIE;                          // Enable RX interrupt
    UCB0IE |= UCTXIE;                          // Enable TX interrupt
    
    i2cSlaveCtx.Rx_Proc_Data = cb_config->Rx_Proc_Data;
    i2cSlaveCtx.i2c_mode = I2C_IDLE_MODE;
}

int16_t transmitI2C(const uint8_t* data, uint8_t size)
{
    // Copy response to TransmitBuffer
    CopyArray((uint8_t*)data, TransmitBuffer, MIN(size, MAX_BUFFER_SIZE));

    i2cSlaveCtx.i2c_mode = I2C_TX_MODE;

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
    case USCI_I2C_UCSTTIFG:  break;         // Vector 6: STTIFG
    case USCI_I2C_UCSTPIFG:                 // Vector 8: STPIFG
        if (i2cSlaveCtx.i2c_mode == I2C_RX_MODE) {          // Recieve
            ReceiveIndex = 0;
        } else if (i2cSlaveCtx.i2c_mode == I2C_TX_MODE) {   // Transmit
            TransmitIndex = 0;
        } 
        // else {
            // TODO: throw an error. 
        // }

        // Done 
        i2cSlaveCtx.i2c_mode = I2C_IDLE_MODE;
        break;
    case USCI_I2C_UCRXIFG3:  break;         // Vector 10: RXIFG3
    case USCI_I2C_UCTXIFG3:  break;         // Vector 12: TXIFG3
    case USCI_I2C_UCRXIFG2:  break;         // Vector 14: RXIFG2
    case USCI_I2C_UCTXIFG2:  break;         // Vector 16: TXIFG2
    case USCI_I2C_UCRXIFG1:  break;         // Vector 18: RXIFG1
    case USCI_I2C_UCTXIFG1:  break;         // Vector 20: TXIFG1
    case USCI_I2C_UCRXIFG0:                 // Vector 22: RXIFG0  -> Receive one byte from MASTER (SAMV71)
        ReceiveBuffer[ReceiveIndex] = UCB0RXBUF;         // -> Get the single byte
        i2cSlaveCtx.Rx_Proc_Data(ReceiveBuffer[ReceiveIndex]);

        ReceiveIndex = (ReceiveIndex + 1) % MAX_BUFFER_SIZE;
        i2cSlaveCtx.i2c_mode = I2C_RX_MODE;
        break;
    case USCI_I2C_UCTXIFG0:                 // Vector 24: TXIFG0  -> Send one byte to MASTER (SAMV71)
        UCB0TXBUF = TransmitBuffer[TransmitIndex];

        TransmitIndex = (TransmitIndex + 1) % MAX_BUFFER_SIZE;
        i2cSlaveCtx.i2c_mode = I2C_TX_MODE;
        break;                      // Interrupt Vector: I2C Mode: UCTXIFG
    default: 
        break;
  }
}

