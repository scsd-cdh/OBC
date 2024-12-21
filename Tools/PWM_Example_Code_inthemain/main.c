#include <msp430.h>
#include "include/pmm.h"
#include "PWM.h"
#include "gpio.h"

/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    PWM_PinSelect(GPIO_PORT_P1, GPIO_PIN4);
    PWM_PinSelect(GPIO_PORT_P2, GPIO_PIN6);

    PMM_unlockLPM5();

    PWM_Generate(524,329, CompareRegister_1);

    //Enter LPM0
    __bis_SR_register(LPM0_bits);

    //For debugger
    __no_operation();

    return 0;
}
