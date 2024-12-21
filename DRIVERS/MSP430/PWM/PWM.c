#include "PWM.h"
#include "gpio.h"

void PWM_PinSelect(uint8_t selectedPort, uint16_t selectedPin){
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        selectedPort,
        selectedPin,
        GPIO_PRIMARY_MODULE_FUNCTION
        );
}

void PWM_Generate(uint16_t Timer_Period, uint16_t Duty_Cycle, Compare_Register CompareRegister){
    Timer_B_outputPWMParam param = {0};
    param.clockSource = TIMER_B_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_1;
    param.timerPeriod = Timer_Period;
    param.compareRegister = CompareRegister;
    param.compareOutputMode = TIMER_B_OUTPUTMODE_RESET_SET;
    param.dutyCycle = Duty_Cycle;

    Timer_B_outputPWM(TIMER_B0_BASE, &param);
}
