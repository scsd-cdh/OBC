//
//

#include "commsdrive.h"
#include "tinyprotocol.h"
#include "CDH/I2C0_wrapper.h"

static int16_t s_transmit_i2c(uint16_t slave_addr, const uint8_t* p_buffer, uint8_t length)
{
	return (int16_t)mas_i2c_0_write(slave_addr, p_buffer, length);
}

const static struct TINYPROTOCOL_Config s_protocol_config =
{
	.TINYPROTOCOL_WriteBufferToSlave = s_transmit_i2c,
};

void commsdrive_init(void)
{
	mas_i2c_0_init();
	TINYPROTOCOL_Initialize();
}

int32_t commsdrive_process_telerequest(uint8_t slave_addr, uint8_t request_id, uint8_t* p_rx_buffer, uint8_t length)
{
	// This does an I2C write via our callback I2C_transfer
	int16_t err = TINYPROTOCOL_SendTelemetryRequest(slave_addr, &s_protocol_config, request_id);
	if (err < -1) { // -1 is "ACK" which is not an error. lol. I2C_ACK
		return err;
	}
	return mas_i2c_0_read(slave_addr, p_rx_buffer, length);
}

int32_t commsdrive_process_telecommand(uint8_t slave_addr, uint8_t cmd_id, const uint8_t* p_buffer, uint8_t size)
{
	mas_i2c_0_set_slave_addr(slave_addr);
	return TINYPROTOCOL_SendTelecommand(slave_addr, &s_protocol_config, cmd_id, p_buffer, size);
}
