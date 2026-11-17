#pragma once
#include <stdint.h>
#include <lfp/header.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>

/**
 * Receive a single LFP message over I2C.
 *  NOTE: Alex has mentioned this needs some work to be more robust
 *
 * @param p_device i2c device to read from
 * @param p_out_header received header
 * @param p_out_lfp_buf received body
 * @param out_lfp_buf_size received body buffer size
 * @param max_duration maximum time to wait for a message
 * @return number of bytes received, or negative error code on failure
 */
lfp_size_or_fail_t lfp_i2c_single_receive(
    const struct i2c_dt_spec * p_device,
    lfp_header_t * p_out_header,
    uint8_t * p_out_lfp_buf,
    int out_lfp_buf_size,
    k_timeout_t max_duration
);
