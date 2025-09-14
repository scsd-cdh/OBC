/**
 * @file
 * @brief Battery Management System (BMS) application interface for MSP430.
 *
 * @details
 * This module provides hardware abstraction and telemetry/telecommand definitions for
 * battery monitoring and control. It includes ADC channel assignments, protection flag
 * mapping, and protocol response structures for system status, current, voltage, and
 * heater control. Initialization routines set up all hardware and register protocol
 * channels for communication with the central data handler.
 *
 * @author Brendan Kelly
 */

#ifndef BMS_H
#define BMS_H

#include "tinyprotocol.h"
#include "i2c.h"

/**
 * @def SYSTEM_STATUS_RESP_LEN
 * @brief Number of bytes in system status telemetry response.
 */
#define SYSTEM_STATUS_RESP_LEN   5

/**
 * @def CURRENT_VOLTAGE_RESP_LEN
 * @brief Number of bytes in current or voltage telemetry response.
 */
#define CURRENT_VOLTAGE_RESP_LEN 4

/**
 * @def ADC_NUM_CHANNELS
 * @brief Number of ADC channels used for battery monitoring.
 */
#define ADC_NUM_CHANNELS         8

/**
 * @enum BMS_TelemetryRequestCmdId
 * @brief Command IDs for BMS telemetry requests.
 * @note If you don't manually assign these values, bad things happen that I don't understand.
 */
enum BMS_TelemetryRequestCmdId {
  BMS_SYSTEM_STATUS_ID        = 1,  /**< System status telemetry */
  BMS_HEALTH_CHECK_ID         = 2,  /**< Health check telemetry */
  BMS_FLAG_ID                 = 3,  /**< Protection flags telemetry */
  BMS_CURRENT_DRAW_ID         = 4,  /**< Discharge current telemetry */
  BMS_CURRENT_CHARGE_ID       = 5,  /**< Charge current telemetry */
  BMS_VOLTAGE_BATTERY1_ID     = 6,  /**< Battery 1 voltage telemetry */
  BMS_VOLTAGE_BATTERY2_ID     = 7,  /**< Battery 2 voltage telemetry */
  BMS_VOLTAGE_COMBINED_ID     = 8,  /**< Combined battery voltage telemetry */
  BMS_HEATERS_CONTROLLER_ID   = 9,  /**< Heater controller telecommand */
  BMS_THERMISTOR03_DATA_ID    = 10, /**< Thermistor 0-3 external ADC telemetry */
  BMS_THERMISTOR47_DATA_ID    = 11, /**< Thermistor 4-7 external ADC telemetry */
};

/**
 * @union SystemStatusResp_t
 * @brief Telemetry response for system status.
 */
typedef union SystemStatusResp {
    struct {
        uint32_t runtime;    /**< System runtime in seconds */
        uint8_t fw_version;  /**< Firmware version */
    };
    uint8_t buffer[SYSTEM_STATUS_RESP_LEN]; /**< Raw buffer for protocol transmission */
} SystemStatusResp_t;

/**
 * @union CurrentResp_t
 * @brief Telemetry response for current sensing.
 */
typedef union CurrentResp {
    struct {
        uint16_t isense1; /**< Current sensor 1 reading */
        uint16_t isense2; /**< Current sensor 2 reading */
    };
    uint8_t buffer[CURRENT_VOLTAGE_RESP_LEN]; /**< Raw buffer for protocol transmission */
} CurrentResp_t;

/**
 * @union VoltageResp_t
 * @brief Telemetry response for voltage sensing.
 */
typedef union VoltageResp {
    struct {
        uint16_t vcell_a; /**< Voltage cell A reading */
        uint16_t vcell_b; /**< Voltage cell B reading */
    };
    uint8_t buffer[CURRENT_VOLTAGE_RESP_LEN]; /**< Raw buffer for protocol transmission */
} VoltageResp_t;

/**
 * @union CombinedVoltageResp_t
 * @brief Telemetry response for combined battery voltages.
 */
typedef union CombinedVoltageResp {
    struct {
        uint16_t vbatt1; /**< Battery 1 voltage */
        uint16_t vbatt2; /**< Battery 2 voltage */
    };
    uint8_t buffer[CURRENT_VOLTAGE_RESP_LEN]; /**< Raw buffer for protocol transmission */
} CombinedVoltageResp_t;

/**
 * @union Flag_t
 * @brief Telemetry response for protection flags.
 */
typedef union Flag {
    uint16_t val;      /**< Packed flag value (bitfield) */
    uint8_t buffer[2]; /**< Raw buffer for protocol transmission */
} Flag_t;

/**
 * @brief Initializes the Battery Management System hardware and communication.
 *
 * Sets up ADCs, GPIOs, PWM, RTC, and registers telemetry/telecommand channels.
 *  
 * @note All logic is handled in RTC interrupt and tinyprotocol callbacks.
 * @code
 * int main(void)
 * {
 *     initBMS(); // Initialize all BMS hardware and protocol
 *     __bis_SR_register(LPM0_bits + GIE); 
 *     return 0;
 * }
 * @endcode
 */
void initBMS();

#endif