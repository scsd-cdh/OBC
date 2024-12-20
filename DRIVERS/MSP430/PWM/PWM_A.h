/*
 * Pins that can be PWM (Timer_A) pins are:
 * P1.2
 */

#ifndef PWM_A_H_
#define PWM_A_H_

#include "timer_a.h"

extern void PWM_PinSelect_A(uint8_t selectedPort, uint16_t selectedPin);
extern void PWM_Generate_A(uint16_t Timer_Period, uint16_t Duty_Cycle);

#endif /* PWM_A_H_ */
