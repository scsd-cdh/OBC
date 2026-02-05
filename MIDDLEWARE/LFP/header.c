#include "lfp/header.h"

#include "cobs_private.h"
#include "crc.h"

lfp_code_t lfp_parse_header(const uint8_t * p_data, lfp_header_t * p_out) {
    // Invalid preamble
    if (p_data[LFP_HEADER_PREAMBLE_POS] != LFP_PREAMBLE) {
        return LFP_EBADDATA;
    }

    // LRI
    const uint8_t lri = p_data[LFP_HEADER_LRI_POS];
    p_out->initiator = LFP_LRI_INITIATOR_GET(lri);
    p_out->target = LFP_LRI_TARGET_GET(lri);

    // Endpoint
    p_out->endpoint = p_data[LFP_HEADER_ENDPOINT_POS] & 0xFE;
    p_out->direction = p_data[LFP_HEADER_ENDPOINT_POS] & 0x01;

    // Encoded length
    p_out->encoded_length = p_data[LFP_HEADER_LEN_HIGH_POS] << 8 | p_data[LFP_HEADER_LEN_LOW_POS];

    // Reserved bytes not set to 0
    if (p_data[LFP_HEADER_RESERVED1_POS] != 0x00 || p_data[LFP_HEADER_RESERVED2_POS] != 0x00) {
        return LFP_EBADDATA;
    }

    // CRC
    const uint16_t actual_crc16 = lfp_crc16(
        p_data + LFP_HEADER_PREAMBLE_POS + 1, // First byte after preamble
        LFP_HEADER_CRC_HIGH_POS - 1 // Up to, excluding, the first CRC byte
    ) & LFP_HEADER_CRC_MASK;
    const uint16_t stored_crc16 = p_data[LFP_HEADER_CRC_HIGH_POS] << 8 | p_data[LFP_HEADER_CRC_LOW_POS];
    if (actual_crc16 != stored_crc16) {
        return LFP_EBADDATA;
    }

    return LFP_EOK;
}
