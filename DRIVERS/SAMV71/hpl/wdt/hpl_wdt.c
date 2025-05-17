/**
 * \file
 *
 * \brief SAM Watchdog
 *
 * Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
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
#include <compiler.h>
#include <hpl_wdt.h>
#include <hpl_wdt_config.h>
#include <utils_assert.h>

/**
 * \brief Macro is used to fill wdt configuration structure
 */
#define WDT_CONFIGURATION                                                                                              \
	((WDT_MR_WDV(CONF_WDT_WDV)) | (CONF_WDT_WDFIEN << 12) | (CONF_WDT_WDRSTEN << 13) | (CONF_WDT_WDDIS << 15)          \
	 | (WDT_MR_WDD(CONF_WDT_WDD)) | (CONF_WDT_WDDBGHLT << 28) | (CONF_WDT_WDIDLEHLT << 29))

/**
 * \brief WDT initialization function
 */
int32_t _wdt_init(struct wdt_dev *const dev)
{
	/* Sanity check arguments */
	ASSERT(dev && dev->hw);

	hri_wdt_write_MR_reg(dev->hw, (hri_wdt_mr_reg_t)WDT_CONFIGURATION);

	return ERR_NONE;
}
/**
 * \brief De-initialize WDT
 */
int32_t _wdt_deinit(struct wdt_dev *const dev)
{
	/* Sanity check arguments */
	ASSERT(dev && dev->hw);
	(void)dev;

	return ERR_UNSUPPORTED_OP;
}

/**
 * \brief set timeout period for WDT instance
 */
int32_t _wdt_set_timeout_period(struct wdt_dev *const dev, const uint32_t clk_rate, const uint16_t timeout_period)
{
	/* Sanity check arguments */
	ASSERT(dev && dev->hw);
	(void)dev;
	(void)clk_rate;
	(void)timeout_period;

	return ERR_UNSUPPORTED_OP;
}

/**
 * \brief get timeout period for WDT instance
 */
uint32_t _wdt_get_timeout_period(const struct wdt_dev *const dev, const uint32_t clk_rate)
{
	uint32_t timeout_period;
	uint16_t timeout_period_reg;

	/* Sanity check arguments */
	ASSERT(dev && dev->hw);

	/* get the register value */
	timeout_period_reg = (uint16_t)hri_wdt_read_MR_WDV_bf(dev->hw);

	/* convert period cycles(clock cycles) to timeout period(ms) */
	timeout_period = timeout_period_reg * 128 / clk_rate;

	return timeout_period;
}

/**
 * \brief enbale watchdog timer
 */
int32_t _wdt_enable(struct wdt_dev *const dev)
{
	/* Sanity check arguments */
	ASSERT(dev && dev->hw);
	(void)dev;

	return ERR_UNSUPPORTED_OP;
}

/**
 * \brief disable watchdog timer
 */
int32_t _wdt_disable(struct wdt_dev *const dev)
{
	/* Sanity check arguments */
	ASSERT(dev && dev->hw);
	(void)dev;

	return ERR_UNSUPPORTED_OP;
}

/**
 * \brief reset watchdog timer to make wdt work from start
 */
int32_t _wdt_feed(struct wdt_dev *const dev)
{
	/* Sanity check arguments */
	ASSERT(dev && dev->hw);

	hri_wdt_write_CR_reg(dev->hw, WDT_CR_KEY_PASSWD | WDT_CR_WDRSTT);

	return ERR_NONE;
}
