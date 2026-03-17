#pragma once

#include "cobs.h"
#include "header.h"


/// Opaque context struct for the lfp_body_decoder_* functions. All members are private
typedef struct {
    /// CRC32 computed so far based off the data in the body
    uint32_t _actual_crc32;
    /// CRC32 we expected to compute at the end
    uint32_t _expected_crc32;
    /// Number of encoded bytes remaining to read
    uint16_t _length;
    /// Context for the COBS decoder
    lfp_cobs_decode_ctx_t _decoder_ctx;
    /// Failure code encountered while decoding the body, or LFP_EOK if no errors occurred
    lfp_code_t _failed;
} lfp_body_decoder_ctx_t;

/**
 * Starts the decoding process for a message. Should be used in conjunction with lfp_body_decoder_update() and
 * lfp_body_decoder_finish()
 * @param p_ctx Pointer to the opaque context
 * @param p_header Pointer to the header that this body is attached to
 */
void lfp_body_decoder_init(lfp_body_decoder_ctx_t * p_ctx, const lfp_header_t * p_header);
/**
 * Updates the state of the decoder with a new encoded byte
 * @param p_ctx Pointer to the opaque context
 * @param chr Byte to process
 * @return LFP_EBUSY if no decoded byte is available, a negative value on error, or the value of a decoded byte
 */
lfp_byte_or_code_t lfp_body_decoder_update(lfp_body_decoder_ctx_t * p_ctx, uint8_t chr);
/**
 * Checks if the decoder is in a valid final state or if the decoding process has failed
 * @param p_ctx Pointer to the opaque context
 * @return LFP_EOK if the decoding process was successful, or an error otherwise.
 */
lfp_code_t lfp_body_decoder_finish(const lfp_body_decoder_ctx_t * p_ctx);

/**
 * Decodes a body using its associated header
 * @param p_header Pointer to the header struct associated with the body to decode
 * @param p_body Pointer to the input buffer for the body. Should be, at minimum, of size p_header->encoded_length
 * @param p_output Pointer to the output buffer for the body. Should be of size output_len
 * @param output_len Size of the buffer pointed to by p_output
 * @return LFP_FAIL if the body is not decodable, or the size of the decoded body
 */
lfp_size_or_fail_t lfp_body_decoder(
    const lfp_header_t *p_header,
    const uint8_t *p_body,
    uint8_t *p_output,
    uint16_t output_len
);

/**
 * Computes the size required to hold a buffer after encoding
 * @param p_data Pointed to the unencoded body
 * @param length Length of p_data
 * @return The size of the buffer required to hold p_data once encoded in an LFP body
 */
uint16_t lfp_encoded_body_length(
    const uint8_t *p_data,
    uint16_t length
);

/**
 * Computes an upper bound for the number of bytes required to hold a body after decoding
 * @param encoded_length Size of the encoded body buffer
 * @return Upper bound size required to hold the buffer after decoding
 */
#define LFP_DECODED_BODY_LENGTH_APPROX(encoded_length) ((encoded_length) - LFP_DATA_CRC32_SIZE - (((encoded_length) - LFP_DATA_CRC32_SIZE) / 254))

/**
 * Computes an upper bound for the number of bytes required to hold a body after encoding
 * @param encoded_length Size of the body buffer
 * @return Upper bound size required to hold the buffer after encoding
 */
#define LFP_ENCODED_BODY_LENGTH_APPROX(length) (LFP_DATA_CRC32_SIZE + (length) + (((length) / 254) + 1))