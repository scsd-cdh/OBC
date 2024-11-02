//******************************************************************************
//   MSP430FR59xx Demo - eUSCI_B0, I2C Slave multiple byte TX/RX
//
//   Description: I2C master communicates to I2C slave sending and receiving
//   3 different messages of different length. (This is the slave code). The
//   slave will be in LPM0 mode, waiting for the master to initiate the
//   communication. The slave will send/receive bytes based on the master's
//   request. The slave will handle I2C bytes sent/received using the
//   I2C interrupt.
//   ACLK = NA, MCLK = SMCLK = DCO 16MHz.
//
//                                     /|\ /|\
//                   MSP430FR5969      4.7k |
//                 -----------------    |  4.7k
//            /|\ |             P1.7|---+---|-- I2C Clock (UCB0SCL)
//             |  |                 |       |
//             ---|RST          P1.6|-------+-- I2C Data (UCB0SDA)
//                |                 |
//                |                 |
//                |                 |
//                |                 |
//                |                 |
//                |                 |
//
//   Nima Eskandari
//   Texas Instruments Inc.
//   April 2017
//   Built with CCS V7.0
//******************************************************************************
#include "MSP430_I2C.h"


void I2C_Slave_ProcessCMD(uint8_t cmd)
{
    // Reset these values in order to receive a new command
    ReceiveIndex = 0;
    TransmitIndex = 0;
    RXByteCtr = 0;
    TXByteCtr = 0;

    switch (cmd)   // For each command, set the board state, set it to data recv mode, set the expected bytes counter, switch interrupts
    {
        case (SYSTEM_STATUS_ID):
            app = system_status;      // Change the board state
            SlaveMode = RX_DATA_MODE;
            RXByteCtr = SYSTEM_STATUS_CMD_LEN;
            UCB0IE &= ~UCTXIE;                       // Disable TX interrupt
            UCB0IE |= UCRXIE;                        // Enable RX interrupt
            break;
        case (HEALTH_CHECK_ID):
            app = health_check;
            SlaveMode = RX_DATA_MODE;
            RXByteCtr = HEALTH_CHECK_CMD_LEN;
            UCB0IE &= ~UCTXIE;                       // Disable TX interrupt
            UCB0IE |= UCRXIE;                        // Enable RX interrupt
            break;
        case (REBOOT_ID):
            app = reboot;
            SlaveMode = RX_DATA_MODE;
            RXByteCtr = REBOOT_CMD_LEN;
            UCB0IE &= ~UCTXIE;                       // Disable TX interrupt
            UCB0IE |= UCRXIE;                        // Enable RX interrupt
            break;
        case (CONVERTER_MONITOR_ID):
            app = converter_monitor;
            SlaveMode = RX_DATA_MODE;
            RXByteCtr = CONVERTER_MONITOR_CMD_LEN;
            UCB0IE &= ~UCTXIE;                       // Disable TX interrupt
            UCB0IE |= UCRXIE;                        // Enable RX interrupt
            break;
        case (TELECOMMAND_ACK_ID):
            app = telecom_acknowledge;
            SlaveMode = RX_DATA_MODE;
            RXByteCtr = TELECOMMAND_ACK_CMD_LEN;
            UCB0IE &= ~UCTXIE;                       // Disable TX interrupt
            UCB0IE |= UCRXIE;                        // Enable RX interrupt
            break;
        case (BINGO_BONGO):
            SlaveMode = RX_DATA_MODE;
            TXByteCtr = 10;
            RXByteCtr = 10;
            UCB0IE &= ~UCTXIE;                       // Disable TX interrupt
            UCB0IE |= UCRXIE;                        // Enable RX interrupt
            break;
        default:  // Unrecognized command
            __no_operation();
            break;
    }
}

// When all bytes are received, copy the args array
void I2C_Slave_TransactionDone(uint8_t cmd)
{
    switch (cmd)
    {
        case (SYSTEM_STATUS_ID):
            CopyArray(ReceiveBuffer, SystemStatusBuf, SYSTEM_STATUS_CMD_LEN);
            break;
        case (HEALTH_CHECK_ID):
            CopyArray(ReceiveBuffer, HealthCheckBuf, HEALTH_CHECK_CMD_LEN);
            break;
        case (REBOOT_ID):
            CopyArray(ReceiveBuffer, RebootBuf, REBOOT_CMD_LEN);
            break;
        case (CONVERTER_MONITOR_ID):
            CopyArray(ReceiveBuffer, ConverterMonitorBuf, CONVERTER_MONITOR_CMD_LEN);
            break;
        case (TELECOMMAND_ACK_ID):
            CopyArray(ReceiveBuffer, TelecommandAckBuf, TELECOMMAND_ACK_CMD_LEN);
            break;
        case (BINGO_BONGO):
            SlaveMode = TX_DATA_MODE;
            CopyArray(ReceiveBuffer, TransmitBuffer, 10);
        default:
            __no_operation();
            break;
    }
}

void CopyArray(uint8_t *source, uint8_t *dest, uint8_t count)
{
    uint8_t copyIndex = 0;
    for (copyIndex = 0; copyIndex < count; copyIndex++)
    {
        dest[copyIndex] = source[copyIndex];
    }
}


//******************************************************************************
// Device Initialization *******************************************************
//******************************************************************************

void initGPIO()
{
    // Configure GPIO
    P1OUT &= ~BIT0;                           // Clear P1.0 output latch
    P1DIR |= BIT0;                            // For LED
    P1SEL1 |= BIT6 | BIT7;                    // I2C pins
    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;
}

void initI2C()
{
    UCB0CTLW0 = UCSWRST;                      // Software reset enabled
    UCB0CTLW0 |= UCMODE_3 | UCSYNC;           // I2C mode, sync mode
    UCB0I2COA0 = SLAVE_ADDR | UCOAEN;;        // Own Address and enable
    UCB0CTLW0 &= ~UCSWRST;                    // clear reset register
    UCB0IE |= UCRXIE + UCSTPIE;
}


void initClockTo16MHz()
{
    // Configure one FRAM waitstate as required by the device datasheet for MCLK
    // operation beyond 8MHz _before_ configuring the clock system.
    FRCTL0 = FRCTLPW | NWAITS_1;

    // Clock System Setup
    CSCTL0_H = CSKEY >> 8;                    // Unlock CS registers
    CSCTL1 = DCORSEL | DCOFSEL_4;             // Set DCO to 16MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers

    CSCTL0_H = 0;                             // Lock CS registerss
}

//******************************************************************************
// Main ************************************************************************
// Enters LPM0 and waits for I2C interrupts. The data sent from the master is  *
// then interpreted and the device will respond accordingly                    *
//******************************************************************************


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    initClockTo16MHz();
    initGPIO();
    initI2C();

    while(1){
        begin_state_machine();
    }
	return 0;
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
    case USCI_I2C_UCNACKIFG:                // Vector 4: NACKIFG
      break;
    case USCI_I2C_UCSTTIFG:  break;         // Vector 6: STTIFG
    case USCI_I2C_UCSTPIFG:
        //Move this to `I2C_Slave_TransactionDone` function
        if(ReceiveRegAddr == BINGO_BONGO)
        {
            CopyArray(ReceiveBuffer, TransmitBuffer, TYPE_3_LENGTH);
            TransmitIndex = 0;
        }

        UCB0IFG &= ~(UCTXIFG0);
        break;         // Vector 8: STPIFG
    case USCI_I2C_UCRXIFG3:  break;         // Vector 10: RXIFG3
    case USCI_I2C_UCTXIFG3:  break;         // Vector 12: TXIFG3
    case USCI_I2C_UCRXIFG2:  break;         // Vector 14: RXIFG2
    case USCI_I2C_UCTXIFG2:  break;         // Vector 16: TXIFG2
    case USCI_I2C_UCRXIFG1:  break;         // Vector 18: RXIFG1
    case USCI_I2C_UCTXIFG1:  break;         // Vector 20: TXIFG1
    case USCI_I2C_UCRXIFG0:                 // Vector 22: RXIFG0
        rx_val = UCB0RXBUF;
        switch (SlaveMode)
        {
          case (RX_REG_ADDRESS_MODE):
              ReceiveRegAddr = rx_val;
              I2C_Slave_ProcessCMD(ReceiveRegAddr);
              break;
          case (RX_DATA_MODE):
              ReceiveBuffer[ReceiveIndex++] = rx_val;
              RXByteCtr--;
              if (RXByteCtr == 0)
              {
                  //Done Receiving MSG
                  SlaveMode = RX_REG_ADDRESS_MODE;
//                  UCB0IE &= ~(UCTXIE);
                  UCB0IE |= UCTXIE;
                  UCB0IE |= UCRXIE;                          // Enable RX interrupt
                  I2C_Slave_TransactionDone(ReceiveRegAddr);
              }
              break;

          default:
              __no_operation();
              break;
        }
        break;
    case USCI_I2C_UCTXIFG0:                 // Vector 24: TXIFG0
        switch (SlaveMode)
        {
          case (TX_DATA_MODE):
              UCB0TXBUF = TransmitBuffer[TransmitIndex++];
              TXByteCtr--;
              if (TXByteCtr == 0)
              {
                  //Done Transmitting MSG
                  SlaveMode = RX_REG_ADDRESS_MODE;
                  UCB0IE &= ~(UCTXIE);
                  UCB0IE |= UCRXIE;                          // Enable RX interrupt
                  I2C_Slave_TransactionDone(ReceiveRegAddr);
              }
              break;
          default: // Returns the Receive Buffer
              UCB0TXBUF = TransmitBuffer[TransmitIndex++];
              TXByteCtr--;
              if(TXByteCtr == 0)
              {
                  //Done Transmitting MSG
                  SlaveMode = RX_REG_ADDRESS_MODE;
                  UCB0IE &= ~(UCTXIE);

                  UCB0IE |= UCRXIE;                          // Enable RX interrupt
                  I2C_Slave_TransactionDone(ReceiveRegAddr);
              }
              break;
        }
        break;                      // Interrupt Vector: I2C Mode: UCTXIFG
    default: break;
  }
}

//******************************************************************************
// State Machine ***************************************************************
//******************************************************************************

void begin_state_machine()
{

    switch(app){

        /* Idle state */
        case idle:
            // Resume LPM after any command is executed
            __bis_SR_register(LPM0_bits + GIE);  // Enter LPM with interrupts
            break;

        /* Modes used for each command */
        case system_status:
            setup_system_status_test(SystemStatusRespBuf,SystemStatusBuf);
            updateTransmissionBuffer(SystemStatusRespBuf, SYSTEM_STATUS_RESP_LEN);
            app = idle;

            // GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);  // Green LED on
            // GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);

            break;
        case health_check:
            setup_health_check_test(HealthCheckRespBuf, HealthCheckBuf);
            updateTransmissionBuffer(HealthCheckRespBuf, HEALTH_CHECK_RESP_LEN);
            app = idle;

            //ToggleGPIO(1, 0, 0); // Port 0, Pin 1, set to low  -> Green LED off
            //ToggleGPIO(4, 6, 1); // Port 4, Pin 6, set to high -> Red LED on

            //
            // GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);  // Green LED off
            // GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
            // GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN6);  // Red LED on
            // GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN6);
            //

            break;
        case reboot:
            // FUNCTION CALL TO REBOOT GOES HERE -> Response?

            updateTransmissionBuffer(RebootRespBuf, REBOOT_RESP_LEN);
            app = idle;
            break;
        case converter_monitor:
            // FUNCTION CALL GOES HERE fills ConverterMonitorRespBuf (len = 3)
            setup_converter_monitor_test(ConverterMonitorRespBuf);
            updateTransmissionBuffer(ConverterMonitorRespBuf, CONVERTER_MONITOR_RESP_LEN);
            app = idle;

            // GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN6);  // Red and Green LEDs on
            // GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN6);
            // GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
            // GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);

            break;
        case telecom_acknowledge:
            setup_telecom_acknowledge_test(TelecommandAckRespBuf);
            updateTransmissionBuffer(TelecommandAckRespBuf, TELECOMMAND_ACK_RESP_LEN);
            app = idle;

            // GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN6);  // Flash both LEDs
            // GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN6);
            // GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
            // GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);

            break;
    }

}

void updateTransmissionBuffer(uint8_t *inputBuffer, uint8_t respLength) 
{
    CopyArray(inputBuffer, TransmitBuffer, respLength);  // Copy response to TransmitBuffer
    TXByteCtr = respLength;
    SlaveMode = TX_DATA_MODE;  // Switch to transfer mode
    UCB0IE |= UCRXIE;          // Enable TX interrupt
}

//******************************************************************************
// Test Functions **************************************************************
//******************************************************************************


void setup_system_status_test(uint8_t *SystemStatusRespBuf, uint8_t *SystemStatusBuf)
{
  // FUNCTION CALL TO RETRIEVE SYSTEM STATUS GOES HERE -> fills SystemStatusRespBuf (len=5)
  SystemStatusRespBuf[0] = 'S';
  SystemStatusRespBuf[1] = 'y';
  SystemStatusRespBuf[2] = 's';
  SystemStatusRespBuf[3] = SystemStatusBuf[0];  // Whichever byte was sent as an arg
  SystemStatusRespBuf[4] = SystemStatusBuf[1];
}

void setup_health_check_test(uint8_t *HealthCheckRespBuf, uint8_t *HealthCheckBuf) {
    // Fills HealthCheckRespBuf (len = 16)
    const char testMessage[] = "TestingHealth";
    int i;
    for (i = 0; i < 12; i++) {
        HealthCheckRespBuf[i] = testMessage[i];
    }
    HealthCheckRespBuf[13] = HealthCheckBuf[0];
    HealthCheckRespBuf[14] = HealthCheckBuf[1];
    HealthCheckRespBuf[15] = '2';
}

void setup_converter_monitor_test(uint8_t *ConverterMonitorRespBuf) {
    // Fills ConverterMonitorRespBuf (len = 3)
    ConverterMonitorRespBuf[0] = 'H';
    ConverterMonitorRespBuf[1] = 'i';
    ConverterMonitorRespBuf[2] = '4';
}

void setup_telecom_acknowledge_test(uint8_t *TelecommandAckRespBuf) {
    // Fills TelecommandAckRespBuf (len = 2)
    TelecommandAckRespBuf[0] = 'O';
    TelecommandAckRespBuf[1] = '5';
}