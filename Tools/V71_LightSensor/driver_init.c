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

struct i2c_m_sync_desc I2C_0;

struct usart_sync_descriptor USART_0;

void I2C_0_PORT_init(void)
{
	// Initialize I2C SDA and SCL Functions
	gpio_set_pin_function(I2C0_SCL, MUX_PA4A_TWIHS0_TWCK0); // Set PA04 as I2C SCL
	gpio_set_pin_function(I2C0_SDA, MUX_PA3A_TWIHS0_TWD0); // Set PA03 as I2C SDA
}

void I2C_0_CLOCK_init(void)
{
	_pmc_enable_periph_clock(ID_TWIHS0);
}

void I2C_0_init(void)
{
	I2C_0_CLOCK_init();

	i2c_m_sync_init(&I2C_0, TWIHS0);

	I2C_0_PORT_init();
}

void USART_0_PORT_init(void)
{
	// Initialize the USART TX and RX Functions
	gpio_set_pin_function(USART0_RX, MUX_PA21A_USART1_RXD1); // Set PA21 as USART TX
	gpio_set_pin_function(USART0_TX, MUX_PB4D_USART1_TXD1);  // Set PB04 as USART RX
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

void system_init(void)
{
	init_mcu();

	_pmc_enable_periph_clock(ID_PIOA);

	/* Disable Watchdog */
	hri_wdt_set_MR_WDDIS_bit(WDT);

	I2C_0_init();

	USART_0_init();

	// Set the LED pin as output
	gpio_set_pin_level(LED0, true);
	gpio_set_pin_direction(LED0, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(LED0, GPIO_PIN_FUNCTION_OFF); // Set the pin function to OFF (default for LED0)
}
