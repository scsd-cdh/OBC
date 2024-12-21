#ifndef ADC_READ_H_
#define ADC_READ_H_

#include "include/adc12_b.h"

/******************************************************************************
 * @brief Used in the ADC_PinSelect() function as the pin parameter.
 ******************************************************************************/
typedef enum{
    P1_0 = ADC12_B_INPUT_A0,
    P1_1 = ADC12_B_INPUT_A1,
    P1_2 = ADC12_B_INPUT_A2,
    P1_3 = ADC12_B_INPUT_A3,
    P1_4 = ADC12_B_INPUT_A4,
    P1_5 = ADC12_B_INPUT_A5,
    P2_3 = ADC12_B_INPUT_A6,
    P2_4 = ADC12_B_INPUT_A7,
    P4_0 = ADC12_B_INPUT_A8,
    P4_1 = ADC12_B_INPUT_A9,
    P4_2 = ADC12_B_INPUT_A10,
    P4_3 = ADC12_B_INPUT_A11,
    P3_0 = ADC12_B_INPUT_A12,
    P3_1 = ADC12_B_INPUT_A13,
    P3_2 = ADC12_B_INPUT_A14,
    P3_3 = ADC12_B_INPUT_A15,

} ADC_Pin;


/******************************************************************************
 * @brief Initializes the ADC driver with standard parameters.
 *
 * This function sets up the ADC driver, configuring it with default settings
 * to prepare for operation.
 *
 * @return None.
 ******************************************************************************/
extern void ADC_init_Standard();


/******************************************************************************
 * @brief Configures a pin as an ADC pin and assigns its values to a memory register.
 *
 * This function selects a specified pin to function as an ADC pin and maps
 * its digital output values to a memory register for retrieval.
 *
 * @param pin
 *        Any pin defined in the `ADC_Pin` enumeration.
 *
 * @param memoryBufferIndex
 *        The memory index to assign to the pin, specified as "ADC12_B_MEMORY_x"
 *        (where 0 <= x <= 31). It is strongly recommended to start at 0 and
 *        assign indexes incrementally when configuring multiple pins.
 *
 * @return None.
 ******************************************************************************/
extern void ADC_PinSelect(ADC_Pin pin, uint8_t memoryBufferIndex);


/******************************************************************************
 * @brief Reads the digital value from an ADC pin.
 *
 * This function retrieves the digital value from a specified memory index
 * associated with an ADC pin. The returned value is a 12-bit integer
 * (ranging from 0 to 4095), where:
 *   - 0 represents 0 volts
 *   - 4095 represents Vref.
 *
 * @param memoryBufferIndex
 *        The memory index assigned to an ADC pin, specified as
 *        "ADC12_B_MEMORY_x" (where 0 <= x <= 31), configured earlier
 *        via ADC_PinSelect(...).
 *
 * @return A 16-bit unsigned integer representing the digital value.
 ******************************************************************************/
extern uint16_t Read_ADC(uint8_t memoryBufferIndex);


#endif /* ADC_READ_H_ */
