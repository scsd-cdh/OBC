#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <ulog/ulog.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <tinyprotocol.h>

#define SLAVE_ADDR 0x08

#define SYSTEM_STATUS_RESP_LEN   5

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

struct i2c_dt_spec dev = I2C_DT_SPEC_GET(DT_NODELABEL(bms));
int16_t TINYPROTOCOL_WriteBufferToSlave(uint16_t slave_addr, const uint8_t* p_buffer, uint8_t length)
{
    dev.addr = slave_addr;
    return (int16_t)i2c_write_dt(&dev, p_buffer, length);
}

const struct TINYPROTOCOL_Config cfg = {
    .TINYPROTOCOL_WriteBufferToSlave = TINYPROTOCOL_WriteBufferToSlave
};

typedef union SystemStatusResp {
    struct {
        uint32_t runtime;    /**< System runtime in seconds */
        uint8_t fw_version;  /**< Firmware version */
    };
    uint8_t buffer[SYSTEM_STATUS_RESP_LEN]; /**< Raw buffer for protocol transmission */
} SystemStatusResp_t;


int main(void)
{
    TINYPROTOCOL_Initialize();
    SystemStatusResp_t p_buf;

    // k_msleep(2000);
    while (1) {
        k_msleep(3000);
        TINYPROTOCOL_SendTelemetryRequest(SLAVE_ADDR, &cfg, BMS_SYSTEM_STATUS_ID);
        k_msleep(2000);
        i2c_read_dt(&dev, p_buf.buffer, SYSTEM_STATUS_RESP_LEN);
    }
}
