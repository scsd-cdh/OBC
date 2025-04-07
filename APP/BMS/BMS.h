#ifndef BMS_H
#define BMS_H

#include "tinyprotocol.h"
#include "i2c.h"

#define SYSTEM_STATUS_RESP_LEN 5     // Response length for system status 
#define POWER_STATUS_RESP_LEN 16  

void initBSP();

enum BMS_TelemetryRequestCmdId {
  BMS_SYSTEM_STATUS_ID = TINYPROTOCOL_TLM_RESERVED,
  BMS_HEALTH_CHECK_ID,
  BMS_FLAG_ID,
  BMS_POWER_STATUS_ID,
  BMS_HEATERS_CONTROLLER_ID
};

typedef union SystemStatusResp {
    struct {
        uint32_t runtime : 32;
        uint8_t fw_version : 8;
    };
    uint8_t buffer[SYSTEM_STATUS_RESP_LEN];
} SystemStatusResp_t;

typedef union PowerStatusResp {
    struct {
        uint16_t current : 15;
        uint16_t voltage : 15;
        uint8_t battery_number : 3;
    };
    uint8_t buffer[SYSTEM_STATUS_RESP_LEN];
} PowerStatusResp_t;


extern PowerStatusResp_t PowerStatusBattery1Out;
extern PowerStatusResp_t PowerStatusBattery2Out;

#endif