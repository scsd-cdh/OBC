
#include "rtc_b.h"
#include "gpio.h"
#include "bsp.h"

void shutdownConvA(void) {
    GPIO_setOutputLowOnPin(CONV_RUN_A_PORT, CONV_RUN_A_PIN);
}

void shutdownConvB(void) {
    GPIO_setOutputLowOnPin(CONV_RUN_B_PORT, CONV_RUN_B_PIN);
}

/*ISR that maintains LPM until 30 minutes has passed*/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=RTC_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(RTC_VECTOR)))
#endif
void RTC_B_ISR (void)
{
    switch (__even_in_range(RTCIV,16)){
        case 2:     //RTCRDYIFG, triggered every second
            break;
        case 4:     //RTCEVIFG, triggered every minute
            break;

        case 6:     //RTCAIFG, triggers at set alarm
            shutdownConvA();
            shutdownConvB();
            break;

        default: 
            break;
    }
}
