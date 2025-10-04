#ifndef GPIO_READ_H_
#define GPIO_READ_H_

#include "include/gpio.h"

/******************************************************************************
 * @brief Used in the GPIO_PinSelect() function as the pin parameter.
******************************************************************************/

typedef enum{
     PORT1 = GPIO_PORT_P1,
     PORT2 = GPIO_PORT_P2,
     PORT3 = GPIO_PORT_P3,
     PORT4 = GPIO_PORT_P4,
     PORT5 = GPIO_PORT_P5,
     PORT6 = GPIO_PORT_P6,
     PORT7 = GPIO_PORT_P7,
     PORT8 = GPIO_PORT_P8,
     PORT9 = GPIO_PORT_P9,
     PORT10 = GPIO_PORT_P10,
     PORT11= GPIO_PORT_P11,
     PORTA = GPIO_PORT_PA,
     PORTB = GPIO_PORT_PB,
     PORTC = GPIO_PORT_PC,
     PORTD = GPIO_PORT_PD,
     PORTE = GPIO_PORT_PE,
     PORTF = GPIO_PORT_PF,
     PORTJ = GPIO_PORT_PJ,
}GPIO_Port;

typedef enum{
    GP1_0 = GPIO_PIN0,
    GP1_1 = GPIO_PIN1,
    GP1_2 = GPIO_PIN2,
    GP1_3 = GPIO_PIN3,
    GP1_4 = GPIO_PIN4,
    GP1_5 = GPIO_PIN5,
    GP2_3 = GPIO_PIN6,
    GP2_4 = GPIO_PIN7,
    GP4_0 = GPIO_PIN8,
    GP4_1 = GPIO_PIN9,
    GP4_2 = GPIO_PIN10,
    GP4_3 = GPIO_PIN11,
    GP3_0 = GPIO_PIN12,
    GP3_1 = GPIO_PIN13,
    GP3_2 = GPIO_PIN14,
    GP3_3 = GPIO_PIN15,
    GP8 = GPIO_PIN_ALL8, // GPIO All 8 pins in port
    GP16 = GPIO_PIN_ALL16, // GPIO All 16 pins in port
}GPIO_pin;

/******************************************************************************
 * @brief Chooses GPIO Port and GPIO Pin
 *
 * This function selects a GPIO Pin within a specified Port and configures
 * them as either input or output
 *
 * @param port
 *        Any port defined in the `GPIO_Port` enumeration.
 *
 * @param pin
 *        Any port defined in the `GPIO_Pin` enumeration.
 *
 * * @param purpose
 *        1 -> Set a an Output
 *        0 -> Set as Input
 *
 * @return None.
 ******************************************************************************/
extern void GPIO_Initialize_Pin(GPIO_Port port, GPIO_pin pin, uint8_t purpose);


/******************************************************************************
 * @brief Chooses GPIO Port and GPIO Pin
 *
 * This function selects a specified pin to function as an GPIO pin and reads the
 * value of the pin
 *
 * @param port
 *        Any port defined in the `GPIO_Port` enumeration.
 *
 * @param pin
 *        Any port defined in the `GPIO_Pin` enumeration.
 *
 * * @param purpose
 *        1 -> Set a an Output
 *        0 -> Set as Input
 *
 * @return 1 or 0 indicating whether it is high or low.
 ******************************************************************************/
extern uint8_t GPIO_Read(GPIO_Port port,GPIO_pin pin);


/******************************************************************************
 * @brief Chooses GPIO Port and GPIO Pin
 *
 * This function selects a specified pin to function as an GPIO pin and sets the
 * value of the pin. Pin must be an output pin, otherwise the function will do nothing
 *
 * @param port
 *        Any port defined in the `GPIO_Port` enumeration.
 *
 * @param pin
 *        Any port defined in the `GPIO_Pin` enumeration.
 *
 * @param value
 *        1 -> Set HIGH
 *        0 -> Set LOW
 *
 * @return nothing
 ******************************************************************************/
extern void GPIO_Write(GPIO_Port port,GPIO_pin pin, uint8_t value);

#endif /* GPIO_READ_H_ */
