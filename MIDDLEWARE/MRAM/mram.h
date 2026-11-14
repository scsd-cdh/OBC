#pragma once

#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/util.h>

/*
 * S3A3204V0M MRAM commands
 */

//New MRAM cmd
#define MRAM_CMD_WREN       0x06   // Command to Write Memory Enable (typical for MRAM)
#define MRAM_CMD_WRDI       0x04  // Command to Write Memory Disable (typical for MRAM)
#define Read_Status_Reg     0x05
#define Write_Status_Reg    0x01
#define Read_Mem_Array      0x03
#define Write_Mem_Array     0x02
#define SLEEP_CMD           0xB9
#define WAKE_CMD            0xAB

#define MRAM_SIZE           (4U * 1024U * 1024U)
#define MRAM_ADDRESS_SIZE   3U
#define READ_ANY_COMMAND_SIZE 4

/*
 * MRAM SPI device from app.overlay
 */
#define MRAM_NODE DT_NODELABEL(mram)

#if !DT_NODE_EXISTS(MRAM_NODE)
#error "MRAM node not found in Devicetree"
#endif

/*
 * Zephyr automatically gets:
 * - SPI controller
 * - chip select GPIO
 * - frequency
 * - SPI mode
 * - word size
 */
static const struct spi_dt_spec mram =
    SPI_DT_SPEC_GET(
        MRAM_NODE,
        SPI_WORD_SET(8) | SPI_TRANSFER_MSB
    );

struct mram_device_config {
    struct spi_dt_spec spi;
};

int mram_read(const struct spi_dt_spec *spi, uint32_t address, void* data, int read_size);
int mram_write(const struct spi_dt_spec *spi, uint32_t address, const uint8_t *data, size_t length);

int MRAM_Spi_Test(const struct device *dev);

// Test print
void print_buffer(const char *name, const uint8_t *data, size_t length);
