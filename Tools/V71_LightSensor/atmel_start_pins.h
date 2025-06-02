/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef ATMEL_START_PINS_H_INCLUDED
#define ATMEL_START_PINS_H_INCLUDED

#include <hal_gpio.h>

// SAMV71 has 4 pin functions
#define GPIO_PIN_FUNCTION_A 0
#define GPIO_PIN_FUNCTION_B 1
#define GPIO_PIN_FUNCTION_C 2
#define GPIO_PIN_FUNCTION_D 3

// LED 
#define LED0 GPIO(GPIO_PORTA, 23) // PA23

// I2C Pins
#define I2C0_SDA GPIO(GPIO_PORTA, 3) // PA03
#define I2C0_SCL GPIO(GPIO_PORTA, 4) // PA04

// USART Pins
#define USART0_RX GPIO(GPIO_PORTA, 21) // PA21
#define USART0_TX GPIO(GPIO_PORTB, 4)  // PB04


#endif // ATMEL_START_PINS_H_INCLUDED
