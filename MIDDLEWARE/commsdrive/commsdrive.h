//
// Created by Mohamed on 2025-06-04.
//

#include <stdint.h>

#ifndef COMMS_DRIVER_H
#define COMMS_DRIVER_H

void CommsDrive_Init(int16_t slaveAddr);
void CommsDrive_ProcessTeleRequest(uint8_t slaveAddr, uint8_t request, uint8_t* rx_buf, uint8_t len);
void CommsDrive_ProcessTeleCommand(uint8_t slaveAddr, uint8_t cmd, const uint8_t* buffer, uint8_t size);

#endif //COMMS_DRIVER_H
