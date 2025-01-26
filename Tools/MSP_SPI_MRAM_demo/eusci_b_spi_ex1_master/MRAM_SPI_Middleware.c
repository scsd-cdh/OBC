#include "driverlib.h"
#include <stddef.h>
#include "MRAM_SPI_Middleware.h"

#define READ_DEVICE_ID_CMD 0x9F  // Command to read device ID (typical for MRAM)
#define READ_UNIQUE_ID_CMD 0x4C  // Command to read unique ID (typical for MRAM)
#define Read_Memory_Array 0x03  // Command to Read Memory Array (typical for MRAM)
#define Write_Memory_Array 0x02  // Command to Write Memory Array (typical for MRAM)
#define Write_Memory_Enable 0x06  // Command to Write Memory Enable (typical for MRAM)
#define Write_Memory_Disable 0x04  // Command to Write Memory Disable (typical for MRAM)
#define RX_BUFFER_SIZE 128

static volatile int8_t numBytesExpected = 0;

static volatile uint8_t* rx_buffer_ptr = NULL;

static volatile uint8_t device_id[4] = {0};
// FIXME: disgusting 
static volatile uint8_t haveSeenFirstByte = 0;
static volatile uint8_t originalNumBytes = 0;
static volatile uint8_t ISR_index = 0;

void CS_LOW()
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN3);
}

void CS_HIGH()
{
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN3);
}

// TODO: make flexible, allow different values to be set from function parameters 
void initSPI()
{
    //Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    /*
     * Select Port 1
     * Set Pin 0 as output
     */
    GPIO_setAsOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN0
    );
    /*
    * Select Port 1
    * Set Pin 0 to output Low.
    */
    GPIO_setOutputLowOnPin(
        GPIO_PORT_P1,
        GPIO_PIN0
    );

    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN3);  // CS pin
    CS_HIGH();  // Initialize CS pin high

    //Set DCO frequency to max DCO setting
    CS_setDCOFreq(CS_DCORSEL_0,CS_DCOFSEL_3);
    //Select DCO as the clock source for SMCLK with no frequency divider
    CS_initClockSignal(CS_SMCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);

    /*
    * Select Port 2
    * Set Pin 2 to input Secondary Module Function, (UCB0CLK).
    */
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P2,
        GPIO_PIN2,
        GPIO_SECONDARY_MODULE_FUNCTION
    );

    /*
    * Select Port 1
    * Set Pin 6, 7 to input Secondary Module Function, (UCB0TXD/UCB0SIMO, UCB0RXD/UCB0SOMI).
    */
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P1,
        GPIO_PIN7,
        GPIO_SECONDARY_MODULE_FUNCTION
    );

    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P1,
        GPIO_PIN6,
        GPIO_SECONDARY_MODULE_FUNCTION
    );


    /*
    * Disable the GPIO power-on default high-impedance mode to activate
    * previously configured port settings
    */
    PMM_unlockLPM5();

    //Initialize Master
    EUSCI_B_SPI_initMasterParam param = {0};
    param.selectClockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK;
    param.clockSourceFrequency = CS_getSMCLK();
    param.desiredSpiClock = 10000;
    param.msbFirst = EUSCI_B_SPI_MSB_FIRST;
    param.clockPhase = EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT;
    param.clockPolarity = EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW;
    param.spiMode = EUSCI_B_SPI_3PIN;
    EUSCI_B_SPI_initMaster(EUSCI_B0_BASE, &param);

    //Enable SPI module
    EUSCI_B_SPI_enable(EUSCI_B0_BASE);

    EUSCI_B_SPI_clearInterrupt(EUSCI_B0_BASE,
            EUSCI_B_SPI_RECEIVE_INTERRUPT);
    // Enable USCI_B0 RX interrupt
    EUSCI_B_SPI_enableInterrupt(EUSCI_B0_BASE,
        EUSCI_B_SPI_RECEIVE_INTERRUPT);
}

void writeMemoryEn(){
    CS_LOW();
    spiTransfer(Write_Memory_Enable, NULL, 0);
    CS_HIGH();
}

void spiTransfer(uint8_t cmd, uint8_t* rx_data, uint8_t rx_size)
{
    numBytesExpected = rx_size;
    rx_buffer_ptr = rx_data;

    while (!EUSCI_B_SPI_getInterruptStatus(EUSCI_B0_BASE, EUSCI_B_SPI_TRANSMIT_INTERRUPT));
    EUSCI_B_SPI_transmitData(EUSCI_B0_BASE, cmd);
    __bis_SR_register(LPM0_bits + GIE); // enable interrupts put in low power mode

    // memcpy(rx_data, RX_buffer, rx_size/* TODO MIN(RX_BUFFER_SIZE, rx_size)*/);
}

void readUniqueId(uint8_t id_buffer[4])
{
    CS_LOW();
    spiTransfer(READ_DEVICE_ID_CMD, id_buffer, 4);

    // Once we exit the interrupt we jump back here via __bic_SR_register_on_exit(LPM0_bits);
    CS_HIGH();
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_B0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_B0_VECTOR)))
#endif
void USCI_B0_ISR (void)
{
    switch (__even_in_range(UCB0IV, USCI_SPI_UCTXIFG))
    {
        case USCI_SPI_UCRXIFG:      // UCRXIFG
            if (numBytesExpected <= 0) {
                ISR_index = 0;
                __bic_SR_register_on_exit(LPM0_bits);
                return;
            }

            // //USCI_B0 TX buffer ready?
            // while (!EUSCI_B_SPI_getInterruptStatus(EUSCI_B0_BASE,
            //             EUSCI_B_SPI_TRANSMIT_INTERRUPT));

            // NOTE: we need to skip the first byte because it will always be zero
            if (haveSeenFirstByte) {
                rx_buffer_ptr[ISR_index] = EUSCI_B_SPI_receiveData(EUSCI_B0_BASE);
                numBytesExpected--;
                ISR_index++;
            }else{
                haveSeenFirstByte = 1;
            }

            //Send next value
            EUSCI_B_SPI_transmitData(EUSCI_B0_BASE, 0x00);

            //Delay between transmissions for slave to process information
            __delay_cycles(40);
            break;
        default:
            break;
    }
}
