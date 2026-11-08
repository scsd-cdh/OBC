/**
 * @file pins.h
 * @description This file aims to store global variables for all pins relevant to the CDH Board
 *
 */

/* 1. Include Guards to prevent multiple definition errors */
#ifndef PINS_H
#define PINS_H

/* 2. System Library Includes */
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

/* 3. External / Project Includes */

/* 4. Macro Definitions / Constants */
#define TRANSCEIVER_NODE DT_NODELABEL(transceiver)
#define YEAST_NODE DT_NODELABEL(astroyeast)
#define IMAGER_NODE DT_NODELABEL(imager)
#define BMS_NODE DT_NODELABEL(bms)
#define PDS_NODE DT_NODELABEL(pds)
#define HDRM_NODE DT_NODELABEL(hdrm)
#define MRAM_NODE DT_NODELABEL(mram)
#define CAM_NODE DT_NODELABEL(camera)
#define ADCS_NODE DT_NODELABEL(adcs)

#define PIN_TABLE(X) \
	/* Transceiver */ \
	X(trans_on,        transceiver, transceiver_gpios, 0, GPIO_OUTPUT_INACTIVE) \
	X(trans_off,       transceiver, transceiver_gpios, 1, GPIO_OUTPUT_INACTIVE) \
	X(trans_mi2c_dis,  transceiver, transceiver_gpios, 2, GPIO_INPUT) \
	X(trans_ri2c_dis,  transceiver, transceiver_gpios, 3, GPIO_INPUT) \
	X(trans_irq,       transceiver, transceiver_gpios, 4, GPIO_INPUT) \
	/* Astroyeast */ \
	X(yeast_on,        astroyeast,  astroyeast_gpios,  0, GPIO_OUTPUT_INACTIVE) \
	X(yeast_off,       astroyeast,  astroyeast_gpios,  1, GPIO_OUTPUT_INACTIVE) \
	X(yeast_mi2c_dis,  astroyeast,  astroyeast_gpios,  2, GPIO_INPUT) \
	X(yeast_ri2c_dis,  astroyeast,  astroyeast_gpios,  3, GPIO_INPUT) \
	X(yeast_irq,       astroyeast,  astroyeast_gpios,  4, GPIO_INPUT) \
	/* Imager */ \
	X(imager_on,       imager,      imager_gpios,      0, GPIO_OUTPUT_INACTIVE) \
	X(imager_off,      imager,      imager_gpios,      1, GPIO_OUTPUT_INACTIVE) \
	X(imager_mi2c_dis, imager,      imager_gpios,      2, GPIO_INPUT) \
	X(imager_ri2c_dis, imager,      imager_gpios,      3, GPIO_INPUT) \
	X(imager_irq,      imager,      imager_gpios,      4, GPIO_INPUT) \
	/* BMS (first two have no INPUT/OUTPUT comment: assumed INPUT) */ \
	X(bms_mi2c_dis,    bms,         bms_gpios,         0, GPIO_INPUT) \
	X(bms_ri2c_dis,    bms,         bms_gpios,         1, GPIO_INPUT) \
	X(bms_irq,         bms,         bms_gpios,         2, GPIO_INPUT) \
	/* PDS */ \
	X(pds_mcu_en,      pds,         pds_gpios,         0, GPIO_OUTPUT_INACTIVE) \
	X(pds_irq,         pds,         pds_gpios,         1, GPIO_INPUT) \
	/* HDRM */ \
	X(arm_hdrm,        hdrm,        hdrm_gpios,        0, GPIO_INPUT) \
	X(burn_hdrm,       hdrm,        hdrm_gpios,        1, GPIO_INPUT) \
	X(stat_hdrm,       hdrm,        hdrm_gpios,        2, GPIO_INPUT) \
	X(arm_hdrm_2,      hdrm,        hdrm_gpios,        3, GPIO_INPUT) \
	X(burn_hdrm_2,     hdrm,        hdrm_gpios,        4, GPIO_INPUT) \
	X(boot_hdrm_2,     hdrm,        hdrm_gpios,        5, GPIO_INPUT) \
	/* MRAM: !CS / !EN are ACTIVE_HIGH in the overlay, so start them */ \
	/* "active" = physical high = deselected/disabled (see notes)     */ \
	X(ram_cs,          mram,        mram_gpios,        0, GPIO_OUTPUT_ACTIVE) \
	X(ram_cs_2,        mram,        mram_gpios,        1, GPIO_OUTPUT_ACTIVE) \
	X(ram_en,          mram,        mram_gpios,        2, GPIO_OUTPUT_ACTIVE) \
	X(ram_en_2,        mram,        mram_gpios,        3, GPIO_OUTPUT_ACTIVE) \
	X(ram_wp,          mram,        mram_gpios,        4, GPIO_OUTPUT_INACTIVE) \
	X(ram_wp_2,        mram,        mram_gpios,        5, GPIO_OUTPUT_INACTIVE) \
	/* Camera */ \
	X(cam_id,          camera,      cam_gpios,         0, GPIO_OUTPUT_INACTIVE) \
	/* ADCS */ \
	X(adcs_on,         adcs,        adcs_gpios,        0, GPIO_OUTPUT_INACTIVE) \
	X(adcs_off,        adcs,        adcs_gpios,        1, GPIO_OUTPUT_INACTIVE) \
	X(adcs_mi2c_dis,   adcs,        adcs_gpios,        2, GPIO_INPUT) \
	X(adcs_ri2c_dis,   adcs,        adcs_gpios,        3, GPIO_INPUT)

/* extern declarations: usable from any file as <name> */
#define DECLARE_PIN(name, lbl, prop, idx, flags) \
	extern const struct gpio_dt_spec name;
PIN_TABLE(DECLARE_PIN)
#undef DECLARE_PIN

/* Configure every pin. Returns 0, or the first error encountered. */



/* 5. Custom Data Type Definitions (Enums, Structs, Typedefs) */

/* 6. Global Variable Declarations (Use 'extern' sparingly) */

/* 7. Function Prototypes (Public Interface) */
int pins_init(void);

#endif /* PINS_H */