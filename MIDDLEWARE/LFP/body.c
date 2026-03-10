#include "lfp/body.h"

#include "cobs_private.h"
#include "crc.h"

void lfp_body_decoder_init(lfp_body_decoder_ctx_t * p_ctx, const lfp_header_t * p_header) {
    *p_ctx = (lfp_body_decoder_ctx_t){
        ._length = p_header->encoded_length,
        ._actual_crc32 = LFP_CRC32_INIT,
        ._failed = LFP_EOK,
        ._expected_crc32 = 0,
        ._decoder_ctx = {0}
    };
    lfp_cobs_decode_init(&p_ctx->_decoder_ctx);
}

lfp_byte_or_code_t lfp_body_decoder_update(lfp_body_decoder_ctx_t * p_ctx, const uint8_t chr) {
    // Failure: We are failed, don't even try
    if (p_ctx->_failed != LFP_EOK) {
        return p_ctx->_failed;
    }

    // Failure: We got given more bytes than announced by the header
    if (p_ctx->_length == 0) {
        p_ctx->_failed = LFP_EOVERFLOW;
        return LFP_EOVERFLOW;
    }

    // Last CRC32 byte
    if (p_ctx->_length == 1) {
        p_ctx->_length--;

        // Failure: The last CRC byte has unexpected bits
        if ((chr & 0xF0) != 0) {
            p_ctx->_failed = LFP_EBADDATA;
            return LFP_EBADDATA;
        }

        p_ctx->_expected_crc32 |= (uint32_t)chr;
        // Failure: We received the last CRC byte, and it isn't what it should be
        if (p_ctx->_actual_crc32 != p_ctx->_expected_crc32) {
            p_ctx->_failed = LFP_EBADDATA;
            return LFP_EBADDATA;
        }
        // OK: No data
        return LFP_EBUSY;
    }

    // Other CRC32 bytes
    if (p_ctx->_length <= LFP_DATA_CRC32_SIZE) {
        p_ctx->_expected_crc32 |= (uint32_t)chr << (7 * p_ctx->_length - 10);
        p_ctx->_length--;
        // OK: No data
        return LFP_EBUSY;
    }

    // Normal bytes
    p_ctx->_length--;
    const lfp_byte_or_code_t new_byte = lfp_cobs_decode_update(&p_ctx->_decoder_ctx, chr);
    if (new_byte >= 0) {
        p_ctx->_actual_crc32 = lfp_crc32_update(p_ctx->_actual_crc32, new_byte);
    }
    // OK: No data
    return new_byte;
}

lfp_code_t lfp_body_decoder_finish(const lfp_body_decoder_ctx_t * p_ctx) {
    // Failure: Unspecified failure
    if (p_ctx->_failed != LFP_EOK) {
        return p_ctx->_failed;
    }

    // Failure: We are still expecting more bytes, this isn't okay
    if (p_ctx->_length != 0) {
        return LFP_EBADDATA;
    }

    // OK: Probably fine!
    return lfp_cobs_decode_finish(&p_ctx->_decoder_ctx);
}

lfp_size_or_fail_t lfp_body_decoder(
    const lfp_header_t *p_header,
    const uint8_t *p_body,
    uint8_t *p_output,
    const uint16_t output_len
) {
    uint16_t output_size = 0;

    lfp_body_decoder_ctx_t ctx;
    lfp_body_decoder_init(&ctx, p_header);

    for (const uint8_t * p_in = p_body; p_in < p_body + p_header->encoded_length; p_in++) {
        const lfp_byte_or_code_t res = lfp_body_decoder_update(&ctx, *p_in);
        // No data, continue
        if (res == LFP_EBUSY) continue;
        // Error, abort
        if (res < 0) return res;

        // Overflow!!
        if (output_size >= output_len) return LFP_FAIL;
        // Data byte
        p_output[output_size++] = res;
    }

    const lfp_code_t res = lfp_body_decoder_finish(&ctx);
    if (res != LFP_EOK) {
        return res;
    }
    return output_size;
}

uint16_t lfp_encoded_body_length(const uint8_t *p_data, const uint16_t length) {
    return lfp_cobs_encode_size(p_data, length) + LFP_DATA_CRC32_SIZE;
}