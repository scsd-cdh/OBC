//
// Created by Mohamed on 2025-06-04.
//

#include "commsdrive.h"
#include "tinyprotocol.h"

#include "hal_i2c_m_sync.h"
#include "driver_init.h"

//***************************Private functions definitions***************************************************

static int16_t ProcessTelecommand(uint8_t command, const uint8_t* buffer, uint8_t size);
static int16_t ProcessTelemetryRequest(uint8_t request);
static int16_t SendTelemetryResponse();

//***************************Variables***************************************************

const static struct TINYPROTOCOL_Config protocolConfig =
{
    .TINYPROTOCOL_ProcessTelecommand = ProcessTelecommand,
    .TINYPROTOCOL_ProcessTelemetryRequest = ProcessTelemetryRequest,
    .TINYPROTOCOL_WriteBuffer = transmitI2C
};

static struct io_descriptor *Mas_I2C_0_io;
static struct io_descriptor *Red_I2C_0_io;

void CommsDrive_Init(int16_t slaveAddr)
{
	i2c_m_sync_enable(&Mas_I2C_0);
    i2c_m_sync_enable(&Red_I2C_2);

    i2c_m_sync_set_slaveaddr(&Mas_I2C_0, slaveAddr, I2C_M_SEVEN);
	i2c_m_sync_set_slaveaddr(&Red_I2C_2, slaveAddr, I2C_M_SEVEN);
    

	i2c_m_sync_get_io_descriptor(&Mas_I2C_0, &Mas_I2C_0_io);
    i2c_m_sync_get_io_descriptor(&Red_I2C_2, &Red_I2C_0_io);

    


	// io_write(Mas_I2C_0_io, (uint8_t *)"Hello World!", 12);
}

void CommsDrive_Process()
{
    io_read(&Mas_I2C_0_io, uint8_t *const buf, const uint16_t length)
    io_read(&Red_I2C_0_io, uint8_t *const buf, const uint16_t length)

	i2c_m_sync_read_packet(&Mas_I2C_0, &Mas_I2C_0_io, &rxBuffer, 1);
	i2c_m_sync_read_packet(&Red_I2C_2, &Red_I2C_0_io, &rxBuffer, 1);
}