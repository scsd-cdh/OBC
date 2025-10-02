#include "ads7138irter.h"
#include "swi2c.h"

#define ADS7138_GEN_CMD             0x00
#define ADS7138_SW_RESET            0x06
#define ADS7138_SNG_REG_WRITE       0x08
#define ADS7138_SNG_REG_READ        0x10
#define ADS7138_I2C_ADDRESS         0x10
#define ADS7138_REG_CHANNEL_SEL     0x11
#define ADS7138_SET_BIT             0x18
#define ADS7138_CLEAR_BIT           0x20

int32_t ADS7138IRTER_Initialize(SWI2C_Descriptor *descriptor) {
    descriptor->address = ADS7138_I2C_ADDRESS;
    SWI2C_Init(descriptor);
}

int32_t ADS7138IRTER_SW_Reset(SWI2C_Descriptor *descriptor) {
    uint8_t write_buffer[1] = { ADS7138_SW_RESET };
    descriptor->address = ADS7138_GEN_CMD;
    int32_t status = SWI2C_Write(descriptor, ADS7138_SW_RESET, 1);
    descriptor->address = ADS7138_I2C_ADDRESS;
    return status;
}

uint16_t ADS7138IRTER_Read(SWI2C_Descriptor *descriptor) {
    uint8_t read_buffer[2];
    SWI2C_Read(descriptor, read_buffer, 2);
    return ((uint16_t)read_buffer[0] << 4) | (read_buffer[1] >> 4);
}

uint8_t ADS7138IRTER_SingleRegisterRead(SWI2C_Descriptor *descriptor, ADS7138_Register reg_addr) {
    uint8_t wbuffer[2] = { ADS7138_SNG_REG_READ, reg_addr };
    SWI2C_Write(descriptor, wbuffer, 2);
    uint8_t rbuffer[1];
    SWI2C_Read(descriptor, rbuffer, 1);
    return rbuffer[0];
}

int32_t ADS7138IRTER_SingleRegisterWrite(SWI2C_Descriptor *descriptor, ADS7138_Register reg_addr, uint8_t reg_data) {
    uint8_t wbuffer[3] = { ADS7138_SNG_REG_WRITE, reg_addr, reg_data };
    return SWI2C_Write(descriptor, wbuffer, 3);
}

int32_t ADS7138IRTER_ClearBit(SWI2C_Descriptor *descriptor, ADS7138_Register reg_addr, uint8_t clear_bits) {
    uint8_t wbuffer[3] = {ADS7138_CLEAR_BIT, reg_addr, clear_bits };
    return SWI2C_Write(descriptor, wbuffer, 3);
}
