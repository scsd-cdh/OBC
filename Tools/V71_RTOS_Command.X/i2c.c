#include "i2c.h"

void i2c_write(struct io_descriptor *const io_descr, 
                struct i2c_m_sync_desc *i2c, 
                uint8_t addr, uint16_t data){
    
    // Set Address
    i2c_m_sync_set_slaveaddr(i2c, addr, I2C_M_SEVEN); 
    
    // Transmit to Slave - FIXED: pass the buffer directly
    io_write(io_descr, (uint8_t*)&data, 2);
}

void i2c_read(struct i2c_m_sync_desc *i2c, uint8_t addr, uint8_t *data){        
   
    // Set Address to Read
    i2c_m_sync_set_slaveaddr(
        i2c, 
        addr, 
        I2C_M_SEVEN | I2C_M_RD
    );  
    
    // Read Data
    io_read(command_io, data, 4);
    
}
