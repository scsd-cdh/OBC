
#ifndef _APP_COMM_
#define _APP_COMM_

#include <stdint.h>

#include "comm.h"
#include "i2c.h"
#include "tinyprotocol.h"

//*******************************************************************************
// Buffers **********************************************************************
//*******************************************************************************

// Buffers for holding command arguments received from the master - TODO remove
static uint8_t SystemStatusBuf[SYSTEM_STATUS_CMD_LEN] = {0};  /**< Buffer for system status command arguments */
static uint8_t HealthCheckBuf[HEALTH_CHECK_CMD_LEN] = {0};    /**< Buffer for health check command arguments */
static uint8_t RebootBuf[REBOOT_CMD_LEN] = {0};               /**< Buffer for reboot command arguments */
static uint8_t ConverterMonitorBuf[CONVERTER_MONITOR_CMD_LEN] = {0}; /**< Buffer for converter monitor command arguments */
static uint8_t TelecommandAckBuf[TELECOMMAND_ACK_CMD_LEN] = {0};     /**< Buffer for telecommand acknowledgement arguments */

// Buffers for holding response data to be sent back to the master
static uint8_t SystemStatusRespBuf[SYSTEM_STATUS_RESP_LEN] = {0};    /**< Buffer for system status response data */
static uint8_t HealthCheckRespBuf[HEALTH_CHECK_RESP_LEN] = {0};      /**< Buffer for health check response data */
static uint8_t RebootRespBuf[REBOOT_RESP_LEN] = {0};                 /**< Buffer for reboot response data */
static uint8_t ConverterMonitorRespBuf[CONVERTER_MONITOR_RESP_LEN] = {0}; /**< Buffer for converter monitor response data */
static uint8_t TelecommandAckRespBuf[TELECOMMAND_ACK_RESP_LEN] = {0};     /**< Buffer for telecommand acknowledgement response */

enum CustomProtocolTelecommand {
  SYSTEM_STATUS_ID = TINYPROTOCOL_TC_RESERVED,
  HEALTH_CHECK_ID,
  REBOOT_ID,
  CONVERTER_MONITOR_ID,
  TELECOMMAND_ACK_ID,
  BINGO_BONGO_ID
};

/**
 * @brief Initalize host communication
 */
void InitAppComm(void);

#endif // _APP_COMM_
