#pragma once

#include <zephyr/drivers/spi.h>

/*
 * S3A3204V0M MRAM commands
 */

//Commands not needing ADDR or DATA
#define MRAM_CMD_WREN 0x06   // Command to Write Memory Enable (typical for MRAM)
#define MRAM_CMD_WRDI 0x04  // Command to Write Memory Disable (typical for MRAM)
#define SSPI_EN 0xFF
#define Entr_Deep_Pwr_Mode 0xB9
#define Exit_Deep_Pwr_Mode 0xAB
#define Software_Reset_EN 0x66
#define Software_Reset 0x99
#define ADDRESS_SIZE_IN_BYTES 3

//Commands needing Data
#define Read_Status_Reg 0x05
#define Read_Serial_Reg 0xC3
#define READ_DEVICE_ID_CMD 0x9F  // Command to read device ID (typical for MRAM)
#define READ_UNIQUE_ID_CMD 0x4C  // Command to read unique ID (typical for MRAM)
#define Write_Status_Reg 0x01

//Commands needing Data + Address
#define Read_Any_Reg 0x65
#define Write_Any_Reg 0x71
#define Read_Mem_Array 0x03
#define Write_Mem_Array 0x02
#define Read_Aug_Array 0x4B
#define Write_Aug_Array 0x42

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

int mram_read(uint32_t address, void* data, int read_size);
int mram_write(uint32_t address, const uint8_t *data, size_t length);

int MRAM_Spi_Test(void);

// Test print
void print_buffer(const char *name, const uint8_t *data, size_t length);
