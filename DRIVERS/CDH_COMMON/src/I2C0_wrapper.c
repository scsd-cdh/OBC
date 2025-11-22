//
// Created by EXT-BXK09021 on 11/20/2025.
//

#include "CDH/I2C0_wrapper.h"

#include "hal_gpio.h"
#include "hal_i2c_m_sync.h"
#include "hpl_pmc.h"
#include "CDH/pins.h"

static struct i2c_m_sync_desc s_i2c_master;
static struct io_descriptor *p_i2c_io;

void mas_i2c_0_init(void)
{
    _pmc_enable_periph_clock(ID_TWIHS0);
    i2c_m_sync_init(&s_i2c_master, TWIHS0);
    gpio_set_pin_function(PA4, MUX_PA4A_TWIHS0_TWCK0);
    gpio_set_pin_function(PA3, MUX_PA3A_TWIHS0_TWD0);
    i2c_m_sync_enable(&s_i2c_master);
    i2c_m_sync_get_io_descriptor(&s_i2c_master, &p_i2c_io);
}

void mas_i2c_0_set_slave_addr(int16_t slave_addr)
{
    i2c_m_sync_set_slaveaddr(&s_i2c_master, slave_addr, I2C_M_SEVEN);
}

int32_t mas_i2c_0_write(uint16_t slave_addr, const uint8_t* p_tx_buffer, uint8_t length)
{
    struct _i2c_m_msg msg;
    msg.addr = slave_addr;
    msg.buffer = p_tx_buffer;
    msg.flags = 0; // Not sure why there isn't I2C_M_WR, just checks if the flag is set for read
    msg.len = length;
    return i2c_m_sync_transfer(&s_i2c_master, &msg);
}

int32_t mas_i2c_0_read(int16_t slave_addr, uint8_t* p_rx_buffer, uint8_t length)
{
    struct _i2c_m_msg msg;
    msg.addr = slave_addr;
    msg.buffer = p_rx_buffer;
    msg.flags = I2C_M_RD;
    msg.len = length;
    return i2c_m_sync_transfer(&s_i2c_master, &msg);
}