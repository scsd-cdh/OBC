#pragma once
#include <stdint.h>
#include <lfp/header.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>


lfp_size_or_fail_t lfp_i2c_single_receive(
    const struct i2c_dt_spec * p_device,
    lfp_header_t * p_out_header,
    uint8_t * p_out_lfp_buf,
    int out_lfp_buf_size,
    k_timeout_t max_duration
);
