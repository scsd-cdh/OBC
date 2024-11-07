
#include "i2c.h"

/** Register address or command to use (determined by master) */
static uint8_t ReceiveRegAddr = 0;

void initI2C(sI2cConfigCb_t* cb_config)
{
    UCB0CTLW0 = UCSWRST;                      // Software reset enabled
    UCB0CTLW0 |= UCMODE_3 | UCSYNC;           // I2C mode, sync mode
    UCB0I2COA0 = SLAVE_ADDR | UCOAEN;;        // Own Address and enable
    UCB0CTLW0 &= ~UCSWRST;                    // clear reset register

    // UCB0IE |= UCRXIE + UCSTPIE;             // Enable STOP interrupt
    UCB0IE |= UCTXIE;                          // Enable TX interrupt
    UCB0IE |= UCRXIE;                          // Enable RX interrupt

    SlaveMode = RX_REG_ADDRESS_MODE;
    slaveFuncsCb.Rx_Proc_Cmd = cb_config->Rx_Proc_Cmd;
    slaveFuncsCb.Rx_Proc_Data = cb_config->Rx_Proc_Data;
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
  uint8_t rx_val = 0;
  switch(__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG))
  {
    case USCI_NONE:          break;         // Vector 0: No interrupts
    case USCI_I2C_UCALIFG:   break;         // Vector 2: ALIFG
    case USCI_I2C_UCNACKIFG: break;         // Vector 4: NACKIFG
    case USCI_I2C_UCSTTIFG:  break;         // Vector 6: STTIFG
    case USCI_I2C_UCSTPIFG:                 // Vector 8: STPIFG
        UCB0IFG &= ~(UCTXIFG0);  // Disable TX interrupt at the end of transmission (?)
        break;
    case USCI_I2C_UCRXIFG3:  break;         // Vector 10: RXIFG3
    case USCI_I2C_UCTXIFG3:  break;         // Vector 12: TXIFG3
    case USCI_I2C_UCRXIFG2:  break;         // Vector 14: RXIFG2
    case USCI_I2C_UCTXIFG2:  break;         // Vector 16: TXIFG2
    case USCI_I2C_UCRXIFG1:  break;         // Vector 18: RXIFG1
    case USCI_I2C_UCTXIFG1:  break;         // Vector 20: TXIFG1
    case USCI_I2C_UCRXIFG0:                 // Vector 22: RXIFG0  -> Receive one byte from MASTER (SAMV71)
        rx_val = UCB0RXBUF;         // -> Get the single byte
        switch (SlaveMode)
        {
          case (RX_REG_ADDRESS_MODE):   // If currently receiving command ID
              ReceiveRegAddr = rx_val;
              RXByteCtr = slaveFuncsCb.Rx_Proc_Cmd(ReceiveRegAddr);
              SlaveMode = RX_DATA_MODE;
              break;
          case (RX_DATA_MODE):          // If currently receiving command payload
              ReceiveBuffer[ReceiveIndex++] = rx_val;
              RXByteCtr--;
              if (RXByteCtr == 0)       // If all bytes received
              {
                  SlaveMode = RX_REG_ADDRESS_MODE;
                  slaveFuncsCb.Rx_Proc_Data(ReceiveRegAddr); // Done Receiving MSG
                //   __bic_SR_register_on_exit(CPUOFF);  // Exit LPM, TODO: check if needed
              }
              break;
          default:
              __no_operation();
              break;
        }
        break;
    case USCI_I2C_UCTXIFG0:                 // Vector 24: TXIFG0  -> Send one byte to MASTER (SAMV71)
        UCB0TXBUF = TransmitBuffer[TransmitIndex++];
        TXByteCtr--;
        if(TXByteCtr == 0)
        {
            SlaveMode = RX_REG_ADDRESS_MODE;  //Done Transmitting MSG, switch to Rx
        }
        break;                      // Interrupt Vector: I2C Mode: UCTXIFG
    default: break;
  }
}
