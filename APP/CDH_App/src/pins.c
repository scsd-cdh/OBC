#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#include "include/pins.h"

/* Transceiver */
const struct gpio_dt_spec trans_on        = GPIO_DT_SPEC_GET_BY_IDX(TRANSCEIVER_NODE, transceiver_gpios, 0);
const struct gpio_dt_spec trans_off       = GPIO_DT_SPEC_GET_BY_IDX(TRANSCEIVER_NODE, transceiver_gpios, 1);
const struct gpio_dt_spec trans_mi2c_dis  = GPIO_DT_SPEC_GET_BY_IDX(TRANSCEIVER_NODE, transceiver_gpios, 2);
const struct gpio_dt_spec trans_ri2c_dis  = GPIO_DT_SPEC_GET_BY_IDX(TRANSCEIVER_NODE, transceiver_gpios, 3);
const struct gpio_dt_spec trans_irq       = GPIO_DT_SPEC_GET_BY_IDX(TRANSCEIVER_NODE, transceiver_gpios, 4);

/* Astroyeast */
const struct gpio_dt_spec yeast_on        = GPIO_DT_SPEC_GET_BY_IDX(YEAST_NODE, astroyeast_gpios, 0);
const struct gpio_dt_spec yeast_off       = GPIO_DT_SPEC_GET_BY_IDX(YEAST_NODE, astroyeast_gpios, 1);
const struct gpio_dt_spec yeast_mi2c_dis  = GPIO_DT_SPEC_GET_BY_IDX(YEAST_NODE, astroyeast_gpios, 2);
const struct gpio_dt_spec yeast_ri2c_dis  = GPIO_DT_SPEC_GET_BY_IDX(YEAST_NODE, astroyeast_gpios, 3);
const struct gpio_dt_spec yeast_irq       = GPIO_DT_SPEC_GET_BY_IDX(YEAST_NODE, astroyeast_gpios, 4);

/* Imager */
const struct gpio_dt_spec imager_on       = GPIO_DT_SPEC_GET_BY_IDX(IMAGER_NODE, imager_gpios, 0);
const struct gpio_dt_spec imager_off      = GPIO_DT_SPEC_GET_BY_IDX(IMAGER_NODE, imager_gpios, 1);
const struct gpio_dt_spec imager_mi2c_dis = GPIO_DT_SPEC_GET_BY_IDX(IMAGER_NODE, imager_gpios, 2);
const struct gpio_dt_spec imager_ri2c_dis = GPIO_DT_SPEC_GET_BY_IDX(IMAGER_NODE, imager_gpios, 3);
const struct gpio_dt_spec imager_irq      = GPIO_DT_SPEC_GET_BY_IDX(IMAGER_NODE, imager_gpios, 4);

/* BMS */
const struct gpio_dt_spec bms_mi2c_dis    = GPIO_DT_SPEC_GET_BY_IDX(BMS_NODE, bms_gpios, 0);
const struct gpio_dt_spec bms_ri2c_dis    = GPIO_DT_SPEC_GET_BY_IDX(BMS_NODE, bms_gpios, 1);
const struct gpio_dt_spec bms_irq         = GPIO_DT_SPEC_GET_BY_IDX(BMS_NODE, bms_gpios, 2);

/* PDS */
const struct gpio_dt_spec pds_mcu_en      = GPIO_DT_SPEC_GET_BY_IDX(PDS_NODE, pds_gpios, 0);
const struct gpio_dt_spec pds_irq         = GPIO_DT_SPEC_GET_BY_IDX(PDS_NODE, pds_gpios, 1);

/* HDRM */
const struct gpio_dt_spec arm_hdrm        = GPIO_DT_SPEC_GET_BY_IDX(HDRM_NODE, hdrm_gpios, 0);
const struct gpio_dt_spec burn_hdrm       = GPIO_DT_SPEC_GET_BY_IDX(HDRM_NODE, hdrm_gpios, 1);
const struct gpio_dt_spec stat_hdrm       = GPIO_DT_SPEC_GET_BY_IDX(HDRM_NODE, hdrm_gpios, 2);
const struct gpio_dt_spec arm_hdrm_2      = GPIO_DT_SPEC_GET_BY_IDX(HDRM_NODE, hdrm_gpios, 3);
const struct gpio_dt_spec burn_hdrm_2     = GPIO_DT_SPEC_GET_BY_IDX(HDRM_NODE, hdrm_gpios, 4);
const struct gpio_dt_spec boot_hdrm_2     = GPIO_DT_SPEC_GET_BY_IDX(HDRM_NODE, hdrm_gpios, 5);

/* MRAM */
const struct gpio_dt_spec ram_cs          = GPIO_DT_SPEC_GET_BY_IDX(MRAM_NODE, mram_gpios, 0);
const struct gpio_dt_spec ram_cs_2        = GPIO_DT_SPEC_GET_BY_IDX(MRAM_NODE, mram_gpios, 1);
const struct gpio_dt_spec ram_en          = GPIO_DT_SPEC_GET_BY_IDX(MRAM_NODE, mram_gpios, 2);
const struct gpio_dt_spec ram_en_2        = GPIO_DT_SPEC_GET_BY_IDX(MRAM_NODE, mram_gpios, 3);
const struct gpio_dt_spec ram_wp          = GPIO_DT_SPEC_GET_BY_IDX(MRAM_NODE, mram_gpios, 4);
const struct gpio_dt_spec ram_wp_2        = GPIO_DT_SPEC_GET_BY_IDX(MRAM_NODE, mram_gpios, 5);

/* Camera */
const struct gpio_dt_spec cam_id          = GPIO_DT_SPEC_GET_BY_IDX(CAM_NODE, cam_gpios, 0);

/* ADCS */
const struct gpio_dt_spec adcs_on         = GPIO_DT_SPEC_GET_BY_IDX(ADCS_NODE, adcs_gpios, 0);
const struct gpio_dt_spec adcs_off        = GPIO_DT_SPEC_GET_BY_IDX(ADCS_NODE, adcs_gpios, 1);
const struct gpio_dt_spec adcs_mi2c_dis   = GPIO_DT_SPEC_GET_BY_IDX(ADCS_NODE, adcs_gpios, 2);
const struct gpio_dt_spec adcs_ri2c_dis   = GPIO_DT_SPEC_GET_BY_IDX(ADCS_NODE, adcs_gpios, 3);

void gpio_init() {
    /* INCOMPLETE*/
    const struct gpio_dt_spec trans_pins[] = {
        DT_FOREACH_PROP_ELEM_SEP(DT_NODELABEL(transceiver), transceiver_gpios,
                                 GPIO_DT_SPEC_GET_BY_IDX, (,))
    };
    /* ...one array per group... */

    const struct pin_group groups[] = {
        { DT_PROP(DT_NODELABEL(transceiver), transceiver_label),
          trans_pins, ARRAY_SIZE(trans_pins) },
        { DT_PROP(DT_NODELABEL(astroyeast), astroyeast_label),
          yeast_pins, ARRAY_SIZE(yeast_pins) },
        /* ... */
    };
    int board_nodes[] = {TRANSCEIVER_NODE, };
}