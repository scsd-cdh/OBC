 #include "cobs_private.h"

#include "lfp.h"

uint16_t lfp_cobs_encode_size(
    const uint8_t * p_buf,
    const uint16_t length
) {
    uint16_t encoded_size = 0;

    const uint8_t * p_block = p_buf;
    while (p_block <= p_buf + length) {
        uint8_t block_length = 0;

        // Find the size of the current block
        while (block_length < 0xFF) {
            // We reached the end of the buffer, we assume there's an extra LFP_PREAMBLE byte, so we increment the block length
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
        encoded_size += block_length;

        // Move to the next block, skipping the last byte
        p_block = p_block + block_length;

        // If we have a full block, un-skip the last byte so that it can go in the next buffer, unless it's EOF, because
        // we don't care about encoding the virtual byte
        if (block_length == 0xFF && p_block <= p_buf + length) {
            p_block--;
        }
    }

    return encoded_size;
}

void lfp_cobs_encode_init(lfp_cobs_encode_ctx_t * p_ctx, const uint8_t * p_buf, const uint16_t length)
{
    *p_ctx = (lfp_cobs_encode_ctx_t) {
        .p_buf = p_buf,
        .length = length,
        .p_byte = p_buf,
        .block_length = 0,
        .full_block = false
    };
}

static uint8_t lfp_cobs_encode_block_length(const lfp_cobs_encode_ctx_t* p_ctx)
{
    uint8_t block_length = 0;

    // Find the size of the current block
    while (block_length < 0xFF) {
        // We reached the end of the buffer, we assume there's an extra LFP_PREAMBLE byte, so we increment the block length
        if (p_ctx->p_byte + block_length >= p_ctx->p_buf + p_ctx->length) {
            // Consume the virtual byte
            block_length++;
            break;
        }

        // Consume non-null bytes
        if (p_ctx->p_byte[block_length++] == LFP_PREAMBLE) {
            break;
        }
    }

    return block_length;
}

uint8_t lfp_cobs_encode_next_byte(lfp_cobs_encode_ctx_t* p_ctx)
{
    // Allow a single virtual delim byte at the end of the buffer
    if (p_ctx->p_byte > p_ctx->p_buf + p_ctx->length) {
        return LFP_PREAMBLE;
    }

    uint8_t byte;

    // We didn't compute a block length for this block yet. It's impossible to have a 0-length block.
    if (p_ctx->block_length == 0) {
        p_ctx->block_length = lfp_cobs_encode_block_length(p_ctx);
        p_ctx->full_block = p_ctx->block_length == 0xFF;
        byte = (uint8_t)(p_ctx->block_length + LFP_PREAMBLE);
    } else {
        // Write out all bytes in the block except the last one
        byte = *p_ctx->p_byte++;
        p_ctx->block_length--;
    }

    // Special handling for the second-to-last byte, as the last byte is not encoded in the block
    if (p_ctx->block_length <= 1)
    {
        // If the block isn't a full block, or if we are hitting the virtual byte, ignore the last byte of the block.
        // If it's not a full block, the block length already encodes it, and if it's the virtual byte in a full block,
        // we don't want to transcribe that virtual byte
        if (!p_ctx->full_block || p_ctx->p_byte >= (p_ctx->p_buf + p_ctx->length))
        {
            p_ctx->p_byte++;
        }

        // Reset the block length to move to the next one
        p_ctx->block_length = 0;
    }

    return byte;
}

lfp_code_t lfp_cobs_encode_finish(const lfp_cobs_encode_ctx_t* p_ctx)
{
    if (p_ctx->p_byte <= p_ctx->p_buf + p_ctx->length) {
        return LFP_FAIL;
    }

    return LFP_EOK;
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

    // Set up the next block
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