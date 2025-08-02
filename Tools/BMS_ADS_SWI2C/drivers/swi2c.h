/*
 * swi2c.h
 *
 *  Created on: Feb. 21, 2024
 *      Author: Ruben
 */

#ifndef SRC_DRIVERS_SWI2C_H_
#define SRC_DRIVERS_SWI2C_H_

#include <stdint.h>

/* The I2C data rate frequency can be calculated by:
 *
 * I2C Data Rate =      SMCLK Frequency
 *                    ___________________
 *
 *                      2 * TimerPeriod
 */
#define SWI2C_TIMER_PERIOD  5

/* Macro for a timer iteration */
#define TIMER_ITERATION()            TB0CCTL0 &= ~(CCIFG);           \
                                     while(!(TB0CCTL0 & CCIFG));

typedef struct {
    volatile uint8_t* sda_port_out;
    volatile uint8_t* sda_port_in;
    volatile uint8_t* sda_port_dir;
    uint16_t sda_pin;
    volatile uint8_t* scl_port_out;
    volatile uint8_t* scl_port_in;
    volatile uint8_t* scl_port_dir;
    uint16_t scl_pin;
    uint8_t address;
} SWI2C_Descriptor;

void SWI2C_Init(SWI2C_Descriptor *descriptor);

int32_t SWI2C_Read(SWI2C_Descriptor *descriptor, uint8_t *buffer, uint16_t len);
int32_t SWI2C_Write(SWI2C_Descriptor *descriptor, uint8_t *buffer, uint16_t len);

#endif /* SRC_DRIVERS_SWI2C_H_ */
