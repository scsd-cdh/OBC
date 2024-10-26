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

#include <app/AppMulti.h>


// Address of this board
#define SLAVE_ADDR  0x08

// The state of this board
static volatile appStatus app;


// Buffers to hold command arguments sent from the master (to then be processed in command)
uint8_t SystemStatusBuf [SYSTEM_STATUS_LEN] = {0};
uint8_t HealthCheckBuf [HEALTH_CHECK_LEN] = {0};
uint8_t RebootBuf [REBOOT_LEN] = {0};
uint8_t ConverterMonitorBuf [CONVERTER_MONITOR_LEN] = {0};
uint8_t TelecommandAckBuf [TELECOMMAND_ACK_LEN] = {0};


// Buffers of data (filled by function calls to peripherals) to be sent to the master
uint8_t SystemStatusRespBuf [SYSTEM_STATUS_RESP_LEN] = {0};
uint8_t HealthCheckRespBuf [HEALTH_CHECK_RESP_LEN] = {0};
uint8_t RebootRespBuf [REBOOT_RESP_LEN] = {0};  // Is this necessary?
uint8_t ConverterMonitorRespBuf [CONVERTER_MONITOR_RESP_LEN] = {0};
uint8_t TelecommandAckRespBuf [TELECOMMAND_ACK_RESP_LEN] = {0};

//******************************************************************************
// General I2C State Machine
typedef enum I2C_ModeEnum{
    IDLE_MODE,
    NACK_MODE,
    TX_REG_ADDRESS_MODE,
    RX_REG_ADDRESS_MODE,
    TX_DATA_MODE,
    RX_DATA_MODE,
    SWITCH_TO_RX_MODE,
    SWITHC_TO_TX_MODE,
    TIMEOUT_MODE,
} I2C_Mode;

/* Used to track the state of the I2C state machine*/
I2C_Mode SlaveMode = RX_REG_ADDRESS_MODE;

uint8_t ReceiveRegAddr = 0;                     // The command ID to be processed
uint8_t ReceiveBuffer[MAX_BUFFER_SIZE] = {0};   // Buffer used to receive data in the ISR
uint8_t RXByteCtr = 0;                          // Number of bytes left to receive
uint8_t ReceiveIndex = 0;                       // The index of the next byte to be received in ReceiveBuffer
uint8_t TransmitBuffer[MAX_BUFFER_SIZE] = {0};  // Buffer used to transmit data in the ISR
uint8_t TXByteCtr = 0;                          // Number of bytes left to transfer
uint8_t TransmitIndex = 0;                      // The index of the next byte to be transmitted in TransmitBuffer


void initialize() {

#ifdef PDS
    app = idle;  // Initialize the state to idle
#endif
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

// Helper function to copy buffer contents to specific command buffer
void CopyArray(uint8_t *source, uint8_t *dest, uint8_t count)
{
    uint8_t copyIndex = 0;
    for (copyIndex = 0; copyIndex < count; copyIndex++)
    {
        dest[copyIndex] = source[copyIndex];
    }
}

#define CONCATENATE(prefix, num) prefix##num
#define direction(x) CONCATENATE(P, x)DIR
#define port_out(x) CONCATENATE(P, x)OUT
#define set_pin(x) CONCATENATE(BIT, x)

// Helper function to toggle GPIO
// If mode==0 then set to low, if mode==1 then set to high
void ToggleGPIO(uint8_t port, uint8_t pin, uint8_t mode) {

//    switch (pin) {
//        GPIO_CASE(1, P1OUT |= BIT0)
//        GPIO_CASE(2, P2OUT |= BIT1)
//        GPIO_CASE(3, P3OUT |= BIT2)
//        // Add more cases as needed
//        default:
//            // Handle unknown pins
//            break;
//    }
//
//
//
//    if (mode) {
//        direction(port) |= BIT0;  // Set direction of port to BIT0
//        port_out(port) |= set_pin(pin);  // Set pin of port
//    } else {
//        port_out(port) &= ~set_pin(pin);  // Reset pin of port
//    }
}


// Process the incoming command
// Once command ID is extracted, set the correct mode to receive the rest of the data
void I2C_Slave_ProcessCMD(uint8_t cmd)
{
    // Reset these values in order to receive a new command
    ReceiveIndex = 0;
    TransmitIndex = 0;
    RXByteCtr = 0;
    TXByteCtr = 0;

    switch (cmd)

    // For each command, set the board state, set it to data recv mode, set the expected bytes counter, switch interrupts

    {
        case (SYSTEM_STATUS):
            app = system_status;      // Change the board state
            SlaveMode = RX_DATA_MODE;
            RXByteCtr = SYSTEM_STATUS_LEN;
            UCB0IE &= ~UCTXIE;                       // Disable TX interrupt
            UCB0IE |= UCRXIE;                        // Enable RX interrupt
            break;
        case (HEALTH_CHECK):
            app = health_check;
            SlaveMode = RX_DATA_MODE;
            RXByteCtr = HEALTH_CHECK_LEN;
            UCB0IE &= ~UCTXIE;                       // Disable TX interrupt
            UCB0IE |= UCRXIE;                        // Enable RX interrupt
            break;
        case (REBOOT):
            app = reboot;
            SlaveMode = RX_DATA_MODE;
            RXByteCtr = REBOOT_LEN;
            UCB0IE &= ~UCTXIE;                       // Disable TX interrupt
            UCB0IE |= UCRXIE;                        // Enable RX interrupt
            break;
        case (CONVERTER_MONITOR):
            app = converter_monitor;
            SlaveMode = RX_DATA_MODE;
            RXByteCtr = CONVERTER_MONITOR_LEN;
            UCB0IE &= ~UCTXIE;                       // Disable TX interrupt
            UCB0IE |= UCRXIE;                        // Enable RX interrupt
            break;
        case (TELECOMMAND_ACK):
            app = telecom_acknowledge;
            SlaveMode = RX_DATA_MODE;
            RXByteCtr = TELECOMMAND_ACK_LEN;
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
        case (SYSTEM_STATUS):
            CopyArray(ReceiveBuffer, SystemStatusBuf, SYSTEM_STATUS_LEN);
            break;
        case (HEALTH_CHECK):
            CopyArray(ReceiveBuffer, HealthCheckBuf, HEALTH_CHECK_LEN);
            break;
        case (REBOOT):
            CopyArray(ReceiveBuffer, RebootBuf, REBOOT_LEN);
            break;
        case (CONVERTER_MONITOR):
            CopyArray(ReceiveBuffer, ConverterMonitorBuf, CONVERTER_MONITOR_LEN);
            break;
        case (TELECOMMAND_ACK):
            CopyArray(ReceiveBuffer, TelecommandAckBuf, TELECOMMAND_ACK_LEN);
            break;
        case (BINGO_BONGO):
            SlaveMode = TX_DATA_MODE;
            CopyArray(ReceiveBuffer, TransmitBuffer, 10);
        default:
            __no_operation();
            break;
    }
}

//******************************************************************************
// I2C ISR

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
        rx_val = UCB0RXBUF; // -> Get the single byte
        switch (SlaveMode)
        {
          case (RX_REG_ADDRESS_MODE):   // If currently receiving command ID
              ReceiveRegAddr = rx_val;
              I2C_Slave_ProcessCMD(ReceiveRegAddr);
              break;
          case (RX_DATA_MODE):          // If currently receiving command payload
              ReceiveBuffer[ReceiveIndex++] = rx_val;
              RXByteCtr--;
              if (RXByteCtr == 0)  // If all bytes received
              {
                  //Done Receiving MSG
                  SlaveMode = RX_REG_ADDRESS_MODE;
//                  UCB0IE &= ~(UCTXIE);
                  UCB0IE |= UCTXIE;
                  UCB0IE |= UCRXIE;                          // Enable RX interrupt
                  I2C_Slave_TransactionDone(ReceiveRegAddr);
                  __bic_SR_register_on_exit(CPUOFF);  // Exit LPM
              }
              break;
          default:
              __no_operation();
              break;
        }
        break;
    case USCI_I2C_UCTXIFG0:                 // Vector 24: TXIFG0  -> Send one byte to MASTER (SAMV71)
        switch (SlaveMode)
        {
          case (TX_DATA_MODE):      // Sending a byte
              UCB0TXBUF = TransmitBuffer[TransmitIndex++];
              TXByteCtr--;
              if (TXByteCtr == 0)  // If done sending message
              {
                  //Done Transmitting MSG
                  SlaveMode = RX_REG_ADDRESS_MODE;
                  UCB0IE &= ~(UCTXIE);                       // Disable TX interrupt
                  UCB0IE |= UCRXIE;                          // Enable RX interrupt
              }
              break;
          default:
              break;  // Unrecognized mode
        }
        break;
    default: break;
  }
}


void updateTransmissionBuffer(uint8_t *inputBuffer, uint8_t respLength) {
    CopyArray(inputBuffer, TransmitBuffer, respLength);  // Copy response to TransmitBuffer
    TXByteCtr = respLength;
    SlaveMode = TX_DATA_MODE;  // Switch to transfer mode
    UCB0IE |= UCRXIE;          // Enable TX interrupt
}


  // State machine for the board, LEDs used for illustration of state change
  void run(){

      switch(app){

          /* Idle state */
          case idle:
              // Resume LPM after any command is executed
              __bis_SR_register(LPM0_bits + GIE);  // Enter LPM with interrupts
              break;

          /* Modes used for each command */
          case system_status:
              // FUNCTION CALL TO RETRIEVE SYSTEM STATUS GOES HERE -> fills SystemStatusRespBuf (len=5)
              SystemStatusRespBuf[0] = 'S';
              SystemStatusRespBuf[1] = 'y';
              SystemStatusRespBuf[2] = 's';
              SystemStatusRespBuf[3] = SystemStatusBuf[0];  // Whichever byte was sent as an arg
              SystemStatusRespBuf[4] = SystemStatusBuf[1];

              updateTransmissionBuffer(SystemStatusRespBuf, SYSTEM_STATUS_RESP_LEN);
              app = idle;

              GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);  // Green LED on
              GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);

              break;
          case health_check:
              // FUNCTION CALL TO RETRIEVE HEALTH CHECK INFO GOES HERE -> fills HealthCheckRespBuf (len = 16)
              HealthCheckRespBuf[0] = 'T';
              HealthCheckRespBuf[1] = 'e';
              HealthCheckRespBuf[2] = 's';
              HealthCheckRespBuf[3] = 't';
              HealthCheckRespBuf[4] = 'i';
              HealthCheckRespBuf[5] = 'n';
              HealthCheckRespBuf[6] = 'g';
              HealthCheckRespBuf[7] = 'H';
              HealthCheckRespBuf[8] = 'e';
              HealthCheckRespBuf[9] = 'a';
              HealthCheckRespBuf[10] = 'l';
              HealthCheckRespBuf[11] = 't';
              HealthCheckRespBuf[12] = 'h';
              HealthCheckRespBuf[13] = HealthCheckBuf[0];  // Whichever byte was sent as an arg
              HealthCheckRespBuf[14] = HealthCheckBuf[1];  // Whichever byte was sent as an arg
              HealthCheckRespBuf[15] = '2';

              updateTransmissionBuffer(HealthCheckRespBuf, HEALTH_CHECK_RESP_LEN);
              app = idle;

              //ToggleGPIO(1, 0, 0); // Port 0, Pin 1, set to low  -> Green LED off
              //ToggleGPIO(4, 6, 1); // Port 4, Pin 6, set to high -> Red LED on

              //
              GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);  // Green LED off
              GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
              GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN6);  // Red LED on
              GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN6);
              //

              break;
          case reboot:
              // FUNCTION CALL TO REBOOT GOES HERE -> Response?

              updateTransmissionBuffer(RebootRespBuf, REBOOT_RESP_LEN);
              app = idle;
              break;
          case converter_monitor:
              // FUNCTION CALL GOES HERE fills ConverterMonitorRespBuf (len = 3)
              ConverterMonitorRespBuf[0] = 'H';
              ConverterMonitorRespBuf[1] = 'i';
              ConverterMonitorRespBuf[2] = '4';

              updateTransmissionBuffer(ConverterMonitorRespBuf, CONVERTER_MONITOR_RESP_LEN);
              app = idle;

              GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN6);  // Red and Green LEDs on
              GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN6);
              GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
              GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);

              break;
          case telecom_acknowledge:
              // FUNCTION CALL GOES HERE fills TelecommandAckRespBuf (len = 2)
              TelecommandAckRespBuf[0] = 'O';
              TelecommandAckRespBuf[1] = '5';

              updateTransmissionBuffer(TelecommandAckRespBuf, TELECOMMAND_ACK_RESP_LEN);
              app = idle;

              GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN6);  // Flash both LEDs
              GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN6);
              GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
              GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);

              break;
      }

  }
