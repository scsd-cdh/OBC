//
// Created by Ruben on 2024-10-28.
//

#include "tinyprotocol.h"

static const uint8_t *tlm_pointer[TINYPROTOCOL_MAX_CMD_NUM] = {0};
static uint8_t tlm_pointer_size[TINYPROTOCOL_MAX_CMD_NUM] = {0};
static uint8_t tlm_current_channel = 0;
static uint8_t tlm_buffer_idx = 0;

static uint8_t tc_size[TINYPROTOCOL_MAX_CMD_NUM] = {0};
static uint8_t tlcmd_buffer_idx = 0;
static uint8_t tlcmd_current = 0;

static uint8_t rx_buffer[TINYPROTOCOL_RX_BUFF_SIZE] = {0};
TINYPROTOCOL_ReceiveFSM current_state = TINYPROTOCOL_FSM_IDLE;

static uint8_t tc_send_buffer_tmp[TINYPROTOCOL_MAX_PACKET_SIZE] = {0};

static volatile TINYPROTOCOL_TlmAckPacket TlmAckPacket = {0};

uint8_t TINYPROTOCOL_CalculateCRC(const uint8_t* buffer, uint8_t buffer_size) {
    uint8_t crc = crc_init_value;

    uint8_t i;
    for (i = 0; i < buffer_size; i++) {
        crc = crc_lookup_table[buffer[i] ^ crc];
    }

    return crc ^ crc_xor_value;
}

int16_t TINYPROTOCOL_Initialize() {
    // Register reserved Telecommands
    // Telecommand IDs are included in the size
    tc_size[TINYPROTOCOL_TC_PING] = 1;

    // Register reserved Telemetry Requests
    tlm_pointer_size[TINYPROTOCOL_TLM_ACK] = sizeof(TINYPROTOCOL_TlmAckPacket);
    tlm_pointer[TINYPROTOCOL_TLM_ACK] = (const uint8_t *) &TlmAckPacket;

    return ETINYPROTOCOL_SUCCESS;
}

int16_t TINYPROTOCOL_ParseByte(const struct TINYPROTOCOL_Config *cfg, uint8_t byte) {
    switch(current_state) {
        case TINYPROTOCOL_FSM_IDLE:
            if (byte == TINYPROTOCOL_MAGIC)
                current_state = TINYPROTOCOL_FSM_EXPECT_CMD;
            break;
        case TINYPROTOCOL_FSM_EXPECT_CMD:
            TlmAckPacket.last_command = byte;
            TlmAckPacket.result = TLM_ACK_PACKET_RESULT_RECEIVED;

            if ((byte & 0x80) != 0) {
                // get the channel crosponding for the tele request cmd received. 
                tlm_current_channel = (byte & 0x7F);
                tlm_buffer_idx = 0;
                current_state = TINYPROTOCOL_FSM_EXPECT_TLM_REQ;

                // check if tele request cmd is a valid cmd
                if(tlm_pointer_size[tlm_current_channel] <= 0) {
                    TlmAckPacket.result = TLM_ACK_PACKET_RESULT_EINVALID_TLM_REQ;
                    current_state = TINYPROTOCOL_FSM_IDLE;
                    // TODO add a return here
                }
            } else {
                tlcmd_current = byte;
                tlcmd_buffer_idx = 0;
                current_state = TINYPROTOCOL_FSM_EXPECT_TC;

                if(tc_size[tlcmd_current] <= 0) {
                    TlmAckPacket.result = TLM_ACK_PACKET_RESULT_EINVALID_TC;
                    current_state = TINYPROTOCOL_FSM_IDLE;
                    return -ETINYPROTOCOL_INVALID_CMD_ID;
                }

                rx_buffer[tlcmd_buffer_idx ++] = tlcmd_current;
            }
            break;
        case TINYPROTOCOL_FSM_EXPECT_TLM_REQ:
            if (TINYPROTOCOL_CalculateCRC(&tlm_current_channel, 1) != byte) {
                TlmAckPacket.result = TLM_ACK_PACKET_RESULT_EINVALID_CRC;
            } else {
                cfg->TINYPROTOCOL_ProcessTelemetryRequest(TlmAckPacket.last_command & 0x7F);
                TlmAckPacket.result = TLM_ACK_PACKET_RESULT_COMPLETED;
            }

            current_state = TINYPROTOCOL_FSM_IDLE;
            break;
        case TINYPROTOCOL_FSM_EXPECT_TC:
            if (tlcmd_buffer_idx == tc_size[tlcmd_current]) {
                if (TINYPROTOCOL_CalculateCRC(rx_buffer, tc_size[tlcmd_current]) == byte) {
                    TlmAckPacket.result = TLM_ACK_PACKET_RESULT_PROCESSING;

                    if (tc_size[tlcmd_current] == 1) {
                        uint8_t dummy = 0;
                        cfg->TINYPROTOCOL_ProcessTelecommand(rx_buffer[0], &dummy, 1);
                    } else {
                        cfg->TINYPROTOCOL_ProcessTelecommand(rx_buffer[0], &rx_buffer[1], tc_size[tlcmd_current] - 1);
                    }

                    TlmAckPacket.result = TLM_ACK_PACKET_RESULT_COMPLETED;
                } else {
                    TlmAckPacket.result = TLM_ACK_PACKET_RESULT_EINVALID_CRC;
                }
                // TODO send Telecommand ACK 
                // TINYPROTOCOL_SendTelecommand()

                current_state = TINYPROTOCOL_FSM_IDLE;
            } else {
                rx_buffer[tlcmd_buffer_idx ++] = byte;
            }

            break;
    }

    return ETINYPROTOCOL_SUCCESS;
}

int16_t TINYPROTOCOL_RegisterTelecommand(uint8_t cmd, uint8_t size) {
    if (cmd > TINYPROTOCOL_MAX_CMD_NUM)
        return -ETINYPROTOCOL_INVALID_CMD_ID;

    if (cmd < TINYPROTOCOL_TC_RESERVED || tc_size[cmd] > 0)
        return -ETINYPROTOCOL_CMD_USED;

    tc_size[cmd] = size + 1;
    return ETINYPROTOCOL_SUCCESS;
}

int16_t TINYPROTOCOL_SendTelecommand(const struct TINYPROTOCOL_Config *cfg, uint8_t command, const uint8_t* buffer, uint8_t size) {
    if (size > TINYPROTOCOL_MAX_PAYLOAD_SIZE)
        return -ETINYPROTOCOL_INVALID_PAYLOAD_SIZE;

    tc_send_buffer_tmp[0] = TINYPROTOCOL_MAGIC;
    tc_send_buffer_tmp[1] = command;

    uint8_t i;
    for(i = 0; i < size; i ++) {
        tc_send_buffer_tmp[i+2] = buffer[i];
    }

    tc_send_buffer_tmp[size + 2] = TINYPROTOCOL_CalculateCRC(&tc_send_buffer_tmp[1], size + 1);
    return cfg->TINYPROTOCOL_WriteBuffer(tc_send_buffer_tmp, size + 3);
}

int16_t TINYPROTOCOL_SendEmptyTelecommand(const struct TINYPROTOCOL_Config *cfg, uint8_t command) {
    tc_send_buffer_tmp[0] = TINYPROTOCOL_MAGIC;
    tc_send_buffer_tmp[1] = command;
    tc_send_buffer_tmp[2] = TINYPROTOCOL_CalculateCRC(&tc_send_buffer_tmp[1], 1);
    return cfg->TINYPROTOCOL_WriteBuffer(tc_send_buffer_tmp, 3);
}

int16_t TINYPROTOCOL_RegisterTelemetryChannel(uint8_t tlm_channel, const uint8_t* ptr, uint8_t size) {
    if (size > TINYPROTOCOL_MAX_PAYLOAD_SIZE)
        return -ETINYPROTOCOL_INVALID_PAYLOAD_SIZE;

    if (tlm_channel > TINYPROTOCOL_MAX_CMD_NUM)
        return -ETINYPROTOCOL_INVALID_CMD_ID;

    if (tlm_channel < TINYPROTOCOL_TLM_RESERVED || tlm_pointer_size[tlm_channel] > 0)
        return -ETINYPROTOCOL_CMD_USED;

    tlm_pointer[tlm_channel] = ptr;
    tlm_pointer_size[tlm_channel] = size + 1; // Include 1 extra byte for the CRC

    return ETINYPROTOCOL_SUCCESS;
}

int16_t TINYPROTOCOL_SendTelemetryRequest(const struct TINYPROTOCOL_Config *cfg, uint8_t tlm_req) {
    // Calculate crc and create buffer with proper content.
    uint8_t crc = TINYPROTOCOL_CalculateCRC(&tlm_req, 1);

    // Telemetry requests must have the most significant bit set to 1.s
    tlm_req |= 0x80;
    uint8_t buf[3] = {TINYPROTOCOL_MAGIC, tlm_req, crc};

    return cfg->TINYPROTOCOL_WriteBuffer(buf, 3);
}

int16_t TINYPROTOCOL_ReadNextTelemetryByte(uint8_t *byte) {
    *byte = 0xFF;

    if(tlm_current_channel > TINYPROTOCOL_MAX_CMD_NUM || tlm_pointer_size[tlm_current_channel] <= 0) {
        TlmAckPacket.result = TLM_ACK_PACKET_RESULT_EINVALID_TLM_REQ;
        return -ETINYPROTOCOL_INVALID_CMD_ID;
    }

    if (tlm_buffer_idx >= tlm_pointer_size[tlm_current_channel]) {
        TlmAckPacket.result = TLM_ACK_PACKET_RESULT_EOVERFLOW;
        return -ETINYPROTOCOL_OVERFLOW;
    }

    if (tlm_buffer_idx == tlm_pointer_size[tlm_current_channel] - 1) {
        *byte = TINYPROTOCOL_CalculateCRC(tlm_pointer[tlm_current_channel], tlm_pointer_size[tlm_current_channel] - 1);
        tlm_buffer_idx ++;
    } else {
        *byte = tlm_pointer[tlm_current_channel][tlm_buffer_idx++];
    }

    return ETINYPROTOCOL_SUCCESS;
}

int16_t TINYPROTOCOL_TelemetryBytesLeft() {
    if(tlm_current_channel > TINYPROTOCOL_MAX_CMD_NUM || tlm_pointer_size[tlm_current_channel] <= 0) {
        return -ETINYPROTOCOL_INVALID_CMD_ID;
    }

    if (tlm_buffer_idx >= tlm_pointer_size[tlm_current_channel]) {
        return -ETINYPROTOCOL_OVERFLOW;
    }

    return (int16_t)(tlm_pointer_size[tlm_current_channel] - tlm_buffer_idx);
}
