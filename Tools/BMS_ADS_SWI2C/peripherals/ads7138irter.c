#include "ads7138irter.h"

#define ADS7138_I2C_ADDRESS         0x10
#define ADS7138_CMD_SINGLE_WRITE    0x08
#define ADS7138_REG_CHANNEL_SEL     0x11

void ADS7138IRTER_Initialize(SWI2C_Descriptor *descriptor) {
    descriptor->address = ADS7138_I2C_ADDRESS;
    SWI2C_Init(descriptor);

    uint8_t write_buffer[3];
    write_buffer[0] = ADS7138_CMD_SINGLE_WRITE; 
    write_buffer[1] = ADS7138_REG_CHANNEL_SEL; 
    write_buffer[2] = 0x00;                     

    SWI2C_Write(descriptor, write_buffer, 3);
}

uint16_t ADS7138IRTER_Read(SWI2C_Descriptor *descriptor) {
    uint8_t read_buffer[2];
    SWI2C_Read(descriptor, read_buffer, 2);
    return ((uint16_t)read_buffer[0] << 4) | (read_buffer[1] >> 4);
}
