/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef DRIVER_INIT_H_INCLUDED
#define DRIVER_INIT_H_INCLUDED

#include "atmel_start_pins.h"

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
#include <hal_usart_async.h>

extern struct spi_m_sync_descriptor SPI;

extern struct i2c_m_sync_desc I2C_Main;

extern struct i2c_m_sync_desc        I2C_Backup;
extern struct usart_async_descriptor VCOM;

void SPI_PORT_init(void);
void SPI_CLOCK_init(void);
void SPI_init(void);

void I2C_Main_CLOCK_init(void);
void I2C_Main_init(void);
void I2C_Main_PORT_init(void);

void I2C_Backup_CLOCK_init(void);
void I2C_Backup_init(void);
void I2C_Backup_PORT_init(void);

void VCOM_PORT_init(void);
void VCOM_CLOCK_init(void);
void VCOM_init(void);

/**
 * \brief Perform system initialization, initialize pins and clocks for
 * peripherals
 */
void system_init(void);

#ifdef __cplusplus
}
#endif
#endif // DRIVER_INIT_H_INCLUDED
