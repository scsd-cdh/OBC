#include "threads.h"
#include "cdh_bms.h"
#include "cdh_pds.h"
#include "cdh_comms.h"
#include "zephyr/logging/log.h"
#include "uart/uart_transport.h"
#include <string.h>

#include "lfp/stream.h"

LOG_MODULE_REGISTER(cdh_threads);

/* Given by cdh_command_start() once UART2 is open. */
K_SEM_DEFINE(command_start_sem, 0, 1);

#define COMMAND_BODY_BUFFER_SIZE 256
#define COMMAND_RX_BACKOFF_MS    10

#define BMS_ADC_MAX     4095U
#define BMS_VREF_MV     3300U

#define BMS_SENSE_GAIN_NUM 3U   // 0.75 V/V
#define BMS_SENSE_GAIN_DEN 4U
#define MV_TO_ADC(mv)  ((uint32_t)(((uint32_t)(mv)  * BMS_SENSE_GAIN_NUM * 4096U) / (BMS_SENSE_GAIN_DEN * BMS_VREF_MV)))
#define ADC_TO_MV(adc) ((uint32_t)(((uint32_t)(adc) * BMS_VREF_MV * BMS_SENSE_GAIN_DEN) / (4096U * BMS_SENSE_GAIN_NUM)))

#define BMS_SAFE_ENTRY_ADC MV_TO_ADC(3376U) // 30% battery voltage from the curve.
#define BMS_SAFE_EXIT_ADC  MV_TO_ADC(3400U) // 40% battery voltage from the curve.

enum cdh_mode {
    CDH_MODE_STANDBY,
    CDH_MODE_SAFE,
};

static enum cdh_mode cdh_mode = CDH_MODE_STANDBY;

static void cdh_set_mode(enum cdh_mode next_mode)
{
    if (cdh_mode == next_mode) {
        return;
    }

    LOG_INF("CDH mode changing from %s to %s",
        cdh_mode == CDH_MODE_SAFE ? "SAFE" : "STANDBY",
        next_mode == CDH_MODE_SAFE ? "SAFE" : "STANDBY");
    cdh_mode = next_mode;

    /* TODO: Communicate mode changes to subsystems */
}

static inline double bms_adc_to_float(const uint16_t val) {
    return ((double)val * 3.3) / (1 << 12);
}

void cdh_critical_evaluate_power_status(
    const BMSPowerStatusResponse *status
)
{
    // FIXME: we know for sure uv/oc trigger safe mode, not sure what to do about ov
    const bool protection_fault =
        status->batteryPack1.overcurrent || status->batteryPack2.overcurrent ||
        status->batteryPack1.cellA.undervoltage ||
        status->batteryPack1.cellB.undervoltage ||
        status->batteryPack2.cellA.undervoltage ||
        status->batteryPack2.cellB.undervoltage;

    const bool low_battery =
        status->batteryPack1.voltage < BMS_SAFE_ENTRY_ADC ||
        status->batteryPack2.voltage < BMS_SAFE_ENTRY_ADC;
    const bool recovered =
        status->batteryPack1.voltage > BMS_SAFE_EXIT_ADC &&
        status->batteryPack2.voltage > BMS_SAFE_EXIT_ADC;

    LOG_DBG("Pack1: raw=%u (%u mV) | Pack2: raw=%u (%u mV) | entry=%u (%u mV) exit=%u (%u mV) | low=%d recovered=%d",
          status->batteryPack1.voltage, ADC_TO_MV(status->batteryPack1.voltage),
          status->batteryPack2.voltage, ADC_TO_MV(status->batteryPack2.voltage),
          BMS_SAFE_ENTRY_ADC, ADC_TO_MV(BMS_SAFE_ENTRY_ADC),
          BMS_SAFE_EXIT_ADC,  ADC_TO_MV(BMS_SAFE_EXIT_ADC),
          low_battery, recovered);
    if (cdh_mode == CDH_MODE_STANDBY && (protection_fault || low_battery)) {
        cdh_set_mode(CDH_MODE_SAFE);
    } else if (cdh_mode == CDH_MODE_SAFE && !protection_fault && recovered) {
        cdh_set_mode(CDH_MODE_STANDBY);
    }
}

bool cdh_critical_is_safe(void)
{
    return cdh_mode == CDH_MODE_SAFE;
}

K_MSGQ_DEFINE(telemetry_queue, sizeof(struct cdh_telemetry_message),
              TELEMETRY_QUEUE_DEPTH, sizeof(size_t));

int cdh_telemetry_enqueue(const uint8_t *data, size_t length)
{
    if (data == NULL || length == 0 || length > TELEMETRY_BUFFER_SIZE) {
        return -EINVAL;
    }

    struct cdh_telemetry_message message = {
        .length = length,
    };
    memcpy(message.data, data, length);

    // This is thread safe
    return k_msgq_put(&telemetry_queue, &message, K_NO_WAIT);
}

static void transmit_queued_telemetry(void)
{
    struct cdh_telemetry_message message;

    while (k_msgq_get(&telemetry_queue, &message, K_NO_WAIT) == 0) {
        const int ret = uart_transport_write(transmitter, message.data,
                                             message.length);
        if (ret < 0) {
            LOG_WRN("Failed to transmit telemetry: %d", ret);
        }
    }
}

static void discard_queued_telemetry(void)
{
    k_msgq_purge(&telemetry_queue);
}

static bool command_on_header(const lfp_header_t *header, void *user_data)
{
    ARG_UNUSED(user_data);

    LOG_DBG("Command header: length=%d initiator=%d target=%d endpoint=%d",
            header->encoded_length,
            header->initiator,
            header->target,
            header->endpoint);

    /* TODO: reject frames not addressed to the CDH (check lfp/stream.h for
     * what returning false does to the parser before relying on it). */
    return true;
}

static void command_on_message(
    const lfp_header_t *header,
    const uint8_t *body,
    uint16_t body_length,
    void *user_data
)
{
    ARG_UNUSED(user_data);

    LOG_INF("Command received: initiator=%d target=%d endpoint=%d body_length=%u",
            header->initiator,
            header->target,
            header->endpoint,
            body_length);
    LOG_HEXDUMP_INF(body, body_length, "Command body:");
}

static void command_on_error(lfp_code_t reason, void *user_data)
{
    ARG_UNUSED(user_data);

    LOG_WRN("Command LFP stream error: %d", reason);

    const uint32_t dropped = uart_transport_debug_dropped();
    if (dropped != 0) {
        LOG_WRN("UART RX ring has dropped %u bytes", dropped);
    }
}


static void critical_thread(const void *p_arg)
{
    ARG_UNUSED(p_arg);

    while (1) {
        LOG_DBG("Critical thread waking up, requesting BMS power status");
        request_bms_power_status();
        if (cdh_critical_is_safe()) {
            LOG_DBG("CDH is in SAFE mode; skipping non-BMS critical telemetry");
            k_sleep(K_MSEC(CRITICAL_PERIOD_MS));
            continue;
        }
        request_bms_temperature_status();
        request_pds_converter_monitor();
        request_pds_health_check();
        LOG_DBG("Critical thread sleeping for %d seconds", CRITICAL_PERIOD_MS / 1000);
        k_sleep(K_MSEC(CRITICAL_PERIOD_MS));
    }
}

static void telemetry_thread(const void *p_arg)
{
    ARG_UNUSED(p_arg);

    while (1) {
        // Pretty sure we don't log in safe mode
        if (cdh_critical_is_safe()) {
            LOG_DBG("CDH is in SAFE mode; skipping regular telemetry");
            discard_queued_telemetry();
            k_sleep(K_MSEC(TELEMETRY_PERIOD_MS));
            continue;
        }

        LOG_DBG("Telemetry thread waking up, transmitting queued telemetry and requesting BMS and PDS system status");
        transmit_queued_telemetry();
        request_bms_system_status();
        request_pds_system_status();
        transmit_queued_telemetry();
        LOG_DBG("Telemetry thread sleeping for %d seconds", TELEMETRY_PERIOD_MS / 1000);
        k_sleep(K_MSEC(TELEMETRY_PERIOD_MS));
    }
}

static void command_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    static uint8_t body_buffer[COMMAND_BODY_BUFFER_SIZE];
    static lfp_stream_ctx_t stream;

    lfp_stream_init(
        &stream,
        body_buffer,
        sizeof(body_buffer),
        command_on_header,
        command_on_message,
        command_on_error,
        NULL
    );

    // Only start listening on uart once main has allowed us to. 
    k_sem_take(&command_start_sem, K_FOREVER);
    LOG_INF("Command thread started, listening for commands on UART2");

    while (1) {
        uint8_t byte;
        const int ret = uart_transport_read(&byte, sizeof(byte), K_FOREVER);

        if (ret <= 0) {
            if (ret < 0) {
                LOG_WRN("Failed to read from UART2: %d", ret);
            }
            /* Highest priority thread: never spin, or the critical and
             * telemetry threads starve. */
            k_msleep(COMMAND_RX_BACKOFF_MS);
            continue;
        }

        lfp_stream_update(&stream, byte);
    }
}


K_THREAD_DEFINE(p_command_thread, THREAD_STACK_SIZE, command_thread,
        NULL, NULL, NULL, THREAD_PRIORITY_COMMAND, 0, 0);

void cdh_command_start(void)
{
    k_sem_give(&command_start_sem);
}

K_THREAD_DEFINE(p_critical_thread, THREAD_STACK_SIZE, critical_thread, NULL, NULL, NULL,
        THREAD_PRIORITY_CRITICAL, 0, 0);

K_THREAD_DEFINE(p_telemetry_thread, THREAD_STACK_SIZE, telemetry_thread, NULL, NULL, NULL,
        THREAD_PRIORITY_TELEMETRY, 0, 2000);
