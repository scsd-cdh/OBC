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

// I2C Slave Pins
#define PA3 GPIO(GPIO_PORTA, 3)		// SDA
#define PA4 GPIO(GPIO_PORTA, 4)		// SCL

// VCOM USART1 Pins
#define PA21 GPIO(GPIO_PORTA, 21)	// TX
#define PB4 GPIO(GPIO_PORTB, 4)		// RX

// LED
#define LED0 GPIO(GPIO_PORTA, 23) // PA23

#endif // ATMEL_START_PINS_H_INCLUDED
