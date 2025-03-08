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

struct rand_sync_desc RAND_0;

struct usart_sync_descriptor USART_0;

struct wdt_descriptor WDT_0;

void RAND_0_CLOCK_init(void)
{
	_pmc_enable_periph_clock(ID_TRNG);
}

void RAND_0_init(void)
{
	RAND_0_CLOCK_init();
	rand_sync_init(&RAND_0, TRNG);
}

void delay_driver_init(void)
{
	delay_init(SysTick);
}

void USART_0_PORT_init(void)
{
}

void USART_0_CLOCK_init(void)
{
	_pmc_enable_periph_clock(ID_UART0);
}

void USART_0_init(void)
{
	USART_0_CLOCK_init();
	usart_sync_init(&USART_0, UART0, _uart_get_usart_sync());
	USART_0_PORT_init();
}

void WDT_0_init(void)
{
	wdt_init(&WDT_0, WDT);
}

void system_init(void)
{
	init_mcu();

	RAND_0_init();

	delay_driver_init();

	USART_0_init();

	WDT_0_init();
}
