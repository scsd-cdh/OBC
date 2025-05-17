#ifndef BMS_H
#define BMS_H

#include "tinyprotocol.h"
#include "i2c.h"

#define SYSTEM_STATUS_RESP_LEN 5     // Response length for system status 
#define CURRENT_VOLTAGE_RESP_LEN 4

void initBSP();

// NOTE: Strangest thing... If I don't assign these, when they get passed into TINYPROTOCOL_RegisterTelemetryChannel
// The copied variable appears to be incremented by 1... No idea
enum BMS_TelemetryRequestCmdId {
  BMS_SYSTEM_STATUS_ID = TINYPROTOCOL_TLM_RESERVED,
  BMS_HEALTH_CHECK_ID = 1,
  BMS_FLAG_ID = 2,
  BMS_CURRENT_DRAW_ID = 3,
  BMS_CURRENT_CHARGE_ID = 4,
  BMS_VOLTAGE_BATTERY1_ID = 5,
  BMS_VOLTAGE_BATTERY2_ID = 6,
  BMS_VOLTAGE_COMBINED_ID = 7,
  BMS_HEATERS_CONTROLLER_ID = 8,
};

typedef union SystemStatusResp {
    struct {
        uint32_t runtime : 32;
        uint8_t fw_version : 8;
    };
    uint8_t buffer[SYSTEM_STATUS_RESP_LEN];
} SystemStatusResp_t;

typedef union CurrentResp {
    struct {
        uint16_t isense1 : 16;
        uint16_t isense2 : 16;
    };
    uint8_t buffer[CURRENT_VOLTAGE_RESP_LEN];
} CurrentResp_t;

typedef union VoltageResp {
    struct {
        uint16_t vcell_a : 16;
        uint16_t vcell_b : 16;
    };
    uint8_t buffer[CURRENT_VOLTAGE_RESP_LEN];
} VoltageResp_t;

typedef union CombinedVoltageResp {
    struct {
        uint16_t vbatt1 : 16;
        uint16_t vbatt2 : 16;
    };
    uint8_t buffer[CURRENT_VOLTAGE_RESP_LEN];
} CombinedVoltageResp_t;

typedef uint8_t Flag_t;

void InitAppComm(void);

#endif