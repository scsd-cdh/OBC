#include "cobs_private.h"

#include <stddef.h>

#include "lfp.h"

uint16_t lfp_cobs_encode(
    const uint8_t * p_buf,
    const uint16_t length,
    const lfp_cobs_write_cb_t p_write_cb,
    void * p_ctx
) {
    uint16_t encoded_size = 0;

    const uint8_t * p_block = p_buf;
    while (p_block <= p_buf + length) {
        uint8_t block_length = 0;

        // Find the size of the current block
        while (block_length < 0xFF) {
            // We reached the end of the buffer, we assume there's an extra 0 byte so we increment the block length
            if (p_block + block_length >= p_buf + length) {
                // Consume the virtual byte
                block_length++;
                break;
            }

            // Consume non-null bytes
            if (p_block[block_length++] == LFP_PREAMBLE) {
                break;
            }
        }

        // Write out the block size
        if (p_write_cb != NULL) {
            p_write_cb(block_length + LFP_PREAMBLE, p_ctx);
        }
        encoded_size++;

        // Write out all bytes in block except the last one
        if (p_write_cb != NULL) {
            for (const uint8_t * p_byte = p_block; p_byte < p_block + block_length - 1; p_byte++) {
                p_write_cb(*p_byte, p_ctx);
            }
        }
        encoded_size += block_length - 1;

        // Move to the next block, skipping the last byte
        p_block = p_block + block_length;

        // If we have a full block, unskip the last byte so that it can go in the next buffer, unless its EOF, because
        // we don't care about encoding the virtual byte
        if (block_length == 0xFF && p_block <= p_buf + length) {
            p_block--;
        }
    }

    return encoded_size;
}

// the decoded size cant be 0xFFFF because of the overhead bytes, so LFP_FAIL is safe to return
lfp_size_or_fail_t lfp_cobs_decode(
    const uint8_t * p_buf,
    const uint16_t length,
    const lfp_cobs_write_cb_t p_write_cb,
    void * p_ctx
) {
    const uint8_t * p_byte = p_buf;
    // Final size after decoding
    uint16_t decoded_size = 0;

    // While we are inside the encoded buffer
    while (p_byte < p_buf + length) {
        // Get the length of the current block, but make sure it's not going to go out of bounds
        const uint8_t block_length = *(p_byte++) - 1 - LFP_PREAMBLE;
        if (p_byte + block_length > p_buf + length) {
            return LFP_FAIL;
        }

        // Fast path: if we don't have a callback, we only care about the length, so don't iterate every single byte
        if (p_write_cb == NULL) {
            p_byte += block_length;
        } else {
            for (uint8_t i = 0; i < block_length; i++) {
                p_write_cb(*(p_byte++), p_ctx);
            }
        }

        // Add the block we just read to the size
        decoded_size += block_length;

        // Add the delimiter byte we removed back in:
        // - If the block size isn't 0xFE
        if (block_length != 0xFE && p_byte < p_buf + length) {
            if (p_write_cb != NULL) {
                p_write_cb(LFP_PREAMBLE, p_ctx);
            }
            decoded_size++;
        }
    }
    return decoded_size;
}


void lfp_cobs_decode_init(lfp_cobs_decode_ctx_t * p_ctx) {
    *p_ctx = (lfp_cobs_decode_ctx_t) {
        ._block_length = 0,
        ._emit_delim = false
    };
}

lfp_byte_or_code_t lfp_cobs_decode_update(lfp_cobs_decode_ctx_t * p_ctx, const uint8_t chr) {
    // We aren't done with the current block
    if (p_ctx->_block_length > 0) {
        p_ctx->_block_length--;
        return chr;
    }

    const bool emit_delim = p_ctx->_emit_delim;

    // Setup next block
    const uint8_t block_length = chr - 1 - LFP_PREAMBLE;
    p_ctx->_block_length = block_length;
    p_ctx->_emit_delim = block_length != 0xFE;

    // Block is over and we need to emit a removed delimiter byte
    if (emit_delim) {
        return LFP_PREAMBLE;
    }

    // No byte emitted
    return LFP_EBUSY;
}

lfp_code_t lfp_cobs_decode_finish(const lfp_cobs_decode_ctx_t * p_ctx) {
    if (p_ctx->_block_length != 0) {
        return LFP_EBADDATA;
    }
    return LFP_EOK;
}