/* 
 * File:   i2c.h
 * Author: Srira
 *
 * Created on October 11, 2025, 1:53 PM
 */

#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include "driver_init.h"

extern void i2c_init();
extern void i2c_write(uint8_t addr, uint8_t *buffer, uint8_t length);
extern void i2c_reg_write(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t length);
extern uint8_t *i2c_read(uint8_t addr, uint8_t length);
extern uint8_t *i2c_reg_read(uint8_t addr, uint8_t reg, uint8_t length);


#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

