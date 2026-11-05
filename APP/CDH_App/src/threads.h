#pragma once

#include <zephyr/kernel.h>

// Lower numeric values have higher priority.
#define THREAD_PRIORITY_BMS 1
#define THREAD_PRIORITY_PDS 1
#define THREAD_PRIORITY_COMMS_RX 1

extern struct k_mutex uart_rx_lock;
