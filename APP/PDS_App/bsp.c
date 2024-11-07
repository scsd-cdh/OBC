
#include "bsp.h"
#include "gpio.h"
#include "i2c.h"
#include <msp430.h> 

//******************************************************************************
// Device Initialization *******************************************************
//******************************************************************************

void initBsp()
{
    initClockTo16MHz();
    initGPIO();
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
