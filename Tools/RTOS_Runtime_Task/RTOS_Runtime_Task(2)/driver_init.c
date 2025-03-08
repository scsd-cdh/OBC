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

struct usart_sync_descriptor USART_3;

void USART_3_PORT_init(void)
{

	gpio_set_pin_function(PD28, MUX_PD28A_UART3_URXD3);

	gpio_set_pin_function(PD30, MUX_PD30A_UART3_UTXD3);
}

void USART_3_CLOCK_init(void)
{
	_pmc_enable_periph_clock(ID_UART3);
}

void USART_3_init(void)
{
	USART_3_CLOCK_init();
	usart_sync_init(&USART_3, UART3, _uart_get_usart_sync());
	USART_3_PORT_init();
}

void system_init(void)
{
	init_mcu();

	/* Disable Watchdog */
	hri_wdt_set_MR_WDDIS_bit(WDT);

	USART_3_init();
}
