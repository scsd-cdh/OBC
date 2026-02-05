#pragma once

#include <stdint.h>

#include "lfp.h"
#include "lfp/cobs.h"


/**
 * Callback when a COBS byte is available
 * @param chr Newly available COBS byte
 * @param p_ctx Pointer to the user context provided via lfp_cobs_encode() or lfp_cobs_decode()
 */
typedef void (*lfp_cobs_write_cb_t)(uint8_t chr, void * p_ctx);

/**
 * COBS encodes a buffer of data
 * @param p_buf Pointer to the buffer containing the data to encode
 * @param length Length of p_buf
 * @param p_write_cb Callback for when a new encoded byte is available. Can be NULL to only compute size
 * @param p_ctx User context, provided to p_write_cb
 * @return The size of the encoded data, this value is undefined if the encoded data exceeds a u16
 */
uint16_t lfp_cobs_encode(
    const uint8_t * p_buf,
    uint16_t length,
    lfp_cobs_write_cb_t p_write_cb,
    void * p_ctx
);

/**
 * COBS decodes a buffer of data
 * @param p_buf Pointer to the buffer containing the data to decode
 * @param length Length of p_buf
 * @param p_write_cb Callback for when a new decoded byte is available. Can be NULL to only compute size
 * @param p_ctx User context, provided to p_write_cb
 * @return The size of the decoded data
 */
lfp_size_or_fail_t lfp_cobs_decode(
    const uint8_t * p_buf,
    uint16_t length,
    lfp_cobs_write_cb_t p_write_cb,
    void * p_ctx
);

/**
 * Starts the decoding process for a COBS encoded buffer
 * @param p_ctx Pointer to the opaque context
 */
void lfp_cobs_decode_init(lfp_cobs_decode_ctx_t * p_ctx);
/**
 * Adds an encoded byte to the decoder for decoding
 * @param p_ctx Pointer to the opaque context
 * @param chr Encoded byte to add to the decoder
 * @return LFP_EBUSY if no decoded byte is available, a negative value on error, or the size of the decoded data
 */
lfp_byte_or_code_t lfp_cobs_decode_update(lfp_cobs_decode_ctx_t * p_ctx, uint8_t chr);
/**
 * Checks if the decoder is in a valid final state or if the decoding process has failed
 * @param p_ctx Pointer to the opaque context
 * @return LFP_EOK if the decoding process was successful, or an error code otherwise
 */
lfp_code_t lfp_cobs_decode_finish(const lfp_cobs_decode_ctx_t * p_ctx);