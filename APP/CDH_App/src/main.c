#include "main.h"

#include <asn1_lfp.h>
#include <lfp.h>
#include <asn1/bms.h>
#include <asn1/_systems.h>
#include <lfp/body.h>
#include <lfp/header.h>
#include <lfp/stream.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#define SYSID_CDH 1
#define SYSID_COMMS 3
#define ENDPOINT_COMMS_TEST 154

LOG_MODULE_REGISTER(main);

void asn1_example_bms_system_status_req(const BMSSystemStatusRequest * p_payload, const asn1_lfp_decode_data_t * p_data) {
    LOG_INF("Received system status request");
}
void asn1_example_bms_system_status_res(const BMSSystemStatusResponse * p_payload, const asn1_lfp_decode_data_t * p_data) {
    LOG_INF("Received system status response: version %d, uptime %d", p_payload->version, p_payload->uptime);
}

void asn1_example_stream_error_handler(lfp_code_t reason, void * p_ctx) {
    ARG_UNUSED(p_ctx);

    LOG_ERR("Stream error: %d", reason);
}

void asn1_example_error_handler(const int asn1_error, const asn1_lfp_decode_data_t * p_data) {
    LOG_ERR("Failed to decode message %d (error %d)", lfp_composite_id(p_data->p_header), asn1_error);
}

void asn1_example_on_msg(const lfp_header_t * p_header, const uint8_t * p_body, const uint16_t body_length, void * p_ctx) {
    ARG_UNUSED(p_ctx);
    ARG_UNUSED(body_length);

    LOG_INF("Decoded packet on endpoint %d", p_header->endpoint);
    LOG_HEXDUMP_INF(p_body, body_length, "Decoded packet:");

    const asn1_lfp_decode_data_t data = {
        .p_header = p_header,
        .p_body = p_body,
        .p_ctx = p_ctx,
        .body_length = body_length,
        .p_on_error_cb = asn1_example_error_handler
    };

    if (ASN1_LFP_HANDLE_MSG(data, bmsSystemId, BMSSystemStatusRequest, asn1_example_bms_system_status_req)) {
        return;
    }
    if (ASN1_LFP_HANDLE_MSG(data, bmsSystemId, BMSSystemStatusResponse, asn1_example_bms_system_status_res)) {
        return;
    }

    LOG_HEXDUMP_WRN(p_header, sizeof(lfp_header_t), "Received unknown message:");
}

void asn1_example(void) {
    uint8_t payload[ASN1_LFP_SEND_BUF_SIZE(BMSSystemStatusResponse)];

    const uint32_t size = ASN1_LFP_SERIALIZE(cdhSystemId, bmsSystemId, BMSSystemStatusResponse, payload, sizeof(payload), {
        .uptime = 0xDEADBEEF,
        .version = 12
    });
    if (!size) {
        LOG_ERR("LFP encoding failed! %d %d", ASN1_LFP_ERROR_CODE.asn1, ASN1_LFP_ERROR_CODE.lfp);
    }

    LOG_HEXDUMP_INF(payload, size, "Sending:");

    uint8_t payload2[ASN1_LFP_RECV_BUF_SIZE(BMSSystemStatusResponse)];
    lfp_stream_ctx_t stream;
    lfp_stream_init(&stream, payload2, sizeof(payload2), NULL, asn1_example_on_msg, asn1_example_stream_error_handler, NULL);
    lfp_stream_update_buf(&stream, payload, size);
}

int main(void)
{
    k_sleep(K_FOREVER);
    // asn1_example();
}