#include "PWM_A.h"
#include "gpio.h"

void PWM_PinSelect_A(uint8_t selectedPort, uint16_t selectedPin){
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        selectedPort,
        selectedPin,
        GPIO_PRIMARY_MODULE_FUNCTION
        );
}

void PWM_Generate_A(uint16_t Timer_Period, uint16_t Duty_Cycle){
    Timer_A_outputPWMParam param = {0};
    param.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    param.timerPeriod = Timer_Period;
    param.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    param.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    param.dutyCycle = Duty_Cycle;

    Timer_A_outputPWM(TIMER_A1_BASE, &param);
}
