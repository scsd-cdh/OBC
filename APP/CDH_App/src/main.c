#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <ulog/ulog.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <tinyprotocol.h>
#include <bms_types.h>
#include <math.h>

struct i2c_dt_spec dev = I2C_DT_SPEC_GET(DT_NODELABEL(bms));
int16_t TINYPROTOCOL_WriteBufferToSlave(uint16_t slave_addr, const uint8_t* p_buffer, uint8_t length)
{
    dev.addr = slave_addr;
    return (int16_t)i2c_write_dt(&dev, p_buffer, length);
}

const struct TINYPROTOCOL_Config cfg = {
    .TINYPROTOCOL_WriteBufferToSlave = TINYPROTOCOL_WriteBufferToSlave
};

// --- Thermistor Constants (MUST MATCH PICO SIMULATOR) ---
const float THERMISTOR_NOMINAL = 10000; // 10k at 25C
const float TEMPERATURE_NOMINAL = 25;
const float BETA_COEFFICIENT = 3950;
const float SERIES_RESISTOR = 10000;

// --- Helper: Convert ADC Value back to Temperature ---
float adcToTemperature(uint16_t adcVal) {
    // 1. Handle edge cases to prevent division by zero
    // If ADC is 4095, resistance is infinite (open circuit), temp is essentially absolute zero
    if (adcVal >= 4095) return -273.15;
    if (adcVal == 0) return 999.0; // Short circuit (very hot)

    // 2. Calculate Resistance from ADC value
    // Derived from Voltage Divider Rule: V_out = V_in * (R_therm / (R_series + R_therm))
    // Rearranged: R_therm = R_series * (ADC / (ADC_MAX - ADC))
    float resistance = SERIES_RESISTOR * ((float)adcVal / (4095.0 - (float)adcVal));

    // 3. Apply Beta Equation to get Kelvin
    // 1/T = 1/T0 + (1/B) * ln(R/R0)
    float steinhart;
    steinhart = resistance / THERMISTOR_NOMINAL;       // (R/Ro)
    steinhart = log(steinhart);                        // ln(R/Ro)
    steinhart /= BETA_COEFFICIENT;                     // 1/B * ln(R/Ro)
    steinhart += 1.0 / (TEMPERATURE_NOMINAL + 273.15); // + 1/To
    steinhart = 1.0 / steinhart;                       // Invert to get Kelvin

    // 4. Convert Kelvin to Celsius
    float tempC = steinhart - 273.15;

    return tempC;
}

int main(void)
{
    TINYPROTOCOL_Initialize();
    SystemStatusResp_t p_buf;
    uint8_t sExtADCBuffer03[8] = {};
    uint8_t sExtADCBuffer47[8] = {};
    uint16_t adc_vals[8] = {};
    while (1) {
        k_msleep(3000);
        TINYPROTOCOL_SendTelemetryRequest(BMS_SLAVE_ADDR, &cfg, BMS_THERMISTOR03_DATA_ID);
        k_msleep(2000);
        i2c_read_dt(&dev, sExtADCBuffer03, 8);
        for (int i = 0; i < 4; ++i) {
            adc_vals[i] = (uint16_t)sExtADCBuffer03[i] << 4 | (uint16_t)sExtADCBuffer03[i + 1];
        }
//        k_msleep(3000);

    }
}
