#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <hal_i2c_m_sync.h>
#include <hal_usart_sync.h>

void Red_I2C_2_CLOCK_init(void);
void Red_I2C_2_init(void);
void Red_I2C_2_PORT_init(void);

void Debug_USART_0_PORT_init(void);
void Debug_USART_0_CLOCK_init(void);
void Debug_USART_0_init(void);

void ULog_USART_1_PORT_init(void);
void ULog_USART_1_CLOCK_init(void);
void ULog_USART_1_init(void);

void LVDS_UART_1_PORT_init(void);
void LVDS_UART_1_CLOCK_init(void);
void LVDS_UART_1_init(void);

void LVDS2_UART_2_PORT_init(void);
void LVDS2_UART_2_CLOCK_init(void);
void LVDS2_UART_2_init(void);

void ULOG_USART_1_PORT_init(void);
void ULOG_USART_1_CLOCK_init(void);
void ULOG_USART_1_init(void);

#ifdef __cplusplus
}
#endif
