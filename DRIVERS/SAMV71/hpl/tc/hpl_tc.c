/**
 * \file
 *
 * \brief SAM TC
 *
 * Copyright (c) 2016-2019 Microchip Technology Inc. and its subsidiaries.
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

#include <utils_assert.h>
#include <hpl_tc_config.h>
#include <hpl_timer.h>
#include <hpl_pwm.h>
#include <utils.h>
#include <hpl_tc.h>

#ifndef CONF_TC0_ENABLE
#define CONF_TC0_ENABLE 0
#endif
#ifndef CONF_TC1_ENABLE
#define CONF_TC1_ENABLE 0
#endif
#ifndef CONF_TC2_ENABLE
#define CONF_TC2_ENABLE 0
#endif
#ifndef CONF_TC3_ENABLE
#define CONF_TC3_ENABLE 0
#endif
#ifndef CONF_TC4_ENABLE
#define CONF_TC4_ENABLE 0
#endif

/**
 * \brief Macro is used to fill usart configuration structure based on its
 * number
 *
 * \param[in] n The number of structures
 */
#define TC_CONFIGURATION(n, m)                                                                                         \
	{                                                                                                                  \
		TC##n, TC##m##_IRQn, CONF_TC##n##_CMR_REG_VAL, CONF_TC##n##_NODIVCLK,                                          \
		    (uint32_t)(CONF_TC##n##_FAULT_CHAN0_ENABLE | (CONF_TC##n##_FAULT_CHAN1_ENABLE << TC_FMR_ENCF1_Pos)),       \
		    TC_RA_RA(CONF_TC##n##_RA), TC_RB_RB(CONF_TC##n##_RB), TC_RC_RC(CONF_TC##n##_RC),                           \
		    CONF_TC##n##_TICKS_PER_US,                                                                                 \
	}

/**
 * \brief TC configuration type
 */
struct tc_configuration {
	void *           hw;
	IRQn_Type        irq;
	hri_tc_cmr_reg_t channel_mode;
	hri_tc_emr_reg_t ext_mode;
	hri_tc_fmr_reg_t fmr;
	hri_tc_ra_reg_t  ra;
	hri_tc_rb_reg_t  rb;
	hri_tc_rc_reg_t  rc;
	uint32_t         tick_us;
};

#define TC_AMOUNT (CONF_TC0_ENABLE + CONF_TC1_ENABLE + CONF_TC2_ENABLE + CONF_TC3_ENABLE)

#if TC_AMOUNT < 1
/** Dummy array for compiling. */
static const struct tc_configuration _tcs[1] = {{0}};
#else
/**
 * \brief Array of TC configurations
 */
static struct tc_configuration _tcs[] = {
#if CONF_TC0_ENABLE == 1
    TC_CONFIGURATION(0, 0),
#endif
#if CONF_TC1_ENABLE == 1
    TC_CONFIGURATION(1, 3),
#endif
#if CONF_TC2_ENABLE == 1
    TC_CONFIGURATION(2, 6),
#endif
#if CONF_TC3_ENABLE == 1
    TC_CONFIGURATION(3, 9),
#endif
};
#endif

static struct _timer_device *_tc0_dev = NULL;

static struct tc_configuration *get_cfg(void *hw)
{
	uint8_t i;

	for (i = 0; i < ARRAY_SIZE(_tcs); i++) {
		if (_tcs[i].hw == hw) {
			return &(_tcs[i]);
		}
	}

	return NULL;
}

/**
 * \brief Init irq param with the given tc hardware instance
 */
static void _tc_init_irq_param(const void *const hw, void *dev)
{
	if (hw == TC0) {
		_tc0_dev = (struct _timer_device *)dev;
	}
}

/**
 * \brief Initialize TC
 */
int32_t _timer_init(struct _timer_device *const device, void *const hw)
{
	struct tc_configuration *cfg     = get_cfg(hw);
	uint32_t                 ch_mode = cfg->channel_mode;

	device->hw = hw;
	ASSERT(ARRAY_SIZE(_tcs));

	if (ch_mode & TC_CMR_WAVE) {
		/* Enable event control mode */
		ch_mode |= (0x02 << 13) | (0x01 << 16) | (0x02 << 18);
	}

	hri_tc_write_CMR_reg(hw, 0, ch_mode);
	hri_tc_write_RA_reg(hw, 0, cfg->ra);
	hri_tc_write_RB_reg(hw, 0, cfg->rb);
	hri_tc_write_EMR_reg(hw, 0, cfg->ext_mode);
	hri_tc_write_RC_reg(hw, 0, cfg->rc);
	hri_tc_set_IMR_reg(hw, 0, TC_IER_CPCS);
	hri_tc_write_FMR_reg(hw, cfg->fmr);

	_tc_init_irq_param(hw, device);
	NVIC_DisableIRQ(cfg->irq);
	NVIC_ClearPendingIRQ(cfg->irq);
	NVIC_EnableIRQ(cfg->irq);

	return ERR_NONE;
}

/**
 * \brief De-initialize TC
 */
void _timer_deinit(struct _timer_device *const device)
{
	struct tc_configuration *cfg = get_cfg(device->hw);
	NVIC_DisableIRQ(cfg->irq);
	hri_tc_write_CCR_reg(device->hw, 0, TC_CCR_CLKDIS);
}

/**
 * \brief Start hardware timer
 */
void _timer_start(struct _timer_device *const device)
{
	hri_tc_write_CCR_reg(device->hw, 0, TC_CCR_CLKEN | TC_CCR_SWTRG);
}

/**
 * \brief Stop hardware timer
 */
void _timer_stop(struct _timer_device *const device)
{
	hri_tc_write_CCR_reg(device->hw, 0, TC_CCR_CLKDIS);
}

/**
 * \brief Set timer period
 */
void _timer_set_period(struct _timer_device *const device, const uint32_t clock_cycles)
{
	hri_tc_write_RC_reg(device->hw, 0, clock_cycles);
}

/**
 * \brief Retrieve timer period
 */
uint32_t _timer_get_period(const struct _timer_device *const device)
{
	return hri_tc_read_RC_reg(device->hw, 0);
}

/**
 * \brief Check if timer is running
 */
bool _timer_is_started(const struct _timer_device *const device)
{
	return hri_tc_get_SR_CLKSTA_bit(device->hw, 0);
}

/**
 * \brief Set timer IRQ
 *
 * \param[in] hw The pointer to hardware instance
 */
void _timer_set_irq(struct _timer_device *const device)
{
	struct tc_configuration *cfg = get_cfg(device->hw);

	_irq_set((IRQn_Type)(cfg->irq));
}

/**
 * \internal TC interrupt handler
 *
 * \param[in] instance TC instance number
 */
static void tc_interrupt_handler(struct _timer_device *device)
{
	void *const hw = device->hw;

	if (hri_tc_get_SR_CPCS_bit(hw, 0)) {
		device->timer_cb.period_expired(device);
	}
}

/**
 * \brief Retrieve timer helper functions
 */
struct _timer_hpl_interface *_tc_get_timer(void)
{
	return NULL;
}

/**
 * \brief TC interrupt handler
 */
void TC0_Handler(void)
{
	tc_interrupt_handler(_tc0_dev);
}

/**
 * \brief Retrieve PWM helper functions
 */
struct _pwm_hpl_interface *_tc_get_pwm(void)
{
	return NULL;
}
