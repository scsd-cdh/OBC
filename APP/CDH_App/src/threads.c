#include "threads.h"
#include "cdh_bms.h"
#include "cdh_pds.h"
#include "cdh_comms.h"
#include "zephyr/logging/log.h"
#include "uart/uart_transport.h"
#include <string.h>

LOG_MODULE_REGISTER(cdh_threads);

// FIXME: Currently going into safe mode at 30% battery voltage max ADC value, which I have no idea is correct or not.
// Complete placeholder, we will revisit and tweak accordingly
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
    // FIXME: Not sure if we actually want to go into safe mode on any flag being set or not. Again revisit
    const bool protection_fault = false;
    // const bool protection_fault =
    //     status->batteryPack1.overcurrent || status->batteryPack2.overcurrent ||
    //     status->batteryPack1.cellA.undervoltage ||
    //     status->batteryPack1.cellB.undervoltage ||
    //     status->batteryPack2.cellA.undervoltage ||
    //     status->batteryPack2.cellB.undervoltage ||
    //     status->batteryPack1.cellA.overvoltage ||
    //     status->batteryPack1.cellB.overvoltage ||
    //     status->batteryPack2.cellA.overvoltage ||
    //     status->batteryPack2.cellB.overvoltage;


    // Feels safer to go into safe mode if any cell is below the safe entry threshold, and only exit safe mode if all cells are above the safe exit threshold
    // Not sure what is correct
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

K_THREAD_DEFINE(p_critical_thread, THREAD_STACK_SIZE, critical_thread, NULL, NULL, NULL,
        THREAD_PRIORITY_CRITICAL, 0, 0);

K_THREAD_DEFINE(p_telemetry_thread, THREAD_STACK_SIZE, telemetry_thread, NULL, NULL, NULL,
        THREAD_PRIORITY_TELEMETRY, 0, 2000);
