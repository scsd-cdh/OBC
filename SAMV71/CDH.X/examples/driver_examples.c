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

/**
 * Example of using SPI to write "Hello World" using the IO abstraction.
 */
static uint8_t example_SPI[12] = "Hello World!";

void SPI_example(void)
{
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI, &io);

	spi_m_sync_enable(&SPI);
	io_write(io, example_SPI, 12);
}

void I2C_Main_example(void)
{
	struct io_descriptor *I2C_Main_io;

	i2c_m_sync_get_io_descriptor(&I2C_Main, &I2C_Main_io);
	i2c_m_sync_enable(&I2C_Main);
	i2c_m_sync_set_slaveaddr(&I2C_Main, 0x12, I2C_M_SEVEN);
	io_write(I2C_Main_io, (uint8_t *)"Hello World!", 12);
}

void I2C_Backup_example(void)
{
	struct io_descriptor *I2C_Backup_io;

	i2c_m_sync_get_io_descriptor(&I2C_Backup, &I2C_Backup_io);
	i2c_m_sync_enable(&I2C_Backup);
	i2c_m_sync_set_slaveaddr(&I2C_Backup, 0x12, I2C_M_SEVEN);
	io_write(I2C_Backup_io, (uint8_t *)"Hello World!", 12);
}

/**
 * Example of using VCOM to write "Hello World" using the IO abstraction.
 *
 * Since the driver is asynchronous we need to use statically allocated memory for string
 * because driver initiates transfer and then returns before the transmission is completed.
 *
 * Once transfer has been completed the tx_cb function will be called.
 */

static uint8_t example_VCOM[12] = "Hello World!";

static void tx_cb_VCOM(const struct usart_async_descriptor *const io_descr)
{
	/* Transfer completed */
}

void VCOM_example(void)
{
	struct io_descriptor *io;

	usart_async_register_callback(&VCOM, USART_ASYNC_TXC_CB, tx_cb_VCOM);
	/*usart_async_register_callback(&VCOM, USART_ASYNC_RXC_CB, rx_cb);
	usart_async_register_callback(&VCOM, USART_ASYNC_ERROR_CB, err_cb);*/
	usart_async_get_io_descriptor(&VCOM, &io);
	usart_async_enable(&VCOM);

	io_write(io, example_VCOM, 12);
}
