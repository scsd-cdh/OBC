#pragma once

#include <asn1_lfp.h>
#include <stddef.h>
#include <stdint.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>

#if DT_NODE_HAS_STATUS(DT_ALIAS(loopback_uart), okay)
static const struct device *uart = DEVICE_DT_GET(DT_ALIAS(loopback_uart));
#else
#error "Unsupported board: loopback-uart devicetree alias is not defined"
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(transmitter), okay)
static const struct device *transmitter = DEVICE_DT_GET(DT_ALIAS(transmitter));
#else
#error "Unsupported board: transmitter devicetree alias is not defined"
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(receiver), okay)
static const struct device *receiver = DEVICE_DT_GET(DT_ALIAS(receiver));
#else
#error "Unsupported board: receiver devicetree alias is not defined"
#endif
#if DT_NODE_HAS_STATUS(DT_ALIAS(test), okay)
static const struct device *uart3 = DEVICE_DT_GET(DT_ALIAS(test));
#else
#error "Unsupported board: uart3 devicetree alias is not defined"
#endif


int cdh_comms_fwd_lfp_msg(
    const asn1_lfp_decode_data_t *data,
    uint8_t *send_buf,
    size_t send_buf_size
);