
#include "AppComm.h"
#include "bsp.h"

//***************************Private functions definitions***************************************************

static int16_t SendTelemetryResponse();
static int16_t ProcessTelemetryRequest(uint8_t command);
static int16_t ProcessTelecommand(uint8_t command, const uint8_t* buffer, uint8_t size);

static const struct TINYPROTOCOL_Config protocolConfig = 
{
    .TINYPROTOCOL_ProcessTelecommand = ProcessTelecommand,
    .TINYPROTOCOL_ProcessTelemetryRequest = ProcessTelemetryRequest, 
    .TINYPROTOCOL_WriteBuffer = transmitI2C
};

static void I2C_Proc_RX_Data(uint8_t data);

/**
 * @brief Sets up the system status response buffer.
 *
 * @param SystemStatusRespBuf Buffer to hold the system status response data
 * @param SystemStatusBuf Buffer holding system status command arguments
 */
static void setup_system_status_test(uint8_t *SystemStatusRespBuf, uint8_t *SystemStatusBuf);

/**
 * @brief Sets up the health check response buffer.
 *
 * @param HealthCheckRespBuf Buffer to hold the health check response data
 * @param HealthCheckBuf Buffer holding health check command arguments
 */
static void setup_health_check_test(uint8_t *HealthCheckRespBuf, uint8_t *HealthCheckBuf);

/**
 * @brief Sets up the converter monitor response buffer.
 *
 * @param ConverterMonitorRespBuf Buffer to hold the converter monitor response data
 */
static void setup_converter_monitor_test(uint8_t *ConverterMonitorRespBuf);

/**
 * @brief Sets up the telecommand acknowledgement response buffer.
 *
 * @param TelecommandAckRespBuf Buffer to hold the telecommand acknowledgement response data
 */
static void setup_telecom_acknowledge_test(uint8_t *TelecommandAckRespBuf);


//***************************Public functions***************************************************
void InitAppComm(void) {
    sI2cConfigCb_t i2cConfig = {
        .Rx_Proc_Data = I2C_Proc_RX_Data,
        .slave_addr = SLAVE_ADDR
    };

    initI2C(&i2cConfig);
    
    TINYPROTOCOL_Initialize();

    TINYPROTOCOL_RegisterTelemetryChannel(SYSTEM_STATUS_ID, SystemStatusRespBuf, sizeof(SystemStatusRespBuf));
    TINYPROTOCOL_RegisterTelemetryChannel(HEALTH_CHECK_ID, HealthCheckRespBuf, sizeof(HealthCheckRespBuf));
    TINYPROTOCOL_RegisterTelemetryChannel(REBOOT_ID, RebootRespBuf, sizeof(RebootRespBuf));
    TINYPROTOCOL_RegisterTelemetryChannel(CONVERTER_MONITOR_ID, ConverterMonitorRespBuf, sizeof(ConverterMonitorRespBuf));
    TINYPROTOCOL_RegisterTelemetryChannel(TELECOMMAND_ACK_ID, TelecommandAckRespBuf, sizeof(TelecommandAckRespBuf));
    TINYPROTOCOL_RegisterTelemetryChannel(BINGO_BONGO_ID, NULL, 0); // TODO
}

//***************************Private Functions Implementations***************************************************
void I2C_Proc_RX_Data(uint8_t data) {
    TINYPROTOCOL_ParseByte(&protocolConfig, data);
}

int16_t ProcessTelecommand(uint8_t command, const uint8_t* buffer, uint8_t size) {
  switch (command) {
    // TODO
    default:
        break;
  }
  return ETINYPROTOCOL_SUCCESS;
}

int16_t ProcessTelemetryRequest(uint8_t command) {
    switch(command) {
        case SYSTEM_STATUS_ID:
            setup_system_status_test(SystemStatusRespBuf,SystemStatusBuf);
            break;
        case HEALTH_CHECK_ID:
            setup_health_check_test(HealthCheckRespBuf, HealthCheckBuf);
            break;
        case REBOOT_ID:
            // TODO move to command
            break;
        case CONVERTER_MONITOR_ID:
            setup_converter_monitor_test(ConverterMonitorRespBuf);
            break;
        case TELECOMMAND_ACK_ID:
            setup_telecom_acknowledge_test(TelecommandAckRespBuf);
            break;
    }
    // todo move this function inside tiny embededded
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

//******************************************************************************
// Test Functions **************************************************************
//******************************************************************************
void setup_system_status_test(uint8_t *SystemStatusRespBuf, uint8_t *SystemStatusBuf)
{
  // FUNCTION CALL TO RETRIEVE SYSTEM STATUS GOES HERE -> fills SystemStatusRespBuf (len=5)
  SystemStatusRespBuf[0] = 'S';
  SystemStatusRespBuf[1] = 'y';
  SystemStatusRespBuf[2] = 's';
  SystemStatusRespBuf[3] = SystemStatusBuf[0];  // Whichever byte was sent as an arg
  SystemStatusRespBuf[4] = SystemStatusBuf[1];
}

void setup_health_check_test(uint8_t *HealthCheckRespBuf, uint8_t *HealthCheckBuf) {
    // Fills HealthCheckRespBuf (len = 16)
    const char testMessage[] = "TestingHealth";
    int i;
    for (i = 0; i < 12; i++) {
        HealthCheckRespBuf[i] = testMessage[i];
    }
    HealthCheckRespBuf[13] = HealthCheckBuf[0];
    HealthCheckRespBuf[14] = HealthCheckBuf[1];
    HealthCheckRespBuf[15] = '2';
}

void setup_converter_monitor_test(uint8_t *ConverterMonitorRespBuf) {
    // Fills ConverterMonitorRespBuf (len = 3)
    ConverterMonitorRespBuf[0] = 'H';
    ConverterMonitorRespBuf[1] = 'i';
    ConverterMonitorRespBuf[2] = '4';
}

void setup_telecom_acknowledge_test(uint8_t *TelecommandAckRespBuf) {
    // Fills TelecommandAckRespBuf (len = 2)
    TelecommandAckRespBuf[0] = 'O';
    TelecommandAckRespBuf[1] = '5';
}
