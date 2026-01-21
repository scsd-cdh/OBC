#include "logging_adapter.h"

#include <ulog/ulog.h>
#include <zephyr/logging/log_backend.h>
#include <zephyr/logging/log_backend_std.h>
#include <zephyr/logging/log_core.h>

#include "logging.h"

// Buffer to hold the output strings
static uint8_t logging_adapter_buffer[512] = {0};

// ReSharper disable CppParameterMayBeConstPtrOrRef
static int char_out(uint8_t *p_data, const size_t length, void *p_ctx) // NOLINT(*-non-const-parameter)
// ReSharper restore CppParameterMayBeConstPtrOrRef
{
    ARG_UNUSED(p_ctx);

    // Strip trailing newlines
    if (p_data[length - 1] == '\n') {
        p_data[length - 1] = '\0';
    }
    // Add null terminator at the end
    p_data[length] = '\0';

    // Print via ulog depending on the level
    const uint32_t level = *(uint32_t *)p_ctx;
    switch (level) {
        case LOG_LEVEL_ERR:
            ULOG_ERR("{}", (char *)p_data);
            break;
        case LOG_LEVEL_WRN:
            ULOG_WARN("{}", (char *)p_data);
            break;
        case LOG_LEVEL_DBG:
            ULOG_DEBUG("{}", (char *)p_data);
            break;
        case LOG_LEVEL_INF:
        default:
            ULOG_INFO("{}", (char *)p_data);
    }

    return min(length, sizeof(logging_adapter_buffer) - 1);
}
LOG_OUTPUT_DEFINE(log_output, char_out, logging_adapter_buffer, sizeof(logging_adapter_buffer) - 1);


static void logging_adapter_panic(const struct log_backend *const p_backend) {
    ARG_UNUSED(p_backend);

    // We reset the logger in case we were in the middle of sending a frame. This will corrupt that frame but it should
    // let us get crashlogs out
    logging_force_reset();
}


static void logging_adapter_process(const struct log_backend *const p_backend, union log_msg_generic *p_msg) {
    ARG_UNUSED(p_backend);

    // Save the log level to the context
    uint32_t level = log_msg_get_level(&p_msg->log);
    log_output.control_block->ctx = &level;

    const uint32_t flags = log_backend_std_get_flags();
    log_output_msg_process(&log_output, &p_msg->log, flags);
}

static void logging_adapter_dropped(const struct log_backend *const p_backend, const uint32_t cnt) {
    ARG_UNUSED(p_backend);

    log_backend_std_dropped(&log_output, cnt);
}

const static struct log_backend_api log_backend_ulog_api = {
    .process = logging_adapter_process,
    .panic = logging_adapter_panic,
    .dropped = logging_adapter_dropped,
};

LOG_BACKEND_DEFINE(log_backend_ulog, log_backend_ulog_api, true);
