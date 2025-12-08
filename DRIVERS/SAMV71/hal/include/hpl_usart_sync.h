/**
 * \file
 *
 * \brief USART related functionality declaration.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

#ifndef _HPL_SYNC_USART_H_INCLUDED
#define _HPL_SYNC_USART_H_INCLUDED

/**
 * \addtogroup HPL USART SYNC
 *
 * \section hpl_usart_sync_rev Revision History
 * - v1.0.0 Initial Release
 *
 *@{
 */

#include <hpl_usart.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief USART descriptor device structure
 */
struct _usart_sync_device {
	void *hw;
};

/**
 * \brief Usart functions, pointers to low-level functions
 */
struct _usart_sync_hpl_interface {
	int32_t (*sync_init)(struct _usart_sync_device *const device, void *const hw);
	void (*sync_deinit)(struct _usart_sync_device *const device);
	void (*sync_enable)(struct _usart_sync_device *const device);
	void (*sync_disable)(struct _usart_sync_device *const device);
	uint16_t (*sync_calculate_baud_rate)(const uint32_t baud, const uint32_t clock_rate, const uint8_t samples,
	                                     const enum usart_baud_rate_mode mode, const uint8_t fraction);
	void (*sync_set_baud_rate)(struct _usart_sync_device *const device, const uint32_t baud_rate);
	void (*sync_set_data_order)(struct _usart_sync_device *const device, const enum usart_data_order order);
	void (*sync_set_mode)(struct _usart_sync_device *const device, const enum usart_mode mode);
	void (*sync_set_parity)(struct _usart_sync_device *const device, const enum usart_parity parity);
	void (*sync_set_stop_bits)(struct _usart_sync_device *const device, const enum usart_stop_bits stop_bits);
	void (*sync_set_character_size)(struct _usart_sync_device *const device, const enum usart_character_size size);
	uint32_t (*sync_get_status)(const struct _usart_sync_device *const device);
	void (*sync_write_byte)(struct _usart_sync_device *const device, uint8_t data);
	uint8_t (*sync_read_byte)(const struct _usart_sync_device *const device);
	bool (*sync_is_ready_to_send)(const struct _usart_sync_device *const device);
	bool (*sync_is_transmit_done)(const struct _usart_sync_device *const device);
	bool (*sync_is_byte_received)(const struct _usart_sync_device *const device);
	void (*sync_set_flow_control_state)(struct _usart_sync_device *const     device,
	                                    const union usart_flow_control_state state);
	union usart_flow_control_state (*sync_get_flow_control_state)(const struct _usart_sync_device *const device);
	uint8_t (*sync_get_hardware_index)(const struct _usart_sync_device *const device);
};
//@}

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* _HPL_SYNC_USART_H_INCLUDED */
