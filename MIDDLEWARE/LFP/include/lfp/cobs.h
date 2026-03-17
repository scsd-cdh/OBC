#pragma once

#include <stdbool.h>
#include <stdint.h>

/// Opaque context struct for the lfp_cobs_encode_* functions. All members are private
typedef struct {
    /// Pointer to the unencoded data buffer
    const uint8_t * p_buf;
    /// Length of the buffer pointed by p_buf
    uint16_t length;
    /// Pointer to the current byte
    const uint8_t * p_byte;
    /// Length of the current COBS block being processed
    uint8_t block_length;
    /// If the current block is a full block or a block that ends in an LFP_PREAMBLE byte
    bool full_block;
} lfp_cobs_encode_ctx_t;


/// Opaque context struct for the lfp_cobs_decode_* functions. All members are private
typedef struct {
    /// Number of encoded bytes left in the current block
    uint8_t _block_length;
    /// Whether a delimiter byte should be emitted at the end of the current block
    bool _emit_delim;
} lfp_cobs_decode_ctx_t;
