#include <stddef.h>
#include <asn1_lfp.h>
#include <asn1/pds.h>
#include <asn1/_systems.h>
#include <lfp/header.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/i2c.h>

#include "lfp_i2c_single.h"

#include "cdh_pds_example.h"

LOG_MODULE_REGISTER(cdh_pds_example);

static const struct i2c_dt_spec dev_pds = I2C_DT_SPEC_GET(DT_NODELABEL(pds_i2c));

static uint8_t lfp_send_buf[MAX_FROM_LIST( // NOLINT(bugprone-branch-clone)
    ASN1_LFP_SEND_BUF_SIZE(PDSSystemStatusRequest),
    ASN1_LFP_SEND_BUF_SIZE(PDSHealthCheckRequest),
    ASN1_LFP_SEND_BUF_SIZE(PDSConverterMonitorRequest),
    ASN1_LFP_SEND_BUF_SIZE(PDSRebootRequest)
)];
static uint8_t lfp_recv_buf[MAX_FROM_LIST( // NOLINT(bugprone-branch-clone)
    ASN1_LFP_RECV_BUF_SIZE(PDSSystemStatusResponse),
    ASN1_LFP_RECV_BUF_SIZE(PDSHealthCheckResponse),
    ASN1_LFP_RECV_BUF_SIZE(PDSConverterMonitorResponse),
    ASN1_LFP_RECV_BUF_SIZE(PDSRebootResponse)
)];

static inline double bms_adc_to_float(const uint16_t val) {
    return ((double)val * 3.3) / (1 << 12);
}

static void on_error(int error_code, const struct asn1_lfp_decode_data_t* p_data) {
    ARG_UNUSED(p_data);
    LOG_WRN("ASN1 parse failure: %d", error_code);
}

static void pds_logging_log_systemstatus(const PDSSystemStatusResponse * p_status) {
    LOG_INF(
        "PDS System Status:\n\tVersion: %d\n\tUptime: 0x%x",
        p_status->version,
        p_status->uptime
    );
}

static void pds_logging_log_healthcheck(const PDSHealthCheckResponse * p_status) {
    uint8_t hash[sizeof(p_status->hash.arr) / sizeof(p_status->hash.arr[0])];

    for (size_t i = 0; i < sizeof(hash); ++i) {
        hash[i] = (uint8_t)p_status->hash.arr[i];
    }

    LOG_HEXDUMP_INF(hash, sizeof(hash), "PDS Health Check Hash:");
}

static void pds_logging_log_converter_monitor(const PDSConverterMonitorResponse * p_status) {
    LOG_INF(
        "PDS Converter Monitor:\n"
        "\tint5vVs: %.3f V\n"
        "\treg5vVs: %.3f V\n"
        "\tconverterA5vVs: %.3f V\n"
        "\tconverterB5vVs: %.3f V\n"
        "\ttemperatureSense: %.3f V\n",
        bms_adc_to_float(p_status->int5vVs),
        bms_adc_to_float(p_status->reg5vVs),
        bms_adc_to_float(p_status->converterA5vVs),
        bms_adc_to_float(p_status->converterB5vVs),
        bms_adc_to_float(p_status->temperatureSense)
    );
}

static void pds_logging_systemstatus_callback(PDSSystemStatusResponse * p_payload,
                                              asn1_lfp_decode_data_t * p_data) {
    ARG_UNUSED(p_data);

    pds_logging_log_systemstatus(p_payload);
}

static void pds_logging_healthcheck_callback(PDSHealthCheckResponse * p_payload,
                                             asn1_lfp_decode_data_t * p_data) {
    ARG_UNUSED(p_data);

    pds_logging_log_healthcheck(p_payload);
}

static void pds_logging_converter_monitor_callback(PDSConverterMonitorResponse * p_payload,
                                                  asn1_lfp_decode_data_t * p_data) {
    ARG_UNUSED(p_data);

    pds_logging_log_converter_monitor(p_payload);
}

void get_pds_system_status(void) {
    const uint32_t size = ASN1_LFP_SERIALIZE(cdhSystemId, pdsSystemId,
                                             PDSSystemStatusRequest, lfp_send_buf,
                                             sizeof(lfp_send_buf), {});
    if (!size) {
        LOG_WRN("Failed to serialize system status request: a:%d l:%d",
            ASN1_LFP_ERROR_CODE.asn1, ASN1_LFP_ERROR_CODE.lfp);
        return;
    }

    LOG_HEXDUMP_INF(lfp_send_buf, size, "Sending:");

    const int ret = i2c_write_dt(&dev_pds, lfp_send_buf, size);
    if (ret != 0) {
        LOG_WRN("Failed to send system status request: %d", ret);
        return;
    }

    lfp_header_t header;
    asn1_lfp_decode_data_t data = {.p_on_error_cb = on_error, .p_header = &header, .p_body = lfp_recv_buf};
    data.body_length = lfp_i2c_single_receive(
        &dev_pds,
        &header,
        lfp_recv_buf, sizeof(lfp_recv_buf),
        K_MSEC(100));
    if (data.body_length == LFP_FAIL) {
        LOG_WRN("Failed to read PDS response");
        return;
    }

    (void)ASN1_LFP_HANDLE_MSG(data, pdsSystemId, PDSSystemStatusResponse,
                              pds_logging_systemstatus_callback);
}

void get_pds_health_check(void) {
    const uint32_t size = ASN1_LFP_SERIALIZE(cdhSystemId, pdsSystemId,
                                             PDSHealthCheckRequest, lfp_send_buf,
                                             sizeof(lfp_send_buf), {});
    if (!size) {
        LOG_WRN("Failed to serialize health check request: a:%d l:%d",
            ASN1_LFP_ERROR_CODE.asn1, ASN1_LFP_ERROR_CODE.lfp);
        return;
    }

    LOG_HEXDUMP_INF(lfp_send_buf, size, "Sending:");

    const int ret = i2c_write_dt(&dev_pds, lfp_send_buf, size);
    if (ret != 0) {
        LOG_WRN("Failed to send health check request: %d", ret);
        return;
    }

    lfp_header_t header;
    asn1_lfp_decode_data_t data = {.p_on_error_cb = on_error, .p_header = &header, .p_body = lfp_recv_buf};
    data.body_length = lfp_i2c_single_receive(
        &dev_pds,
        &header,
        lfp_recv_buf, sizeof(lfp_recv_buf),
        K_MSEC(100));
    if (data.body_length == LFP_FAIL) {
        LOG_WRN("Failed to read PDS response");
        return;
    }

    (void)ASN1_LFP_HANDLE_MSG(data, pdsSystemId, PDSHealthCheckResponse,
                              pds_logging_healthcheck_callback);
}

void get_pds_converter(void) {
    const uint32_t size = ASN1_LFP_SERIALIZE(cdhSystemId, pdsSystemId,
                                             PDSConverterMonitorRequest, lfp_send_buf,
                                             sizeof(lfp_send_buf), {});
    if (!size) {
        LOG_WRN("Failed to serialize converter monitor request: a:%d l:%d",
            ASN1_LFP_ERROR_CODE.asn1, ASN1_LFP_ERROR_CODE.lfp);
        return;
    }

    LOG_HEXDUMP_INF(lfp_send_buf, size, "Sending:");

    const int ret = i2c_write_dt(&dev_pds, lfp_send_buf, size);
    if (ret != 0) {
        LOG_WRN("Failed to send converter monitor request: %d", ret);
        return;
    }

    lfp_header_t header;
    asn1_lfp_decode_data_t data = {.p_on_error_cb = on_error, .p_header = &header, .p_body = lfp_recv_buf};
    data.body_length = lfp_i2c_single_receive(
        &dev_pds,
        &header,
        lfp_recv_buf, sizeof(lfp_recv_buf),
        K_MSEC(100));
    if (data.body_length == LFP_FAIL) {
        LOG_WRN("Failed to read PDS response");
        return;
    }

    (void)ASN1_LFP_HANDLE_MSG(data, pdsSystemId, PDSConverterMonitorResponse,
                              pds_logging_converter_monitor_callback);
}