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

#include "cdh_bms_example.h"

LOG_MODULE_REGISTER(cdh_bms_example);

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
static void bms_logging_log_battery(const BMSBatteryPackStatus * p_pack, const int pack_id) {

    LOG_INF(
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

static void bms_logging_log_systemstatus(const BMSSystemStatusResponse * p_status) {
    LOG_INF(
        "BMS System Status:\n\tVersion: %d\n\tUptime: 0x%x\n\t",
        p_status->version,
        p_status->uptime
    );
}

static void bms_logging_battery_callback(BMSPowerStatusResponse * p_payload, asn1_lfp_decode_data_t * p_data) {
    ARG_UNUSED(p_data);

    bms_logging_log_battery(&p_payload->batteryPack1, 1);
    bms_logging_log_battery(&p_payload->batteryPack2, 2);
}

static void bms_logging_systemstatus_callback(BMSSystemStatusResponse * p_payload, asn1_lfp_decode_data_t * p_data) {
    ARG_UNUSED(p_data);

    bms_logging_log_systemstatus(p_payload);
}

void get_bms_system_status(void) {
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
    asn1_lfp_decode_data_t data = {.p_on_error_cb = on_error, .p_header = &header, .p_body = lfp_recv_buf};
    data.body_length = lfp_i2c_single_receive(
        &dev_bms,
        &header,
        lfp_recv_buf, sizeof(lfp_recv_buf),
        K_MSEC(100));
    if (data.body_length == LFP_FAIL) {
        LOG_WRN("Failed to read BMS response");
        return;
    }

    if (ASN1_LFP_HANDLE_MSG(data, bmsSystemId, BMSSystemStatusResponse, bms_logging_systemstatus_callback)) {
        return;
    }
}

void get_bms_power_status(void) {
    const uint32_t size = ASN1_LFP_SERIALIZE(cdhSystemId, bmsSystemId, BMSPowerStatusRequest, lfp_send_buf, sizeof(lfp_send_buf), {});
    if (!size) {
        LOG_WRN("Failed to serialize power status request: a:%d l:%d",
            ASN1_LFP_ERROR_CODE.asn1, ASN1_LFP_ERROR_CODE.lfp);
        return;
    }

    LOG_HEXDUMP_INF(lfp_send_buf, size, "Sending:");

    const int ret = i2c_write_dt(&dev_bms, lfp_send_buf, size);
    if (ret != 0) {
        LOG_WRN("Failed to send power status request: %d", ret);
        return;
    }

    lfp_header_t header;
    asn1_lfp_decode_data_t data = {.p_on_error_cb = on_error, .p_header = &header, .p_body = lfp_recv_buf};
    data.body_length = lfp_i2c_single_receive(
        &dev_bms,
        &header,
        lfp_recv_buf, sizeof(lfp_recv_buf),
        K_MSEC(100));
    if (data.body_length == LFP_FAIL) {
        LOG_WRN("Failed to read BMS response");
        return;
    }

    if (ASN1_LFP_HANDLE_MSG(data, bmsSystemId, BMSPowerStatusResponse, bms_logging_battery_callback)) {
        return;
    }
}
