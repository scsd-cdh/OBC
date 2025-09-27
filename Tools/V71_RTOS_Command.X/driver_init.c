/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_init.h"
#include <hal_init.h>
#include <hpl_pmc.h>
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hpl_usart_base.h>

struct i2c_m_sync_desc command;
struct usart_sync_descriptor serial;
struct io_descriptor *command_io;
struct io_descriptor *serial_io;

void command_PORT_init(void)
{
	gpio_set_pin_function(PA4, MUX_PA4A_TWIHS0_TWCK0);
	gpio_set_pin_function(PA3, MUX_PA3A_TWIHS0_TWD0);
}

void command_CLOCK_init(void)
{
	_pmc_enable_periph_clock(ID_TWIHS0);
}

void command_IO_init(void){
    i2c_m_sync_get_io_descriptor(&command, &command_io);
	i2c_m_sync_enable(&command);
}

void command_init(void)
{
	command_CLOCK_init();
	i2c_m_sync_init(&command, TWIHS0);
	command_PORT_init();
    command_IO_init();
}

void serial_PORT_init(void)
{
	gpio_set_pin_function(PA21, MUX_PA21A_USART1_RXD1);
	gpio_set_pin_function(PB4, MUX_PB4D_USART1_TXD1);
}

void serial_CLOCK_init(void)
{
	_pmc_enable_periph_clock(ID_USART1);
}

void serial_IO_init(void){
    usart_sync_get_io_descriptor(&serial, &serial_io);
	usart_sync_enable(&serial);
}

void serial_init(void)
{
	serial_CLOCK_init();
	serial_PORT_init();
    usart_sync_set_baud_rate(&serial,115200); // Set Baudrate 
    usart_sync_set_mode(&serial, USART_MODE_ASYNCHRONOUS); // ASynchronous Mode
    usart_sync_set_parity(&serial, USART_PARITY_NONE); // No Parity Bits
    usart_sync_set_stopbits(&serial, USART_STOP_BITS_ONE); // 1 Stop Bit
	usart_sync_init(&serial, USART1, _usart_get_usart_sync());
    serial_IO_init();
}

void system_init(void)
{
	init_mcu();

	/* Disable Watchdog */
	hri_wdt_set_MR_WDDIS_bit(WDT);

	command_init();

	serial_init();
}
