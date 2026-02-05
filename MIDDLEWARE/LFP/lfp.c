#include "lfp.h"

#include "cobs_private.h"
#include "crc.h"
#include "lfp/body.h"

lfp_code_t lfp_encode(
    const uint8_t initiator,
    const uint8_t target,
    const uint8_t endpoint,
    const lfp_direction_t direction,
    const uint8_t *p_data,
    const uint16_t length,
    const lfp_write_cb_t p_write_cb,
    void * p_ctx
) {
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

    // Preamble
    p_write_cb(LFP_PREAMBLE, p_ctx);

    // LRI
    uint8_t byte = LFP_LRI_INITIATOR_SET(0, initiator) | LFP_LRI_TARGET_SET(0, target);
    p_write_cb(byte, p_ctx);
    crc16 = lfp_crc16_update(crc16, byte);

    // Endpoint
    byte = endpoint | direction;
    p_write_cb(byte, p_ctx);
    crc16 = lfp_crc16_update(crc16, byte);

    // Length (High)
    byte = (encoded_length & 0xFF00) >> 8;
    p_write_cb(byte, p_ctx);
    crc16 = lfp_crc16_update(crc16, byte);

    // Length (Low)
    byte = (encoded_length & 0x00FF) >> 0;
    p_write_cb(byte, p_ctx);
    crc16 = lfp_crc16_update(crc16, byte);

    // Reserved 0
    byte = 0;
    p_write_cb(byte, p_ctx);
    crc16 = lfp_crc16_update(crc16, byte);

    // Reserved 1
    byte = 0;
    p_write_cb(byte, p_ctx);
    crc16 = lfp_crc16_update(crc16, byte);

    // Header checksum
    crc16 &= LFP_HEADER_CRC_MASK;
    p_write_cb((crc16 & 0xFF00) >> 8, p_ctx);
    p_write_cb((crc16 & 0x00FF) >> 0, p_ctx);

    // Data segment
    lfp_cobs_encode(p_data, length, p_write_cb, p_ctx);

    // Data checksum
    const uint32_t crc32 = lfp_crc32(p_data, length);
    p_write_cb(crc32 >> 25 & 0b01111111, p_ctx);
    p_write_cb(crc32 >> 18 & 0b01111111, p_ctx);
    p_write_cb(crc32 >> 11 & 0b01111111, p_ctx);
    p_write_cb(crc32 >> 4 & 0b01111111, p_ctx);
    p_write_cb(crc32 >> 0 & 0b00001111, p_ctx);

    return LFP_EOK;
}

// If size > capacity, we ran out of space
typedef struct lfp_encode_ctx {
    int32_t size;
    uint32_t capacity;
    uint8_t * data;
} lfp_encode_ctx_t;

static void lfp_encode_cb(const uint8_t chr, void * _p_ctx) {
    lfp_encode_ctx_t * p_ctx = _p_ctx;
    if (p_ctx->size >= p_ctx->capacity) {
        // Go 1 above the limit, to flag the overflow
        if (p_ctx->size == p_ctx->capacity) {
            p_ctx->size++;
        }
        // Stop here, do not actually write into the buffer
        return;
    }
    p_ctx->data[p_ctx->size++] = chr;
}
lfp_size_or_code_t lfp_encode_to_buf(
    const uint8_t initiator,
    const uint8_t target,
    const uint8_t endpoint,
    const lfp_direction_t direction,
    const uint8_t *p_data,
    const uint16_t length,
    uint8_t *p_dst,
    const uint32_t dst_len
) {
    lfp_encode_ctx_t ctx = {
        .data = p_dst,
        .size = 0,
        .capacity = dst_len,
    };
    const lfp_code_t res = lfp_encode(initiator, target, endpoint, direction, p_data, length, lfp_encode_cb, &ctx);
    if (res != LFP_EOK) {
        return res;
    }
    if (ctx.size > ctx.capacity) {
        return LFP_EOVERFLOW;
    }
    return ctx.size;
}