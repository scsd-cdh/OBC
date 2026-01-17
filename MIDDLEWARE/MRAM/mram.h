#ifndef MRAM_H
#define MRAM_H

#include <hpl_spi_base.h>

// S3A3204V0M MRAM Command Definitions

//Commands not needing ADDR or DATA
#define Write_EN 0x06   // Command to Write Memory Enable (typical for MRAM)
#define Write_DIS 0x04  // Command to Write Memory Disable (typical for MRAM)
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

const uint32_t STATUS_REG = 0x000000;
const uint32_t CONFIGURATION_REG1 = 0x000002;
const uint32_t CONFIGURATION_REG2 = 0x000003;
const uint32_t CONFIGURATION_REG3 = 0x000004;
const uint32_t CONFIGURATION_REG4 = 0x000005;
const uint32_t DEVICE_ID_REG = 0x000030;
const uint32_t UNIQUE_ID_REG = 0x000040;
const uint32_t SERIAL_NUM_REG = 0x000080;
const uint32_t NON_AUGMENTED_REG = 0x000200;

void MRAMwrite_cmd_addr_data(uint8_t, uint32_t, const void*, int);
void MRAMwrite_rollover_helper(uint8_t, uint32_t, uint32_t, const void*, int, int); //For telemetry
void MRAMread_cmd_addr(uint8_t, uint32_t, void*, int);
void configWP(void);
void WREN_cmd(void);
void WRDI_cmd(void);
void initMRAM(void);


#endif