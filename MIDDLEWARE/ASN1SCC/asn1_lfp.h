#pragma once
#include <lfp.h>
#include <lfp/header.h>
#include <stddef.h>
#include <stdint.h>

/// Struct to hold the success state of lfp and asn1
///
/// Care should be taken to not access this variable from multiple threads in the same .c file as it is a static.
static struct {
    /// last LFP error code, 0 on success
    lfp_code_t lfp;
    /// last asn1 error code, 0 on success
    int asn1;
} asn1_lfp_last_error = {0};

/// Reference to the static success struct
#define ASN1_LFP_ERROR_CODE (asn1_lfp_last_error)
/// Bool value for success or failure of the last ASN1_LFP operation
#define ASN1_LFP_SUCCESS() (ASN1_LFP_ERROR_CODE.lfp == LFP_EOK && ASN1_LFP_ERROR_CODE.asn1 == 0)

/**
 * Macro to serialize a struct to an LFP target. Use ASN1_LFP_SUCCESS() to check for success
 * @param INITIATOR System ID of the transaction initiator
 * @param TARGET System ID of the transaction target
 * @param TYPE type name of the asn1 generated struct, pass directly, do not quote.
 * @param P_WRITE_CB callback passed to lfp_encode()
 * @param P_CTX user context pointer passed to lfp_encode()
 * @param ... the struct value to serialize. Passed as the last argument
 */
#define ASN1_LFP_SERIALIZE(INITIATOR, TARGET, TYPE, P_WRITE_CB, P_CTX, ...)                                            \
    do {                                                                                                               \
        /*Reset last error*/                                                                                           \
        asn1_lfp_last_error.lfp = 0;                                                                                   \
        asn1_lfp_last_error.asn1 = 0;                                                                                  \
                                                                                                                       \
        /*Setup serialized but unencoded buffer*/                                                                      \
        const TYPE payload = __VA_ARGS__;                                                                              \
        unsigned char payload_buffer[TYPE ## _REQUIRED_BYTES_FOR_ENCODING];                                            \
        /*Prepare encoder*/                                                                                            \
        BitStream encoder;                                                                                             \
        BitStream_Init(&encoder, payload_buffer, sizeof(payload_buffer));                                              \
                                                                                                                       \
        const uint8_t endpoint = (lfpId ## TYPE);                                                                      \
                                                                                                                       \
        /*Do encoding*/                                                                                                \
        if ((TYPE ## _Encode)(&payload, &encoder, &asn1_lfp_last_error.asn1, true)) {                                  \
            asn1_lfp_last_error.lfp = lfp_encode(                                                                      \
                INITIATOR,                                                                                             \
                TARGET,                                                                                                \
                endpoint & 0xFE,                                                                                       \
                endpoint & 0x01,                                                                                       \
                payload_buffer,                                                                                        \
                BitStream_GetLength(&encoder),                                                                         \
                P_WRITE_CB,                                                                                            \
                P_CTX                                                                                                  \
            );                                                                                                         \
        }                                                                                                              \
    } while(0)

// Forward decl for the struct to break the cycle
struct asn1_lfp_decode_data_t;

/// Error handler callback, gets passed an ASN1 error code and a *const* pointer to the asn1 data struct.
typedef void (*asn1_lfp_error_handler_t)(int error_code, const struct asn1_lfp_decode_data_t * p_data);

///Struct containing the data to decode
struct asn1_lfp_decode_data_t{
    /// Pointer to the LFP header of the message
    const lfp_header_t * p_header;
    /// Pointer to the decoded LFP message body
    const uint8_t * p_body;
    /// Length of p_body
    const uint16_t body_length;
    /// Pointer to user context
    void * p_ctx;
    /// Error handler callback, see :asn1_lfp_error_handler_t
    asn1_lfp_error_handler_t p_on_error_cb;
};

// Typedef the struct now that it actually exists
typedef struct asn1_lfp_decode_data_t asn1_lfp_decode_data_t;


/**
 * Macro to setup a handler for an ASN1_LFP message.
 *
 * Should be used in a chain of elses, ex: ASN1_LFP_HANDLE_MSG(...) else ASN1_LFP_HANDLE_MSG(...) else unknown_msg()
 * @param DATA Pointer to the struct containing the decode data
 * @param TARGET_SYSTEMID System ID to handle messages for
 * @param TYPE Type of the message to handle. Pass directly, do not quote
 * @param HANDLER
 */
#define ASN1_LFP_HANDLE_MSG(DATA, TARGET_SYSTEMID, TYPE, HANDLER)                                                      \
    if (lfp_composite_id(DATA.p_header) == ((TARGET_SYSTEMID << 8) | (lfpId ## TYPE))) {                               \
        BitStream decoder;                                                                                             \
        BitStream_Init(&decoder, (void *)DATA.p_body, DATA.body_length);                                               \
        int errCode;                                                                                                   \
        TYPE payload;                                                                                                  \
                                                                                                                       \
        /*Attempt to decode*/                                                                                          \
        if ((TYPE ## _Decode)(&payload, &decoder, &errCode)) {                                                         \
            (HANDLER)(&payload, &DATA);                                                                                \
        } else if (DATA.p_on_error_cb != NULL) {                                                                       \
            DATA.p_on_error_cb(errCode, &DATA);                                                                        \
        }                                                                                                              \
    }
