#include "cdh_comms.h"
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>

#include "lfp/stream.h"

LOG_MODULE_REGISTER(cdh_comms);

#define NOBAD_BYTES(byte) ((byte) >= 0x20 && (byte) <= 0x7e) ? (byte) : '.'

#define UART2_TX 0 // used for loopback transmitter
#define UART2_RX 1 // used for loopback receiver
#define DUMMY_ENDPOINT 2

void cdh_comms_local_loopback_test(const struct device *lp_uart)
{
    cdh_comms_d2d_loopback_test(lp_uart, lp_uart);
}

void cdh_comms_d2d_loopback_test(const struct device *lp_txuart, const struct device* lp_rxuart)
{
    static const uint8_t test_data[] = "D2D loopback\r\n";
    static bool ready_logged;

    if (!device_is_ready(lp_txuart)) {
        LOG_ERR("D2D loopback TX device %s not ready", lp_txuart->name);
        return;
    }

    if (!device_is_ready(lp_rxuart)) {
        LOG_ERR("D2D loopback RX device %s not ready", lp_rxuart->name);
        return;
    }

    if (!ready_logged) {
        LOG_INF("D2D loopback is ready");
        ready_logged = true;
    }
    unsigned char byte;
    while (uart_poll_in(lp_rxuart, &byte) == 0) {}

    for (size_t i = 0; i < sizeof(test_data) - 1; i++) {
        uart_poll_out(lp_txuart, test_data[i]);

        int got = -1;
        for (int retry = 0; retry < 1000; retry++)
        {
            if (uart_poll_in(lp_rxuart, &byte) == 0) {
                got = byte;
                break;
            }
            k_busy_wait(100);
        }
        if (got != test_data[i]) {
            LOG_INF("%s -> %s: D2D loopback, Got the wrong data: %c, expected: %c", lp_txuart->name, lp_rxuart->name, NOBAD_BYTES(got), NOBAD_BYTES(test_data[i]));
            continue;
        }
        LOG_INF("%s -> %s: D2D loopback received: 0x%02x ('%c')",
            lp_txuart->name, lp_rxuart->name, got, NOBAD_BYTES(got));
    }
}

static bool on_header(const lfp_header_t * p_header, void * p_ctx)
{
    ARG_UNUSED(p_ctx);
    LOG_INF("{ length: %d, initiater: %d, target: %d, endpoint: %d }",
        p_header->encoded_length,
        p_header->initiator,
        p_header->target,
        p_header->endpoint);
    return true;
}

static bool on_message(const lfp_header_t * p_header,
    const uint8_t * p_body,
    uint16_t body_length,
    void * p_ctx
)
{
    ARG_UNUSED(body_length);
    ARG_UNUSED(p_ctx);
    LOG_INF("{ length: %d, initiater: %d, target: %d, endpoint: %d } { body: %s }",
        p_header->encoded_length,
        p_header->initiator,
        p_header->target,
        p_header->endpoint,
        p_body);
    return true;
}


void cdh_comms_lfp_uart_lp(
    const struct device *lp_txuart,
    const struct device *lp_rxuart,
    const uint8_t *txdata,
    size_t txlen,
    uint8_t *rxdata,
    size_t rxlen
)
{
    if (!device_is_ready(lp_txuart)) {
        LOG_ERR("LFP UART device %s not ready", lp_txuart->name);
        return;
    }

    if (!device_is_ready(lp_rxuart)) {
        LOG_ERR("LFP UART device %s not ready", lp_rxuart->name);
        return;
    }

    static uint8_t lfp_encoded_tx_buffer[256]; // Extra space for LFP overhead
    int encode_result = lfp_encode(UART2_TX,
        UART2_RX,
        DUMMY_ENDPOINT,
        LFP_DIRECTION_TARGET,
        txdata,
        txlen,
        lfp_encoded_tx_buffer,
        sizeof(lfp_encoded_tx_buffer));
    if (encode_result < 0) {
        LOG_ERR("Failed to encode LFP message: %d", encode_result);
        if (encode_result == LFP_EOVERFLOW) {
            LOG_ERR("LFP message is too large to fit in the buffer %d > %d", txlen, sizeof(lfp_encoded_tx_buffer));
        }
        return;
    }

    lfp_stream_ctx_t input_stream;
    lfp_stream_init(&input_stream, rxdata, rxlen, on_header, on_message, NULL, NULL);
    for (size_t i = 0; i < encode_result; i++) {
        uart_poll_out(lp_txuart, lfp_encoded_tx_buffer[i]);
        uint8_t byte;
        int got = -1;
        for (int retry = 0; retry < 1000; retry++)
        {
            if (uart_poll_in(lp_rxuart, &byte) == 0) {
                got = byte;
                break;
            }
            k_busy_wait(100);

        }
        if (got != lfp_encoded_tx_buffer[i]) {
            LOG_INF("LFP UART loopback, Got the wrong data: %c, expected: %c", NOBAD_BYTES(got), NOBAD_BYTES(lfp_encoded_tx_buffer[i]));
            return;
        }
        lfp_stream_update(&input_stream, got);
    }
}