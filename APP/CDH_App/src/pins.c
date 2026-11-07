#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

#include "include/pins.h"

LOG_MODULE_REGISTER(pins, LOG_LEVEL_INF);



/* Define each global spec once */
#define DEFINE_PIN(name, lbl, prop, idx, flags) \
const struct gpio_dt_spec name = \
GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(lbl), prop, idx);
PIN_TABLE(DEFINE_PIN)
#undef DEFINE_PIN

static int init_one(const struct gpio_dt_spec *spec, gpio_flags_t flags,
            const char *name)
{
    if (!gpio_is_ready_dt(spec)) {
        LOG_ERR("%s: GPIO controller not ready", name);
        return -ENODEV;
    }

    /* dt_flags (ACTIVE_HIGH/LOW) are merged in by gpio_pin_configure_dt() */
    int ret = gpio_pin_configure_dt(spec, flags);

    if (ret < 0) {
        LOG_ERR("%s: configure failed (%d)", name, ret);
    }
    return ret;
}

int pins_init(void)
{
    int first_err = 0;
    int ret;

// Initialize every pin name defined in app.overlay with their respective attribute
#define INIT_PIN(name, lbl, prop, idx, flags) \
ret = init_one(&name, (flags), #name); \
if (ret < 0 && first_err == 0) { \
first_err = ret; \
}
    PIN_TABLE(INIT_PIN)
#undef INIT_PIN

    return first_err;
}