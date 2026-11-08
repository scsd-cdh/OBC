#pragma once

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

struct cdh_telemetry_message {
    size_t length;
    uint8_t data[TELEMETRY_BUFFER_SIZE];
};

int cdh_telemetry_enqueue(const uint8_t *data, size_t length);
