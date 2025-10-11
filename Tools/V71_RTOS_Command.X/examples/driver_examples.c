/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_examples.h"
#include "driver_init.h"
#include "utils.h"

void command_example(void)
{
	struct io_descriptor *command_io;

	i2c_m_sync_get_io_descriptor(&command, &command_io);
	i2c_m_sync_enable(&command);
	i2c_m_sync_set_slaveaddr(&command, 0x12, I2C_M_SEVEN);
	io_write(command_io, (uint8_t *)"Hello World!", 12);
}

/**
 * Example of using serial to write "Hello World" using the IO abstraction.
 */
void serial_example(void)
{
	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&serial, &io);
	usart_sync_enable(&serial);

	io_write(io, (uint8_t *)"Hello World!", 12);
}
