//
// Created by EXT-BXK09021 on 11/20/2025.
//

#ifndef OBC_I2C_TWIHS0_WRAPPER_H
#define OBC_I2C_TWIHS0_WRAPPER_H
#include <stdint.h>

/**
 * @brief Initialize the TWIHS0 I2C master instance.
 *
 * Performs clock enable, i2c_m_sync initialization, port configuration,
 * enables the master, and obtains the IO descriptor.
 */
void mas_i2c_0_init(void);

/**
 * @brief Set the slave address for subsequent I2C operations.
 * @param slave_addr The 7-bit I2C slave address.
 */
void mas_i2c_0_set_slave_addr(int16_t slave_addr);

/**
 * @brief Write data to the I2C slave device at the specified slave address.
 * @param slave_addr The 7-bit I2C slave address.
 * @param p_tx_buffer to write
 * @param length length of buffer
 * @return 32 bit return code defined in hal_i2c_m_sync.h
 */
int32_t mas_i2c_0_write(uint16_t slave_addr, const uint8_t* p_tx_buffer, uint8_t length);

/**
 * @brief Read data from the I2C slave device at the specified slave address.
 * @param slave_addr The 7-bit I2C slave address.
 * @param p_rx_buffer Buffer to store received data.
 * @param length Length of data to read.
 * @return 32 bit return code defined in hal_i2c_m_sync.h
 */
int32_t mas_i2c_0_read(int16_t slave_addr, uint8_t* p_rx_buffer, uint8_t length);

#endif // OBC_I2C_TWIHS0_WRAPPER_H

