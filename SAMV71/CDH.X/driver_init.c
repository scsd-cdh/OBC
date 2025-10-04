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
#include <hpl_usart_base.h>

/*! The buffer size for USART */
#define VCOM_BUFFER_SIZE 16

struct spi_m_sync_descriptor  SPI;
struct usart_async_descriptor VCOM;

static uint8_t VCOM_buffer[VCOM_BUFFER_SIZE];

struct i2c_m_sync_desc I2C_Main;

struct i2c_m_sync_desc I2C_Backup;

void SPI_PORT_init(void)
{

	gpio_set_pin_function(PD20, MUX_PD20B_SPI0_MISO);

	gpio_set_pin_function(PD21, MUX_PD21B_SPI0_MOSI);

	gpio_set_pin_function(PD22, MUX_PD22B_SPI0_SPCK);
}

void SPI_CLOCK_init(void)
{
	_pmc_enable_periph_clock(ID_SPI0);
}

void SPI_init(void)
{
	SPI_CLOCK_init();
	spi_m_sync_set_func_ptr(&SPI, _spi_get_spi_m_sync());
	spi_m_sync_init(&SPI, SPI0);
	SPI_PORT_init();
}

void I2C_Main_PORT_init(void)
{

	gpio_set_pin_function(PA4, MUX_PA4A_TWIHS0_TWCK0);

	gpio_set_pin_function(PA3, MUX_PA3A_TWIHS0_TWD0);
}

void I2C_Main_CLOCK_init(void)
{
	_pmc_enable_periph_clock(ID_TWIHS0);
}

void I2C_Main_init(void)
{
	I2C_Main_CLOCK_init();

	i2c_m_sync_init(&I2C_Main, TWIHS0);

	I2C_Main_PORT_init();
}

void I2C_Backup_PORT_init(void)
{

	gpio_set_pin_function(PD28, MUX_PD28C_TWIHS2_TWCK2);

	gpio_set_pin_function(PD27, MUX_PD27C_TWIHS2_TWD2);
}

void I2C_Backup_CLOCK_init(void)
{
	_pmc_enable_periph_clock(ID_TWIHS2);
}

void I2C_Backup_init(void)
{
	I2C_Backup_CLOCK_init();

	i2c_m_sync_init(&I2C_Backup, TWIHS2);

	I2C_Backup_PORT_init();
}

/**
 * \brief USART Clock initialization function
 *
 * Enables register interface and peripheral clock
 */
void VCOM_CLOCK_init()
{
	_pmc_enable_periph_clock(ID_USART1);
}

/**
 * \brief USART pinmux initialization function
 *
 * Set each required pin to USART functionality
 */
void VCOM_PORT_init()
{

	gpio_set_pin_function(PA21, MUX_PA21A_USART1_RXD1);

	gpio_set_pin_function(PB4, MUX_PB4D_USART1_TXD1);
}

/**
 * \brief USART initialization function
 *
 * Enables USART peripheral, clocks and initializes USART driver
 */
void VCOM_init(void)
{
	VCOM_CLOCK_init();
	VCOM_PORT_init();
	usart_async_init(&VCOM, USART1, VCOM_buffer, VCOM_BUFFER_SIZE, _usart_get_usart_async());
}

void system_init(void)
{
	init_mcu();

	/* Disable Watchdog */
	hri_wdt_set_MR_WDDIS_bit(WDT);

	SPI_init();

	I2C_Main_init();

	I2C_Backup_init();
	VCOM_init();
}
