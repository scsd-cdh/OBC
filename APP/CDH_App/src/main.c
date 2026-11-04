#include "main.h"

#include "cdh_bms.h"
#include "cdh_comms.h"
#include "lfp/stream.h"
#include "zephyr/kernel.h"
#include "zephyr/logging/log.h"

LOG_MODULE_REGISTER(cdh);

static bool loopback_on_header(const lfp_header_t *header, void *user_data)
{
    ARG_UNUSED(user_data);

    LOG_DBG(
        "Loopback LFP header: length=%d initiator=%d target=%d endpoint=%d direction=%d",
        header->encoded_length,
        header->initiator,
        header->target,
        header->endpoint,
        header->direction
    );
    return true;
}

static void loopback_on_message(
    const lfp_header_t *header,
    const uint8_t *body,
    uint16_t body_length,
    void *user_data
)
{
    ARG_UNUSED(user_data);

    char message[256];
    size_t message_length = MIN(body_length, sizeof(message) - 1);
    memcpy(message, body, message_length);
    message[message_length] = '\0';

    LOG_INF(
        "Loopback telemetry: target=%d endpoint=%d body=%s",
        header->target,
        header->endpoint,
        message
    );
}

static void loopback_on_error(lfp_code_t reason, void *user_data)
{
    ARG_UNUSED(user_data);
    LOG_WRN("Loopback LFP decode failed: %d", reason);
}

int main(void)
{
    static uint8_t uart_rx_buffer[256];
    static uint8_t lfp_body_buffer[256];
    lfp_stream_ctx_t lfp_stream;

    lfp_stream_init(
        &lfp_stream,
        lfp_body_buffer,
        sizeof(lfp_body_buffer),
        loopback_on_header,
        loopback_on_message,
        loopback_on_error,
        NULL
    );

    int ret = cdh_comms_uart_open(transmitter);
    if (ret != 0) {
        LOG_ERR("Failed to open UART2 RX: %d", ret);
        return ret;
    }

    LOG_INF("Starting BMS I2C to UART2 loopback test");

    while (1) {
        get_bms_power_status();

        ret = cdh_comms_uart_read(
            uart_rx_buffer,
            sizeof(uart_rx_buffer),
            K_MSEC(250)
        );
        while (ret > 0) {
            lfp_stream_update_buf(&lfp_stream, uart_rx_buffer, ret);

            ret = cdh_comms_uart_read(
                uart_rx_buffer,
                sizeof(uart_rx_buffer),
                K_NO_WAIT
            );
        }
        if (ret < 0) {
            LOG_WRN("Failed to read from UART2: %d", ret);
        }

        k_msleep(1000);
    }
}
