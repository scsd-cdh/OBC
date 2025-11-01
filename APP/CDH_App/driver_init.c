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
#include <hpl_spi_base.h>

#include "SPI0_wrapper.h"


struct i2c_m_sync_desc Red_I2C_2;

struct usart_sync_descriptor Debug_USART_0;

struct usart_sync_descriptor LVDS_USART_1;

struct usart_sync_descriptor LVDS2_USART_2;

<<<<<<< Updated upstream
void Mas_I2C_0_PORT_init(void)
{

	gpio_set_pin_function(PA4, MUX_PA4A_TWIHS0_TWCK0);

	gpio_set_pin_function(PA3, MUX_PA3A_TWIHS0_TWD0);
}

void Mas_I2C_0_CLOCK_init(void)
{
	_pmc_enable_periph_clock(ID_TWIHS0);
}

void Mas_I2C_0_init(void)
{
	Mas_I2C_0_CLOCK_init();

	i2c_m_sync_init(&Mas_I2C_0, TWIHS0);

	Mas_I2C_0_PORT_init();
}
=======
void MRAM_SPI_0_PORT_init(void)
{

	gpio_set_pin_function(PD20, MUX_PD20B_SPI0_MISO);

	gpio_set_pin_function(PD21, MUX_PD21B_SPI0_MOSI);

	gpio_set_pin_function(PD22, MUX_PD22B_SPI0_SPCK);
}

void MRAM_SPI_0_CLOCK_init(void)
{
	_pmc_enable_periph_clock(ID_SPI0);
}

void MRAM_SPI_0_init(void)
{
	MRAM_SPI_0_CLOCK_init();
	spi_m_sync_set_func_ptr(&MRAM_SPI_0, _spi_get_spi_m_sync());
	spi_m_sync_init(&MRAM_SPI_0, SPI0);
	MRAM_SPI_0_PORT_init();
}

>>>>>>> Stashed changes

void Red_I2C_2_PORT_init(void)
{

	gpio_set_pin_function(PD28, MUX_PD28C_TWIHS2_TWCK2);

	gpio_set_pin_function(PD27, MUX_PD27C_TWIHS2_TWD2);
}

void Red_I2C_2_CLOCK_init(void)
{
	_pmc_enable_periph_clock(ID_TWIHS2);
}

void Red_I2C_2_init(void)
{
	Red_I2C_2_CLOCK_init();

	i2c_m_sync_init(&Red_I2C_2, TWIHS2);

	Red_I2C_2_PORT_init();
}

void Debug_USART_0_PORT_init(void)
{

	gpio_set_pin_function(PA9, MUX_PA9A_UART0_URXD0);

	gpio_set_pin_function(PA10, MUX_PA10A_UART0_UTXD0);
}

void Debug_USART_0_CLOCK_init(void)
{
	_pmc_enable_periph_clock(ID_UART0);
}

void Debug_USART_0_init(void)
{
	Debug_USART_0_CLOCK_init();
	usart_sync_init(&Debug_USART_0, UART0, _uart_get_usart_sync());
	Debug_USART_0_PORT_init();
}

void LVDS_USART_1_PORT_init(void)
{

	gpio_set_pin_function(PA5, MUX_PA5C_UART1_URXD1);

	gpio_set_pin_function(PA6, MUX_PA6C_UART1_UTXD1);
}

void LVDS_USART_1_CLOCK_init(void)
{
	_pmc_enable_periph_clock(ID_UART1);
}

void LVDS_USART_1_init(void)
{
	LVDS_USART_1_CLOCK_init();
	usart_sync_init(&LVDS_USART_1, UART1, _uart_get_usart_sync());
	LVDS_USART_1_PORT_init();
}

void LVDS2_USART_2_PORT_init(void)
{

	gpio_set_pin_function(PD25, MUX_PD25C_UART2_URXD2);

	gpio_set_pin_function(PD26, MUX_PD26C_UART2_UTXD2);
}

void LVDS2_USART_2_CLOCK_init(void)
{
	_pmc_enable_periph_clock(ID_UART2);
}

void LVDS2_USART_2_init(void)
{
	LVDS2_USART_2_CLOCK_init();
	usart_sync_init(&LVDS2_USART_2, UART2, _uart_get_usart_sync());
	LVDS2_USART_2_PORT_init();
}

void system_init(void)
{
	init_mcu();

	_pmc_enable_periph_clock(ID_PIOC);

	/* Disable Watchdog */
	hri_wdt_set_MR_WDDIS_bit(WDT);

	/* GPIO on PC15 */

	gpio_set_pin_level(Yeast_Off,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(Yeast_Off, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(Yeast_Off, GPIO_PIN_FUNCTION_OFF);

	/* GPIO on PC22 */

	// Set pin direction to input
	gpio_set_pin_direction(Trans_Irq, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(Trans_Irq,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(Trans_Irq, GPIO_PIN_FUNCTION_OFF);

	gpio_set_pin_direction(LED0, GPIO_DIRECTION_OUT);

	gpio_set_pin_pull_mode(LED0, GPIO_PULL_UP);

<<<<<<< Updated upstream
	SPI0_init();
	
	Mas_I2C_0_init();
=======
	MRAM_SPI_0_init();
>>>>>>> Stashed changes

	Red_I2C_2_init();

	// Debug_USART_0_init();

	// LVDS_USART_1_init();

	// LVDS2_USART_2_init();
}
