#pragma once

#include <stdbool.h>

#include "./body.h"
#include "./header.h"
#include "../lfp.h"

// Forward declaration
struct lfp_stream_ctx_t;

/**
 * Callback fired when the stream errors out. Intended to reset any state created by the other callbacks. Messages
 * rejected by ::lfp_stream_header_cb_t will not trigger this callback.
 * @param reason Reason why the stream encountered an error
 * @param p_ctx Pointer to the user context provided to lfp_stream_init
 */
typedef void (*lfp_stream_error_cb_t)(lfp_code_t reason, void * p_ctx);
/**
 * Callback fired when a header is successfully parsed
 * @param p_header Pointer to the parsed header, only valid for a brief amount of time. Special care should be taken to
 * ensure that the stream parser does not receive the complete following header before this function returns
 * @param p_ctx Pointer to the user context provided to lfp_stream_init()
 * @return true if the message should be accepted, or false if it should be rejected
 */
typedef bool (*lfp_stream_header_cb_t)(const lfp_header_t * p_header, void * p_ctx);
/**
 * Callback fired when a message body is successfully decoded
 * @param p_header Pointer to the parsed header, only valid for a brief amount of time. Special care should be taken to
 * ensure that the stream parser does not receive the complete following header before this function returns
 * @param p_body Pointer to the parsed body, only valid for a brief amount of time. Special care should be taken to
 * ensure that the stream parser does not receive the beginning of the next valid body before this function returns
 * @param body_length Length of p_body
 * @param p_ctx Pointer to the user context provided to lfp_stream_init()
 */
typedef void (*lfp_stream_message_cb_t)(const lfp_header_t * p_header, const uint8_t * p_body, uint16_t body_length, void * p_ctx);

/// Opaque context struct for the lfp_stream_* functions. All members are private
typedef struct lfp_stream_ctx_t {
    /// Callback called when an error in the stream is detected
    lfp_stream_error_cb_t _p_error_cb;
    /// Callback called when a header is successfully parsed
    lfp_stream_header_cb_t _p_header_cb;
    /// Callback called when a message is successfully parsed
    lfp_stream_message_cb_t _p_message_cb;
    /// Scratch buffer to hold the raw header as it comes in
    uint8_t _header_buffer[LFP_HEADER_SIZE];
    /// Total capacity of p_body_buffer
    uint16_t _body_buffer_capacity;
    /// Pointer to the user context provided to lfp_stream_init()
    void * _p_user_ctx;

    /// First unused byte in _header_buffer. Also doubles as a size field. A size of less than LFP_HEADER_SIZE means
    /// that _header is not valid. A size of LFP_HEADER_SIZE means that _header is valid
    uint8_t _header_buffer_end;
    /// Struct to hold the last parsed header
    lfp_header_t _header;

    /// Opaque context for the body docoder
    lfp_body_decoder_ctx_t _body_decoder_ctx;
    /// User provided scratch buffer used to store the decoded buffer
    uint8_t * _p_body_buffer;
    /// First unused byte in _p_body_buffer. Also doubles as a size field
    uint16_t _body_buffer_end;
    /// Number of encoded bode bytes received so far
    uint16_t _encoded_body_bytes_received;
} lfp_stream_ctx_t;

/**
 * Starts the decoding process for a stream of LFP messages
 * @param p_ctx Pointer to the opaque context
 * @param p_body_buffer Pointer to a buffer that will be used to hold messages' contents
 * @param body_buffer_length Length of p_body_buffer
 * @param p_header_cb Callback called when a header is successfully parsed. Can be NULL
 * @param p_message_cb Callback called when a message is successfully parsed. Can be NULL
 * @param p_error_cb Callback called when an error is detected in the stream. Can be NULL
 * @param p_user_ctx Pointer to arbritary data passed to all callbacks
 */
void lfp_stream_init(
    lfp_stream_ctx_t * p_ctx,
    uint8_t * p_body_buffer,
    uint16_t body_buffer_length,
    lfp_stream_header_cb_t p_header_cb,
    lfp_stream_message_cb_t p_message_cb,
    lfp_stream_error_cb_t p_error_cb,
    void * p_user_ctx
);
/**
 * Updates the stream parser with a byte of data
 * @param p_ctx Pointer to the opaque context
 * @param chr Data byte to process
 * @return A size hint for the number of bytes expected by the stream parser. A special value of 0 is used to indicate
 * that the stream should skip to the next LFP_COBS_DELIMITER byte
 */
lfp_size_or_fail_t lfp_stream_update(lfp_stream_ctx_t * p_ctx, uint8_t chr);
/**
 * Updates the stream parser with a buffer of data
 * @param p_ctx Pointer to the opaque context
 * @param p_data Pointer to the data
 * @param data_length Number of bytes in p_data
 * @return A size hint for the number of bytes expected by the stream parser. A special value of 0 is used to indicate
 * that the stream should skip to the next LFP_COBS_DELIMITER byte
 */
lfp_size_or_fail_t lfp_stream_update_buf(lfp_stream_ctx_t * p_ctx, const uint8_t * p_data, uint16_t data_length);