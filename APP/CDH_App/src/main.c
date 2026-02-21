#include "main.h"

#include <lfp.h>
#include <lfp/body.h>
#include <lfp/header.h>
#include <lfp/stream.h>
#include <ulog/ulog.h>
#include <zephyr/kernel.h>

#define SYSID_CDH 3
#define SYSID_COMMS 4
#define ENDPOINT_COMMS_TEST 154



void simple_example(void) {
    while (1) {
        const uint8_t payload[10] = "Meow meow";
        uint8_t buf[LFP_HEADER_SIZE + lfp_encoded_body_length(payload, sizeof(payload))];

        if (lfp_encode_to_buf(SYSID_CDH, SYSID_COMMS, ENDPOINT_COMMS_TEST, LFP_DIRECTION_REQUEST,
            payload,sizeof(payload),
            buf,sizeof(buf)
        ) < 0) {
            ULOG_ERROR("Failed to encode LFP packet!");
        }

        ULOG_INFO("LFP encoding successful: {}", ULOG_SLICE(buf));

        lfp_header_t header;
        if (lfp_parse_header(buf, &header) != LFP_EOK) {
            ULOG_ERROR("Failed to decode LFP header!");
        }

        ULOG_INFO(
            "LFP header decoded! \n\tInitiator: {} \n\tTarget: {} \n\tEndpoint: {} \n\tDirection: {} \n\tLen: {}",
            header.initiator, header.target, header.endpoint, (uint8_t)header.direction, header.encoded_length,
        );

        uint8_t * p_body_buf = buf + LFP_HEADER_SIZE;
        if (lfp_body_decoder(&header, p_body_buf, p_body_buf, sizeof(buf) - LFP_HEADER_SIZE) == LFP_FAIL) {
            ULOG_ERROR("Failed to decode buffer");
        }

        ULOG_INFO("Packet decoded successfully: {}", (char *)p_body_buf);

        k_msleep(2000);
    }
}

void on_msg(const lfp_header_t * p_header, const uint8_t * p_body, uint16_t body_length, void * p_ctx) {
    ARG_UNUSED(p_ctx);
    ARG_UNUSED(body_length);

    ULOG_INFO("Decoded packet on endpoint {}: {}", p_header->endpoint, (char *)p_body);
}

void on_error(const lfp_code_t reason, void * p_ctx) {
    ARG_UNUSED(p_ctx);

    ULOG_ERROR("Stream error: {}", reason);
}

void stream_example(void) {
    const uint8_t payload[10] = "Woof woof";
    uint8_t buf[LFP_HEADER_SIZE + lfp_encoded_body_length(payload, sizeof(payload))];

    if (lfp_encode_to_buf(SYSID_CDH, SYSID_COMMS, ENDPOINT_COMMS_TEST, LFP_DIRECTION_REQUEST,
        payload,sizeof(payload),
        buf,sizeof(buf)
    ) < 0) {
        ULOG_ERROR("Failed to encode LFP packet!");
    }

    ULOG_INFO("LFP encoding successful: {}", ULOG_SLICE(buf));

    lfp_stream_ctx_t ctx;
    lfp_stream_init(&ctx, (uint8_t[1024]){}, 1024, NULL, on_msg, on_error, NULL);
    lfp_stream_update_buf(&ctx, buf, sizeof(buf));

    ULOG_INFO("LFP Packet ingested");
}

int main(void)
{
    stream_example();
}