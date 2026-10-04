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


/* 5. Custom Data Type Definitions (Enums, Structs, Typedefs) */
struct pin_group {
    const char *name;
    const struct gpio_dt_spec *pins;
    size_t count;
};



/* 6. Global Variable Declarations (Use 'extern' sparingly) */

/* 7. Function Prototypes (Public Interface) */
void gpio_init(void);

#endif /* PINS_H */