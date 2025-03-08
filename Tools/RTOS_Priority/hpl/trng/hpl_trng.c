/**
 * \file
 *
 * \brief True Random Number Generator
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
#include <err_codes.h>
#include <hpl_rand_sync.h>
#include <utils_assert.h>

int32_t _rand_sync_init(struct _rand_sync_dev *const dev, void *const hw)
{
	ASSERT(dev && hw);

	dev->prvt   = hw;
	dev->n_bits = 32;

	return ERR_NONE;
}

void _rand_sync_deinit(struct _rand_sync_dev *const dev)
{
	_rand_sync_disable(dev);
}

int32_t _rand_sync_enable(struct _rand_sync_dev *const dev)
{
	ASSERT(dev);
	ASSERT(dev->prvt);

	hri_trng_write_CR_reg(dev->prvt, TRNG_CR_KEY_PASSWD | 0x1);
	return ERR_NONE;
}

void _rand_sync_disable(struct _rand_sync_dev *const dev)
{
	ASSERT(dev);
	ASSERT(dev->prvt);

	hri_trng_write_CR_reg(dev->prvt, TRNG_CR_KEY_PASSWD);
}

int32_t _rand_sync_set_seed(struct _rand_sync_dev *const dev, const uint32_t seed)
{
	(void)dev;
	(void)seed;
	return ERR_UNSUPPORTED_OP;
}

uint32_t _rand_sync_read_one(const struct _rand_sync_dev *const dev)
{
	ASSERT(dev);
	ASSERT(dev->prvt);

	while (!hri_trng_get_ISR_DATRDY_bit(dev->prvt)) {
		/* Wait until data ready. */
	}
	return hri_trng_read_ODATA_ODATA_bf(dev->prvt);
}
