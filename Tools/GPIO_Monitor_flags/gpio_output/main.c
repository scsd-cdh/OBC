// MSP Library
#include <msp430.h> 

// C Library
#include <stdio.h>
#include <stdlib.h>

// Header
#include <GPIO/GPIO_Read.h>


// Constants
GPIO_pin LED_PINS[] = {
   GP3_0,
   GP3_1,
   GP3_2,
   GP3_3
};

void initializeGPIO(){
    // Setup Pins
    GPIO_Initialize_Pin(PORT3,LED_PINS[0],1); // Cell 1 Flag LED
    GPIO_Initialize_Pin(PORT3,LED_PINS[1],1); // Cell 2 Flag LED
    GPIO_Initialize_Pin(PORT3,LED_PINS[2],1); // Cell 3 Flag LED
    GPIO_Initialize_Pin(PORT3,LED_PINS[3],1); // Cell 4 Flag LED
    PM5CTL0 &= ~LOCKLPM5; // Remove High-Impedance
}


void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    // Initialize Pins
    initializeGPIO();

    // Pins
    GPIO_Write(PORT3,LED_PINS[0],1); // Write to Pins
    volatile uint8_t value = GPIO_Read(PORT3,LED_PINS[0]); // Read to Pins

}
