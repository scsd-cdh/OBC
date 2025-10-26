//
// Created by Mohamed on 2025-06-04.
//

#include "commsdrive.h"
#include "tinyprotocol.h"

#include "hal_i2c_m_sync.h"
#include "driver_init.h"

static struct io_descriptor *Mas_I2C_0_io;

int16_t transmitI2C(const uint8_t* buffer, uint8_t size)
{
	return io_write(Mas_I2C_0_io, buffer, size);
}

const static struct TINYPROTOCOL_Config protocolConfig =
{
	.TINYPROTOCOL_WriteBuffer = transmitI2C
};

void CommsDrive_Init(int16_t slaveAddr)
{
	i2c_m_sync_enable(&Mas_I2C_0);
    i2c_m_sync_set_slaveaddr(&Mas_I2C_0, slaveAddr, I2C_M_SEVEN);
	i2c_m_sync_get_io_descriptor(&Mas_I2C_0, &Mas_I2C_0_io);
	TINYPROTOCOL_Initialize();
}

void CommsDrive_ProcessTeleRequest(uint8_t slaveAddr, uint8_t request, uint8_t* rx_buf, uint8_t len)
{
	i2c_m_sync_set_slaveaddr(&Mas_I2C_0, slaveAddr, I2C_M_SEVEN);
	TINYPROTOCOL_SendTelemetryRequest(&protocolConfig, request);
	struct _i2c_m_msg msg;
	msg.addr = slaveAddr;
	msg.buffer = rx_buf;
	msg.flags = I2C_M_RD;
	msg.len = len;
	i2c_m_sync_transfer(&Mas_I2C_0, &msg);
}

void CommsDrive_ProcessTeleCommand(uint8_t slaveAddr, uint8_t cmd, const uint8_t* buffer, uint8_t size)
{
	i2c_m_sync_set_slaveaddr(&Mas_I2C_0, slaveAddr, I2C_M_SEVEN);
	TINYPROTOCOL_SendTelecommand(&protocolConfig, cmd, buffer, size);
}

