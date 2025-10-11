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

#include <hal_i2c_m_sync.h>

#include <hal_usart_sync.h>
  
/* Definitions */
#define MSG_SIZE 1024 // String Message

extern struct i2c_m_sync_desc command;
extern struct usart_sync_descriptor serial;
extern struct io_descriptor *command_io;
extern struct io_descriptor *serial_io;

/* Variables */
char io_message[MSG_SIZE];

void command_CLOCK_init(void);
void command_IO_init(void);
void command_PORT_init(void);
void command_init(void);

void serial_PORT_init(void);
void serial_CLOCK_init(void);
void serial_IO_init(void);
void serial_init(void);

/**
 * \brief Perform system initialization, initialize pins and clocks for
 * peripherals
 */
void system_init(void);

#ifdef __cplusplus
}
#endif
#endif // DRIVER_INIT_H_INCLUDED
