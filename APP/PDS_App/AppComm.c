
#include "AppComm.h"
#include "RoutineCycle.h"
#include "bsp.h"

//***************************Private functions definitions***************************************************
static int16_t ProcessTelecommand(uint8_t command, const uint8_t* buffer, uint8_t size);
static int16_t ProcessTelemetryRequest(uint8_t request);
static int16_t SendTelemetryResponse();

static void I2C_Proc_RX_Data(uint8_t data);

const struct TINYPROTOCOL_Config protocolConfig =
{
    .TINYPROTOCOL_ProcessTelecommand = ProcessTelecommand,
    .TINYPROTOCOL_ProcessTelemetryRequest = ProcessTelemetryRequest,
    .TINYPROTOCOL_WriteBuffer = transmitI2C
};

//***************************Public functions***************************************************
void InitAppComm(void) {
    sI2cConfigCb_t i2cConfig = {
        .Rx_Proc_Data = I2C_Proc_RX_Data,
        .slave_addr = SLAVE_ADDR
    };
    initI2C(&i2cConfig);
    
    TINYPROTOCOL_Initialize();
    TINYPROTOCOL_RegisterTelemetryChannel(SYSTEM_STATUS_ID, SystemStatusRespBuf.bytes, sizeof(SystemStatusRespBuf.bytes));
    TINYPROTOCOL_RegisterTelemetryChannel(HEALTH_CHECK_ID, pds_fw_hash, sizeof(pds_fw_hash));
    TINYPROTOCOL_RegisterTelemetryChannel(CONVERTER_MONITOR_ID, RoutineCycle_GetVoltageConvData(), CONVERTER_MONITOR_DATA_LEN);

    TINYPROTOCOL_RegisterTelecommand(REBOOT_ID, REBOOT_CMD_LEN);
}

//***************************Private Functions Implementations***************************************************
void I2C_Proc_RX_Data(uint8_t data) {
    TINYPROTOCOL_ParseByte(&protocolConfig, data);
}

int16_t ProcessTelecommand(uint8_t command, const uint8_t* buffer, uint8_t size) {
    switch (command) {
        case REBOOT_ID:
            RoutineCycle_Reboot();
            break;

        default:
            break;
    }
    return ETINYPROTOCOL_SUCCESS;
}

int16_t ProcessTelemetryRequest(uint8_t request) {
    switch(request) {
        case SYSTEM_STATUS_ID:
            SystemStatusRespBuf.runtime++;      // TODO get actual system runtime
            break;
        
        case HEALTH_CHECK_ID:
            heartbeat_msgs_recieved = false;
            break;

        default:
            break;
    }
    // // todo move this function inside tiny embededded
    return SendTelemetryResponse();
}

int16_t SendTelemetryResponse() {
    uint8_t bytes[TINYPROTOCOL_MAX_PACKET_SIZE];
    uint8_t count = 0;
    uint8_t* pbyte = bytes;

    while(TINYPROTOCOL_TelemetryBytesLeft() > 0) {
        int16_t result = TINYPROTOCOL_ReadNextTelemetryByte(pbyte);
        if (result == ETINYPROTOCOL_SUCCESS) {
            pbyte++;
            count++;
        } else {
            return result;
        }
    }
    
    transmitI2C(bytes, count);
    return ETINYPROTOCOL_SUCCESS;
}
