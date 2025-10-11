#include "i2c.h"

struct io_descriptor *i2c_io;

void i2c_init(){
    i2c_m_sync_get_io_descriptor(&I2C, &i2c_io);
	i2c_m_sync_enable(&I2C);
}

void i2c_write(uint8_t addr, uint8_t *buffer, uint8_t length){
    i2c_m_sync_set_slaveaddr(&I2C, addr, I2C_M_SEVEN); // Set Address
    io_write(i2c_io, buffer, length); // Transmit
}

void i2c_reg_write(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t length){
    i2c_m_sync_set_slaveaddr(&I2C, addr, I2C_M_SEVEN); // Set Address
    i2c_m_sync_cmd_write(&I2C, reg, buffer, length);
}

uint8_t *i2c_read(uint8_t addr, uint8_t length){
    uint8_t data[length]; uint8_t *data_ptr=data;  // Return Data
    i2c_m_sync_set_slaveaddr(&I2C, addr, I2C_M_SEVEN|I2C_M_RD); // Set Address
    io_read(i2c_io, data_ptr, length); // Read Data
    return data_ptr;
}

uint8_t *i2c_reg_read(uint8_t addr, uint8_t reg, uint8_t length){
    uint8_t data[length]; uint8_t *data_ptr=data;  // Return Data
    i2c_m_sync_set_slaveaddr(&I2C, addr, I2C_M_SEVEN|I2C_M_RD); // Set Address
    i2c_m_sync_cmd_read(&I2C, reg, data_ptr, length);
    return data_ptr;
}