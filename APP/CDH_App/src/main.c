#include "main.h"
#include "pins.h"

#include <asn1_lfp.h>
#include <asn1/bms.h>
#include <asn1/pds.h>
#include <asn1/_systems.h>

#include "cdh_comms.h"
#include "uart/uart_transport.h"
#include "lfp/stream.h"
#include "zephyr/kernel.h"
#include "zephyr/logging/log.h"

LOG_MODULE_REGISTER(cdh, LOG_LEVEL_DBG);

typedef struct
{
    lfp_header_t *p_header;
    uint8_t rx_history[32];
    uint8_t rx_history_next;
    uint8_t rx_history_count;
} user_ctx_t;

static inline double cdh_adc_to_float(const uint16_t value)
{
    return ((double)value * 3.3) / (1 << 12);
}

static void cdh_bms_system_status_callback(
    BMSSystemStatusResponse *payload,
    asn1_lfp_decode_data_t *data
)
{
    ARG_UNUSED(data);
    LOG_DBG(
        "BMS System Status:\n\tVersion: %d\n\tUptime: 0x%x",
        payload->version,
        payload->uptime
    );
}

static void cdh_bms_power_status_callback(
    BMSPowerStatusResponse *payload,
    asn1_lfp_decode_data_t *data
)
{
    ARG_UNUSED(data);

    for (int pack_id = 1; pack_id <= 2; ++pack_id) {
        const BMSBatteryPackStatus *pack = pack_id == 1
            ? &payload->batteryPack1
            : &payload->batteryPack2;

        LOG_DBG(
            "Battery %d:\n\tVoltage: %.3f | Current Draw: %.3f | "
            "Current Charge: %.3f | Overcurrent: %d\n"
            "\tCell A:\n\t\tVoltage: %.3f\n\t\tOV/UV: %d/%d\n"
            "\tCell B:\n\t\tVoltage: %.3f\n\t\tOV/UV: %d/%d",
            pack_id,
            cdh_adc_to_float(pack->voltage),
            cdh_adc_to_float(pack->currentDraw),
            cdh_adc_to_float(pack->currentCharge),
            pack->overcurrent,
            cdh_adc_to_float(pack->cellA.voltage),
            pack->cellA.overvoltage,
            pack->cellA.undervoltage,
            cdh_adc_to_float(pack->cellB.voltage),
            pack->cellB.overvoltage,
            pack->cellB.undervoltage
        );
    }
}

static void cdh_bms_temperature_status_callback(
    BMSTemperatureStatusResponse *payload,
    asn1_lfp_decode_data_t *data
)
{
    ARG_UNUSED(data);
    LOG_DBG(
        "BMS Temperature Status: "
        "T1=%u T2=%u T3=%u T4=%u T5=%u T6=%u T7=%u T8=%u",
        payload->thermistors.arr[0],
        payload->thermistors.arr[1],
        payload->thermistors.arr[2],
        payload->thermistors.arr[3],
        payload->thermistors.arr[4],
        payload->thermistors.arr[5],
        payload->thermistors.arr[6],
        payload->thermistors.arr[7]
    );
}

static void cdh_pds_system_status_callback(
    PDSSystemStatusResponse *payload,
    asn1_lfp_decode_data_t *data
)
{
    ARG_UNUSED(data);
    LOG_DBG(
        "PDS System Status:\n\tVersion: %d\n\tUptime: 0x%x",
        payload->version,
        payload->uptime
    );
}

static void cdh_pds_health_check_callback(
    PDSHealthCheckResponse *payload,
    asn1_lfp_decode_data_t *data
)
{
    ARG_UNUSED(data);
#ifdef LOG_LEVEL_DBG
    uint8_t hash[sizeof(payload->hash.arr) / sizeof(payload->hash.arr[0])];

    for (size_t i = 0; i < sizeof(hash); ++i) {
        hash[i] = (uint8_t)payload->hash.arr[i];
    }

    LOG_HEXDUMP_DBG(hash, sizeof(hash), "PDS Health Check Hash:");
#endif
}

static void cdh_pds_converter_monitor_callback(
    PDSConverterMonitorResponse *payload,
    asn1_lfp_decode_data_t *data
)
{
    ARG_UNUSED(data);
    LOG_DBG(
        "PDS Converter Monitor:\n"
        "\tint5vVs: %.3f V\n"
        "\treg5vVs: %.3f V\n"
        "\tconverterA5vVs: %.3f V\n"
        "\tconverterB5vVs: %.3f V\n"
        "\ttemperatureSense: %.3f V\n",
        cdh_adc_to_float(payload->int5vVs),
        cdh_adc_to_float(payload->reg5vVs),
        cdh_adc_to_float(payload->converterA5vVs),
        cdh_adc_to_float(payload->converterB5vVs),
        cdh_adc_to_float(payload->temperatureSense)
    );
}

void target_to_str(uint8_t target, char *buf, size_t buf_size)
{
    if (target == cdhSystemId) {
        snprintf(buf, buf_size, "CDH(%d)", target);
    } else if (target == bmsSystemId) {
        snprintf(buf, buf_size, "BMS(%d)", target);
    } else if (target == pdsSystemId) {
        snprintf(buf, buf_size, "PDS(%d)", target);
    } else {
        snprintf(buf, buf_size, "Unknown(%d)", target);
    }
}

void endpoint_to_str(uint8_t endpoint, char *buf, size_t buf_size)
{
    if (endpoint == lfpIdBMSPowerStatusResponse) {
        snprintf(buf, buf_size, "BMSPowerStatusResponse");
    } else if (endpoint == lfpIdBMSSystemStatusResponse) {
        snprintf(buf, buf_size, "BMSSystemStatusResponse");
    } else if (endpoint == lfpIdBMSTemperatureStatusResponse) {
        snprintf(buf, buf_size, "BMSTemperatureStatusResponse");
    } else if (endpoint == lfpIdPDSSystemStatusResponse) {
        snprintf(buf, buf_size, "PDSSystemStatusResponse");
    } else if (endpoint == lfpIdPDSHealthCheckResponse) {
        snprintf(buf, buf_size, "PDSHealthCheckResponse");
    } else if (endpoint == lfpIdPDSConverterMonitorResponse) {
        snprintf(buf, buf_size, "PDSConverterMonitorResponse");
    } else {
        snprintf(buf, buf_size, "Unknown(%d)", endpoint);
    }
}

static void cdh_asn1_error_callback(
    int error_code,
    const asn1_lfp_decode_data_t *data
)
{

    char target[16];
    char endpoint[16];
    target_to_str(data->p_header->target, target, sizeof(target));
    endpoint_to_str(data->p_header->endpoint, endpoint, sizeof(endpoint));
    LOG_WRN("CDH ASN.1 parse failed, error_code=%d, target=%s, endpoint=%s", error_code, target, endpoint);
}

#define ASN1_RESPONSE_MESSAGES(X) \
X(bmsSystemId, BMSSystemStatusResponse, cdh_bms_system_status_callback) \
X(bmsSystemId, BMSPowerStatusResponse, cdh_bms_power_status_callback) \
X(bmsSystemId, BMSTemperatureStatusResponse, cdh_bms_temperature_status_callback) \
X(pdsSystemId, PDSSystemStatusResponse, cdh_pds_system_status_callback) \
X(pdsSystemId, PDSHealthCheckResponse, cdh_pds_health_check_callback) \
X(pdsSystemId, PDSConverterMonitorResponse, cdh_pds_converter_monitor_callback)

#define TRY_ASN1_MESSAGE(target, type, callback) \
if (ASN1_LFP_HANDLE_MSG(tmp, target, type, callback)) return true;

static bool decode_message(asn1_lfp_decode_data_t *data)
{
    asn1_lfp_decode_data_t tmp = *data;
    ASN1_RESPONSE_MESSAGES(TRY_ASN1_MESSAGE)

    return false;
}

static bool loopback_on_header(const lfp_header_t *header, void *user_data)
{
    user_ctx_t* ctx = user_data;
    ctx->p_header = (lfp_header_t*)header;
    LOG_DBG("Received header: length=%d initiator=%d target=%d endpoint=%d",
        header->encoded_length,
        header->initiator,
        header->target,
        header->endpoint
    );
    return true;
}

static void loopback_on_message(
    const lfp_header_t *header,
    const uint8_t *body,
    uint16_t body_length,
    void *user_data
)
{
    ARG_UNUSED(user_data);

    LOG_DBG(
        "Loopback telemetry: target=%d endpoint=%d",
        header->target,
        header->endpoint
    );

    asn1_lfp_decode_data_t data = {
        .p_on_error_cb = cdh_asn1_error_callback,
        .p_header = header,
        .p_body = body,
        .body_length = body_length
    };
    decode_message(&data);
}


static void loopback_on_error(lfp_code_t reason, void *user_data)
{
    user_ctx_t *ctx = user_data;
    LOG_WRN("Loopback LFP stream error detected: %d", reason);

    // Print the last 32 bytes received over UART
    uint8_t history[sizeof(ctx->rx_history)];
    for (size_t i = 0; i < ctx->rx_history_count; ++i) {
        const size_t index = (ctx->rx_history_next + sizeof(ctx->rx_history)
                              - ctx->rx_history_count + i)
                             % sizeof(ctx->rx_history);
        history[i] = ctx->rx_history[index];
    }
    LOG_HEXDUMP_WRN(history, ctx->rx_history_count,
                    "UART bytes preceding and including error:");

    // Print the last 128 bytes captured by the UART RX ISR for comparison
    uint8_t raw_history[128];
    const size_t raw_count = uart_transport_debug_snapshot(
        raw_history, sizeof(raw_history));
    LOG_HEXDUMP_WRN(raw_history, raw_count,
                    "Raw UART bytes captured by RX ISR:");

    const uint32_t dropped = uart_transport_debug_dropped();
    if (dropped != 0) {
        LOG_WRN("UART RX ring has dropped %u bytes", dropped);
    }
}

int main(void)
{
    uint8_t uart_rx_byte;
    static uint8_t lfp_body_buffer[256];
    lfp_stream_ctx_t lfp_stream;
    user_ctx_t usr_ctx = {0};

    int err = pins_init();
    if (err != 0) {
        LOG_ERR("Pin initialization failed");
        return err;
    }

    lfp_stream_init(
        &lfp_stream,
        lfp_body_buffer,
        sizeof(lfp_body_buffer),
        loopback_on_header,
        loopback_on_message,
        loopback_on_error,
        &usr_ctx
    );

    int ret = uart_transport_open(transmitter);
    if (ret != 0) {
        LOG_ERR("Failed to open UART2 RX: %d", ret);
        return ret;
    }

    LOG_INF("Starting BMS I2C to UART2 loopback test");

    while (1) {
        ret = uart_transport_read(
            &uart_rx_byte,
            sizeof(uart_rx_byte),
            K_FOREVER
        );
        if (ret < 0) {
            LOG_WRN("Failed to read from UART2: %d", ret);
            continue;
        }
        if (ret == 0) {
            continue;
        }

        usr_ctx.rx_history[usr_ctx.rx_history_next] = uart_rx_byte;
        usr_ctx.rx_history_next =
            (usr_ctx.rx_history_next + 1) % sizeof(usr_ctx.rx_history);
        if (usr_ctx.rx_history_count < sizeof(usr_ctx.rx_history)) {
            usr_ctx.rx_history_count++;
        }

        lfp_stream_update(&lfp_stream, uart_rx_byte);
    }
}
