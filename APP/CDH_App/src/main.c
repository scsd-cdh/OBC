#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <ulog/ulog.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <tinyprotocol.h>
#include <bms_types.h>

struct i2c_dt_spec dev = I2C_DT_SPEC_GET(DT_NODELABEL(bms));
int16_t TINYPROTOCOL_WriteBufferToSlave(uint16_t slave_addr, const uint8_t* p_buffer, uint8_t length)
{
    dev.addr = slave_addr;
    return (int16_t)i2c_write_dt(&dev, p_buffer, length);
}

const struct TINYPROTOCOL_Config cfg = {
    .TINYPROTOCOL_WriteBufferToSlave = TINYPROTOCOL_WriteBufferToSlave
};

int main(void)
{
    TINYPROTOCOL_Initialize();
    SystemStatusResp_t p_buf;
    uint8_t sExtADCBuffer03[8] = {};
    uint8_t sExtADCBuffer47[8] = {};
    while (1) {
        k_msleep(3000);
        TINYPROTOCOL_SendTelemetryRequest(BMS_SLAVE_ADDR, &cfg, BMS_THERMISTOR03_DATA_ID);
        k_msleep(2000);
        i2c_read_dt(&dev, sExtADCBuffer03, 8);
    }
}
