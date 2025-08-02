#include "drivers/gpio.h"
#include "drivers/swi2c.h"
// #include "msp430fr5989.h"
#include "intrinsics.h"
#include "msp430.h"
#include "msp430fr5xx_6xxgeneric.h"
#include <msp430.h>
#include <stdint.h>
#include <drivers/drivers.h>
#include <peripherals/ads7138irter.h>
// Firmware Version
#define FIRMWARE_VERSION        1           // Maximum version is 31

// Experiment Thresholds
#define MIN_VIABLE_EXP_TEMP     5           // Celsius
#define MAX_VIABLE_EXP_TEMP     40          // Celsius
#define MIN_EXP_TEMP            20          // Celsius
#define MAX_EXP_TEMP            37          // Celsius
#define MIX_PUMP_DURATION       20         // Seconds
#define WELL_COUNT              14          // Units

// External I2C Buses
#define MAIN_I2C_SDA            GPIO_PIN6
#define MAIN_I2C_SCL            GPIO_PIN7
#define MAIN_I2C_PORT           GPIO_PORT_P1
#define MAIN_SLAVE_ADDR         0x48

#define REDUNDANT_I2C_SDA       GPIO_PIN1
#define REDUNDANT_I2C_SCL       GPIO_PIN1
#define REDUNDANT_I2C_SDA_PORT  GPIO_PORT_P3
#define REDUNDANT_I2C_SCL_PORT  GPIO_PORT_P4
#define REDUNDANT_SLAVE_ADDR    0x48

// Bipump Pinout
#define BIPUMP_Q2_PORT          GPIO_PORT_P9
#define BIPUMP_Q2_PIN           GPIO_PIN2
#define BIPUMP_Q1_PORT          GPIO_PORT_P9
#define BIPUMP_Q1_PIN           GPIO_PIN3
#define BIPUMP_Q6_PORT          GPIO_PORT_P9
#define BIPUMP_Q6_PIN           GPIO_PIN4
#define BIPUMP_Q5_PORT          GPIO_PORT_P9
#define BIPUMP_Q5_PIN           GPIO_PIN5

// Pump Pinout
#define PUMP_PORT               GPIO_PORT_P3
#define PUMP_PIN                GPIO_PIN3

// BME280 Pinout
#define BME_SDO_PORT            GPIO_PORT_P4
#define BME_SDO_PIN             GPIO_PIN3
#define BME_CS_PORT             GPIO_PORT_P1
#define BME_CS_PIN              GPIO_PIN4
#define BME_SCK_PORT            GPIO_PORT_P1
#define BME_SCK_PIN             GPIO_PIN5
#define BME_SDI_PORT            GPIO_PORT_P2
#define BME_SDI_PIN             GPIO_PIN0

// LMT01 Pinout
#define LMT01_PORT              GPIO_PORT_P2
#define LMT01_PIN               GPIO_PIN7

// Valve Pinout
#define VALVE_PORT              GPIO_PORT_P2
#define VALVE_PIN               GPIO_PIN1

// Heater Pinout
#define HEATER_PORT             GPIO_PORT_P5
#define HEATER_PIN              GPIO_PIN0

// MPQ3326 Pinout
#define MPQ3326_W570_ADDR       0x32
#define MPQ3326_W630_ADDR       0x36
#define MPQ3326_W470_ADDR       0x33
#define MPQ3326_W850_ADDR       0x34
#define MPQ3326_I2C_SDA_PxOUT   P3OUT
#define MPQ3326_I2C_SDA_PxIN    P3IN
#define MPQ3326_I2C_SDA_PxDIR   P3DIR
#define MPQ3326_I2C_SDA_PIN     GPIO_PIN0
#define MPQ3326_I2C_SCL_PxOUT   P5OUT
#define MPQ3326_I2C_SCL_PxIN    P5IN
#define MPQ3326_I2C_SCL_PxDIR   P5DIR
#define MPQ3326_I2C_SCL_PIN     GPIO_PIN3
#define MPQ3326_ENABLE_PORT     GPIO_PORT_P5
#define MPQ3326_ENABLE_PIN      GPIO_PIN1

// OPT4003 Pinout
#define OPT4003_I2C_ADDR_1      0x44
#define OPT4003_I2C_ADDR_2      0x45
#define OPT4003_I2C_ADDR_3      0x46
#define OPT4003_I2C_ADDR_4      0x47

#define OPT4003_I2C_SDA_1_PxOUT P2OUT
#define OPT4003_I2C_SDA_1_PxIN  P2IN
#define OPT4003_I2C_SDA_1_PxDIR P2DIR
#define OPT4003_I2C_SDA_1_PIN   GPIO_PIN2
#define OPT4003_I2C_SCL_1_PxOUT P2OUT
#define OPT4003_I2C_SCL_1_PxIN  P2IN
#define OPT4003_I2C_SCL_1_PxDIR P2DIR
#define OPT4003_I2C_SCL_1_PIN   GPIO_PIN3

#define OPT4003_I2C_SDA_2_PxOUT P1OUT
#define OPT4003_I2C_SDA_2_PxIN  P1IN
#define OPT4003_I2C_SDA_2_PxDIR P1DIR
#define OPT4003_I2C_SDA_2_PIN   GPIO_PIN1
#define OPT4003_I2C_SCL_2_PxOUT P1OUT
#define OPT4003_I2C_SCL_2_PxIN  P1IN
#define OPT4003_I2C_SCL_2_PxDIR P1DIR
#define OPT4003_I2C_SCL_2_PIN   GPIO_PIN0

#define OPT4003_I2C_SDA_3_PxOUT P9OUT
#define OPT4003_I2C_SDA_3_PxIN  P9IN
#define OPT4003_I2C_SDA_3_PxDIR P9DIR
#define OPT4003_I2C_SDA_3_PIN   GPIO_PIN7
#define OPT4003_I2C_SCL_3_PxOUT P9OUT
#define OPT4003_I2C_SCL_3_PxIN  P9IN
#define OPT4003_I2C_SCL_3_PxDIR P9DIR
#define OPT4003_I2C_SCL_3_PIN   GPIO_PIN6

#define OPT4003_I2C_SDA_4_PxOUT P3OUT
#define OPT4003_I2C_SDA_4_PxIN  P3IN
#define OPT4003_I2C_SDA_4_PxDIR P3DIR
#define OPT4003_I2C_SDA_4_PIN   GPIO_PIN6
#define OPT4003_I2C_SCL_4_PxOUT P3OUT
#define OPT4003_I2C_SCL_4_PxIN  P3IN
#define OPT4003_I2C_SCL_4_PxDIR P3DIR
#define OPT4003_I2C_SCL_4_PIN   GPIO_PIN5

volatile uint8_t dummy;

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
    case USCI_I2C_UCRXIFG0:              // master wrote a byte
        dummy = UCB0RXBUF;
        break;
    case USCI_I2C_UCTXIFG0:              // master is reading
        UCB0TXBUF = 0xAA;                // return a test pattern
        break;
    default: break;
    }

}
// TODO: update MAX_CMD_SIZE to the actual size
#define MAX_CMD_SIZE 255

void I2C_initSlave()
{
    WDT_A_hold(WDT_A_BASE);     // Stop watchdog
    PMM_unlockLPM5();           // Disable the GPIO power-on default high-impedance mode

    // P1SEL0 &= ~BIT7;          // make sure it's GPIO
    // P1DIR  |=  BIT7 | BIT6;          // output
    // P1OUT  &= ~BIT7;          // drive low

    UCB0CTLW0 = UCSWRST;                      // Software reset enabled
    UCB0CTLW0 |= UCMODE_3 | UCSYNC;           // I2C mode, sync mode
    UCB0I2COA0 = 0x48 | UCOAEN; // Own Address and enable

    // Configure GPIO
    P1OUT &= ~BIT0;                           // Clear P1.0 output latch
    P1DIR |= BIT0;                            // For LED

    // 1)  Pins to I2C function
    P1SEL0 |= BIT6 | BIT7;
    P1SEL1 &= ~(BIT6 | BIT7);

    UCB0IE |= UCSTPIE;                         // Enable STOP interrupt
    UCB0IE |= UCRXIE;                          // Enable RX interrupt
    UCB0IE |= UCTXIE;                          // Enable TX interrupt

    UCB0CTLW0 &= ~UCSWRST;                    // clear reset register
}

void initClock()
{
     //Set DCO frequency to 1MHz
    CS_setDCOFreq(CS_DCORSEL_0,CS_DCOFSEL_0);
    //Set ACLK = VLO with frequency divider of 1
    CS_initClockSignal(CS_ACLK,CS_VLOCLK_SELECT,CS_CLOCK_DIVIDER_1);
    //Set SMCLK = DCO with frequency divider of 1
    CS_initClockSignal(CS_SMCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);
    //Set MCLK = DCO with frequency divider of 1
    CS_initClockSignal(CS_MCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);

}

void TI_I2C_initSlave()
{
 
    // Initialize communication protocol
    // Main I2C Bus
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                                               GPIO_PIN6 + GPIO_PIN7,
                                               GPIO_PRIMARY_MODULE_FUNCTION);

    EUSCI_B_I2C_initSlaveParam param = { 0 };
    param.slaveAddress = (MAIN_SLAVE_ADDR | UCOAEN);
    param.slaveAddressOffset = EUSCI_B_I2C_OWN_ADDRESS_OFFSET0;
    param.slaveOwnAddressEnable = EUSCI_B_I2C_OWN_ADDRESS_ENABLE;
    EUSCI_B_I2C_initSlave(EUSCI_B0_BASE, &param);

    EUSCI_B_I2C_enable(EUSCI_B0_BASE);

    EUSCI_B_I2C_clearInterrupt(EUSCI_B0_BASE,
                               EUSCI_B_I2C_CLOCK_LOW_TIMEOUT_INTERRUPT +
                               EUSCI_B_I2C_RECEIVE_INTERRUPT0 +
                               EUSCI_B_I2C_TRANSMIT_INTERRUPT0 +
                               EUSCI_B_I2C_STOP_INTERRUPT);

    EUSCI_B_I2C_enableInterrupt(EUSCI_B0_BASE,
                                EUSCI_B_I2C_CLOCK_LOW_TIMEOUT_INTERRUPT+
                                EUSCI_B_I2C_RECEIVE_INTERRUPT0 +
                                EUSCI_B_I2C_TRANSMIT_INTERRUPT0 +
                                EUSCI_B_I2C_STOP_INTERRUPT);

}

#define __MSP430FR5969__ 1
#define __MSP430FR5989__ 0

void main(void)
{
    WDTCTL  = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    SWI2C_Descriptor descriptor;
    descriptor.sda_port_out =   &P4OUT;
    descriptor.sda_port_in =    &P4IN;
    descriptor.sda_port_dir =   &P4DIR;
    descriptor.sda_pin =        GPIO_PIN0;
    descriptor.scl_port_out =   &P4OUT;
    descriptor.scl_port_in =    &P4IN;
    descriptor.scl_port_dir =   &P4DIR;
    descriptor.scl_pin =        GPIO_PIN1;
    P4SEL0 &= ~(BIT0 | BIT1);
    P4SEL1 &= ~(BIT0 | BIT1); 
    // descriptor.sda_port_out =   &P1OUT;
    // descriptor.sda_port_in =    &P1IN;
    // descriptor.sda_port_dir =   &P1DIR;
    // descriptor.sda_pin =        GPIO_PIN6;
    // descriptor.scl_port_out =   &P1OUT;
    // descriptor.scl_port_in =    &P1IN;
    // descriptor.scl_port_dir =   &P1DIR;
    // descriptor.scl_pin =        GPIO_PIN7;
    // // Select GPIO on those pins (Table 12-2 MSP430FR user guide)
    // // 0 0 General purpose I/O is selected
    // P1SEL0 &= ~(BIT6 | BIT7);
    // P1SEL1 &= ~(BIT6 | BIT7);

    ADS7138IRTER_Initialize(&descriptor);
    // P1DIR |= BIT6 | BIT7;

    initClock();
    // I2C_initSlave();

    // __bis_SR_register(LPM0_bits | GIE);


    volatile uint16_t rx = 0;
     while (1)
    {
        rx = ADS7138IRTER_Read(&descriptor);
    
        __delay_cycles(500000);               /* ≈0.5 s idle              */
    }
   
}