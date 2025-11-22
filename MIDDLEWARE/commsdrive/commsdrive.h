//
// Created by Mohamed and Brendan on 2025-06-04.
//

#ifndef COMMS_DRIVER_H
#define COMMS_DRIVER_H

#include <stdint.h>

/**
 * @brief Initializes the Comms Drive module. Sets up tinyprotocol and initializes I2C config and hardware.
 */
void commsdrive_init(void);

/**
 * @brief Processes a tinyprotocol telemetry request.
 *        Sends a request for data to the slave via tinyprotocol (i2c write) and reads back the response (i2c read).
 *        Requests are identified by their request ID and are described in the slave ICD.
 * @param slave_addr The I2C address of the slave device.
 * @param request_id The telemetry request ID.
 * @param p_rx_buffer out parameter buffer to store received data.
 * @param length The length of the expected data.
 * @return 32 bit error code. 0 indicates success. Error codes are I2C error codes defined in hal_i2c_m_sync.h
 */
int32_t commsdrive_process_telerequest(uint8_t slave_addr, uint8_t request_id, uint8_t* p_rx_buffer, uint8_t length);

/**
 * @brief Processes a tinyprotocol telecommand.
 *        Telecommands are identified by their command ID and are described in the slave ICD.
 *        Telecommands do not return any data back from the slave and result in some kind of action being performed.
 * @param slave_addr The I2C address of the slave device.
 * @param cmd_id The command ID.
 * @param p_buffer The buffer containing the telecommand data, parameters, etc...
 * @param size The size of the telecommand data.
 * @return 32 bit error code. 0 indicates success. Error codes are I2C error codes defined in hal_i2c_m_sync.h
 */
int32_t commsdrive_process_telecommand(uint8_t slave_addr, uint8_t cmd_id, const uint8_t* p_buffer, uint8_t size);

#endif //COMMS_DRIVER_H
