#pragma once
#include <stdint.h>

// Do not change without making sure this byte cannot occur in the header
/// Delimiter to use as the preamble and as a delimiter between messages
#define LFP_PREAMBLE (0xFF)

// LRI Target

#define LFP_LRI_TARGET_POS (0)
#define LFP_LRI_TARGET_MAX (0xF)
#define LFP_LRI_TARGET_MASK (LFP_LRI_TARGET_MAX << LFP_LRI_TARGET_POS)
#define LFP_LRI_TARGET_GET(LRI) (((LRI) & LFP_LRI_TARGET_MASK) >> LFP_LRI_TARGET_POS)
#define LFP_LRI_TARGET_SET(LRI, x) ((LRI) | (x) << LFP_LRI_TARGET_POS)

// LRI Initiator

#define LFP_LRI_INITIATOR_POS (4)
#define LFP_LRI_INITIATOR_MAX (0xF)
#define LFP_LRI_INITIATOR_MASK (LFP_LRI_INITIATOR_MAX << LFP_LRI_INITIATOR_POS)
#define LFP_LRI_INITIATOR_GET(LRI) (((LRI) & LFP_LRI_INITIATOR_MASK) >> LFP_LRI_INITIATOR_POS)
#define LFP_LRI_INITIATOR_SET(LRI, x) ((LRI) | (x) << LFP_LRI_INITIATOR_POS)

// Header byte positions

#define LFP_HEADER_PREAMBLE_POS     (0)
#define LFP_HEADER_LRI_POS          (1)
#define LFP_HEADER_ENDPOINT_POS     (2)
#define LFP_HEADER_LEN_HIGH_POS     (3)
#define LFP_HEADER_LEN_LOW_POS      (4)
#define LFP_HEADER_RESERVED1_POS    (5)
#define LFP_HEADER_RESERVED2_POS    (6)
#define LFP_HEADER_CRC_HIGH_POS     (7)
#define LFP_HEADER_CRC_LOW_POS      (8)
#define LFP_HEADER_SIZE             (9)

/// Number of bytes required to hold the checksum value
#define LFP_DATA_CRC32_SIZE         (5)

/// Value to mask the header CRC16 with
#define LFP_HEADER_CRC_MASK 0xFEFE

// Can technically be higher but this simplifies things
/// Maximum amount of data safe to hold in an LFP message
#define LFP_MAX_BODY_SIZE 0xFEF8

// Error codes

/// Success
#define LFP_OK              (-0)
/// Success
#define LFP_EOK             (-0)
/// Unspecified error
#define LFP_EFAIL           (-1)
/// Invalid argument
#define LFP_EINVAL          (-2)
/// Busy, try again later
#define LFP_EBUSY           (-3)
/// A value could not fit in a buffer
#define LFP_EOVERFLOW       (-4)
/// Corrupted or invalid data was detected
#define LFP_EBADDATA        (-5)

/// Generic failure
#define LFP_FAIL (0xFFFF)

/**
 * Callback called when a new encoded data byte is available
 * @param chr Encoded data byte
 * @param p_ctx Pointer to the user context provided to lfp_encode()
 */
typedef void (*lfp_write_cb_t)(uint8_t chr, void * p_ctx);

/// Direction that a message is intended towards
///
/// TARGET == REQUEST
///
/// INITIATOR == RESPONSE
typedef enum {
    LFP_DIRECTION_TARGET = 0,
    LFP_DIRECTION_REQUEST = 0,
    LFP_DIRECTION_INITIATOR = 1,
    LFP_DIRECTION_RESPONSE = 1,
} lfp_direction_t;


/// 0-65534 on success, LFP_FAIL on unspecified failure
typedef uint16_t lfp_size_or_fail_t;
/// 0-65535 on success, negative values are error codes
typedef int32_t lfp_size_or_code_t;
/// 0-254 on success, negative values are error codes
typedef int16_t lfp_byte_or_code_t;
/// 0 on success, negative values are error codes
typedef int16_t lfp_code_t;


/**
 * Encode an LFP message
 * @param initiator ID of the system who initiated the request
 * @param target ID of the system the request was/is intended for. Sends the response
 * @param endpoint Endpoint to use
 * @param direction Direction of the LFP message
 * @param p_data Pointer to the data buffer
 * @param length Length of p_data
 * @param p_write_cb Callback to call when a new encoded data byte is available
 * @param p_ctx User context to provide to p_write_cb
 * @return LFP_EOK if the message was successfully encoded, an error code otherwise
 */
lfp_code_t lfp_encode(
    uint8_t initiator,
    uint8_t target,
    uint8_t endpoint,
    lfp_direction_t direction,
    const uint8_t *p_data,
    uint16_t length,
    lfp_write_cb_t p_write_cb,
    void * p_ctx
);

/**
 * Helper to encode an LFP message directly to a buffer instead of using the callback system
 * @param initiator ID of the system who initiated the request
 * @param target ID of the system the request was/is intended for. Sends the response
 * @param endpoint Endpoint to use
 * @param direction Direction of the LFP message
 * @param p_data Pointer to the data buffer
 * @param length Length of p_data
 * @param p_dst Pointer to the buffer that will contain the encoded message
 * @param dst_len Length of p_dst
 * @return An error code if negative, the size of the encoded message otherwise
 */
lfp_size_or_code_t lfp_encode_to_buf(
    uint8_t initiator,
    uint8_t target,
    uint8_t endpoint,
    lfp_direction_t direction,
    const uint8_t *p_data,
    uint16_t length,
    uint8_t *p_dst,
    uint32_t dst_len
);
