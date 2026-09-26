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
#include <zephyr/drivers/i2c.h>

#include "lfp_i2c_single.h"

#define SYSID_CDH 1
#define SYSID_COMMS 3
#define SYSID_BMS 5
#define ENDPOINT_COMMS_TEST 154

LOG_MODULE_REGISTER(main);

static const struct i2c_dt_spec dev_bms = I2C_DT_SPEC_GET(DT_NODELABEL(bms_i2c));

static uint8_t lfp_send_buf[MAX_FROM_LIST( // NOLINT(bugprone-branch-clone)
    ASN1_LFP_SEND_BUF_SIZE(BMSPowerStatusRequest),
    ASN1_LFP_SEND_BUF_SIZE(BMSTemperatureStatusRequest),
    ASN1_LFP_SEND_BUF_SIZE(BMSSystemStatusRequest),
    ASN1_LFP_SEND_BUF_SIZE(BMSSetHeaterDutyRequest)
)];
static uint8_t lfp_recv_buf[MAX_FROM_LIST( // NOLINT(bugprone-branch-clone)
    ASN1_LFP_RECV_BUF_SIZE(BMSPowerStatusResponse),
    ASN1_LFP_RECV_BUF_SIZE(BMSTemperatureStatusResponse),
    ASN1_LFP_RECV_BUF_SIZE(BMSSystemStatusResponse),
    ASN1_LFP_RECV_BUF_SIZE(BMSSetHeaterDutyResponse)
)];

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
static void test_function(int error_code, const struct asn1_lfp_decode_data_t* p_data) {
    ARG_UNUSED(p_data);
    LOG_WRN("ASN1 parse failure: %d", error_code);
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
    const uint32_t size = ASN1_LFP_SERIALIZE(cdhSystemId, bmsSystemId, BMSSystemStatusRequest, lfp_send_buf, sizeof(lfp_send_buf), {});
    if (!size) {
        LOG_WRN("Failed to serialize system status request: a:%d l:%d",
            ASN1_LFP_ERROR_CODE.asn1, ASN1_LFP_ERROR_CODE.lfp);
        return;
    }

    LOG_HEXDUMP_INF(lfp_send_buf, size, "Sending:");

    const int ret = i2c_write_dt(&dev_bms, lfp_send_buf, size);
    if (ret != 0) {
        LOG_WRN("Failed to send system status request: %d", ret);
        return;
    }

    lfp_header_t header;
    asn1_lfp_decode_data_t data = {.p_on_error_cb = test_function, .p_header = &header, .p_body = lfp_recv_buf};
    data.body_length = lfp_i2c_single_receive(
        &dev_bms,
        &header,
        lfp_recv_buf, sizeof(lfp_recv_buf),
        K_MSEC(100));
    if (data.body_length == LFP_FAIL) {
        LOG_WRN("Failed to read BMS response");
        return;
    }

}

static void bms_i2c_test() {
    int ret = i2c_write_dt(&dev_bms, (uint8_t[]){0xAA}, 1 );
    if (ret != 0) {
        LOG_INF("i2c write failed: %d", ret);
    } else {
        LOG_WRN("i2c write successful: %d", ret);
    }
}


int main(void)
{
    while (1) {
        asn1_example();
        k_msleep(500);
    }
}
