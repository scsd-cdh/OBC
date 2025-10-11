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

struct usart_sync_descriptor USART;

void USART_PORT_init(void)
{
	gpio_set_pin_function(PA21, MUX_PA21A_USART1_RXD1);
	gpio_set_pin_function(PB4, MUX_PB4D_USART1_TXD1);
}

void USART_CLOCK_init(void)
{
	_pmc_enable_periph_clock(ID_USART1);
}

void USART_init(void)
{
	USART_CLOCK_init();
	USART_PORT_init();
    usart_sync_init(&USART, USART1, _usart_get_usart_sync());
}

void system_init(void)
{
	init_mcu();

	/* Disable Watchdog */
	hri_wdt_set_MR_WDDIS_bit(WDT);

	USART_init();
}
