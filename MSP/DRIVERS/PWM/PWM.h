// Note: this uses Timer_B

#ifndef PWM_B_H_
#define PWM_B_H_

#include "include/timer_b.h"

/******************************************************************************
 * @brief Used in the PWM_PinSelect() function as the CompareRegister parameter.
 ******************************************************************************/
typedef enum {

    CompareRegister_0 = TIMER_B_CAPTURECOMPARE_REGISTER_0,
    CompareRegister_1 = TIMER_B_CAPTURECOMPARE_REGISTER_1,
    CompareRegister_2 = TIMER_B_CAPTURECOMPARE_REGISTER_2,
    CompareRegister_3 = TIMER_B_CAPTURECOMPARE_REGISTER_3,
    CompareRegister_4 = TIMER_B_CAPTURECOMPARE_REGISTER_4,
    CompareRegister_5 = TIMER_B_CAPTURECOMPARE_REGISTER_5,
    CompareRegister_6 = TIMER_B_CAPTURECOMPARE_REGISTER_6

} Compare_Register;


/******************************************************************************
 * @brief Configures a pin to be PWM-capable
 *
 * This function sets up a pin to be configured as a PWM pin.
 *
 * @param selectedPort
 *        The port. As in, it is x in Px.y
 *
 * @param selectedPin
 *        The pin. As in, it is y in Px.y
 *
 * @return None.
 ******************************************************************************/
extern void PWM_PinSelect(uint8_t selectedPort, uint16_t selectedPin);

/******************************************************************************
 * @brief Generates a PWM waveform in up-mode
 *
 * This function generates a PWM Waveform with the specified timer period (in micro-seconds) and duty cycle (in microseconds)
 * Example: To generate a 75% waveform, a valid example is Timer_Period = 524 and duty cycle = 393. (As in 393 is 75% of 524)
 *
 * @param Timer_Period
 *        Period of waveform in microseconds
 *
 * @param Duty_Cycle
 *        Duty cycle time in microseconds
 *
 * @param CompareRegister
 *        This parameter expects "CompareRegister_x" from the Compare_Register enum where 0 <= x <= 6.
 *        This parameter is responsible for specifying exactly what registers are to be generating this waveform.
 *        There are two pins per register, and they are set by manufacturer as shown below:
 *        Compare Register 0:
 *        P2.1
 *        P2.5
 *
 *        Compare Register 1:
 *        P1.4
 *        P2.6
 *
 *        Compare Register 2:
 *        P1.5
 *        P2.2
 *
 *        Compare Register 3:
 *        P3.4
 *        P1.6
 *
 *        Compare Register 4:
 *        P3.5
 *        P1.7
 *
 *        Compare Register 5:
 *        P3.6
 *        P4.4
 *
 *        Compare Register 6:
 *        P3.7
 *        P2.0
 *
 * @return None.
 ******************************************************************************/
extern void PWM_Generate(uint16_t Timer_Period, uint16_t Duty_Cycle, Compare_Register CompareRegister);


#endif /* PWM_B_H_ */
