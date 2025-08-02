#include "../drivers/swi2c.h"
#include <stdint.h>

void ADS7138IRTER_Initialize(SWI2C_Descriptor *descriptor);
uint16_t ADS7138IRTER_Read(SWI2C_Descriptor *descriptor);