#ifndef I2C_WRAPPER    /* Guard against multiple inclusion */
#define I2C_WRAPPER

/* Includes */
// Header
#include "atmel_start_pins.h"
#include "atmel_start.h"

// C Library
#include <stdio.h>

/* Definitions */
#define COMMAND_ADDRESS 0x42

/* Prototypes */
void i2c_write(struct io_descriptor *const io_descr, 
				 struct i2c_m_sync_desc *i2c, 
				 uint8_t addr,uint16_t data);
void i2c_read(struct i2c_m_sync_desc *i2c, uint8_t addr, 
				 uint8_t *data);

#endif 
