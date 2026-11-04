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

#define LOG_MESSAGE_SIZE 256
#define LOG_ENCODED_SIZE ASN1_LFP_SEND_BUF_SIZE(CommsTelemetryTransmission)

void cdh_comms_local_loopback_test(const struct device* lp_uart);
void cdh_comms_d2d_loopback_test(const struct device *lp_txuart, const struct device *lp_rxuart);
void cdh_comms_lfp_uart_lp(
    const struct device *lp_txuart,
    const struct device *lp_rxuart,
    const uint8_t *txdata,
    size_t txlen,
    uint8_t *rxdata,
    size_t rxlen
);

int cdh_comms_encode_lfp_msg(
    const asn1_lfp_decode_data_t *data,
    uint8_t *send_buf,
    size_t send_buf_size
);

void cdh_comms_send_msg(
    const struct device *tx_dev,
    const uint8_t *txdata,
    size_t txlen
);

int cdh_comms_uart_open(const struct device *rx_dev);

int cdh_comms_uart_read(
    uint8_t *rxdata,
    size_t rxlen,
    k_timeout_t timeout
);
