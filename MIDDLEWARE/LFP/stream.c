#include "lfp/stream.h"

#include <stddef.h>

void lfp_stream_init(
    lfp_stream_ctx_t * p_ctx,
    uint8_t * p_body_buffer, // NOLINT(*-non-const-parameter) This has to be non const, tidy is wrong
    const uint16_t body_buffer_length,
    const lfp_stream_header_cb_t p_header_cb,
    const lfp_stream_message_cb_t p_message_cb,
    const lfp_stream_error_cb_t p_error_cb,
    void * p_user_ctx
) {
    *p_ctx = (lfp_stream_ctx_t) {
        ._p_body_buffer = p_body_buffer,
        ._body_buffer_capacity = body_buffer_length,
        ._body_buffer_end = 0,
        ._p_error_cb = p_error_cb,
        ._p_header_cb = p_header_cb,
        ._p_message_cb = p_message_cb,
        ._header = {0},
        ._header_buffer = {0},
        ._header_buffer_end = 0,
        ._body_decoder_ctx = {0},
        ._encoded_body_bytes_received = 0,
        ._p_user_ctx = p_user_ctx
    };
}

static uint16_t lfp_stream_header_update(lfp_stream_ctx_t * p_ctx, const uint8_t chr) {
    // Scan mode, search for preamble
    if (p_ctx->_header_buffer_end == 0 && chr != LFP_PREAMBLE) {
        // In scan mode, we ignore anything that isn't the preamble byte
        return 0;
    }

    // Accept more header bytes
    if (p_ctx->_header_buffer_end < LFP_HEADER_SIZE) {
        p_ctx->_header_buffer[p_ctx->_header_buffer_end++] = chr;
    }

    // Incomplete header, not enough bytes to parse
    if (p_ctx->_header_buffer_end < LFP_HEADER_SIZE) {
        return LFP_HEADER_SIZE - p_ctx->_header_buffer_end;
    }

    // Complete header, try to parse
    const lfp_code_t result = lfp_parse_header(p_ctx->_header_buffer, &p_ctx->_header);
    if (result != LFP_EOK) {
        // Header parse failure
        if (p_ctx->_p_error_cb != NULL) {
            p_ctx->_p_error_cb(result, p_ctx->_p_user_ctx);
        }
        p_ctx->_header_buffer_end = 0;
        // Return to scan mode
        return 0;
    }

    // Header is parsed and valid! Allow the user to reject it
    if (p_ctx->_p_header_cb != NULL) {
        const bool header_accepted = p_ctx->_p_header_cb(&p_ctx->_header, p_ctx->_p_user_ctx);
        // Message was rejected, go back to scan mode. Do not trigger error callback, user is expected to handle the reset
        if (!header_accepted) {
            p_ctx->_header_buffer_end = 0;
            return 0;
        }
    }

    // We're all good, proceed with body decoding
    lfp_body_decoder_init(&p_ctx->_body_decoder_ctx, &p_ctx->_header);
    p_ctx->_body_buffer_end = 0;
    p_ctx->_encoded_body_bytes_received = 0;
    return p_ctx->_header.encoded_length;
}

lfp_size_or_fail_t lfp_stream_update(lfp_stream_ctx_t * p_ctx, const uint8_t chr) {
    // Preamble detected, abort the parse and start anew; the message being parsed couldn't possibly be valid if we
    // found a preamble byte.
    if (chr == LFP_PREAMBLE) {
        // We were in the middle of decoding a message; this isn't normal.
        // There's a special exception if the 0xFFs are back to back for the sake of padding
        if (p_ctx->_header_buffer_end > 1 && p_ctx->_p_error_cb != NULL) {
            p_ctx->_p_error_cb(LFP_EBADDATA, p_ctx->_p_user_ctx);
        }
        p_ctx->_header_buffer_end = 0;
        // We don't return because we can process this byte
    }

    // We don't have a valid header, get that done first
    if (p_ctx->_header_buffer_end != LFP_HEADER_SIZE) {
        return lfp_stream_header_update(p_ctx, chr);
    }

    // We have our header, we are ready to parse the body
    const lfp_byte_or_code_t result = lfp_body_decoder_update(&p_ctx->_body_decoder_ctx, chr);
    if (result < 0 && result != LFP_EBUSY) {
        // Body decoder failed, error out and reset
        if (p_ctx->_p_error_cb != NULL) {
            p_ctx->_p_error_cb(result, p_ctx->_p_user_ctx);
        }
        p_ctx->_header_buffer_end = 0;
        return 0;
    }

    // New byte!
    p_ctx->_encoded_body_bytes_received++;
    if (result != LFP_EBUSY) {
        if (p_ctx->_body_buffer_end >= p_ctx->_body_buffer_capacity) {
            // We were about to overflow the decoded body buffer, reset!
            if (p_ctx->_p_error_cb != NULL) {
                p_ctx->_p_error_cb(LFP_EOVERFLOW, p_ctx->_p_user_ctx);
            }
            p_ctx->_header_buffer_end = 0;
            return 0;
        }

        p_ctx->_p_body_buffer[p_ctx->_body_buffer_end++] = result;
    }

    // Check if we still have bytes to receive
    const uint16_t encoded_bytes_left = p_ctx->_header.encoded_length - p_ctx->_encoded_body_bytes_received;
    if (encoded_bytes_left > 0) {
        return encoded_bytes_left;
    }

    // We should have a full message!
    const lfp_code_t result2 = lfp_body_decoder_finish(&p_ctx->_body_decoder_ctx);
    if (result2 != LFP_EOK) {
        // So close yet so far, message is bad, reset
        if (p_ctx->_p_error_cb != NULL) {
            p_ctx->_p_error_cb(result2, p_ctx->_p_user_ctx);
        }
        p_ctx->_header_buffer_end = 0;
        return 0;
    }

    // I don't know why someone would not set a message callback but let's not crash just in case
    if (p_ctx->_p_message_cb != NULL) {
        p_ctx->_p_message_cb(&p_ctx->_header, p_ctx->_p_body_buffer, p_ctx->_body_buffer_end, p_ctx->_p_user_ctx);
    }

    // Reset the parser for the next message
    p_ctx->_header_buffer_end = 0;

    // And back to scan mode we go
    return 0;
}

lfp_size_or_fail_t lfp_stream_update_buf(lfp_stream_ctx_t * p_ctx, const uint8_t * p_data, const uint16_t data_length) {
    if (data_length == 0) {
        return LFP_FAIL;
    }

    lfp_size_or_fail_t result = 0;
    for (const uint8_t * p_byte = p_data; p_byte < p_data + data_length; p_byte++) {
        result = lfp_stream_update(p_ctx, *p_byte);
    }
    return result;
}
