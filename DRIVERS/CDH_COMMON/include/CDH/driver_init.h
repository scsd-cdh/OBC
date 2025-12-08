#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern struct i2c_m_sync_desc Mas_I2C_0;

extern struct i2c_m_sync_desc Red_I2C_2;

extern struct usart_sync_descriptor Debug_USART_0;

extern struct usart_sync_descriptor LVDS_UART_1;

extern struct usart_sync_descriptor LVDS2_UART_2;

extern struct usart_sync_descriptor ULOG_USART_1;

/**
 * \brief Perform system initialization, initialize pins and clocks for
 * peripherals
 */
void system_init(void);

#ifdef __cplusplus
}
#endif
