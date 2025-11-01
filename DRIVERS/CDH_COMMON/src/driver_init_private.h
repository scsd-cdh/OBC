#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <hal_i2c_m_sync.h>
#include <hal_usart_sync.h>

extern struct i2c_m_sync_desc Mas_I2C_0;

extern struct i2c_m_sync_desc Red_I2C_2;

extern struct usart_sync_descriptor Debug_USART_0;

extern struct usart_sync_descriptor LVDS_USART_1;

extern struct usart_sync_descriptor LVDS2_USART_2;

void Mas_I2C_0_CLOCK_init(void);
void Mas_I2C_0_init(void);
void Mas_I2C_0_PORT_init(void);

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

#ifdef __cplusplus
}
#endif
