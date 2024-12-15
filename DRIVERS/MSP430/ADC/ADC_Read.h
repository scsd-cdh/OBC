#include "adc12_b.h"

#ifndef ADC_READ_H_
#define ADC_READ_H_

//*****************************************************************************
//
//! \brief Used in the ADC_PinSelect() function as the pin parameter.
//
//*****************************************************************************
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


//*****************************************************************************
//
//! \brief Initialises the ADC Driver
//!
//! Initialises the ADC with standard parameters
//!
//! \return None
//
//*****************************************************************************
void ADC_init();


//*****************************************************************************
//
//! \brief Select a pin to set as ADC pin and assign its values to a memory register
//!
//! \param pin is any pin defined in the ADC_Pin enum
//! \param memoryBufferIndex expects "ADC12_B_MEMORY_x" where 0 <= x <= 31. It is recommended to start at 0.
//!     It is highly recommended to start at 0 and assign incrementally when selecting more pins.
//!
//! \return None
//
//*****************************************************************************
void ADC_PinSelect(ADC_Pin pin, uint8_t memoryBufferIndex);


//*****************************************************************************
//
//! \brief Read a ADC pin
//!
//! Returns the digital value held at a memory index which was assigned to an ADC pin.
//!     The digital value is a 12-bit integer (0 to 4095) where 0 is 0 volts and 4095 is Vref.
//!
//! \param memoryBufferIndex expects "ADC12_B_MEMORY_x" (where 0 <= x <= 31) which was set before in ADC_PinSelect(...).
//!
//! \return 16 bit unsigned integer
//
//*****************************************************************************
uint16_t Read_ADC(uint8_t memoryBufferIndex);


#endif /* ADC_READ_H_ */
