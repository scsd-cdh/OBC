#pragma once

#include <stdint.h>

#include "lfp.h"
#include "lfp/cobs.h"


/**
 * Gets the encoded size of the data if it were to be COBS encoded
 * @param p_buf Pointer to the buffer containing the data to encode
 * @param length Length of p_buf
 * @return The size of the encoded data, this value is undefined if the encoded data exceeds an u16
 */
uint16_t lfp_cobs_encode_size(const uint8_t * p_buf, uint16_t length);

/**
 * Starts the COBS encoding process for an unencoded buffer
 * @param p_ctx Pointer to the opaque context
 * @param p_buf Pointer to the unencoded data buffer
 * @param length Length of the unencoded data buffer
 */
void lfp_cobs_encode_init(lfp_cobs_encode_ctx_t * p_ctx, const uint8_t * p_buf, uint16_t length);
/**
 * Gets the next encoded byte
 * @param p_ctx Pointer to the opaque context
 * @return The next encoded byte, or LFP_PREAMBLE if complete
 */
uint8_t lfp_cobs_encode_next_byte(lfp_cobs_encode_ctx_t * p_ctx);
/**
 * Checks if the encoder is in a valid final state
 * @param p_ctx Pointer to the opaque context
 * @return LFP_EOK if the encoding process was successful, or an error code otherwise
 */
lfp_code_t lfp_cobs_encode_finish(const lfp_cobs_encode_ctx_t * p_ctx);



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