/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_examples.h"

#include "commsdrive.h"
#include "driver_init.h"
#include "utils.h"

/**
 * Example of using MRAM_SPI_0 to write "Hello World" using the IO abstraction.
 */
static uint8_t example_MRAM_SPI_0[12] = "Hello World!";

void MRAM_SPI_0_example(void)
{
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&MRAM_SPI_0, &io);

	spi_m_sync_enable(&MRAM_SPI_0);
	io_write(io, example_MRAM_SPI_0, 12);
}

void Mas_I2C_0_example(void)
{
	struct io_descriptor *Mas_I2C_0_io;

	i2c_m_sync_get_io_descriptor(&Mas_I2C_0, &Mas_I2C_0_io);
	i2c_m_sync_enable(&Mas_I2C_0);
	i2c_m_sync_set_slaveaddr(&Mas_I2C_0, 0x12, I2C_M_SEVEN);
	io_write(Mas_I2C_0_io, (uint8_t *)"Hello World!", 12);
}

void Red_I2C_2_example(void)
{
	struct io_descriptor *Red_I2C_2_io;

	i2c_m_sync_get_io_descriptor(&Red_I2C_2, &Red_I2C_2_io);
	i2c_m_sync_enable(&Red_I2C_2);
	i2c_m_sync_set_slaveaddr(&Red_I2C_2, 0x12, I2C_M_SEVEN);
	io_write(Red_I2C_2_io, (uint8_t *)"Hello World!", 12);
}

/**
 * Example of using Debug_USART_0 to write "Hello World" using the IO abstraction.
 */
void Debug_USART_0_example(void)
{
	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&Debug_USART_0, &io);
	usart_sync_enable(&Debug_USART_0);

	io_write(io, (uint8_t *)"Hello World!", 12);
}

/**
 * Example of using LVDS_USART_1 to write "Hello World" using the IO abstraction.
 */
void LVDS_USART_1_example(void)
{
	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&LVDS_USART_1, &io);
	usart_sync_enable(&LVDS_USART_1);

	io_write(io, (uint8_t *)"Hello World!", 12);
}

/**
 * Example of using LVDS2_USART_2 to write "Hello World" using the IO abstraction.
 */
void LVDS2_USART_2_example(void)
{
	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&LVDS2_USART_2, &io);
	usart_sync_enable(&LVDS2_USART_2);

	io_write(io, (uint8_t *)"Hello World!", 12);
}
