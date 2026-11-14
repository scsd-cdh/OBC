#include "mram.h"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(MRAM_TEST, LOG_LEVEL_INF);

static int mram_write_enable(const struct spi_dt_spec *spi)
{
    uint8_t command = MRAM_CMD_WREN;

    const struct spi_buf tx_buf = {
        .buf = &command,
        .len = sizeof(command),
    };

    const struct spi_buf_set tx = {
        .buffers = &tx_buf,
        .count = 1,
    };

    return spi_write_dt(spi, &tx);
}

static int mram_write_disable(const struct spi_dt_spec *spi)
{
    uint8_t command = MRAM_CMD_WRDI;

    const struct spi_buf tx_buf = {
        .buf = &command,
        .len = sizeof(command),
    };

    const struct spi_buf_set tx = {
        .buffers = &tx_buf,
        .count = 1,
    };

    return spi_write_dt(spi, &tx);
}

/*
 * Read from MRAM
 *
 * SPI transaction:
 *
 *   0x03
 *   address[23:16]
 *   address[15:8]
 *   address[7:0]
 *   data...
 */
int mram_read(const struct spi_dt_spec *spi, uint32_t address, void* data, int read_size)
{
    uint8_t command[READ_ANY_COMMAND_SIZE] = {
        Read_Mem_Array,
        (uint8_t)((address >> 16) & 0xFF),
        (uint8_t)((address >> 8) & 0xFF),
        (uint8_t)(address & 0xFF),
    };

    /*
     * We need to clock the SPI bus for both the command
     * and the returned data.
     */
    uint8_t transmit_tx[READ_ANY_COMMAND_SIZE + read_size];

    for (size_t i = 0; i < sizeof(transmit_tx); i++) {
        transmit_tx[i] = 0x00;
    }

    /*
     * Put the command at the beginning of the TX buffer.
     */
    for (size_t i = 0; i < sizeof(command); i++) {
        transmit_tx[i] = command[i];
    }

    uint8_t rx_buffer[READ_ANY_COMMAND_SIZE + read_size];

    const struct spi_buf tx_buf = {
        .buf = transmit_tx,
        .len = sizeof(transmit_tx),
    };

    const struct spi_buf_set tx = {
        .buffers = &tx_buf,
        .count = 1,
    };

    const struct spi_buf rx_buf = {
        .buf = rx_buffer,
        .len = sizeof(rx_buffer),
    };

    const struct spi_buf_set rx = {
        .buffers = &rx_buf,
        .count = 1,
    };

    int err = spi_transceive_dt(spi, &tx, &rx);

    if (err != 0) {
        LOG_ERR("MRAM read failed: %d", err);
        return err;
    }

    // We copy starting from READ_COMMAND_SIZE because the first READ_COMMAND_SIZE byte of the RX are going to be garbage.
    memcpy(data, &rx_buffer[READ_ANY_COMMAND_SIZE], read_size);

    return 0;
}

/*
 * Write to MRAM
 */
int mram_write(const struct spi_dt_spec *spi, uint32_t address, const uint8_t *data, size_t length)
{
    int err = mram_write_enable(spi);

    if (err != 0) {
        LOG_ERR("MRAM WREN failed: %d", err);
        return err;
    }

    /*
     * Command + 3-byte address + data
     */
    uint8_t command[4] = {
        Write_Mem_Array,
        (uint8_t)((address >> 16) & 0xFF),
        (uint8_t)((address >> 8) & 0xFF),
        (uint8_t)(address & 0xFF),
    };

    const struct spi_buf tx_bufs[] = {
        {
            .buf = command,
            .len = sizeof(command),
        },
        {
            .buf = (void *)data,
            .len = length,
        },
    };

    const struct spi_buf_set tx = {
        .buffers = tx_bufs,
        .count = ARRAY_SIZE(tx_bufs),
    };

    err = spi_write_dt(spi, &tx);


    if (err != 0) {
        LOG_ERR("MRAM write failed: %d", err);
        return err;
    }

    err = mram_write_disable(spi);

    if (err != 0) {
        LOG_ERR("MRAM WRDI failed: %d", err);
        return err;
    }

    return 0;
}


/*
 * Print a byte buffer
 */
void print_buffer(const char *name, const uint8_t *data, size_t length)
{
    LOG_INF("%s:", name);

    for (size_t i = 0; i < length; i++) {
        LOG_INF("  [%d] = 0x%02X", i, data[i]);
    }
}

int MRAM_Spi_Test(const struct device *dev)
{
    const struct mram_device_config *cfg = dev->config;

    LOG_INF("=================================");
    LOG_INF("SAM V71 MRAM SPI Test");
    LOG_INF("=================================");

    if (!spi_is_ready_dt(&cfg->spi)) {
        LOG_ERR("MRAM SPI device is not ready");
        return -ENODEV;
    }

    LOG_INF("SPI device ready");
    LOG_INF("SPI frequency: %u Hz", cfg->spi.config.frequency);
    LOG_INF("SPI slave: %u", cfg->spi.config.slave);

    const uint32_t test_address = 0x000000;

    uint8_t write_data[] = {0x12, 0x34, 0x56, 0x78};
    uint8_t read_data[sizeof(write_data)] = {0};

    LOG_INF("Writing MRAM...");

    int ret = mram_write(
        &cfg->spi,
        test_address,
        write_data,
        sizeof(write_data)
    );

    if (ret != 0) {
        LOG_ERR("MRAM write failed: %d", ret);
        return ret;
    }

    LOG_INF("MRAM write successful");

    LOG_INF("Reading MRAM...");

    ret = mram_read(
        &cfg->spi,
        test_address,
        read_data,
        sizeof(read_data)
    );

    if (ret != 0) {
        LOG_ERR("MRAM read failed: %d", ret);
        return ret;
    }

    LOG_INF("MRAM read successful");

    print_buffer("Read data", read_data, sizeof(read_data));

    if (memcmp(write_data, read_data, sizeof(write_data)) == 0) {
        LOG_INF("===============================");
        LOG_INF("MRAM TEST PASSED");
        LOG_INF("===============================");
    } else {
        LOG_ERR("===============================");
        LOG_ERR("MRAM TEST FAILED");
        LOG_ERR("===============================");
        return -EIO;
    }

    return 0;
}