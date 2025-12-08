#include "ulog/ulog.h"

#include <string.h>
#include "linker_symbols.h"
#include "rzcobs.h"

// Default to INFO level
void * ulog__level_max_ptr = &_eulog_level_info;

// Set the maximum log level to be actually logged
void ulog_set_level(const int level) {
    switch (level) {
        case ULOG_LEVEL_EMERGENCY:
            ulog__level_max_ptr = &_eulog_level_emergency;
            break;
        case ULOG_LEVEL_ALERT:
            ulog__level_max_ptr = &_eulog_level_alert;
            break;
        case ULOG_LEVEL_CRITICAL:
            ulog__level_max_ptr = &_eulog_level_critical;
            break;
        case ULOG_LEVEL_ERROR:
            ulog__level_max_ptr = &_eulog_level_error;
            break;
        case ULOG_LEVEL_WARNING:
            ulog__level_max_ptr = &_eulog_level_warning;
            break;
        case ULOG_LEVEL_NOTICE:
            ulog__level_max_ptr = &_eulog_level_notice;
            break;
        case ULOG_LEVEL_INFO:
            ulog__level_max_ptr = &_eulog_level_info;
            break;
        case ULOG_LEVEL_DEBUG:
            ulog__level_max_ptr = &_eulog_level_debug;
            break;
        case ULOG_LEVEL_TRACE:
            ulog__level_max_ptr = &_eulog_level_trace;
            break;
        default:
            break;
    }
}
// Get the maximum log level that will be logged
int ulog_get_level(void) {
    if (ulog__level_max_ptr == &_sulog_level_emergency)
        return ULOG_LEVEL_EMERGENCY;
    if (ulog__level_max_ptr == &_sulog_level_alert)
        return ULOG_LEVEL_ALERT;
    if (ulog__level_max_ptr == &_sulog_level_critical)
        return ULOG_LEVEL_CRITICAL;
    if (ulog__level_max_ptr == &_sulog_level_error)
        return ULOG_LEVEL_ERROR;
    if (ulog__level_max_ptr == &_sulog_level_warning)
        return ULOG_LEVEL_WARNING;
    if (ulog__level_max_ptr == &_sulog_level_notice)
        return ULOG_LEVEL_NOTICE;
    if (ulog__level_max_ptr == &_sulog_level_info)
        return ULOG_LEVEL_INFO;
    if (ulog__level_max_ptr == &_sulog_level_debug)
        return ULOG_LEVEL_DEBUG;
    if (ulog__level_max_ptr == &_sulog_level_trace)
        return ULOG_LEVEL_TRACE;
    return -1;
}

// Internal function to write a string pointer
void ulog__write_string(const char * string) {
    // Write out the string plus the null terminator
    ulog__write(string, strlen(string) + 1);
}

// Initialize state object for rzcobs
static struct ulog_rzcobs_state rzcobs_state = {
    .run = 0,
    .zeros = 0,
    .write = ulog_external_write
};

// Internal function to write data
void ulog__write(const void * data, const int size) {
    // Go byte by byte and rzcobs encode the whole thing
    const uint8_t * castData = data;
    for (int i = 0; i < size; i++)
        ulog_rzcobs_encoder_write(&rzcobs_state, castData[i]);
}

// Internal function to write data in a BE format
void ulog__write_htobe(const void * data, const int size) {
    const uint8_t * castData = data;

    // If the host is LE, write out bytes in reverse, otherwise just do the same thing as ulog__write()
    for (int i = 0; i < size; i++) {
#if BYTE_ORDER == LITTLE_ENDIAN
        ulog_rzcobs_encoder_write(&rzcobs_state, castData[size - i - 1]);
#else
        ulog_rzcobs_encoder_write(&rzcobs_state, castData[i]);
#endif
    }
}

// Internal function called once at the end of every log
void ulog__flush(void) {
    // End the rzcobs frame to allow self synchronisation in the event of a decode error
    ulog_rzcobs_encoder_end(&rzcobs_state);
    // We only flush at the end of a message, but users shouldn't assume this to be the case
    ulog_external_flush();
}

