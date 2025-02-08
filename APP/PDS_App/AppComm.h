
#ifndef _APP_COMM_
#define _APP_COMM_

#include <stdint.h>

#include "tinyprotocol.h"
#include "version.h"
#include "i2c.h"

//*******************************************************************************
// Device Communication Defines *************************************************
//*******************************************************************************
#define SYSTEM_STATUS_RESP_LEN      5     // Response length for system status 
#define REBOOT_CMD_LEN              1     

// Telecommands
enum CustomProtocolTelecommand {
  REBOOT_ID = TINYPROTOCOL_TC_RESERVED,
};

// Telemetry Requests
enum CustomProtocolTelemetryRequest {
  SYSTEM_STATUS_ID = TINYPROTOCOL_TLM_RESERVED,
  HEALTH_CHECK_ID,
  CONVERTER_MONITOR_ID,  
};

typedef union SystemStatus {
    struct {
        uint32_t runtime : 32;
        uint8_t firmware_version : 8;
    };
    uint8_t bytes[SYSTEM_STATUS_RESP_LEN];
} SystemStatus_t;

//*******************************************************************************
// Buffers **********************************************************************
//*******************************************************************************

// Buffers for holding response data to be sent back to the master
static SystemStatus_t SystemStatusRespBuf = {    /**< Buffer for system status response data */
  .firmware_version = PDS_FW_VERSION,
  .runtime = 0
};   

/**
 * @brief Initalize host communication
 */
void InitAppComm(void);

#endif // _APP_COMM_
