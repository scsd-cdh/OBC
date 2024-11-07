

#include <stdint.h>
#include "comm.h"
#include "i2c.h"

//*******************************************************************************
// Buffers **********************************************************************
//*******************************************************************************

// Buffers for holding command arguments received from the master
uint8_t SystemStatusBuf[SYSTEM_STATUS_CMD_LEN] = {0};  /**< Buffer for system status command arguments */
uint8_t HealthCheckBuf[HEALTH_CHECK_CMD_LEN] = {0};    /**< Buffer for health check command arguments */
uint8_t RebootBuf[REBOOT_CMD_LEN] = {0};               /**< Buffer for reboot command arguments */
uint8_t ConverterMonitorBuf[CONVERTER_MONITOR_CMD_LEN] = {0}; /**< Buffer for converter monitor command arguments */
uint8_t TelecommandAckBuf[TELECOMMAND_ACK_CMD_LEN] = {0};     /**< Buffer for telecommand acknowledgement arguments */

// Buffers for holding response data to be sent back to the master
uint8_t SystemStatusRespBuf[SYSTEM_STATUS_RESP_LEN] = {0};    /**< Buffer for system status response data */
uint8_t HealthCheckRespBuf[HEALTH_CHECK_RESP_LEN] = {0};      /**< Buffer for health check response data */
uint8_t RebootRespBuf[REBOOT_RESP_LEN] = {0};                 /**< Buffer for reboot response data */
uint8_t ConverterMonitorRespBuf[CONVERTER_MONITOR_RESP_LEN] = {0}; /**< Buffer for converter monitor response data */
uint8_t TelecommandAckRespBuf[TELECOMMAND_ACK_RESP_LEN] = {0};     /**< Buffer for telecommand acknowledgement response */

/**
 * @enum appStatus
 * @brief Defines the states for the application, representing different commands or actions
 *        that the MSP430 device will perform in response to master commands.
 */
typedef enum appStatusEnum {
    idle,                    /**< Idle state */
    system_status,           /**< System status command state */
    health_check,            /**< Health check command state */
    reboot,                  /**< Reboot command state */
    telecom_acknowledge,     /**< Telecommand acknowledge state */
    converter_monitor        /**< Converter monitor command state */
} appStatus;

static volatile appStatus app =  idle; /**< Tracks the current state of the application */

/**
 * @brief Initalize host communication
 */
void InitAppComm(void);

/**
 * @brief Run the application state machine, determining the next action based
 *        on the current application state.
 */
void RunAppComm(void);

/**
 * @brief Send Host communication message via i2c
 */
void updateTransmissionBuffer(uint8_t *inputBuffer, uint8_t respLength);