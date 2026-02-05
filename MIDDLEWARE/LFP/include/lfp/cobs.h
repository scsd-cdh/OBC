#pragma once

#include <stdbool.h>
#include <stdint.h>

/// Opaque context struct for the lfp_cobs_decode_* functions. All members are private
typedef struct {
    /// Number of encoded bytes left in the current block
    uint8_t _block_length;
    /// Whether a delimiter byte should be emitted at the end of the current block
    bool _emit_delim;
} lfp_cobs_decode_ctx_t;
