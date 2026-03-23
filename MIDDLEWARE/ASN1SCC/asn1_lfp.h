#pragma once
#include <lfp.h>
#include <lfp/body.h>
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

/// Size of the buffer required by ASN1_LFP_SERIALIZE() for a given message type
#define ASN1_LFP_SEND_BUF_SIZE(TYPE) (LFP_HEADER_SIZE + LFP_ENCODED_BODY_LENGTH_APPROX(TYPE ## _REQUIRED_BYTES_FOR_ENCODING))
/// Size of the buffer required by lfp_stream for a given message type
// We dont need to hold the crc in the recv buffer, the stream parser consumes it
#define ASN1_LFP_RECV_BUF_SIZE(TYPE) (LFP_ENCODED_BODY_LENGTH_APPROX(TYPE ## _REQUIRED_BYTES_FOR_ENCODING) - LFP_DATA_CRC32_SIZE)

/**
 * Macro to serialize a struct to an LFP target in one shot. Use ASN1_LFP_SUCCESS() to check for success
 * @param INITIATOR System ID of the transaction initiator
 * @param TARGET System ID of the transaction target
 * @param TYPE type name of the asn1 generated struct, pass directly, do not quote.
 * @param P_BUFFER buffer that will contain the encoded message, use ASN1_LFP_BUF_SIZE() to determine the maximal size for a given message
 * @param BUFFER_LEN length P_BUFFER
 * @param P_WRITE_CB callback called with the encoded buffer, the length and P_CTX
 * @param P_CTX user context pointer passed to P_WRITE_CB
 * @param ... the struct value to serialize. Passed as the last argument
 * @return encoded length on success, 0 on error (see ASN1_LFP_ERROR_CODE)
 */
#define ASN1_LFP_SERIALIZE(INITIATOR, TARGET, TYPE, P_BUFFER, BUFFER_LEN, ...)                                         \
    ({                                                                                                                 \
        uint32_t asn1_lfp__ret = 0;                                                                                    \
        /*Reset last error*/                                                                                           \
        asn1_lfp_last_error.lfp = 0;                                                                                   \
        asn1_lfp_last_error.asn1 = 0;                                                                                  \
                                                                                                                       \
                                                                                                                       \
        /*Validate that the buffer is big enough, otherwise abort*/                                                    \
        if ((BUFFER_LEN) < ASN1_LFP_SEND_BUF_SIZE(TYPE)) {                                                             \
            asn1_lfp_last_error.lfp = LFP_EOVERFLOW;                                                                   \
        } else {                                                                                                       \
            /*Setup serialized but unencoded buffer*/                                                                  \
            const TYPE asn1_lfp__payload = __VA_ARGS__;                                                                \
            const uint16_t asn1_lfp__asn1_size = TYPE ## _REQUIRED_BYTES_FOR_ENCODING;                                 \
            unsigned char * asn1_lfp__p_asn1_payload = (P_BUFFER) + (BUFFER_LEN) - asn1_lfp__asn1_size;                \
            /*Prepare encoder*/                                                                                        \
            BitStream asn1_lfp__encoder;                                                                               \
            BitStream_Init(&asn1_lfp__encoder, asn1_lfp__p_asn1_payload, asn1_lfp__asn1_size);                         \
            const uint8_t asn1_lfp__endpoint = (lfpId ## TYPE);                                                        \
                                                                                                                       \
            /*Do encoding*/                                                                                            \
            if ((TYPE ## _Encode)(&asn1_lfp__payload, &asn1_lfp__encoder, &asn1_lfp_last_error.asn1, true)) {          \
                 const lfp_size_or_code_t asn1_lfp__lfp_ret = lfp_encode(                                              \
                    (INITIATOR),                                                                                       \
                    (TARGET),                                                                                          \
                    asn1_lfp__endpoint & 0xFE,                                                                         \
                    asn1_lfp__endpoint & 0x01,                                                                         \
                    asn1_lfp__p_asn1_payload,                                                                          \
                    BitStream_GetLength(&asn1_lfp__encoder),                                                           \
                    (P_BUFFER),                                                                                        \
                    (BUFFER_LEN)                                                                                       \
                );                                                                                                     \
                if(asn1_lfp__lfp_ret < 0) {                                                                            \
                    asn1_lfp_last_error.lfp = (lfp_code_t)asn1_lfp__lfp_ret;                                           \
                } else {                                                                                               \
                    asn1_lfp__ret = asn1_lfp__lfp_ret;                                                                 \
                }                                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
        asn1_lfp__ret;                                                                                                 \
    })

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
    uint16_t body_length;
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
    ({                                                                                                                 \
        bool condition = lfp_composite_id(DATA.p_header) == ((TARGET_SYSTEMID << 8) | (lfpId ## TYPE));                \
        if (condition) {                                                                                               \
            BitStream decoder;                                                                                         \
            BitStream_AttachBuffer(&decoder, (void *)DATA.p_body, DATA.body_length);                                   \
            int errCode;                                                                                               \
            TYPE payload;                                                                                              \
                                                                                                                       \
            /*Attempt to decode*/                                                                                      \
            if ((TYPE ## _Decode)(&payload, &decoder, &errCode)) {                                                     \
                (HANDLER)(&payload, &DATA);                                                                            \
            } else if (DATA.p_on_error_cb != NULL) {                                                                   \
                DATA.p_on_error_cb(errCode, &DATA);                                                                    \
            }                                                                                                          \
                                                                                                                       \
        }                                                                                                              \
        condition;                                                                                                     \
    })

