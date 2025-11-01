/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef DRIVER_INIT_H_INCLUDED
#define DRIVER_INIT_H_INCLUDED

#include "../../DRIVERS/SAMV71/atmel_start_pins.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <hal_atomic.h>
#include <hal_delay.h>
#include <hal_gpio.h>
#include <hal_init.h>
#include <hal_io.h>
#include <hal_sleep.h>

#include <hal_spi_m_sync.h>

#include <hal_i2c_m_sync.h>

#include <hal_i2c_m_sync.h>

#include <hal_usart_sync.h>
#include <hpl_uart_base.h>

#include <hal_usart_sync.h>
#include <hpl_uart_base.h>

#include <hal_usart_sync.h>
#include <hpl_uart_base.h>

<<<<<<< Updated upstream
extern struct i2c_m_sync_desc Mas_I2C_0;
=======
extern struct spi_m_sync_descriptor MRAM_SPI_0;
>>>>>>> Stashed changes

extern struct i2c_m_sync_desc Red_I2C_2;

extern struct usart_sync_descriptor Debug_USART_0;

extern struct usart_sync_descriptor LVDS_USART_1;

extern struct usart_sync_descriptor LVDS2_USART_2;

<<<<<<< Updated upstream
void Mas_I2C_0_CLOCK_init(void);
void Mas_I2C_0_init(void);
void Mas_I2C_0_PORT_init(void);
=======
void MRAM_SPI_0_PORT_init(void);
void MRAM_SPI_0_CLOCK_init(void);
void MRAM_SPI_0_init(void);
>>>>>>> Stashed changes

void Red_I2C_2_CLOCK_init(void);
void Red_I2C_2_init(void);
void Red_I2C_2_PORT_init(void);

void Debug_USART_0_PORT_init(void);
void Debug_USART_0_CLOCK_init(void);
void Debug_USART_0_init(void);
void Debug_USART_0_example(void);

void LVDS_USART_1_PORT_init(void);
void LVDS_USART_1_CLOCK_init(void);
void LVDS_USART_1_init(void);
void LVDS_USART_1_example(void);

void LVDS2_USART_2_PORT_init(void);
void LVDS2_USART_2_CLOCK_init(void);
void LVDS2_USART_2_init(void);
void LVDS2_USART_2_example(void);

/**
 * \brief Perform system initialization, initialize pins and clocks for
 * peripherals
 */
void system_init(void);

#ifdef __cplusplus
}
#endif
#endif // DRIVER_INIT_H_INCLUDED
