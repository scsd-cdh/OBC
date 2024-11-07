
#include "GPIO.h"

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
