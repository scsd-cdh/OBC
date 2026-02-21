#pragma once

#include <stdint.h>

#include "../lfp.h"

/// Struct representing a parsed LFP header
typedef struct {
    /// Number of bytes that the encoded body contains
    uint16_t encoded_length;
    /// Direction the LFP message is destined to
    lfp_direction_t direction;
    /// ID of the system who is sending the request/initiated the transaction
    uint8_t initiator;
    /// ID of the system who is sending the response/did not initiate the transaction
    uint8_t target;
    /// Channel to communicate over
    uint8_t endpoint;
} lfp_header_t;

/**
 * Computes a unique identifier representing the data structure contained within the message
 * @param p_header Pointer to an LFP header struct
 * @return The composite ID of p_header
 */
static inline uint16_t lfp_composite_id(const lfp_header_t * p_header)
{
    return (p_header->target << 8) | (p_header->endpoint & 0xFE) | p_header->direction;
}

/**
 * Decode a buffer containing an LFP header into a struct representation of the header
 * @param p_data Pointer to a buffer of at least size LFP_HEADER_SIZE containing an LFP header
 * @param p_out Pointer to an empty lfp_header_t struct to fill with information from the buffer
 * @return LFP_EOK on success, or an error code otherwise
 */
lfp_code_t lfp_parse_header(const uint8_t * p_data, lfp_header_t * p_out);
