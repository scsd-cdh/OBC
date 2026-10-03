#ifndef PDS_H
#define PDS_H

#include <stdbool.h>
#include <stdint.h>

#include "ADC_Read.h"
#include "gpio.h"

#define SLAVE_ADDR (0x08)

#define CONV_RUN_A_PORT GPIO_PORT_P1
#define CONV_RUN_A_PIN GPIO_PIN5
#define CONV_RUN_B_PORT GPIO_PORT_P2
#define CONV_RUN_B_PIN GPIO_PIN4

#define CONV_FLAG1_X_PLUS_PORT GPIO_PORT_P3
#define CONV_FLAG1_X_PLUS_PIN GPIO_PIN7
#define CONV_FLAG2_X_PLUS_PORT GPIO_PORT_P2
#define CONV_FLAG2_X_PLUS_PIN GPIO_PIN3
#define CONV_FLAG1_X_MINUS_PORT GPIO_PORT_P2
#define CONV_FLAG1_X_MINUS_PIN GPIO_PIN2
#define CONV_FLAG2_X_MINUS_PORT GPIO_PORT_P2
#define CONV_FLAG2_X_MINUS_PIN GPIO_PIN1
#define CONV_FLAG1_Y_PLUS_PORT GPIO_PORT_P2
#define CONV_FLAG1_Y_PLUS_PIN GPIO_PIN0
#define CONV_FLAG2_Y_PLUS_PORT GPIO_PORT_P4
#define CONV_FLAG2_Y_PLUS_PIN GPIO_PIN1
#define CONV_FLAG1_Y_MINUS_PORT GPIO_PORT_P4
#define CONV_FLAG1_Y_MINUS_PIN GPIO_PIN0
#define CONV_FLAG2_Y_MINUS_PORT GPIO_PORT_P4
#define CONV_FLAG2_Y_MINUS_PIN GPIO_PIN2

#if defined(__MSP430FR5989__)
#define INT_5V_VS ADC12_B_INPUT_A11 /* P9.3 */
#define REG_5V_VS ADC12_B_INPUT_A12 /* P9.4 */
#define A_5V_VS ADC12_B_INPUT_A13   /* P9.5 */
#define B_5V_VS ADC12_B_INPUT_A14   /* P9.6 */
#define TEMP_SENSE ADC12_B_INPUT_A15 /* P9.7 */
#elif defined(__MSP430FR6989__)
#define INT_5V_VS ADC12_B_INPUT_A10 /* P9.2 */
#define REG_5V_VS ADC12_B_INPUT_A11 /* P9.3 */
#define A_5V_VS ADC12_B_INPUT_A7   /* P8.4 */
#define B_5V_VS ADC12_B_INPUT_A6   /* P8.5 */
#define TEMP_SENSE ADC12_B_INPUT_A5 /* P8.6 */
#else
#define INT_5V_VS P4_2
#define REG_5V_VS P1_0
#define A_5V_VS P1_1
#define B_5V_VS P1_2
#define TEMP_SENSE P1_3
#endif

void pds_init(void);
bool pds_isr_triggered(void);
void pds_collectdata(void);

#endif
