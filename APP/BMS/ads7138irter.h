#include "swi2c.h"
#include <stdint.h>

typedef enum {
    ADS7138_SYSTEM_STATUS_REGISTER = 0x00,
    ADS7138_PIN_CFG_REGISTER = 0x05,
    ADS7138_CHANNEL_SEL_REGISTER = 0x11,
} ADS7138_Register;

int32_t ADS7138IRTER_Initialize(SWI2C_Descriptor *descriptor);
int32_t ADS7138_WrGenCmd(SWI2C_Descriptor *descriptor);
uint16_t ADS7138IRTER_Read(SWI2C_Descriptor *descriptor);
uint8_t ADS7138IRTER_SingleRegisterRead(SWI2C_Descriptor *descriptor, ADS7138_Register reg_addr);
int32_t ADS7138IRTER_SingleRegisterWrite(SWI2C_Descriptor *descriptor, ADS7138_Register reg_addr, uint8_t reg_data);
int32_t ADS7138IRTER_ClearBit(SWI2C_Descriptor *descriptor, ADS7138_Register reg_addr, uint8_t clear_bits);