#define DT_DRV_COMPAT cdh_mram

#include "mram.h"

#include <errno.h>
#include <string.h>

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(cdh_mram, CONFIG_LOG_DEFAULT_LEVEL);

struct mram_config {
    struct spi_dt_spec spi;
};

static const struct flash_parameters mram_parameters = {
    .write_block_size = 1,
    .erase_value = 0xFF,
};

static int mram_check_range(off_t offset, size_t len)
{
    if (offset < 0 || (uint64_t)offset > MRAM_SIZE) {
        return -EINVAL;
    }

    if (len > MRAM_SIZE - (size_t)offset) {
        return -EINVAL;
    }

    return 0;
}

static int mram_read_flash(const struct device *dev,
                          off_t offset, void *data, size_t len)
{
    const struct mram_config *cfg = dev->config;

    return mram_read(offset, data, len);
}

static int mram_write_flash(const struct device *dev,
                           off_t offset, const void *data, size_t len)
{
    const struct mram_config *cfg = dev->config;

    return mram_write(offset, data, len);
}

static const struct flash_parameters *
mram_get_parameters(const struct device *dev)
{
    ARG_UNUSED(dev);
    return &mram_parameters;
}

static DEVICE_API(flash, mram_flash_api) = {
    .read = mram_read_flash,
    .write = mram_write_flash,
    .get_parameters = mram_get_parameters,
};

#define MRAM_DEFINE(inst)                                           \
    static const struct mram_config mram_config_##inst = {          \
        .spi = SPI_DT_SPEC_INST_GET(inst,                           \
            SPI_WORD_SET(8) | SPI_TRANSFER_MSB, 0),                 \
    };                                                              \
    DEVICE_DT_INST_DEFINE(inst, NULL, NULL, NULL,                   \
        &mram_config_##inst, POST_KERNEL,                           \
        CONFIG_KERNEL_INIT_PRIORITY_DEVICE, &mram_flash_api);

DT_INST_FOREACH_STATUS_OKAY(MRAM_DEFINE)
