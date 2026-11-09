#pragma once

/**
 * Get BMS system status over i2c. Optionally print. Optionally mocked depending on MOCK_BMS. Puts the response on the telemetry queue.
 */
void request_bms_system_status(void);

/**
 * Get BMS power status over i2c. Optionally print. Optionally mocked depending on MOCK_BMS.
 * Decides whether to go into safe mode depending on battery voltage thresholds.
 * If not in safe mode, puts the response on the telemetry queue.
 */
void request_bms_power_status(void);

/**
 * Get BMS temperature status over i2c. Optionally print. Optionally mocked depending on MOCK_BMS. Puts the response on the telemetry queue.
 */
void request_bms_temperature_status(void);
