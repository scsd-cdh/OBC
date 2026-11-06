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

    // TODO: handle flags ?
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

    bms_battery_handler(&p_payload->batteryPack1, 1);
    bms_battery_handler(&p_payload->batteryPack2, 2);
}

static inline void bms_systemstatus_callback(BMSSystemStatusResponse * p_payload, asn1_lfp_decode_data_t * p_data) {
    ARG_UNUSED(p_data);

    bms_systemstatus_handler(p_payload);
}

void request_bms_system_status(void)
{
    const uint32_t size = ASN1_LFP_SERIALIZE(cdhSystemId, bmsSystemId, BMSSystemStatusRequest, lfp_send_buf, sizeof(lfp_send_buf), {});
    if (!size) {
        LOG_WRN("Failed to serialize system status request: a:%d l:%d",
            ASN1_LFP_ERROR_CODE.asn1, ASN1_LFP_ERROR_CODE.lfp);
        return;
    }

    const int ret = i2c_write_dt(&dev_bms, lfp_send_buf, size);
    if (ret != 0) {
        LOG_WRN("Failed to send system status request: %d", ret);
        return;
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
        return;
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
        return;
    }

    uart_transport_write(transmitter, encode_buf, fwd_ret);
}

void request_bms_power_status(void) {
    const uint32_t size = ASN1_LFP_SERIALIZE(cdhSystemId, bmsSystemId, BMSPowerStatusRequest, lfp_send_buf, sizeof(lfp_send_buf), {});
    if (!size) {
        LOG_WRN("Failed to serialize power status request: a:%d l:%d",
            ASN1_LFP_ERROR_CODE.asn1, ASN1_LFP_ERROR_CODE.lfp);
        return;
    }

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
        I2C_TIMEOUT_MS);
    if (data.body_length == LFP_FAIL) {
        LOG_WRN("Failed to read BMS response");
        return;
    }

    (void)ASN1_LFP_HANDLE_MSG(data, bmsSystemId, BMSPowerStatusResponse,
                              bms_battery_callback);

    uint8_t encode_buf[ASN1_LFP_SEND_BUF_SIZE(BMSPowerStatusResponse)];
    int fwd_ret = cdh_comms_fwd_lfp_msg(
        &data,
        encode_buf,
        sizeof(encode_buf)
    );
    if (fwd_ret < 0) {
        LOG_WRN("Failed to forward BMS power status response: %d", fwd_ret);
        return;
    }
    uart_transport_write(transmitter, encode_buf, fwd_ret);
}

static void bms_thread(const void *p_arg)
{
    ARG_UNUSED(p_arg);


    while (1)
    {
        LOG_DBG("BMS thread getting power status");
        request_bms_power_status();
        request_bms_system_status();
        k_msleep(2000);
    }
}

K_THREAD_DEFINE(p_bms_thread, 2024, bms_thread, NULL, NULL, NULL,
        THREAD_PRIORITY_BMS, 0, 0);