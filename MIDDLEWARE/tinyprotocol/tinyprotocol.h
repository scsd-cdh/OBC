//
// Created by Ruben on 2024-10-28.
//

#ifndef TINYEMBED_TINYPROTOCOL_H
#define TINYEMBED_TINYPROTOCOL_H

#include <stdint.h>

// Start condition for the CRC algorithm
const static uint8_t crc_init_value = 0xFF;

// Value XORed to the final register before the CRC is returned
const static uint8_t crc_xor_value = 0xFF;

// Pre-computed AUTOSAR CRC8 table
const static uint8_t crc_lookup_table[256] = {
        0x00, 0x2F, 0x5E, 0x71, 0xBC, 0x93, 0xE2, 0xCD, 0x57, 0x78, 0x09, 0x26, 0xEB, 0xC4, 0xB5, 0x9A,
        0xAE, 0x81, 0xF0, 0xDF, 0x12, 0x3D, 0x4C, 0x63, 0xF9, 0xD6, 0xA7, 0x88, 0x45, 0x6A, 0x1B, 0x34,
        0x73, 0x5C, 0x2D, 0x02, 0xCF, 0xE0, 0x91, 0xBE, 0x24, 0x0B, 0x7A, 0x55, 0x98, 0xB7, 0xC6, 0xE9,
        0xDD, 0xF2, 0x83, 0xAC, 0x61, 0x4E, 0x3F, 0x10, 0x8A, 0xA5, 0xD4, 0xFB, 0x36, 0x19, 0x68, 0x47,
        0xE6, 0xC9, 0xB8, 0x97, 0x5A, 0x75, 0x04, 0x2B, 0xB1, 0x9E, 0xEF, 0xC0, 0x0D, 0x22, 0x53, 0x7C,
        0x48, 0x67, 0x16, 0x39, 0xF4, 0xDB, 0xAA, 0x85, 0x1F, 0x30, 0x41, 0x6E, 0xA3, 0x8C, 0xFD, 0xD2,
        0x95, 0xBA, 0xCB, 0xE4, 0x29, 0x06, 0x77, 0x58, 0xC2, 0xED, 0x9C, 0xB3, 0x7E, 0x51, 0x20, 0x0F,
        0x3B, 0x14, 0x65, 0x4A, 0x87, 0xA8, 0xD9, 0xF6, 0x6C, 0x43, 0x32, 0x1D, 0xD0, 0xFF, 0x8E, 0xA1,
        0xE3, 0xCC, 0xBD, 0x92, 0x5F, 0x70, 0x01, 0x2E, 0xB4, 0x9B, 0xEA, 0xC5, 0x08, 0x27, 0x56, 0x79,
        0x4D, 0x62, 0x13, 0x3C, 0xF1, 0xDE, 0xAF, 0x80, 0x1A, 0x35, 0x44, 0x6B, 0xA6, 0x89, 0xF8, 0xD7,
        0x90, 0xBF, 0xCE, 0xE1, 0x2C, 0x03, 0x72, 0x5D, 0xC7, 0xE8, 0x99, 0xB6, 0x7B, 0x54, 0x25, 0x0A,
        0x3E, 0x11, 0x60, 0x4F, 0x82, 0xAD, 0xDC, 0xF3, 0x69, 0x46, 0x37, 0x18, 0xD5, 0xFA, 0x8B, 0xA4,
        0x05, 0x2A, 0x5B, 0x74, 0xB9, 0x96, 0xE7, 0xC8, 0x52, 0x7D, 0x0C, 0x23, 0xEE, 0xC1, 0xB0, 0x9F,
        0xAB, 0x84, 0xF5, 0xDA, 0x17, 0x38, 0x49, 0x66, 0xFC, 0xD3, 0xA2, 0x8D, 0x40, 0x6F, 0x1E, 0x31,
        0x76, 0x59, 0x28, 0x07, 0xCA, 0xE5, 0x94, 0xBB, 0x21, 0x0E, 0x7F, 0x50, 0x9D, 0xB2, 0xC3, 0xEC,
        0xD8, 0xF7, 0x86, 0xA9, 0x64, 0x4B, 0x3A, 0x15, 0x8F, 0xA0, 0xD1, 0xFE, 0x33, 0x1C, 0x6D, 0x42 };

struct TINYPROTOCOL_Config{
    int16_t (*TINYPROTOCOL_ProcessTelecommand)(uint8_t command, const uint8_t* buffer, uint8_t size);
    int16_t (*TINYPROTOCOL_ProcessTelemetryRequest)(uint8_t command);
    int16_t (*TINYPROTOCOL_WriteBuffer)(const uint8_t* buffer, uint8_t size);
};

int16_t TINYPROTOCOL_Initialize();
int16_t TINYPROTOCOL_ParseByte(const struct TINYPROTOCOL_Config *cfg, uint8_t byte);

// Slave functions
int16_t TINYPROTOCOL_RegisterTelecommand(uint8_t command, uint8_t size);
int16_t TINYPROTOCOL_RegisterTelemetryChannel(uint8_t tlm_channel, const uint8_t* ptr, uint8_t size);
int16_t TINYPROTOCOL_ReadNextTelemetryByte(uint8_t *byte);
int16_t TINYPROTOCOL_TelemetryBytesLeft();

// Master functions 
int16_t TINYPROTOCOL_SendTelecommand(const struct TINYPROTOCOL_Config *cfg, uint8_t tlcmd, const uint8_t* buffer, uint8_t size);
int16_t TINYPROTOCOL_SendEmptyTelecommand(const struct TINYPROTOCOL_Config *cfg, uint8_t tlcmd);
int16_t TINYPROTOCOL_SendTelemetryRequest(const struct TINYPROTOCOL_Config *cfg, uint8_t tlm_req);

uint8_t TINYPROTOCOL_CalculateCRC(const uint8_t* buffer, uint8_t buffer_size);

typedef enum {
    TINYPROTOCOL_FSM_IDLE,
    TINYPROTOCOL_FSM_EXPECT_CMD,
    TINYPROTOCOL_FSM_EXPECT_TLM_REQ,
    TINYPROTOCOL_FSM_EXPECT_TC,
} TINYPROTOCOL_ReceiveFSM;

typedef enum {
    TLM_ACK_PACKET_RESULT_RECEIVED          = 0,
    TLM_ACK_PACKET_RESULT_PROCESSING        = 1,
    TLM_ACK_PACKET_RESULT_COMPLETED         = 2,
    TLM_ACK_PACKET_RESULT_EOVERFLOW         = 3,
    TLM_ACK_PACKET_RESULT_EINVALID_CRC      = 4,
    TLM_ACK_PACKET_RESULT_EINVALID_TLM_REQ  = 5,
    TLM_ACK_PACKET_RESULT_EINVALID_TC       = 6,
} TINYPROTOCOL_TlmAckPacketResult;

typedef struct {
    uint8_t last_command;
    TINYPROTOCOL_TlmAckPacketResult result;
} TINYPROTOCOL_TlmAckPacket;

#define TINYPROTOCOL_MAGIC                  0x9b
#define TINYPROTOCOL_MAX_CMD_NUM            127
#define TINYPROTOCOL_MAX_PACKET_SIZE        14
#define TINYPROTOCOL_MAX_PAYLOAD_SIZE       (TINYPROTOCOL_MAX_PACKET_SIZE - 3)

// ERRORS
#define ETINYPROTOCOL_SUCCESS               0
#define ETINYPROTOCOL_UNKNOWN               1
#define ETINYPROTOCOL_OVERFLOW              2
#define ETINYPROTOCOL_INVALID_CMD_ID        3
#define ETINYPROTOCOL_CMD_USED              4
#define ETINYPROTOCOL_INVALID_PAYLOAD_SIZE  5

// COMMAND IDs
#define TINYPROTOCOL_TC_PING                0
#define TINYPROTOCOL_TC_RESERVED            1
#define TINYPROTOCOL_TLM_ACK                0
#define TINYPROTOCOL_TLM_RESERVED           1

// BUFFER SIZES
#define TINYPROTOCOL_RX_BUFF_SIZE           127

#endif //TINYEMBED_TINYPROTOCOL_H
