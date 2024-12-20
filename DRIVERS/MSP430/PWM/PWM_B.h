/*
 * Pins that can be PWM (Timer_B) pins are:
 * P1.4
 * P2.6
 */

#ifndef PWM_B_H_
#define PWM_B_H_

#include "timer_b.h"

extern void PWM_PinSelect_B(uint8_t selectedPort, uint16_t selectedPin);
extern void PWM_Generate_B(uint16_t Timer_Period, uint16_t Duty_Cycle);


#endif /* PWM_B_H_ */
