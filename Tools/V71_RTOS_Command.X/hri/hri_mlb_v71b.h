/**
 * \file
 *
 * \brief SAM MLB
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
 */

#ifdef _SAMV71_MLB_COMPONENT_
#ifndef _HRI_MLB_V71B_H_INCLUDED_
#define _HRI_MLB_V71B_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_MLB_CRITICAL_SECTIONS)
#define MLB_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define MLB_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define MLB_CRITICAL_SECTION_ENTER()
#define MLB_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_mlb_acmr_reg_t;
typedef uint32_t hri_mlb_acsr_reg_t;
typedef uint32_t hri_mlb_actl_reg_t;
typedef uint32_t hri_mlb_hcbr_reg_t;
typedef uint32_t hri_mlb_hcer_reg_t;
typedef uint32_t hri_mlb_hcmr_reg_t;
typedef uint32_t hri_mlb_hctl_reg_t;
typedef uint32_t hri_mlb_madr_reg_t;
typedef uint32_t hri_mlb_mctl_reg_t;
typedef uint32_t hri_mlb_mdat_reg_t;
typedef uint32_t hri_mlb_mdwe_reg_t;
typedef uint32_t hri_mlb_mien_reg_t;
typedef uint32_t hri_mlb_mlbc0_reg_t;
typedef uint32_t hri_mlb_mlbc1_reg_t;
typedef uint32_t hri_mlb_ms0_reg_t;
typedef uint32_t hri_mlb_ms1_reg_t;
typedef uint32_t hri_mlb_msd_reg_t;
typedef uint32_t hri_mlb_mss_reg_t;

static inline hri_mlb_msd_reg_t hri_mlb_get_MSD_SD0_bf(const void *const hw, hri_mlb_msd_reg_t mask)
{
	return (((Mlb *)hw)->MLB_MSD & MLB_MSD_SD0(mask)) >> MLB_MSD_SD0_Pos;
}

static inline hri_mlb_msd_reg_t hri_mlb_read_MSD_SD0_bf(const void *const hw)
{
	return (((Mlb *)hw)->MLB_MSD & MLB_MSD_SD0_Msk) >> MLB_MSD_SD0_Pos;
}

static inline hri_mlb_msd_reg_t hri_mlb_get_MSD_SD1_bf(const void *const hw, hri_mlb_msd_reg_t mask)
{
	return (((Mlb *)hw)->MLB_MSD & MLB_MSD_SD1(mask)) >> MLB_MSD_SD1_Pos;
}

static inline hri_mlb_msd_reg_t hri_mlb_read_MSD_SD1_bf(const void *const hw)
{
	return (((Mlb *)hw)->MLB_MSD & MLB_MSD_SD1_Msk) >> MLB_MSD_SD1_Pos;
}

static inline hri_mlb_msd_reg_t hri_mlb_get_MSD_SD2_bf(const void *const hw, hri_mlb_msd_reg_t mask)
{
	return (((Mlb *)hw)->MLB_MSD & MLB_MSD_SD2(mask)) >> MLB_MSD_SD2_Pos;
}

static inline hri_mlb_msd_reg_t hri_mlb_read_MSD_SD2_bf(const void *const hw)
{
	return (((Mlb *)hw)->MLB_MSD & MLB_MSD_SD2_Msk) >> MLB_MSD_SD2_Pos;
}

static inline hri_mlb_msd_reg_t hri_mlb_get_MSD_SD3_bf(const void *const hw, hri_mlb_msd_reg_t mask)
{
	return (((Mlb *)hw)->MLB_MSD & MLB_MSD_SD3(mask)) >> MLB_MSD_SD3_Pos;
}

static inline hri_mlb_msd_reg_t hri_mlb_read_MSD_SD3_bf(const void *const hw)
{
	return (((Mlb *)hw)->MLB_MSD & MLB_MSD_SD3_Msk) >> MLB_MSD_SD3_Pos;
}

static inline hri_mlb_msd_reg_t hri_mlb_get_MSD_reg(const void *const hw, hri_mlb_msd_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MSD;
	tmp &= mask;
	return tmp;
}

static inline hri_mlb_msd_reg_t hri_mlb_read_MSD_reg(const void *const hw)
{
	return ((Mlb *)hw)->MLB_MSD;
}

static inline hri_mlb_hcer_reg_t hri_mlb_get_HCER_CERR_bf(const void *const hw, uint8_t index, hri_mlb_hcer_reg_t mask)
{
	return (((Mlb *)hw)->MLB_HCER[index] & MLB_HCER_CERR(mask)) >> MLB_HCER_CERR_Pos;
}

static inline hri_mlb_hcer_reg_t hri_mlb_read_HCER_CERR_bf(const void *const hw, uint8_t index)
{
	return (((Mlb *)hw)->MLB_HCER[index] & MLB_HCER_CERR_Msk) >> MLB_HCER_CERR_Pos;
}

static inline hri_mlb_hcer_reg_t hri_mlb_get_HCER_reg(const void *const hw, uint8_t index, hri_mlb_hcer_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_HCER[index];
	tmp &= mask;
	return tmp;
}

static inline hri_mlb_hcer_reg_t hri_mlb_read_HCER_reg(const void *const hw, uint8_t index)
{
	return ((Mlb *)hw)->MLB_HCER[index];
}

static inline hri_mlb_hcbr_reg_t hri_mlb_get_HCBR_CHB_bf(const void *const hw, uint8_t index, hri_mlb_hcbr_reg_t mask)
{
	return (((Mlb *)hw)->MLB_HCBR[index] & MLB_HCBR_CHB(mask)) >> MLB_HCBR_CHB_Pos;
}

static inline hri_mlb_hcbr_reg_t hri_mlb_read_HCBR_CHB_bf(const void *const hw, uint8_t index)
{
	return (((Mlb *)hw)->MLB_HCBR[index] & MLB_HCBR_CHB_Msk) >> MLB_HCBR_CHB_Pos;
}

static inline hri_mlb_hcbr_reg_t hri_mlb_get_HCBR_reg(const void *const hw, uint8_t index, hri_mlb_hcbr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_HCBR[index];
	tmp &= mask;
	return tmp;
}

static inline hri_mlb_hcbr_reg_t hri_mlb_read_HCBR_reg(const void *const hw, uint8_t index)
{
	return ((Mlb *)hw)->MLB_HCBR[index];
}

static inline void hri_mlb_set_MLBC0_MLBEN_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 |= MLB_MLBC0_MLBEN;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MLBC0_MLBEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MLBC0;
	tmp = (tmp & MLB_MLBC0_MLBEN) >> MLB_MLBC0_MLBEN_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MLBC0_MLBEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MLBC0;
	tmp &= ~MLB_MLBC0_MLBEN;
	tmp |= value << MLB_MLBC0_MLBEN_Pos;
	((Mlb *)hw)->MLB_MLBC0 = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MLBC0_MLBEN_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 &= ~MLB_MLBC0_MLBEN;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MLBC0_MLBEN_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 ^= MLB_MLBC0_MLBEN;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MLBC0_ZERO_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 |= MLB_MLBC0_ZERO;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MLBC0_ZERO_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MLBC0;
	tmp = (tmp & MLB_MLBC0_ZERO) >> MLB_MLBC0_ZERO_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MLBC0_ZERO_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MLBC0;
	tmp &= ~MLB_MLBC0_ZERO;
	tmp |= value << MLB_MLBC0_ZERO_Pos;
	((Mlb *)hw)->MLB_MLBC0 = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MLBC0_ZERO_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 &= ~MLB_MLBC0_ZERO;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MLBC0_ZERO_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 ^= MLB_MLBC0_ZERO;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MLBC0_MLBLK_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 |= MLB_MLBC0_MLBLK;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MLBC0_MLBLK_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MLBC0;
	tmp = (tmp & MLB_MLBC0_MLBLK) >> MLB_MLBC0_MLBLK_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MLBC0_MLBLK_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MLBC0;
	tmp &= ~MLB_MLBC0_MLBLK;
	tmp |= value << MLB_MLBC0_MLBLK_Pos;
	((Mlb *)hw)->MLB_MLBC0 = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MLBC0_MLBLK_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 &= ~MLB_MLBC0_MLBLK;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MLBC0_MLBLK_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 ^= MLB_MLBC0_MLBLK;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MLBC0_ASYRETRY_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 |= MLB_MLBC0_ASYRETRY;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MLBC0_ASYRETRY_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MLBC0;
	tmp = (tmp & MLB_MLBC0_ASYRETRY) >> MLB_MLBC0_ASYRETRY_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MLBC0_ASYRETRY_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MLBC0;
	tmp &= ~MLB_MLBC0_ASYRETRY;
	tmp |= value << MLB_MLBC0_ASYRETRY_Pos;
	((Mlb *)hw)->MLB_MLBC0 = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MLBC0_ASYRETRY_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 &= ~MLB_MLBC0_ASYRETRY;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MLBC0_ASYRETRY_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 ^= MLB_MLBC0_ASYRETRY;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MLBC0_CTLRETRY_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 |= MLB_MLBC0_CTLRETRY;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MLBC0_CTLRETRY_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MLBC0;
	tmp = (tmp & MLB_MLBC0_CTLRETRY) >> MLB_MLBC0_CTLRETRY_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MLBC0_CTLRETRY_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MLBC0;
	tmp &= ~MLB_MLBC0_CTLRETRY;
	tmp |= value << MLB_MLBC0_CTLRETRY_Pos;
	((Mlb *)hw)->MLB_MLBC0 = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MLBC0_CTLRETRY_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 &= ~MLB_MLBC0_CTLRETRY;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MLBC0_CTLRETRY_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 ^= MLB_MLBC0_CTLRETRY;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MLBC0_MLBCLK_bf(const void *const hw, hri_mlb_mlbc0_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 |= MLB_MLBC0_MLBCLK(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mlbc0_reg_t hri_mlb_get_MLBC0_MLBCLK_bf(const void *const hw, hri_mlb_mlbc0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MLBC0;
	tmp = (tmp & MLB_MLBC0_MLBCLK(mask)) >> MLB_MLBC0_MLBCLK_Pos;
	return tmp;
}

static inline void hri_mlb_write_MLBC0_MLBCLK_bf(const void *const hw, hri_mlb_mlbc0_reg_t data)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MLBC0;
	tmp &= ~MLB_MLBC0_MLBCLK_Msk;
	tmp |= MLB_MLBC0_MLBCLK(data);
	((Mlb *)hw)->MLB_MLBC0 = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MLBC0_MLBCLK_bf(const void *const hw, hri_mlb_mlbc0_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 &= ~MLB_MLBC0_MLBCLK(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MLBC0_MLBCLK_bf(const void *const hw, hri_mlb_mlbc0_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 ^= MLB_MLBC0_MLBCLK(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mlbc0_reg_t hri_mlb_read_MLBC0_MLBCLK_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MLBC0;
	tmp = (tmp & MLB_MLBC0_MLBCLK_Msk) >> MLB_MLBC0_MLBCLK_Pos;
	return tmp;
}

static inline void hri_mlb_set_MLBC0_FCNT_bf(const void *const hw, hri_mlb_mlbc0_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 |= MLB_MLBC0_FCNT(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mlbc0_reg_t hri_mlb_get_MLBC0_FCNT_bf(const void *const hw, hri_mlb_mlbc0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MLBC0;
	tmp = (tmp & MLB_MLBC0_FCNT(mask)) >> MLB_MLBC0_FCNT_Pos;
	return tmp;
}

static inline void hri_mlb_write_MLBC0_FCNT_bf(const void *const hw, hri_mlb_mlbc0_reg_t data)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MLBC0;
	tmp &= ~MLB_MLBC0_FCNT_Msk;
	tmp |= MLB_MLBC0_FCNT(data);
	((Mlb *)hw)->MLB_MLBC0 = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MLBC0_FCNT_bf(const void *const hw, hri_mlb_mlbc0_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 &= ~MLB_MLBC0_FCNT(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MLBC0_FCNT_bf(const void *const hw, hri_mlb_mlbc0_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 ^= MLB_MLBC0_FCNT(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mlbc0_reg_t hri_mlb_read_MLBC0_FCNT_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MLBC0;
	tmp = (tmp & MLB_MLBC0_FCNT_Msk) >> MLB_MLBC0_FCNT_Pos;
	return tmp;
}

static inline void hri_mlb_set_MLBC0_reg(const void *const hw, hri_mlb_mlbc0_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 |= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mlbc0_reg_t hri_mlb_get_MLBC0_reg(const void *const hw, hri_mlb_mlbc0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MLBC0;
	tmp &= mask;
	return tmp;
}

static inline void hri_mlb_write_MLBC0_reg(const void *const hw, hri_mlb_mlbc0_reg_t data)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 = data;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MLBC0_reg(const void *const hw, hri_mlb_mlbc0_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 &= ~mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MLBC0_reg(const void *const hw, hri_mlb_mlbc0_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC0 ^= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mlbc0_reg_t hri_mlb_read_MLBC0_reg(const void *const hw)
{
	return ((Mlb *)hw)->MLB_MLBC0;
}

static inline void hri_mlb_set_MS0_MCS_bf(const void *const hw, hri_mlb_ms0_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MS0 |= MLB_MS0_MCS(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_ms0_reg_t hri_mlb_get_MS0_MCS_bf(const void *const hw, hri_mlb_ms0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MS0;
	tmp = (tmp & MLB_MS0_MCS(mask)) >> MLB_MS0_MCS_Pos;
	return tmp;
}

static inline void hri_mlb_write_MS0_MCS_bf(const void *const hw, hri_mlb_ms0_reg_t data)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MS0;
	tmp &= ~MLB_MS0_MCS_Msk;
	tmp |= MLB_MS0_MCS(data);
	((Mlb *)hw)->MLB_MS0 = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MS0_MCS_bf(const void *const hw, hri_mlb_ms0_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MS0 &= ~MLB_MS0_MCS(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MS0_MCS_bf(const void *const hw, hri_mlb_ms0_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MS0 ^= MLB_MS0_MCS(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_ms0_reg_t hri_mlb_read_MS0_MCS_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MS0;
	tmp = (tmp & MLB_MS0_MCS_Msk) >> MLB_MS0_MCS_Pos;
	return tmp;
}

static inline void hri_mlb_set_MS0_reg(const void *const hw, hri_mlb_ms0_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MS0 |= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_ms0_reg_t hri_mlb_get_MS0_reg(const void *const hw, hri_mlb_ms0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MS0;
	tmp &= mask;
	return tmp;
}

static inline void hri_mlb_write_MS0_reg(const void *const hw, hri_mlb_ms0_reg_t data)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MS0 = data;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MS0_reg(const void *const hw, hri_mlb_ms0_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MS0 &= ~mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MS0_reg(const void *const hw, hri_mlb_ms0_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MS0 ^= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_ms0_reg_t hri_mlb_read_MS0_reg(const void *const hw)
{
	return ((Mlb *)hw)->MLB_MS0;
}

static inline void hri_mlb_set_MS1_MCS_bf(const void *const hw, hri_mlb_ms1_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MS1 |= MLB_MS1_MCS(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_ms1_reg_t hri_mlb_get_MS1_MCS_bf(const void *const hw, hri_mlb_ms1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MS1;
	tmp = (tmp & MLB_MS1_MCS(mask)) >> MLB_MS1_MCS_Pos;
	return tmp;
}

static inline void hri_mlb_write_MS1_MCS_bf(const void *const hw, hri_mlb_ms1_reg_t data)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MS1;
	tmp &= ~MLB_MS1_MCS_Msk;
	tmp |= MLB_MS1_MCS(data);
	((Mlb *)hw)->MLB_MS1 = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MS1_MCS_bf(const void *const hw, hri_mlb_ms1_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MS1 &= ~MLB_MS1_MCS(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MS1_MCS_bf(const void *const hw, hri_mlb_ms1_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MS1 ^= MLB_MS1_MCS(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_ms1_reg_t hri_mlb_read_MS1_MCS_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MS1;
	tmp = (tmp & MLB_MS1_MCS_Msk) >> MLB_MS1_MCS_Pos;
	return tmp;
}

static inline void hri_mlb_set_MS1_reg(const void *const hw, hri_mlb_ms1_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MS1 |= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_ms1_reg_t hri_mlb_get_MS1_reg(const void *const hw, hri_mlb_ms1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MS1;
	tmp &= mask;
	return tmp;
}

static inline void hri_mlb_write_MS1_reg(const void *const hw, hri_mlb_ms1_reg_t data)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MS1 = data;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MS1_reg(const void *const hw, hri_mlb_ms1_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MS1 &= ~mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MS1_reg(const void *const hw, hri_mlb_ms1_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MS1 ^= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_ms1_reg_t hri_mlb_read_MS1_reg(const void *const hw)
{
	return ((Mlb *)hw)->MLB_MS1;
}

static inline void hri_mlb_set_MSS_RSTSYSCMD_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS |= MLB_MSS_RSTSYSCMD;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MSS_RSTSYSCMD_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MSS;
	tmp = (tmp & MLB_MSS_RSTSYSCMD) >> MLB_MSS_RSTSYSCMD_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MSS_RSTSYSCMD_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MSS;
	tmp &= ~MLB_MSS_RSTSYSCMD;
	tmp |= value << MLB_MSS_RSTSYSCMD_Pos;
	((Mlb *)hw)->MLB_MSS = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MSS_RSTSYSCMD_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS &= ~MLB_MSS_RSTSYSCMD;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MSS_RSTSYSCMD_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS ^= MLB_MSS_RSTSYSCMD;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MSS_LKSYSCMD_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS |= MLB_MSS_LKSYSCMD;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MSS_LKSYSCMD_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MSS;
	tmp = (tmp & MLB_MSS_LKSYSCMD) >> MLB_MSS_LKSYSCMD_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MSS_LKSYSCMD_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MSS;
	tmp &= ~MLB_MSS_LKSYSCMD;
	tmp |= value << MLB_MSS_LKSYSCMD_Pos;
	((Mlb *)hw)->MLB_MSS = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MSS_LKSYSCMD_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS &= ~MLB_MSS_LKSYSCMD;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MSS_LKSYSCMD_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS ^= MLB_MSS_LKSYSCMD;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MSS_ULKSYSCMD_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS |= MLB_MSS_ULKSYSCMD;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MSS_ULKSYSCMD_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MSS;
	tmp = (tmp & MLB_MSS_ULKSYSCMD) >> MLB_MSS_ULKSYSCMD_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MSS_ULKSYSCMD_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MSS;
	tmp &= ~MLB_MSS_ULKSYSCMD;
	tmp |= value << MLB_MSS_ULKSYSCMD_Pos;
	((Mlb *)hw)->MLB_MSS = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MSS_ULKSYSCMD_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS &= ~MLB_MSS_ULKSYSCMD;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MSS_ULKSYSCMD_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS ^= MLB_MSS_ULKSYSCMD;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MSS_CSSYSCMD_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS |= MLB_MSS_CSSYSCMD;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MSS_CSSYSCMD_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MSS;
	tmp = (tmp & MLB_MSS_CSSYSCMD) >> MLB_MSS_CSSYSCMD_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MSS_CSSYSCMD_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MSS;
	tmp &= ~MLB_MSS_CSSYSCMD;
	tmp |= value << MLB_MSS_CSSYSCMD_Pos;
	((Mlb *)hw)->MLB_MSS = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MSS_CSSYSCMD_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS &= ~MLB_MSS_CSSYSCMD;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MSS_CSSYSCMD_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS ^= MLB_MSS_CSSYSCMD;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MSS_SWSYSCMD_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS |= MLB_MSS_SWSYSCMD;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MSS_SWSYSCMD_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MSS;
	tmp = (tmp & MLB_MSS_SWSYSCMD) >> MLB_MSS_SWSYSCMD_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MSS_SWSYSCMD_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MSS;
	tmp &= ~MLB_MSS_SWSYSCMD;
	tmp |= value << MLB_MSS_SWSYSCMD_Pos;
	((Mlb *)hw)->MLB_MSS = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MSS_SWSYSCMD_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS &= ~MLB_MSS_SWSYSCMD;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MSS_SWSYSCMD_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS ^= MLB_MSS_SWSYSCMD;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MSS_SERVREQ_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS |= MLB_MSS_SERVREQ;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MSS_SERVREQ_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MSS;
	tmp = (tmp & MLB_MSS_SERVREQ) >> MLB_MSS_SERVREQ_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MSS_SERVREQ_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MSS;
	tmp &= ~MLB_MSS_SERVREQ;
	tmp |= value << MLB_MSS_SERVREQ_Pos;
	((Mlb *)hw)->MLB_MSS = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MSS_SERVREQ_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS &= ~MLB_MSS_SERVREQ;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MSS_SERVREQ_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS ^= MLB_MSS_SERVREQ;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MSS_reg(const void *const hw, hri_mlb_mss_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS |= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mss_reg_t hri_mlb_get_MSS_reg(const void *const hw, hri_mlb_mss_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MSS;
	tmp &= mask;
	return tmp;
}

static inline void hri_mlb_write_MSS_reg(const void *const hw, hri_mlb_mss_reg_t data)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS = data;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MSS_reg(const void *const hw, hri_mlb_mss_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS &= ~mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MSS_reg(const void *const hw, hri_mlb_mss_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MSS ^= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mss_reg_t hri_mlb_read_MSS_reg(const void *const hw)
{
	return ((Mlb *)hw)->MLB_MSS;
}

static inline void hri_mlb_set_MIEN_ISOC_PE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN |= MLB_MIEN_ISOC_PE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MIEN_ISOC_PE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp = (tmp & MLB_MIEN_ISOC_PE) >> MLB_MIEN_ISOC_PE_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MIEN_ISOC_PE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp &= ~MLB_MIEN_ISOC_PE;
	tmp |= value << MLB_MIEN_ISOC_PE_Pos;
	((Mlb *)hw)->MLB_MIEN = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MIEN_ISOC_PE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN &= ~MLB_MIEN_ISOC_PE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MIEN_ISOC_PE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN ^= MLB_MIEN_ISOC_PE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MIEN_ISOC_BUFO_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN |= MLB_MIEN_ISOC_BUFO;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MIEN_ISOC_BUFO_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp = (tmp & MLB_MIEN_ISOC_BUFO) >> MLB_MIEN_ISOC_BUFO_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MIEN_ISOC_BUFO_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp &= ~MLB_MIEN_ISOC_BUFO;
	tmp |= value << MLB_MIEN_ISOC_BUFO_Pos;
	((Mlb *)hw)->MLB_MIEN = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MIEN_ISOC_BUFO_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN &= ~MLB_MIEN_ISOC_BUFO;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MIEN_ISOC_BUFO_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN ^= MLB_MIEN_ISOC_BUFO;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MIEN_SYNC_PE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN |= MLB_MIEN_SYNC_PE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MIEN_SYNC_PE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp = (tmp & MLB_MIEN_SYNC_PE) >> MLB_MIEN_SYNC_PE_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MIEN_SYNC_PE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp &= ~MLB_MIEN_SYNC_PE;
	tmp |= value << MLB_MIEN_SYNC_PE_Pos;
	((Mlb *)hw)->MLB_MIEN = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MIEN_SYNC_PE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN &= ~MLB_MIEN_SYNC_PE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MIEN_SYNC_PE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN ^= MLB_MIEN_SYNC_PE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MIEN_ARX_DONE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN |= MLB_MIEN_ARX_DONE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MIEN_ARX_DONE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp = (tmp & MLB_MIEN_ARX_DONE) >> MLB_MIEN_ARX_DONE_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MIEN_ARX_DONE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp &= ~MLB_MIEN_ARX_DONE;
	tmp |= value << MLB_MIEN_ARX_DONE_Pos;
	((Mlb *)hw)->MLB_MIEN = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MIEN_ARX_DONE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN &= ~MLB_MIEN_ARX_DONE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MIEN_ARX_DONE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN ^= MLB_MIEN_ARX_DONE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MIEN_ARX_PE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN |= MLB_MIEN_ARX_PE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MIEN_ARX_PE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp = (tmp & MLB_MIEN_ARX_PE) >> MLB_MIEN_ARX_PE_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MIEN_ARX_PE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp &= ~MLB_MIEN_ARX_PE;
	tmp |= value << MLB_MIEN_ARX_PE_Pos;
	((Mlb *)hw)->MLB_MIEN = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MIEN_ARX_PE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN &= ~MLB_MIEN_ARX_PE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MIEN_ARX_PE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN ^= MLB_MIEN_ARX_PE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MIEN_ARX_BREAK_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN |= MLB_MIEN_ARX_BREAK;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MIEN_ARX_BREAK_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp = (tmp & MLB_MIEN_ARX_BREAK) >> MLB_MIEN_ARX_BREAK_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MIEN_ARX_BREAK_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp &= ~MLB_MIEN_ARX_BREAK;
	tmp |= value << MLB_MIEN_ARX_BREAK_Pos;
	((Mlb *)hw)->MLB_MIEN = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MIEN_ARX_BREAK_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN &= ~MLB_MIEN_ARX_BREAK;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MIEN_ARX_BREAK_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN ^= MLB_MIEN_ARX_BREAK;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MIEN_ATX_DONE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN |= MLB_MIEN_ATX_DONE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MIEN_ATX_DONE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp = (tmp & MLB_MIEN_ATX_DONE) >> MLB_MIEN_ATX_DONE_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MIEN_ATX_DONE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp &= ~MLB_MIEN_ATX_DONE;
	tmp |= value << MLB_MIEN_ATX_DONE_Pos;
	((Mlb *)hw)->MLB_MIEN = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MIEN_ATX_DONE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN &= ~MLB_MIEN_ATX_DONE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MIEN_ATX_DONE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN ^= MLB_MIEN_ATX_DONE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MIEN_ATX_PE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN |= MLB_MIEN_ATX_PE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MIEN_ATX_PE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp = (tmp & MLB_MIEN_ATX_PE) >> MLB_MIEN_ATX_PE_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MIEN_ATX_PE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp &= ~MLB_MIEN_ATX_PE;
	tmp |= value << MLB_MIEN_ATX_PE_Pos;
	((Mlb *)hw)->MLB_MIEN = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MIEN_ATX_PE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN &= ~MLB_MIEN_ATX_PE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MIEN_ATX_PE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN ^= MLB_MIEN_ATX_PE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MIEN_ATX_BREAK_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN |= MLB_MIEN_ATX_BREAK;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MIEN_ATX_BREAK_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp = (tmp & MLB_MIEN_ATX_BREAK) >> MLB_MIEN_ATX_BREAK_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MIEN_ATX_BREAK_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp &= ~MLB_MIEN_ATX_BREAK;
	tmp |= value << MLB_MIEN_ATX_BREAK_Pos;
	((Mlb *)hw)->MLB_MIEN = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MIEN_ATX_BREAK_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN &= ~MLB_MIEN_ATX_BREAK;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MIEN_ATX_BREAK_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN ^= MLB_MIEN_ATX_BREAK;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MIEN_CRX_DONE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN |= MLB_MIEN_CRX_DONE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MIEN_CRX_DONE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp = (tmp & MLB_MIEN_CRX_DONE) >> MLB_MIEN_CRX_DONE_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MIEN_CRX_DONE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp &= ~MLB_MIEN_CRX_DONE;
	tmp |= value << MLB_MIEN_CRX_DONE_Pos;
	((Mlb *)hw)->MLB_MIEN = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MIEN_CRX_DONE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN &= ~MLB_MIEN_CRX_DONE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MIEN_CRX_DONE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN ^= MLB_MIEN_CRX_DONE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MIEN_CRX_PE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN |= MLB_MIEN_CRX_PE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MIEN_CRX_PE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp = (tmp & MLB_MIEN_CRX_PE) >> MLB_MIEN_CRX_PE_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MIEN_CRX_PE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp &= ~MLB_MIEN_CRX_PE;
	tmp |= value << MLB_MIEN_CRX_PE_Pos;
	((Mlb *)hw)->MLB_MIEN = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MIEN_CRX_PE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN &= ~MLB_MIEN_CRX_PE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MIEN_CRX_PE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN ^= MLB_MIEN_CRX_PE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MIEN_CRX_BREAK_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN |= MLB_MIEN_CRX_BREAK;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MIEN_CRX_BREAK_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp = (tmp & MLB_MIEN_CRX_BREAK) >> MLB_MIEN_CRX_BREAK_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MIEN_CRX_BREAK_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp &= ~MLB_MIEN_CRX_BREAK;
	tmp |= value << MLB_MIEN_CRX_BREAK_Pos;
	((Mlb *)hw)->MLB_MIEN = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MIEN_CRX_BREAK_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN &= ~MLB_MIEN_CRX_BREAK;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MIEN_CRX_BREAK_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN ^= MLB_MIEN_CRX_BREAK;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MIEN_CTX_DONE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN |= MLB_MIEN_CTX_DONE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MIEN_CTX_DONE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp = (tmp & MLB_MIEN_CTX_DONE) >> MLB_MIEN_CTX_DONE_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MIEN_CTX_DONE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp &= ~MLB_MIEN_CTX_DONE;
	tmp |= value << MLB_MIEN_CTX_DONE_Pos;
	((Mlb *)hw)->MLB_MIEN = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MIEN_CTX_DONE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN &= ~MLB_MIEN_CTX_DONE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MIEN_CTX_DONE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN ^= MLB_MIEN_CTX_DONE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MIEN_CTX_PE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN |= MLB_MIEN_CTX_PE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MIEN_CTX_PE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp = (tmp & MLB_MIEN_CTX_PE) >> MLB_MIEN_CTX_PE_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MIEN_CTX_PE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp &= ~MLB_MIEN_CTX_PE;
	tmp |= value << MLB_MIEN_CTX_PE_Pos;
	((Mlb *)hw)->MLB_MIEN = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MIEN_CTX_PE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN &= ~MLB_MIEN_CTX_PE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MIEN_CTX_PE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN ^= MLB_MIEN_CTX_PE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MIEN_CTX_BREAK_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN |= MLB_MIEN_CTX_BREAK;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MIEN_CTX_BREAK_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp = (tmp & MLB_MIEN_CTX_BREAK) >> MLB_MIEN_CTX_BREAK_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MIEN_CTX_BREAK_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp &= ~MLB_MIEN_CTX_BREAK;
	tmp |= value << MLB_MIEN_CTX_BREAK_Pos;
	((Mlb *)hw)->MLB_MIEN = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MIEN_CTX_BREAK_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN &= ~MLB_MIEN_CTX_BREAK;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MIEN_CTX_BREAK_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN ^= MLB_MIEN_CTX_BREAK;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MIEN_reg(const void *const hw, hri_mlb_mien_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN |= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mien_reg_t hri_mlb_get_MIEN_reg(const void *const hw, hri_mlb_mien_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MIEN;
	tmp &= mask;
	return tmp;
}

static inline void hri_mlb_write_MIEN_reg(const void *const hw, hri_mlb_mien_reg_t data)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN = data;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MIEN_reg(const void *const hw, hri_mlb_mien_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN &= ~mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MIEN_reg(const void *const hw, hri_mlb_mien_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MIEN ^= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mien_reg_t hri_mlb_read_MIEN_reg(const void *const hw)
{
	return ((Mlb *)hw)->MLB_MIEN;
}

static inline void hri_mlb_set_MLBC1_LOCK_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC1 |= MLB_MLBC1_LOCK;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MLBC1_LOCK_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MLBC1;
	tmp = (tmp & MLB_MLBC1_LOCK) >> MLB_MLBC1_LOCK_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MLBC1_LOCK_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MLBC1;
	tmp &= ~MLB_MLBC1_LOCK;
	tmp |= value << MLB_MLBC1_LOCK_Pos;
	((Mlb *)hw)->MLB_MLBC1 = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MLBC1_LOCK_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC1 &= ~MLB_MLBC1_LOCK;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MLBC1_LOCK_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC1 ^= MLB_MLBC1_LOCK;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MLBC1_CLKM_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC1 |= MLB_MLBC1_CLKM;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MLBC1_CLKM_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MLBC1;
	tmp = (tmp & MLB_MLBC1_CLKM) >> MLB_MLBC1_CLKM_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MLBC1_CLKM_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MLBC1;
	tmp &= ~MLB_MLBC1_CLKM;
	tmp |= value << MLB_MLBC1_CLKM_Pos;
	((Mlb *)hw)->MLB_MLBC1 = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MLBC1_CLKM_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC1 &= ~MLB_MLBC1_CLKM;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MLBC1_CLKM_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC1 ^= MLB_MLBC1_CLKM;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MLBC1_NDA_bf(const void *const hw, hri_mlb_mlbc1_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC1 |= MLB_MLBC1_NDA(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mlbc1_reg_t hri_mlb_get_MLBC1_NDA_bf(const void *const hw, hri_mlb_mlbc1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MLBC1;
	tmp = (tmp & MLB_MLBC1_NDA(mask)) >> MLB_MLBC1_NDA_Pos;
	return tmp;
}

static inline void hri_mlb_write_MLBC1_NDA_bf(const void *const hw, hri_mlb_mlbc1_reg_t data)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MLBC1;
	tmp &= ~MLB_MLBC1_NDA_Msk;
	tmp |= MLB_MLBC1_NDA(data);
	((Mlb *)hw)->MLB_MLBC1 = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MLBC1_NDA_bf(const void *const hw, hri_mlb_mlbc1_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC1 &= ~MLB_MLBC1_NDA(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MLBC1_NDA_bf(const void *const hw, hri_mlb_mlbc1_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC1 ^= MLB_MLBC1_NDA(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mlbc1_reg_t hri_mlb_read_MLBC1_NDA_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MLBC1;
	tmp = (tmp & MLB_MLBC1_NDA_Msk) >> MLB_MLBC1_NDA_Pos;
	return tmp;
}

static inline void hri_mlb_set_MLBC1_reg(const void *const hw, hri_mlb_mlbc1_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC1 |= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mlbc1_reg_t hri_mlb_get_MLBC1_reg(const void *const hw, hri_mlb_mlbc1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MLBC1;
	tmp &= mask;
	return tmp;
}

static inline void hri_mlb_write_MLBC1_reg(const void *const hw, hri_mlb_mlbc1_reg_t data)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC1 = data;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MLBC1_reg(const void *const hw, hri_mlb_mlbc1_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC1 &= ~mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MLBC1_reg(const void *const hw, hri_mlb_mlbc1_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MLBC1 ^= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mlbc1_reg_t hri_mlb_read_MLBC1_reg(const void *const hw)
{
	return ((Mlb *)hw)->MLB_MLBC1;
}

static inline void hri_mlb_set_HCTL_RST0_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_HCTL |= MLB_HCTL_RST0;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_HCTL_RST0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_HCTL;
	tmp = (tmp & MLB_HCTL_RST0) >> MLB_HCTL_RST0_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_HCTL_RST0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_HCTL;
	tmp &= ~MLB_HCTL_RST0;
	tmp |= value << MLB_HCTL_RST0_Pos;
	((Mlb *)hw)->MLB_HCTL = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_HCTL_RST0_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_HCTL &= ~MLB_HCTL_RST0;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_HCTL_RST0_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_HCTL ^= MLB_HCTL_RST0;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_HCTL_RST1_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_HCTL |= MLB_HCTL_RST1;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_HCTL_RST1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_HCTL;
	tmp = (tmp & MLB_HCTL_RST1) >> MLB_HCTL_RST1_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_HCTL_RST1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_HCTL;
	tmp &= ~MLB_HCTL_RST1;
	tmp |= value << MLB_HCTL_RST1_Pos;
	((Mlb *)hw)->MLB_HCTL = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_HCTL_RST1_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_HCTL &= ~MLB_HCTL_RST1;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_HCTL_RST1_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_HCTL ^= MLB_HCTL_RST1;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_HCTL_EN_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_HCTL |= MLB_HCTL_EN;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_HCTL_EN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_HCTL;
	tmp = (tmp & MLB_HCTL_EN) >> MLB_HCTL_EN_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_HCTL_EN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_HCTL;
	tmp &= ~MLB_HCTL_EN;
	tmp |= value << MLB_HCTL_EN_Pos;
	((Mlb *)hw)->MLB_HCTL = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_HCTL_EN_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_HCTL &= ~MLB_HCTL_EN;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_HCTL_EN_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_HCTL ^= MLB_HCTL_EN;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_HCTL_reg(const void *const hw, hri_mlb_hctl_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_HCTL |= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_hctl_reg_t hri_mlb_get_HCTL_reg(const void *const hw, hri_mlb_hctl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_HCTL;
	tmp &= mask;
	return tmp;
}

static inline void hri_mlb_write_HCTL_reg(const void *const hw, hri_mlb_hctl_reg_t data)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_HCTL = data;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_HCTL_reg(const void *const hw, hri_mlb_hctl_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_HCTL &= ~mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_HCTL_reg(const void *const hw, hri_mlb_hctl_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_HCTL ^= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_hctl_reg_t hri_mlb_read_HCTL_reg(const void *const hw)
{
	return ((Mlb *)hw)->MLB_HCTL;
}

static inline void hri_mlb_set_HCMR_CHM_bf(const void *const hw, uint8_t index, hri_mlb_hcmr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_HCMR[index] |= MLB_HCMR_CHM(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_hcmr_reg_t hri_mlb_get_HCMR_CHM_bf(const void *const hw, uint8_t index, hri_mlb_hcmr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_HCMR[index];
	tmp = (tmp & MLB_HCMR_CHM(mask)) >> MLB_HCMR_CHM_Pos;
	return tmp;
}

static inline void hri_mlb_write_HCMR_CHM_bf(const void *const hw, uint8_t index, hri_mlb_hcmr_reg_t data)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_HCMR[index];
	tmp &= ~MLB_HCMR_CHM_Msk;
	tmp |= MLB_HCMR_CHM(data);
	((Mlb *)hw)->MLB_HCMR[index] = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_HCMR_CHM_bf(const void *const hw, uint8_t index, hri_mlb_hcmr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_HCMR[index] &= ~MLB_HCMR_CHM(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_HCMR_CHM_bf(const void *const hw, uint8_t index, hri_mlb_hcmr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_HCMR[index] ^= MLB_HCMR_CHM(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_hcmr_reg_t hri_mlb_read_HCMR_CHM_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_HCMR[index];
	tmp = (tmp & MLB_HCMR_CHM_Msk) >> MLB_HCMR_CHM_Pos;
	return tmp;
}

static inline void hri_mlb_set_HCMR_reg(const void *const hw, uint8_t index, hri_mlb_hcmr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_HCMR[index] |= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_hcmr_reg_t hri_mlb_get_HCMR_reg(const void *const hw, uint8_t index, hri_mlb_hcmr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_HCMR[index];
	tmp &= mask;
	return tmp;
}

static inline void hri_mlb_write_HCMR_reg(const void *const hw, uint8_t index, hri_mlb_hcmr_reg_t data)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_HCMR[index] = data;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_HCMR_reg(const void *const hw, uint8_t index, hri_mlb_hcmr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_HCMR[index] &= ~mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_HCMR_reg(const void *const hw, uint8_t index, hri_mlb_hcmr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_HCMR[index] ^= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_hcmr_reg_t hri_mlb_read_HCMR_reg(const void *const hw, uint8_t index)
{
	return ((Mlb *)hw)->MLB_HCMR[index];
}

static inline void hri_mlb_set_MDAT_DATA_bf(const void *const hw, uint8_t index, hri_mlb_mdat_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MDAT[index] |= MLB_MDAT_DATA(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mdat_reg_t hri_mlb_get_MDAT_DATA_bf(const void *const hw, uint8_t index, hri_mlb_mdat_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MDAT[index];
	tmp = (tmp & MLB_MDAT_DATA(mask)) >> MLB_MDAT_DATA_Pos;
	return tmp;
}

static inline void hri_mlb_write_MDAT_DATA_bf(const void *const hw, uint8_t index, hri_mlb_mdat_reg_t data)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MDAT[index];
	tmp &= ~MLB_MDAT_DATA_Msk;
	tmp |= MLB_MDAT_DATA(data);
	((Mlb *)hw)->MLB_MDAT[index] = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MDAT_DATA_bf(const void *const hw, uint8_t index, hri_mlb_mdat_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MDAT[index] &= ~MLB_MDAT_DATA(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MDAT_DATA_bf(const void *const hw, uint8_t index, hri_mlb_mdat_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MDAT[index] ^= MLB_MDAT_DATA(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mdat_reg_t hri_mlb_read_MDAT_DATA_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MDAT[index];
	tmp = (tmp & MLB_MDAT_DATA_Msk) >> MLB_MDAT_DATA_Pos;
	return tmp;
}

static inline void hri_mlb_set_MDAT_reg(const void *const hw, uint8_t index, hri_mlb_mdat_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MDAT[index] |= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mdat_reg_t hri_mlb_get_MDAT_reg(const void *const hw, uint8_t index, hri_mlb_mdat_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MDAT[index];
	tmp &= mask;
	return tmp;
}

static inline void hri_mlb_write_MDAT_reg(const void *const hw, uint8_t index, hri_mlb_mdat_reg_t data)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MDAT[index] = data;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MDAT_reg(const void *const hw, uint8_t index, hri_mlb_mdat_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MDAT[index] &= ~mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MDAT_reg(const void *const hw, uint8_t index, hri_mlb_mdat_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MDAT[index] ^= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mdat_reg_t hri_mlb_read_MDAT_reg(const void *const hw, uint8_t index)
{
	return ((Mlb *)hw)->MLB_MDAT[index];
}

static inline void hri_mlb_set_MDWE_MASK_bf(const void *const hw, uint8_t index, hri_mlb_mdwe_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MDWE[index] |= MLB_MDWE_MASK(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mdwe_reg_t hri_mlb_get_MDWE_MASK_bf(const void *const hw, uint8_t index, hri_mlb_mdwe_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MDWE[index];
	tmp = (tmp & MLB_MDWE_MASK(mask)) >> MLB_MDWE_MASK_Pos;
	return tmp;
}

static inline void hri_mlb_write_MDWE_MASK_bf(const void *const hw, uint8_t index, hri_mlb_mdwe_reg_t data)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MDWE[index];
	tmp &= ~MLB_MDWE_MASK_Msk;
	tmp |= MLB_MDWE_MASK(data);
	((Mlb *)hw)->MLB_MDWE[index] = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MDWE_MASK_bf(const void *const hw, uint8_t index, hri_mlb_mdwe_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MDWE[index] &= ~MLB_MDWE_MASK(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MDWE_MASK_bf(const void *const hw, uint8_t index, hri_mlb_mdwe_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MDWE[index] ^= MLB_MDWE_MASK(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mdwe_reg_t hri_mlb_read_MDWE_MASK_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MDWE[index];
	tmp = (tmp & MLB_MDWE_MASK_Msk) >> MLB_MDWE_MASK_Pos;
	return tmp;
}

static inline void hri_mlb_set_MDWE_reg(const void *const hw, uint8_t index, hri_mlb_mdwe_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MDWE[index] |= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mdwe_reg_t hri_mlb_get_MDWE_reg(const void *const hw, uint8_t index, hri_mlb_mdwe_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MDWE[index];
	tmp &= mask;
	return tmp;
}

static inline void hri_mlb_write_MDWE_reg(const void *const hw, uint8_t index, hri_mlb_mdwe_reg_t data)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MDWE[index] = data;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MDWE_reg(const void *const hw, uint8_t index, hri_mlb_mdwe_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MDWE[index] &= ~mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MDWE_reg(const void *const hw, uint8_t index, hri_mlb_mdwe_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MDWE[index] ^= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mdwe_reg_t hri_mlb_read_MDWE_reg(const void *const hw, uint8_t index)
{
	return ((Mlb *)hw)->MLB_MDWE[index];
}

static inline void hri_mlb_set_MCTL_XCMP_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MCTL |= MLB_MCTL_XCMP;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MCTL_XCMP_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MCTL;
	tmp = (tmp & MLB_MCTL_XCMP) >> MLB_MCTL_XCMP_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MCTL_XCMP_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MCTL;
	tmp &= ~MLB_MCTL_XCMP;
	tmp |= value << MLB_MCTL_XCMP_Pos;
	((Mlb *)hw)->MLB_MCTL = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MCTL_XCMP_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MCTL &= ~MLB_MCTL_XCMP;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MCTL_XCMP_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MCTL ^= MLB_MCTL_XCMP;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MCTL_reg(const void *const hw, hri_mlb_mctl_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MCTL |= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mctl_reg_t hri_mlb_get_MCTL_reg(const void *const hw, hri_mlb_mctl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MCTL;
	tmp &= mask;
	return tmp;
}

static inline void hri_mlb_write_MCTL_reg(const void *const hw, hri_mlb_mctl_reg_t data)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MCTL = data;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MCTL_reg(const void *const hw, hri_mlb_mctl_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MCTL &= ~mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MCTL_reg(const void *const hw, hri_mlb_mctl_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MCTL ^= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_mctl_reg_t hri_mlb_read_MCTL_reg(const void *const hw)
{
	return ((Mlb *)hw)->MLB_MCTL;
}

static inline void hri_mlb_set_MADR_TB_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MADR |= MLB_MADR_TB;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MADR_TB_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MADR;
	tmp = (tmp & MLB_MADR_TB) >> MLB_MADR_TB_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MADR_TB_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MADR;
	tmp &= ~MLB_MADR_TB;
	tmp |= value << MLB_MADR_TB_Pos;
	((Mlb *)hw)->MLB_MADR = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MADR_TB_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MADR &= ~MLB_MADR_TB;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MADR_TB_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MADR ^= MLB_MADR_TB;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MADR_WNR_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MADR |= MLB_MADR_WNR;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_MADR_WNR_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MADR;
	tmp = (tmp & MLB_MADR_WNR) >> MLB_MADR_WNR_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_MADR_WNR_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MADR;
	tmp &= ~MLB_MADR_WNR;
	tmp |= value << MLB_MADR_WNR_Pos;
	((Mlb *)hw)->MLB_MADR = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MADR_WNR_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MADR &= ~MLB_MADR_WNR;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MADR_WNR_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MADR ^= MLB_MADR_WNR;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_MADR_ADDR_bf(const void *const hw, hri_mlb_madr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MADR |= MLB_MADR_ADDR(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_madr_reg_t hri_mlb_get_MADR_ADDR_bf(const void *const hw, hri_mlb_madr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MADR;
	tmp = (tmp & MLB_MADR_ADDR(mask)) >> MLB_MADR_ADDR_Pos;
	return tmp;
}

static inline void hri_mlb_write_MADR_ADDR_bf(const void *const hw, hri_mlb_madr_reg_t data)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_MADR;
	tmp &= ~MLB_MADR_ADDR_Msk;
	tmp |= MLB_MADR_ADDR(data);
	((Mlb *)hw)->MLB_MADR = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MADR_ADDR_bf(const void *const hw, hri_mlb_madr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MADR &= ~MLB_MADR_ADDR(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MADR_ADDR_bf(const void *const hw, hri_mlb_madr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MADR ^= MLB_MADR_ADDR(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_madr_reg_t hri_mlb_read_MADR_ADDR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MADR;
	tmp = (tmp & MLB_MADR_ADDR_Msk) >> MLB_MADR_ADDR_Pos;
	return tmp;
}

static inline void hri_mlb_set_MADR_reg(const void *const hw, hri_mlb_madr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MADR |= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_madr_reg_t hri_mlb_get_MADR_reg(const void *const hw, hri_mlb_madr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_MADR;
	tmp &= mask;
	return tmp;
}

static inline void hri_mlb_write_MADR_reg(const void *const hw, hri_mlb_madr_reg_t data)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MADR = data;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_MADR_reg(const void *const hw, hri_mlb_madr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MADR &= ~mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_MADR_reg(const void *const hw, hri_mlb_madr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_MADR ^= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_madr_reg_t hri_mlb_read_MADR_reg(const void *const hw)
{
	return ((Mlb *)hw)->MLB_MADR;
}

static inline void hri_mlb_set_ACTL_SCE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACTL |= MLB_ACTL_SCE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_ACTL_SCE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_ACTL;
	tmp = (tmp & MLB_ACTL_SCE) >> MLB_ACTL_SCE_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_ACTL_SCE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_ACTL;
	tmp &= ~MLB_ACTL_SCE;
	tmp |= value << MLB_ACTL_SCE_Pos;
	((Mlb *)hw)->MLB_ACTL = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_ACTL_SCE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACTL &= ~MLB_ACTL_SCE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_ACTL_SCE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACTL ^= MLB_ACTL_SCE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_ACTL_SMX_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACTL |= MLB_ACTL_SMX;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_ACTL_SMX_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_ACTL;
	tmp = (tmp & MLB_ACTL_SMX) >> MLB_ACTL_SMX_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_ACTL_SMX_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_ACTL;
	tmp &= ~MLB_ACTL_SMX;
	tmp |= value << MLB_ACTL_SMX_Pos;
	((Mlb *)hw)->MLB_ACTL = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_ACTL_SMX_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACTL &= ~MLB_ACTL_SMX;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_ACTL_SMX_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACTL ^= MLB_ACTL_SMX;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_ACTL_DMA_MODE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACTL |= MLB_ACTL_DMA_MODE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_ACTL_DMA_MODE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_ACTL;
	tmp = (tmp & MLB_ACTL_DMA_MODE) >> MLB_ACTL_DMA_MODE_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_ACTL_DMA_MODE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_ACTL;
	tmp &= ~MLB_ACTL_DMA_MODE;
	tmp |= value << MLB_ACTL_DMA_MODE_Pos;
	((Mlb *)hw)->MLB_ACTL = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_ACTL_DMA_MODE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACTL &= ~MLB_ACTL_DMA_MODE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_ACTL_DMA_MODE_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACTL ^= MLB_ACTL_DMA_MODE;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_ACTL_MPB_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACTL |= MLB_ACTL_MPB;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mlb_get_ACTL_MPB_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_ACTL;
	tmp = (tmp & MLB_ACTL_MPB) >> MLB_ACTL_MPB_Pos;
	return (bool)tmp;
}

static inline void hri_mlb_write_ACTL_MPB_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_ACTL;
	tmp &= ~MLB_ACTL_MPB;
	tmp |= value << MLB_ACTL_MPB_Pos;
	((Mlb *)hw)->MLB_ACTL = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_ACTL_MPB_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACTL &= ~MLB_ACTL_MPB;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_ACTL_MPB_bit(const void *const hw)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACTL ^= MLB_ACTL_MPB;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_set_ACTL_reg(const void *const hw, hri_mlb_actl_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACTL |= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_actl_reg_t hri_mlb_get_ACTL_reg(const void *const hw, hri_mlb_actl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_ACTL;
	tmp &= mask;
	return tmp;
}

static inline void hri_mlb_write_ACTL_reg(const void *const hw, hri_mlb_actl_reg_t data)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACTL = data;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_ACTL_reg(const void *const hw, hri_mlb_actl_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACTL &= ~mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_ACTL_reg(const void *const hw, hri_mlb_actl_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACTL ^= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_actl_reg_t hri_mlb_read_ACTL_reg(const void *const hw)
{
	return ((Mlb *)hw)->MLB_ACTL;
}

static inline void hri_mlb_set_ACSR_CHS_bf(const void *const hw, uint8_t index, hri_mlb_acsr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACSR[index] |= MLB_ACSR_CHS(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_acsr_reg_t hri_mlb_get_ACSR_CHS_bf(const void *const hw, uint8_t index, hri_mlb_acsr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_ACSR[index];
	tmp = (tmp & MLB_ACSR_CHS(mask)) >> MLB_ACSR_CHS_Pos;
	return tmp;
}

static inline void hri_mlb_write_ACSR_CHS_bf(const void *const hw, uint8_t index, hri_mlb_acsr_reg_t data)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_ACSR[index];
	tmp &= ~MLB_ACSR_CHS_Msk;
	tmp |= MLB_ACSR_CHS(data);
	((Mlb *)hw)->MLB_ACSR[index] = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_ACSR_CHS_bf(const void *const hw, uint8_t index, hri_mlb_acsr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACSR[index] &= ~MLB_ACSR_CHS(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_ACSR_CHS_bf(const void *const hw, uint8_t index, hri_mlb_acsr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACSR[index] ^= MLB_ACSR_CHS(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_acsr_reg_t hri_mlb_read_ACSR_CHS_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_ACSR[index];
	tmp = (tmp & MLB_ACSR_CHS_Msk) >> MLB_ACSR_CHS_Pos;
	return tmp;
}

static inline void hri_mlb_set_ACSR_reg(const void *const hw, uint8_t index, hri_mlb_acsr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACSR[index] |= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_acsr_reg_t hri_mlb_get_ACSR_reg(const void *const hw, uint8_t index, hri_mlb_acsr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_ACSR[index];
	tmp &= mask;
	return tmp;
}

static inline void hri_mlb_write_ACSR_reg(const void *const hw, uint8_t index, hri_mlb_acsr_reg_t data)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACSR[index] = data;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_ACSR_reg(const void *const hw, uint8_t index, hri_mlb_acsr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACSR[index] &= ~mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_ACSR_reg(const void *const hw, uint8_t index, hri_mlb_acsr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACSR[index] ^= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_acsr_reg_t hri_mlb_read_ACSR_reg(const void *const hw, uint8_t index)
{
	return ((Mlb *)hw)->MLB_ACSR[index];
}

static inline void hri_mlb_set_ACMR_CHM_bf(const void *const hw, uint8_t index, hri_mlb_acmr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACMR[index] |= MLB_ACMR_CHM(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_acmr_reg_t hri_mlb_get_ACMR_CHM_bf(const void *const hw, uint8_t index, hri_mlb_acmr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_ACMR[index];
	tmp = (tmp & MLB_ACMR_CHM(mask)) >> MLB_ACMR_CHM_Pos;
	return tmp;
}

static inline void hri_mlb_write_ACMR_CHM_bf(const void *const hw, uint8_t index, hri_mlb_acmr_reg_t data)
{
	uint32_t tmp;
	MLB_CRITICAL_SECTION_ENTER();
	tmp = ((Mlb *)hw)->MLB_ACMR[index];
	tmp &= ~MLB_ACMR_CHM_Msk;
	tmp |= MLB_ACMR_CHM(data);
	((Mlb *)hw)->MLB_ACMR[index] = tmp;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_ACMR_CHM_bf(const void *const hw, uint8_t index, hri_mlb_acmr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACMR[index] &= ~MLB_ACMR_CHM(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_ACMR_CHM_bf(const void *const hw, uint8_t index, hri_mlb_acmr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACMR[index] ^= MLB_ACMR_CHM(mask);
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_acmr_reg_t hri_mlb_read_ACMR_CHM_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_ACMR[index];
	tmp = (tmp & MLB_ACMR_CHM_Msk) >> MLB_ACMR_CHM_Pos;
	return tmp;
}

static inline void hri_mlb_set_ACMR_reg(const void *const hw, uint8_t index, hri_mlb_acmr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACMR[index] |= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_acmr_reg_t hri_mlb_get_ACMR_reg(const void *const hw, uint8_t index, hri_mlb_acmr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mlb *)hw)->MLB_ACMR[index];
	tmp &= mask;
	return tmp;
}

static inline void hri_mlb_write_ACMR_reg(const void *const hw, uint8_t index, hri_mlb_acmr_reg_t data)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACMR[index] = data;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_clear_ACMR_reg(const void *const hw, uint8_t index, hri_mlb_acmr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACMR[index] &= ~mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mlb_toggle_ACMR_reg(const void *const hw, uint8_t index, hri_mlb_acmr_reg_t mask)
{
	MLB_CRITICAL_SECTION_ENTER();
	((Mlb *)hw)->MLB_ACMR[index] ^= mask;
	MLB_CRITICAL_SECTION_LEAVE();
}

static inline hri_mlb_acmr_reg_t hri_mlb_read_ACMR_reg(const void *const hw, uint8_t index)
{
	return ((Mlb *)hw)->MLB_ACMR[index];
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_MLB_V71B_H_INCLUDED */
#endif /* _SAMV71_MLB_COMPONENT_ */
