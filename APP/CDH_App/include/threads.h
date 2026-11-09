#pragma once

#include <asn1/bms.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <zephyr/kernel.h>

// Lower numeric values have higher priority.
#define THREAD_PRIORITY_CRITICAL 2
#define THREAD_PRIORITY_TELEMETRY 3
#define THREAD_PRIORITY_COMMAND 1

#define THREAD_STACK_SIZE 2024
#define CRITICAL_PERIOD_MS 2000
#define TELEMETRY_PERIOD_MS 5000
#define TELEMETRY_QUEUE_DEPTH 16
#define TELEMETRY_BUFFER_SIZE 256

#define BMS_ADC_MAX     4095U
#define BMS_VREF_MV     3300U

#define BMS_SENSE_GAIN_NUM 3U   // 0.75 V/V
#define BMS_SENSE_GAIN_DEN 4U
#define MV_TO_ADC(mv)  ((uint32_t)(((uint32_t)(mv)  * BMS_SENSE_GAIN_NUM * 4096U) / (BMS_SENSE_GAIN_DEN * BMS_VREF_MV)))
#define ADC_TO_MV(adc) ((uint32_t)(((uint32_t)(adc) * BMS_VREF_MV * BMS_SENSE_GAIN_DEN) / (4096U * BMS_SENSE_GAIN_NUM)))

struct cdh_telemetry_message {
    size_t length;
    uint8_t data[TELEMETRY_BUFFER_SIZE];
};

int cdh_telemetry_enqueue(const uint8_t *data, size_t length);
void cdh_critical_evaluate_power_status(
    const BMSPowerStatusResponse *status
);
bool cdh_critical_is_safe(void);

// gives semaphore allowing command thread to start. We need to open uart before the thread is allowed to do anything. 
void cdh_command_start(void);
