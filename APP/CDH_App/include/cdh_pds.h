#pragma once

/**
 * Get PDS system status over i2c. Optionally print. Optionally mocked depending on MOCK_PDS. Puts the response on the telemetry queue.
 */
void request_pds_system_status(void);

/**
 * Get PDS health check over i2c. Optionally print. Optionally mocked depending on MOCK_PDS. Puts the response on the telemetry queue.
 */
void request_pds_health_check(void);

/**
 * Get PDS converter monitor over i2c. Optionally print. Optionally mocked depending on MOCK_PDS. Puts the response on the telemetry queue.
 */
void request_pds_converter_monitor(void);
