#include "lfp_i2c_single.h"

#include <asn1_lfp.h>
#include <lfp.h>
#include <lfp/stream.h>
#include <ulog/ulog.h>
#include <zephyr/drivers/i2c.h>

#define I2C_BUF_SIZE 32
#define I2C_PREREAD_SIZE LFP_HEADER_SIZE

// TODO: remove
const static uint8_t fake_data[20] = {
    0xFF, 0x12, 0x01, 0x00, 0x0B, 0x00, 0x00, 0x60, 0x74, 0x05, 0x0C, 0xDE, 0xAD, 0xBE, 0xEF, 0x15,
    0x30, 0x7F, 0x0E, 0x07,
};
static int fake_data_index = 0;
void reset_fake_data(void) {
    fake_data_index = 0;
}
static int read_fake_data(const struct i2c_dt_spec *spec,
                  uint8_t *buf, const uint32_t num_bytes) {
    ARG_UNUSED(spec);

    for (int i = 0; i < num_bytes; ++i) {
        if (fake_data_index >= sizeof(fake_data)) {
            buf[i] = LFP_PREAMBLE;
        } else {
            buf[i] = fake_data[fake_data_index++];
        }
    }
    return 0;
}


typedef struct {
    lfp_header_t * p_out_header;
    lfp_size_or_fail_t body_length;
} lfp_i2c_receive_ctx_t;

static void on_message(const lfp_header_t * p_header, const uint8_t * p_body, const uint16_t body_length, void * _p_ctx) {
    ARG_UNUSED(p_header);
    ARG_UNUSED(p_body);
    ARG_UNUSED(body_length);
    lfp_i2c_receive_ctx_t * p_ctx = _p_ctx;

    p_ctx->body_length = body_length;
    *p_ctx->p_out_header = *p_header;
}

static void on_error(const lfp_code_t error, void * p_ctx) {
    ARG_UNUSED(p_ctx);

    ULOG_WARN("I2C LFP stream error detected: {}", error);
}

lfp_size_or_fail_t lfp_i2c_single_receive(
    const struct i2c_dt_spec * p_device,
    lfp_header_t * p_out_header,
    uint8_t * p_out_lfp_buf,
    const int out_lfp_buf_size,
    const k_timeout_t max_duration
) {
    struct k_timer timeout;
    k_timer_init(&timeout, NULL, NULL);
    k_timer_start(&timeout, max_duration, K_NO_WAIT);

    lfp_i2c_receive_ctx_t ctx = {
        .p_out_header = p_out_header,
        .body_length = LFP_FAIL
    };

    lfp_stream_ctx_t stream;
    lfp_stream_init(&stream, p_out_lfp_buf, out_lfp_buf_size, NULL, on_message, on_error, &ctx);

    uint8_t buf[I2C_BUF_SIZE];

    int next_chunk_size = 0;
    while (true) {
        // We spent too much time trying to read the body, give up
        if (k_timer_status_get(&timeout)) {
            ULOG_WARN("I2C read timeout!");
            goto fail;
        }

        // Clamp read size
        next_chunk_size = CLAMP(next_chunk_size, 0, I2C_BUF_SIZE);

        // If the chunk size is 0, we should be trying to preread
        if (next_chunk_size == 0) {
            next_chunk_size = I2C_PREREAD_SIZE;
        }

        // Read bytes from the target
        // TODO: restore
        // const int read_ret = read_fake_data(p_device, buf, next_chunk_size);
        const int read_ret = i2c_read_dt(p_device, buf, next_chunk_size);

        // Couldn't do that
        if (read_ret != 0) {
            continue;
        }

        ULOG_INFO("Received {}", ((struct ulog_slice) {.data = buf, .size = next_chunk_size}));

        // Update the stream state and record the next chunk size
        next_chunk_size = lfp_stream_update_buf(&stream, buf, next_chunk_size);

        // Check if we are done, if so, then return
        if (ctx.body_length != LFP_FAIL) {
            goto exit;
        }
    }

fail:
    ctx.body_length = LFP_FAIL;
exit:
    k_timer_stop(&timeout);
    return ctx.body_length;
}
