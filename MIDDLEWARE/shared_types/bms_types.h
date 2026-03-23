//
// Created by brendan on 2/1/26.
//

#ifndef OBC_BMS_TYPES_H
#define OBC_BMS_TYPES_H

#include <stdint.h>

#define BMS_SLAVE_ADDR 0x09

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
  BMS_SYSTEM_STATUS_ID        = 0x2,  /**< System status telemetry */
  BMS_HEALTH_CHECK_ID         = 0x4,  /**< Health check telemetry */
  BMS_FLAG_ID                 = 0x6,  /**< Protection flags telemetry */
  BMS_CURRENT_DRAW_ID         = 0x8,  /**< Discharge current telemetry */
  BMS_CURRENT_CHARGE_ID       = 0xA,  /**< Charge current telemetry */
  BMS_VOLTAGE_BATTERY1_ID     = 0xC,  /**< Battery 1 voltage telemetry */
  BMS_VOLTAGE_BATTERY2_ID     = 0xE,  /**< Battery 2 voltage telemetry */
  BMS_VOLTAGE_COMBINED_ID     = 0x10,  /**< Combined battery voltage telemetry */
  BMS_HEATERS_CONTROLLER_ID   = 0x12,  /**< Heater controller telecommand */
  BMS_THERMISTOR03_DATA_ID    = 0x14, /**< Thermistor external ADC telemetry */
  BMS_THERMISTOR47_DATA_ID    = 0x16, /**< Thermistor external ADC telemetry */
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

#endif //OBC_BMS_TYPES_H