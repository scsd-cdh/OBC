#include <stddef.h>
#include <asn1_lfp.h>
#include <asn1/pds.h>
#include <asn1/_systems.h>
#include <lfp/header.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/i2c.h>

#include "lfp_i2c_single.h"

#include "cdh_pds.h"

#include <asn1/comms.h>

#include "cdh_comms.h"
#include "threads.h"

#define I2C_TIMEOUT_MS K_MSEC(1000)

LOG_MODULE_REGISTER(cdh_pds);

K_MUTEX_DEFINE(pds_lock);

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

static inline void on_error(int error_code, const struct asn1_lfp_decode_data_t* p_data) {
    ARG_UNUSED(p_data);
    LOG_WRN("ASN1 parse failure: %d", error_code);
}

static inline void pds_systemstatus_handler(const PDSSystemStatusResponse * p_status) {
    LOG_DBG(
        "PDS System Status:\n\tVersion: %d\n\tUptime: 0x%x",
        p_status->version,
        p_status->uptime
    );
}

static inline void pds_healthcheck_handler(const PDSHealthCheckResponse * p_status) {
#if LOG_CDH_LOGLEVEL == 4
    uint8_t hash[sizeof(p_status->hash.arr) / sizeof(p_status->hash.arr[0])];

    for (size_t i = 0; i < sizeof(hash); ++i) {
        hash[i] = (uint8_t)p_status->hash.arr[i];
    }

    LOG_HEXDUMP_DBG(hash, sizeof(hash), "PDS Health Check Hash:");
#endif
}

static inline void pds_converter_monitor_handler(const PDSConverterMonitorResponse * p_status) {
    LOG_DBG(
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

static inline void pds_systemstatus_callback(PDSSystemStatusResponse * p_payload,
                                              asn1_lfp_decode_data_t * p_data) {
    ARG_UNUSED(p_data);

    pds_systemstatus_handler(p_payload);
}

static inline void pds_healthcheck_callback(PDSHealthCheckResponse * p_payload,
                                          asn1_lfp_decode_data_t * p_data) {
    ARG_UNUSED(p_data);

    pds_healthcheck_handler(p_payload);
}

static inline void pds_converter_monitor_callback(PDSConverterMonitorResponse * p_payload,
                                                  asn1_lfp_decode_data_t * p_data) {
    ARG_UNUSED(p_data);

    pds_converter_monitor_handler(p_payload);
}

static inline void request_pds_mock_system_status(void)
{
    PDSSystemStatusResponse system_status = {
        .version = 1,
        .uptime = 0xdeadbeef
    };
    pds_systemstatus_callback(&system_status, NULL);
    uint8_t encode_buf[ASN1_LFP_SEND_BUF_SIZE(PDSSystemStatusResponse)];
    const uint32_t size = ASN1_LFP_SERIALIZE(cdhSystemId, pdsSystemId, PDSSystemStatusResponse, encode_buf, sizeof(encode_buf), system_status);
    if (!size)
    {
        LOG_WRN("Failed to serialize mock PDS system status");
        return;
    }

    const int ret = cdh_telemetry_enqueue(encode_buf, size);
    if (ret != 0)
    {
        LOG_WRN("Failed to enqueue mock PDS system status telemetry: %d", ret);
    }
}

void request_pds_system_status(void) {
#ifdef MOCK_PDS
    request_pds_mock_system_status();
    return;
#endif
    if (k_mutex_lock(&pds_lock, K_FOREVER) != 0) {
        return;
    }
    const uint32_t size = ASN1_LFP_SERIALIZE(cdhSystemId, pdsSystemId,
                                             PDSSystemStatusRequest, lfp_send_buf,
                                             sizeof(lfp_send_buf), {});
    if (!size) {
        LOG_WRN("Failed to serialize system status request: a:%d l:%d",
            ASN1_LFP_ERROR_CODE.asn1, ASN1_LFP_ERROR_CODE.lfp);
        goto unlock;
    }

    lfp_header_t header;
    asn1_lfp_decode_data_t data = {
        .p_on_error_cb = on_error,
        .p_header = &header,
        .p_body = lfp_recv_buf
    };


    const int ret = i2c_write_dt(&dev_pds, lfp_send_buf, size);
    if (ret != 0) {
        LOG_WRN("Failed to send system status request: %d", ret);
        goto unlock;
    }

    data.body_length = lfp_i2c_single_receive(
        &dev_pds,
        &header,
        lfp_recv_buf, sizeof(lfp_recv_buf),
        I2C_TIMEOUT_MS);
    if (data.body_length == LFP_FAIL) {
        LOG_WRN("Failed to read PDS response");
        goto unlock;
    }

    (void)ASN1_LFP_HANDLE_MSG(data, pdsSystemId, PDSSystemStatusResponse,
                              pds_systemstatus_callback);

    uint8_t encode_buf[ASN1_LFP_SEND_BUF_SIZE(PDSSystemStatusResponse)];
    int fwd_ret = cdh_comms_fwd_lfp_msg(
        &data,
        encode_buf,
        sizeof(encode_buf)
    );
    if (fwd_ret < 0) {
        LOG_WRN("Failed to forward PDS system status response: %d", fwd_ret);
        goto unlock;
    }

    const int enqueue_ret = cdh_telemetry_enqueue(encode_buf, fwd_ret);
    if (enqueue_ret != 0) {
        LOG_WRN("Failed to queue PDS system status telemetry: %d", enqueue_ret);
    }

unlock:
    k_mutex_unlock(&pds_lock);
}

static inline void request_pds_mock_health_check(void)
{
    PDSHealthCheckResponse health_check = {
        .hash = {
            .arr = {0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
                    0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF}
        }
    };
    pds_healthcheck_callback(&health_check, NULL);
    uint8_t encode_buf[ASN1_LFP_SEND_BUF_SIZE(PDSHealthCheckResponse)];
    const uint32_t size = ASN1_LFP_SERIALIZE(cdhSystemId, pdsSystemId, PDSHealthCheckResponse, encode_buf, sizeof(encode_buf), health_check);
    if (!size)
    {
        LOG_WRN("Failed to serialize mock PDS health check");
        return;
    }
    const int ret = cdh_telemetry_enqueue(encode_buf, size);
    if (ret != 0)
    {
        LOG_WRN("Failed to enqueue mock PDS health check telemetry: %d", ret);
    }
}

void request_pds_health_check(void) {
#ifdef MOCK_PDS
    request_pds_mock_health_check();
    return;
#endif

    if (k_mutex_lock(&pds_lock, K_FOREVER) != 0) {
        return;
    }
    const uint32_t size = ASN1_LFP_SERIALIZE(cdhSystemId, pdsSystemId,
                                             PDSHealthCheckRequest, lfp_send_buf,
                                             sizeof(lfp_send_buf), {});
    if (!size) {
        LOG_WRN("Failed to serialize health check request: a:%d l:%d",
            ASN1_LFP_ERROR_CODE.asn1, ASN1_LFP_ERROR_CODE.lfp);
        goto unlock;
    }

    lfp_header_t header;
    asn1_lfp_decode_data_t data = {
        .p_on_error_cb = on_error,
        .p_header = &header,
        .p_body = lfp_recv_buf
    };

    const int ret = i2c_write_dt(&dev_pds, lfp_send_buf, size);
    if (ret != 0) {
        LOG_WRN("Failed to send health check request: %d", ret);
        goto unlock;
    }

    data.body_length = lfp_i2c_single_receive(
        &dev_pds,
        &header,
        lfp_recv_buf, sizeof(lfp_recv_buf),
        I2C_TIMEOUT_MS);
    if (data.body_length == LFP_FAIL) {
        LOG_WRN("Failed to read PDS response");
        goto unlock;
    }

    (void)ASN1_LFP_HANDLE_MSG(data, pdsSystemId, PDSHealthCheckResponse,
                              pds_healthcheck_callback);

    uint8_t encode_buf[ASN1_LFP_SEND_BUF_SIZE(PDSHealthCheckResponse)];
    int fwd_ret = cdh_comms_fwd_lfp_msg(
        &data,
        encode_buf,
        sizeof(encode_buf)
    );
    if (fwd_ret < 0) {
        LOG_WRN("Failed to forward PDS health check response: %d", fwd_ret);
        goto unlock;
    }

    const int enqueue_ret = cdh_telemetry_enqueue(encode_buf, fwd_ret);
    if (enqueue_ret != 0) {
        LOG_WRN("Failed to queue PDS health check telemetry: %d", enqueue_ret);
    }

unlock:
    k_mutex_unlock(&pds_lock);
}

static inline void request_pds_mock_converter_monitor(void)
{
    PDSConverterMonitorResponse converter_monitor = {
        .int5vVs = MV_TO_ADC(5000U),
        .reg5vVs = MV_TO_ADC(5000U),
        .converterA5vVs = MV_TO_ADC(5000U),
        .converterB5vVs = MV_TO_ADC(5000U),
        .temperatureSense = MV_TO_ADC(2500U)
    };
    pds_converter_monitor_callback(&converter_monitor, NULL);
    uint8_t encode_buf[ASN1_LFP_SEND_BUF_SIZE(PDSConverterMonitorResponse)];
    const uint32_t size = ASN1_LFP_SERIALIZE(cdhSystemId, pdsSystemId, PDSConverterMonitorResponse, encode_buf, sizeof(encode_buf), converter_monitor);
    if (!size)
    {
        LOG_WRN("Failed to serialize mock PDS converter monitor");
        return;
    }
    const int ret = cdh_telemetry_enqueue(encode_buf, size);
    if (ret != 0)
    {
        LOG_WRN("Failed to enqueue mock PDS converter monitor telemetry: %d", ret);
    }
}

void request_pds_converter_monitor(void) {
#ifdef MOCK_PDS
    request_pds_mock_converter_monitor();
    return;
#endif

    if (k_mutex_lock(&pds_lock, K_FOREVER) != 0) {
        return;
    }
    const uint32_t size = ASN1_LFP_SERIALIZE(cdhSystemId, pdsSystemId,
                                             PDSConverterMonitorRequest, lfp_send_buf,
                                             sizeof(lfp_send_buf), {});
    if (!size) {
        LOG_WRN("Failed to serialize converter monitor request: a:%d l:%d",
            ASN1_LFP_ERROR_CODE.asn1, ASN1_LFP_ERROR_CODE.lfp);
        goto unlock;
    }

    lfp_header_t header;
    asn1_lfp_decode_data_t data = {
        .p_on_error_cb = on_error,
        .p_header = &header,
        .p_body = lfp_recv_buf
    };

    const int ret = i2c_write_dt(&dev_pds, lfp_send_buf, size);
    if (ret != 0) {
        LOG_WRN("Failed to send converter monitor request: %d", ret);
        goto unlock;
    }

    data.body_length = lfp_i2c_single_receive(
        &dev_pds,
        &header,
        lfp_recv_buf, sizeof(lfp_recv_buf),
        I2C_TIMEOUT_MS);
    if (data.body_length == LFP_FAIL) {
        LOG_WRN("Failed to read PDS response");
        goto unlock;
    }

    (void)ASN1_LFP_HANDLE_MSG(data, pdsSystemId, PDSConverterMonitorResponse,
                              pds_converter_monitor_callback);

    uint8_t encode_buf[ASN1_LFP_SEND_BUF_SIZE(PDSConverterMonitorResponse)];
    int fwd_ret = cdh_comms_fwd_lfp_msg(
        &data,
        encode_buf,
        sizeof(encode_buf)
    );
    if (fwd_ret < 0) {
        LOG_WRN("Failed to forward PDS converter monitor response: %d", fwd_ret);
        goto unlock;
    }

    const int enqueue_ret = cdh_telemetry_enqueue(encode_buf, fwd_ret);
    if (enqueue_ret != 0) {
        LOG_WRN("Failed to queue PDS converter monitor telemetry: %d", enqueue_ret);
    }

unlock:
    k_mutex_unlock(&pds_lock);
}
