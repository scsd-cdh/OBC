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

/**
 *  telemetry message stored in the shared queue between the telemetry and critical threads.
 */
struct cdh_telemetry_message {
    size_t length;
    uint8_t data[TELEMETRY_BUFFER_SIZE];
};

/**
 * Put data in the telemetry queue for the telemetry thread to send over UART.
 *
 * @param data The data to put in the telemetry queue.
 * @param length The length of the data.
 * @returns 0 on success, or a negative error code.
 */
int cdh_telemetry_enqueue(const uint8_t *data, size_t length);

/**
 * Decide whether to go into safe mode based on the BMS power status.
 *
 * @param status The BMS power status to evaluate.
 */
void cdh_critical_evaluate_power_status(
    const BMSPowerStatusResponse *status
);

/**
 * are we in safe mode?
 *
 * @returns true if the CDH is in safe mode, false otherwise
 */
bool cdh_critical_is_safe(void);

/**
 * Gives the command thread semaphore, allowing it to start listening for commands on UART2.
 * Should be called after the UART2 device is initialized and ready to use.
 *
 */
void cdh_command_start(void);
