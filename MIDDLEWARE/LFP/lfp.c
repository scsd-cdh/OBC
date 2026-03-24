#include "lfp.h"

#include "cobs_private.h"
#include "crc.h"
#include "lfp/body.h"

lfp_size_or_code_t lfp_encode(
    const uint8_t initiator,
    const uint8_t target,
    const uint8_t endpoint,
    const lfp_direction_t direction,
    const uint8_t *p_data,
    const uint16_t length,
    uint8_t *p_dst,
    uint32_t dst_len
) {
    int32_t encoded_size = 0;

    if (initiator > LFP_LRI_INITIATOR_MAX) {
        return LFP_EINVAL;
    }

    if (target > LFP_LRI_TARGET_MAX) {
        return LFP_EINVAL;
    }

    if (initiator == target) {
        return LFP_EINVAL;
    }

    if (endpoint == 0xFE) {
        return LFP_EINVAL;
    }

    if (endpoint & 0x1) {
        return LFP_EINVAL;
    }

    if (length > LFP_MAX_BODY_SIZE) {
        return LFP_EINVAL;
    }

    const uint16_t encoded_length = lfp_encoded_body_length(p_data, length);

    uint16_t crc16 = LFP_CRC16_INIT;

#define WRITE_CHR(chr) if (encoded_size == dst_len) { return LFP_EOVERFLOW; } else { p_dst[encoded_size++] = chr; }

    // Preamble
    WRITE_CHR(LFP_PREAMBLE);

    // LRI
    uint8_t byte = LFP_LRI_INITIATOR_SET(0, initiator) | LFP_LRI_TARGET_SET(0, target);
    WRITE_CHR(byte);
    crc16 = lfp_crc16_update(crc16, byte);

    // Endpoint
    byte = endpoint | direction;
    WRITE_CHR(byte);
    crc16 = lfp_crc16_update(crc16, byte);

    // Length (High)
    byte = (encoded_length & 0xFF00) >> 8;
    WRITE_CHR(byte);
    crc16 = lfp_crc16_update(crc16, byte);

    // Length (Low)
    byte = (encoded_length & 0x00FF) >> 0;
    WRITE_CHR(byte);
    crc16 = lfp_crc16_update(crc16, byte);

    // Reserved 0
    byte = 0;
    WRITE_CHR(byte);
    crc16 = lfp_crc16_update(crc16, byte);

    // Reserved 1
    byte = 0;
    WRITE_CHR(byte);
    crc16 = lfp_crc16_update(crc16, byte);

    // Header checksum
    crc16 &= LFP_HEADER_CRC_MASK;
    WRITE_CHR((crc16 & 0xFF00) >> 8);
    WRITE_CHR((crc16 & 0x00FF) >> 0);

    // Data segment
    lfp_cobs_encode_ctx_t encode_ctx;
    lfp_cobs_encode_init(&encode_ctx, p_data, length);
    const uint32_t crc32 = lfp_crc32(p_data, length);
    while (true) {
        const uint8_t chr = lfp_cobs_encode_next_byte(&encode_ctx);
        if (chr == 0xFF) break;
        WRITE_CHR(chr);
    }
    const lfp_code_t ret = lfp_cobs_encode_finish(&encode_ctx);
    if (ret != LFP_OK) {
        return ret;
    }

    // Data checksum
    WRITE_CHR(crc32 >> 25 & 0b01111111);
    WRITE_CHR(crc32 >> 18 & 0b01111111);
    WRITE_CHR(crc32 >> 11 & 0b01111111);
    WRITE_CHR(crc32 >> 4 & 0b01111111);
    WRITE_CHR(crc32 >> 0 & 0b00001111);

#undef WRITE_CHR

    return encoded_size;
}
