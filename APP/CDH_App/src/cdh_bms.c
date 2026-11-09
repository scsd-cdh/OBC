#include <asn1_lfp.h>
#include <asn1/bms.h>
#include <asn1/_systems.h>
#include <lfp/body.h>
#include <lfp/header.h>
#include <lfp/stream.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/i2c.h>

#include "lfp_i2c_single.h"

#include "cdh_bms.h"

#include <asn1/comms.h>

#include "cdh_comms.h"
#include "threads.h"

#define I2C_TIMEOUT_MS K_MSEC(1000)

LOG_MODULE_REGISTER(cdh_bms);

K_MUTEX_DEFINE(bms_lock);

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

static void on_error(int error_code, const struct asn1_lfp_decode_data_t* p_data) {
    ARG_UNUSED(p_data);
    LOG_WRN("ASN1 parse failure: %d", error_code);
}

static inline double bms_adc_to_float(const uint16_t val) {
    return ((double)val * 3.3) / (1 << 12);
}

// We don't build zephyr with log level DBG by default. To enable logs pass -DCONFIG_CDH_LOG_LEVEL=4
// This feels like the easiest way to toggle debug logging without crowding the code with #ifdefs
//

static inline void bms_battery_handler(const BMSBatteryPackStatus * p_pack, const int pack_id) {
    LOG_DBG(
        "Battery %d:\n\tVoltage: %.3f | Current Draw: %.3f | Current Charge: %.3f | Overcurrent: %d\n"
        "\tCell A:\n\t\tVoltage: %.3f\n\t\tOV/UV: %d/%d\n"
        "\tCell B:\n\t\tVoltage: %.3f\n\t\tOV/UV: %d/%d",
        pack_id,
        bms_adc_to_float(p_pack->voltage), bms_adc_to_float(p_pack->currentDraw),
        bms_adc_to_float(p_pack->currentCharge),
        p_pack->overcurrent,

        bms_adc_to_float(p_pack->cellA.voltage),
        p_pack->cellA.overvoltage, p_pack->cellA.undervoltage,

        bms_adc_to_float(p_pack->cellB.voltage),
        p_pack->cellB.overvoltage, p_pack->cellB.undervoltage
    );
}

static inline void bms_systemstatus_handler(const BMSSystemStatusResponse * p_status) {
    LOG_DBG(
        "BMS System Status:\n\tVersion: %d\n\tUptime: 0x%x\n\t",
        p_status->version,
        p_status->uptime
    );

    // TODO: Handle?
}

static inline void bms_battery_callback(BMSPowerStatusResponse * p_payload, asn1_lfp_decode_data_t * p_data) {
    ARG_UNUSED(p_data);

    cdh_critical_evaluate_power_status(p_payload);
    bms_battery_handler(&p_payload->batteryPack1, 1);
    bms_battery_handler(&p_payload->batteryPack2, 2);
}

static inline void bms_systemstatus_callback(BMSSystemStatusResponse * p_payload, asn1_lfp_decode_data_t * p_data) {
    ARG_UNUSED(p_data);

    bms_systemstatus_handler(p_payload);
}

static inline void bms_temperature_handler(
    const BMSTemperatureStatusResponse *p_status
) {
    LOG_DBG(
        "BMS Temperature Status: "
        "T1=%u T2=%u T3=%u T4=%u T5=%u T6=%u T7=%u T8=%u",
        p_status->thermistors.arr[0],
        p_status->thermistors.arr[1],
        p_status->thermistors.arr[2],
        p_status->thermistors.arr[3],
        p_status->thermistors.arr[4],
        p_status->thermistors.arr[5],
        p_status->thermistors.arr[6],
        p_status->thermistors.arr[7]
    );
}

static inline void bms_temperature_callback(
    BMSTemperatureStatusResponse *p_payload,
    asn1_lfp_decode_data_t *p_data
) {
    ARG_UNUSED(p_data);

    bms_temperature_handler(p_payload);
}

static inline void request_mock_bms_system_status(void)
{
    BMSSystemStatusResponse system_status = {
        .version = 1,
        .uptime = 0xdeadbeef
    };
    bms_systemstatus_callback(&system_status, NULL);
    uint8_t encode_buf[ASN1_LFP_SEND_BUF_SIZE(BMSSystemStatusResponse)];
    const uint32_t size = ASN1_LFP_SERIALIZE(cdhSystemId, bmsSystemId, BMSSystemStatusResponse, encode_buf, sizeof(encode_buf), system_status);
    if (!size)
    {
        LOG_WRN("Failed to serialize mock BMS system status");
        return;
    }
    const int ret = cdh_telemetry_enqueue(encode_buf, size);
    if (ret != 0)
    {
        LOG_WRN("Failed to qenqueue mock BMS system status telemetry: %d", ret);
    }
}

void request_bms_system_status(void)
{
#ifdef MOCK_BMS
    request_mock_bms_system_status();
    return;
#endif

    if (k_mutex_lock(&bms_lock, K_FOREVER) != 0) {
        return;
    }

    const uint32_t size = ASN1_LFP_SERIALIZE(cdhSystemId, bmsSystemId, BMSSystemStatusRequest, lfp_send_buf, sizeof(lfp_send_buf), {});
    if (!size) {
        LOG_WRN("Failed to serialize system status request: a:%d l:%d",
            ASN1_LFP_ERROR_CODE.asn1, ASN1_LFP_ERROR_CODE.lfp);
        goto unlock;
    }

    const int ret = i2c_write_dt(&dev_bms, lfp_send_buf, size);
    if (ret != 0) {
        LOG_WRN("Failed to send system status request: %d", ret);
        goto unlock;
    }

    lfp_header_t recv_header;
    asn1_lfp_decode_data_t data = {.p_on_error_cb = on_error, .p_header = &recv_header, .p_body = lfp_recv_buf};
    data.body_length = lfp_i2c_single_receive(
        &dev_bms,
        &recv_header,
        lfp_recv_buf, sizeof(lfp_recv_buf),
        I2C_TIMEOUT_MS);
    if (data.body_length == LFP_FAIL) {
        LOG_WRN("Failed to read BMS response");
        goto unlock;
    }

    (void)ASN1_LFP_HANDLE_MSG(data, bmsSystemId, BMSSystemStatusResponse,
                              bms_systemstatus_callback);


    uint8_t encode_buf[ASN1_LFP_SEND_BUF_SIZE(BMSSystemStatusResponse)];
    int fwd_ret = cdh_comms_fwd_lfp_msg(
        &data,
        encode_buf,
        sizeof(encode_buf)
    );

    if (fwd_ret < 0) {
        LOG_WRN("Failed to forward BMS system status response: %d", fwd_ret);
        goto unlock;
    }

    const int enqueue_ret = cdh_telemetry_enqueue(encode_buf, fwd_ret);
    if (enqueue_ret != 0) {
        LOG_WRN("Failed to queue BMS system status telemetry: %d", enqueue_ret);
    }

unlock:
    k_mutex_unlock(&bms_lock);
}

static inline void request_mock_bms_temperature_status(void)
{
    BMSTemperatureStatusResponse temp_status = {
        .thermistors = {
            .arr = { 25, 26, 27, 28, 29, 30, 31, 32 }
        }
    };
    bms_temperature_callback(&temp_status, NULL);
    uint8_t encode_buf[ASN1_LFP_SEND_BUF_SIZE(BMSTemperatureStatusResponse)];
    const uint32_t size = ASN1_LFP_SERIALIZE(cdhSystemId, bmsSystemId, BMSTemperatureStatusResponse, encode_buf, sizeof(encode_buf), temp_status);
    if (!size)
    {
        LOG_WRN("Failed to serialize mock BMS temperature status");
        return;
    }
    const int ret = cdh_telemetry_enqueue(encode_buf, size);
    if (ret != 0)
    {
        LOG_WRN("Failed to enqueue mock BMS temperature status telemetry: %d", ret);
    }
}

void request_bms_temperature_status(void)
{
#ifdef MOCK_BMS
    request_mock_bms_temperature_status();
    return;
#endif

    if (k_mutex_lock(&bms_lock, K_FOREVER) != 0) {
        return;
    }

    const uint32_t size = ASN1_LFP_SERIALIZE(
        cdhSystemId, bmsSystemId, BMSTemperatureStatusRequest,
        lfp_send_buf, sizeof(lfp_send_buf), {});
    if (!size) {
        LOG_WRN("Failed to serialize temperature status request: a:%d l:%d",
            ASN1_LFP_ERROR_CODE.asn1, ASN1_LFP_ERROR_CODE.lfp);
        goto unlock;
    }

    const int ret = i2c_write_dt(&dev_bms, lfp_send_buf, size);
    if (ret != 0) {
        LOG_WRN("Failed to send temperature status request: %d", ret);
        goto unlock;
    }

    lfp_header_t header;
    asn1_lfp_decode_data_t data = {
        .p_on_error_cb = on_error,
        .p_header = &header,
        .p_body = lfp_recv_buf
    };
    data.body_length = lfp_i2c_single_receive(
        &dev_bms,
        &header,
        lfp_recv_buf, sizeof(lfp_recv_buf),
        I2C_TIMEOUT_MS);
    if (data.body_length == LFP_FAIL) {
        LOG_WRN("Failed to read BMS temperature response");
        goto unlock;
    }

    (void)ASN1_LFP_HANDLE_MSG(data, bmsSystemId,
                              BMSTemperatureStatusResponse,
                              bms_temperature_callback);

    uint8_t encode_buf[ASN1_LFP_SEND_BUF_SIZE(BMSTemperatureStatusResponse)];
    const int fwd_ret = cdh_comms_fwd_lfp_msg(
        &data,
        encode_buf,
        sizeof(encode_buf)
    );
    if (fwd_ret < 0) {
        LOG_WRN("Failed to forward BMS temperature status response: %d",
            fwd_ret);
        goto unlock;
    }

    const int enqueue_ret = cdh_telemetry_enqueue(encode_buf, fwd_ret);
    if (enqueue_ret != 0) {
        LOG_WRN("Failed to queue BMS temperature status telemetry: %d",
            enqueue_ret);
    }

unlock:
    k_mutex_unlock(&bms_lock);
}

// TODO: Configure for if we want safe mode or not. For now values hardcoded for safe thresholds
static inline void request_mock_bms_power_status(void)
{
    BMSPowerStatusResponse power_status = {
        .batteryPack1 = {
            .voltage = MV_TO_ADC(4100U),
            .currentDraw = 0x2000,
            .currentCharge = 0x3000,
            .overcurrent = 0,
            .cellA = {
                .voltage = 0x400,
                .overvoltage = 0,
                .undervoltage = 0
            },
            .cellB = {
                .voltage = 0x500,
                .overvoltage = 0,
                .undervoltage = 0
            }
        },
        .batteryPack2 = {
            .voltage = MV_TO_ADC(4100U),
            .currentDraw = 0x7000,
            .currentCharge = 0x8000,
            .overcurrent = 0,
            .cellA = {
                .voltage = 0x900,
                .overvoltage = 0,
                .undervoltage = 0
            },
            .cellB = {
                .voltage = 0xa00,
                .overvoltage = 0,
                .undervoltage = 0
            }
        }
    };

    // a little  redundant
    bms_battery_callback(&power_status, NULL);
    uint8_t encode_buf[ASN1_LFP_SEND_BUF_SIZE(BMSPowerStatusResponse)];
    const uint32_t size = ASN1_LFP_SERIALIZE(cdhSystemId, bmsSystemId, BMSPowerStatusResponse, encode_buf, sizeof(encode_buf), power_status);
    if (!size)
    {
        LOG_WRN("Failed to serialize mock BMS power status");
        return;
    }
    const int ret = cdh_telemetry_enqueue(encode_buf, size);
    if (ret != 0)
    {
        LOG_WRN("Failed to enqueue mock BMS power status telemetry: %d", ret);
    }
}

void request_bms_power_status(void) {
#ifdef MOCK_BMS
    request_mock_bms_power_status();
    return;
#endif

    if (k_mutex_lock(&bms_lock, K_FOREVER) != 0) {
        return;
    }

    const uint32_t size = ASN1_LFP_SERIALIZE(cdhSystemId, bmsSystemId, BMSPowerStatusRequest, lfp_send_buf, sizeof(lfp_send_buf), {});
    if (!size) {
        LOG_WRN("Failed to serialize power status request: a:%d l:%d",
            ASN1_LFP_ERROR_CODE.asn1, ASN1_LFP_ERROR_CODE.lfp);
        goto unlock;
    }

    const int ret = i2c_write_dt(&dev_bms, lfp_send_buf, size);
    if (ret != 0) {
        LOG_WRN("Failed to send power status request: %d", ret);
        goto unlock;
    }

    lfp_header_t header;
    asn1_lfp_decode_data_t data = {.p_on_error_cb = on_error, .p_header = &header, .p_body = lfp_recv_buf};
    data.body_length = lfp_i2c_single_receive(
        &dev_bms,
        &header,
        lfp_recv_buf, sizeof(lfp_recv_buf),
        I2C_TIMEOUT_MS);
    if (data.body_length == LFP_FAIL) {
        LOG_WRN("Failed to read BMS response");
        goto unlock;
    }

    (void)ASN1_LFP_HANDLE_MSG(data, bmsSystemId, BMSPowerStatusResponse,
                              bms_battery_callback);

    // Don't bother formatting / queueing telemetry in safe mode, we won't be using it anyway (?)
    // FIXME: Do we want to queue / log this telemetry in safe mode? It might be useful for debugging, but it also might be a waste of time / resources
    if (cdh_critical_is_safe()) {
        goto unlock;
    }

    uint8_t encode_buf[ASN1_LFP_SEND_BUF_SIZE(BMSPowerStatusResponse)];
    int fwd_ret = cdh_comms_fwd_lfp_msg(
        &data,
        encode_buf,
        sizeof(encode_buf)
    );
    if (fwd_ret < 0) {
        LOG_WRN("Failed to forward BMS power status response: %d", fwd_ret);
        goto unlock;
    }
    const int enqueue_ret = cdh_telemetry_enqueue(encode_buf, fwd_ret);
    if (enqueue_ret != 0) {
        LOG_WRN("Failed to queue BMS power status telemetry: %d", enqueue_ret);
    }

unlock:
    k_mutex_unlock(&bms_lock);
}
