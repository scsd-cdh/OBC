#include "driverlib.h"
#include <stddef.h>
#include "MRAM_SPI.h"

#define READ_DEVICE_ID_CMD      0x9F  // Command to read device ID (typical for MRAM)
#define READ_UNIQUE_ID_CMD      0x4C  // Command to read unique ID (typical for MRAM)
#define READ_MEMORY_ARRAY       0x03  // Command to Read Memory Array (typical for MRAM)
#define WRITE_MEMORY_ARRAY      0x02  // Command to Write Memory Array (typical for MRAM)
#define WRITE_MEMORY_ENABLE     0x06  // Command to Write Memory Enable (typical for MRAM)
#define WRITE_MEMORY_DISABLE    0x04  // Command to Write Memory Disable (typical for MRAM)
#define RX_BUFFER_SIZE          128

static volatile uint8_t rxData = 0;
// File constrained global for now. We may want a context struct in the future for encapsulation
static uint16_t SPI_CS_pin = 0;

void CS_LOW()
{
    GPIO_setOutputLowOnPin((SPI_CS_pin >> 8), (SPI_CS_pin & 0xFF));
}

void CS_HIGH()
{
    GPIO_setOutputHighOnPin((SPI_CS_pin >> 8), (SPI_CS_pin & 0xFF));
}

uint8_t SPI_transfer(uint8_t byte)
{
    while (!EUSCI_B_SPI_getInterruptStatus(EUSCI_B0_BASE, EUSCI_B_SPI_TRANSMIT_INTERRUPT));
    EUSCI_B_SPI_transmitData(EUSCI_B0_BASE, byte);
    __bis_SR_register(LPM0_bits + GIE); // enable interrupts and put in low power mode

    return rxData;
}

// TODO: Try to get the compiler to not compile when 'mode' out of bounds
// Add pins as params as well
void SPI_init(uint32_t clockSpeed, SPI_Mode mode, uint16_t CS_pin, uint16_t SCLK_pin, uint16_t MOSI_pin, uint16_t MISO_pin)
{
    SPI_CS_pin = CS_pin;
    //Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    /*
     * Select Port 1
     * Set Pin 0 as output
     */
   GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    /*
     * Select Port 1
     * Set Pin 0 to output Low.
     */
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    uint16_t CS_port = CS_pin >> 8;
    uint16_t CS_pinNumber = CS_pin & 0xFF;
    GPIO_setAsOutputPin(CS_port, CS_pinNumber);  // CS pin
    CS_HIGH();  // Initialize CS pin high

    //Set DCO frequency to max DCO setting
    CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_3);
    //Select DCO as the clock source for SMCLK with no frequency divider
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /*
     * Select Port 
     * Set Pin to input Secondary Module Function, (UCB0CLK).
     */
    uint16_t SCLK_port = SCLK_pin >> 8;
    uint16_t SCLK_pinNumber = SCLK_pin & 0xFF;
    GPIO_setAsPeripheralModuleFunctionInputPin(SCLK_port, SCLK_pinNumber, GPIO_SECONDARY_MODULE_FUNCTION);

    /*
     * Select Ports 
     * Set Pins to input Secondary Module Function, (UCB0TXD/UCB0SIMO, UCB0RXD/UCB0SOMI).
     */
    uint16_t MOSI_port = MOSI_pin >> 8;
    uint16_t MOSI_pinNumber = MOSI_pin & 0xFF;
    GPIO_setAsPeripheralModuleFunctionInputPin(MOSI_port, MOSI_pinNumber, GPIO_SECONDARY_MODULE_FUNCTION);

    uint16_t MISO_port = MISO_pin >> 8;
    uint16_t MISO_pinNumber = MISO_pin & 0xFF;
    GPIO_setAsPeripheralModuleFunctionInputPin(MISO_port, MISO_pinNumber, GPIO_SECONDARY_MODULE_FUNCTION);


    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

    //Initialize Master
    EUSCI_B_SPI_initMasterParam param = {0};
    param.selectClockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK;
    param.clockSourceFrequency = CS_getSMCLK();
    param.desiredSpiClock = clockSpeed;
    param.msbFirst = EUSCI_B_SPI_MSB_FIRST;
    switch (mode) {
        case SPI_MODE_ZERO:
            param.clockPhase = EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT;
            param.clockPolarity = EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW;
            break;
        // TODO: Handle other cases. It's just a little confusing with how clockPhase works
        default: break;
    }

    param.spiMode = EUSCI_B_SPI_3PIN;
    EUSCI_B_SPI_initMaster(EUSCI_B0_BASE, &param);

    //Enable SPI module
    EUSCI_B_SPI_enable(EUSCI_B0_BASE);

    EUSCI_B_SPI_clearInterrupt(EUSCI_B0_BASE, EUSCI_B_SPI_RECEIVE_INTERRUPT);
    // Enable USCI_B0 RX interrupt
    EUSCI_B_SPI_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_SPI_RECEIVE_INTERRUPT);
}

void MRAM_writeMemoryEn()
{
    CS_LOW();
    SPI_transfer(WRITE_MEMORY_ENABLE);
    CS_HIGH();
}

// NOTE: deviceId might be able to be a uint32_t, but I'm finding it confusing at the moment how the MSP handles uint32_t.
// Potential refactor in the future...
void MRAM_readDeviceId(uint8_t deviceId[4])
{
    CS_LOW();
    SPI_transfer(READ_DEVICE_ID_CMD);
  
    // Read the 4-byte response (32-bit Device ID register)
    uint8_t i;
    for (i = 0; i < 4; ++i) {
        deviceId[i] = SPI_transfer(0x00); // Send dummy byte to read each byte of the ID
    }

    CS_HIGH();
}

uint8_t MRAM_readMemoryArray(uint8_t addr[3])
{
    CS_LOW(); // Select MRAM device
    SPI_transfer(READ_MEMORY_ARRAY); // Send the Memory Array read command

    // Address
    SPI_transfer(addr[0]);
    SPI_transfer(addr[1]);
    SPI_transfer(addr[2]);

    uint8_t memoryArray = SPI_transfer(0x00);
    CS_HIGH();

    return memoryArray;
}

// NOTE: Same idea here, we might want to condense addr and value into one uint32_t if allowed
// Just don't totally trust it atm
void MRAM_writeMemoryArray(uint8_t addr[3], uint8_t value)
{
    CS_LOW(); // Select MRAM device

    SPI_transfer(WRITE_MEMORY_ARRAY);
    SPI_transfer(addr[0]);
    SPI_transfer(addr[1]);
    SPI_transfer(addr[2]);
    SPI_transfer(value);

    CS_HIGH();
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_B0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_B0_VECTOR)))
#endif
void USCI_B0_ISR(void)
{
    switch (__even_in_range(UCB0IV, USCI_SPI_UCTXIFG))
    {
        case USCI_SPI_UCRXIFG:      // UCRXIFG
            rxData = EUSCI_B_SPI_receiveData(EUSCI_B0_BASE);
            // Delay between transmissions for slave to process information
            __delay_cycles(40);
            __bic_SR_register_on_exit(LPM0_bits);
            break;
        default:
            break;
    }
}
