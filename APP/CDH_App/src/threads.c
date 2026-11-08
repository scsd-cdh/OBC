#include "threads.h"
#include "cdh_bms.h"
#include "cdh_pds.h"
#include "cdh_comms.h"
#include "zephyr/logging/log.h"
#include <string.h>

LOG_MODULE_REGISTER(cdh_threads);

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
    const int ret = k_msgq_put(&telemetry_queue, &message, K_NO_WAIT);
    if (ret != 0) {
        LOG_WRN("Telemetry queue full, dropping message: %d", ret);
    }
    return ret;
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

static void critical_thread(const void *p_arg)
{
    ARG_UNUSED(p_arg);

    while (1) {
        LOG_DBG("Critical thread waking up, requesting BMS and PDS critical data");
        request_bms_power_status();
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
