#pragma once

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <ulog/macros.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

// Internal ELF input section name for level 0 - emergency
#define ULOG__SECTION_EMERGENCY ".ulog.level.emergency"
// Internal ELF input section name for level 1 - alert
#define ULOG__SECTION_ALERT ".ulog.level.alert"
// Internal ELF input section name for level 2 - critical
#define ULOG__SECTION_CRITICAL ".ulog.level.critical"
// Internal ELF input section name for level 3 - error
#define ULOG__SECTION_ERROR ".ulog.level.error"
// Internal ELF input section name for level 4 - warning
#define ULOG__SECTION_WARNING ".ulog.level.warning"
// Internal ELF input section name for level 5 - notice
#define ULOG__SECTION_NOTICE ".ulog.level.notice"
// Internal ELF input section name for level 6 - info
#define ULOG__SECTION_INFO ".ulog.level.info"
// Internal ELF input section name for level 7 - debug
#define ULOG__SECTION_DEBUG ".ulog.level.debug"
// Internal ELF input section name for level 8 - trace
#define ULOG__SECTION_TRACE ".ulog.level.trace"
// Internal ELF input section name for user strings
#define ULOG__SECTION_STRING ".ulog.string"
// Internal ELF input section name for log arguments
#define ULOG__SECTION_ARGUMENT ".ulog.argument"
// Internal ELF input section name for meta information such as the system id
#define ULOG__SECTION_META ".ulog.meta"

// Marker type pointing to a constant string
struct ulog_string {
    uint16_t string;
};
// We assume the type is transparent so any padding will be very evil
static_assert(sizeof(struct ulog_string) == sizeof(uint16_t));

// Define a constant string
#define ULOG_STRING(value) __extension__ ({ \
    extern uint8_t _sulog_string; \
    /* Casting to uint16 is safe because we assert in the linker script that we don't have more than ~16k strings */\
    /* and the VMA starts at 0 so this will be <=~16k */\
    ULOG__EXPORT_MARKER(_ulog__string_definition, ULOG__SECTION_STRING, ULOG__BUILD_UNIQUE_SYMBOL_NAME(value));\
    uint16_t _ulog__string_definition_id = (uint16_t)(&_ulog__string_definition - &_sulog_string); \
    (struct ulog_string) { \
        .string = _ulog__string_definition_id \
    }; \
})
#define ULOG_STR(...) ULOG_STRING(__VA_ARGS__)


// Fat pointer to arbitrary data. At least size
struct ulog_slice {
    // Size of the data
    uint32_t size;
    // Pointer to the data itself
    void *data;
};

// Create a ulog_slice pointing to data
#define ULOG_SLICE(X) ((struct ulog_slice) { \
    .size = sizeof(X), \
    .data = &X \
})
// Convert a pointer to a ulog_slice
#define ULOG_SLICE_PTR(X) ((struct ulog_slice) { \
    .size = sizeof(*X), \
    .data = X \
})

// Internal macro to get the internal type ID of a variable
#define ULOG__TYPE_OF(X) _Generic(X, \
    struct ulog_slice: 1, \
    struct ulog_slice *: 1, \
    float: 2, \
    double: 3, \
    const char *: 4, \
    char *: 4, \
    _Bool: 5,\
    struct ulog_string: 6,\
    /* Uses basic integers instead of sized integers as sized integers are often typedefs, and we can't have duplicates */\
    /* Integers are located from 240 to 247 inclusive */\
    signed char: 239+sizeof(signed char),\
    signed short: 239+sizeof(signed short),\
    signed int: 239+sizeof(signed int),\
    signed long: 239+sizeof(signed long),\
    signed long long: 239+sizeof(signed long long),\
    /* Integers are located from 248 to 255 inclusive */\
    unsigned char: 247+sizeof(unsigned char),\
    unsigned short: 247+sizeof(unsigned short),\
    unsigned int: 247+sizeof(unsigned int),\
    unsigned long: 247+sizeof(unsigned long),\
    unsigned long long: 247+sizeof(unsigned long long),\
    \
    /* char is special because the compiler decides if its signed of not. Here I decide its actually unsigned */\
    char: 247+sizeof(char)\
);


// Internal macro to write out an arbitrary value
#define ULOG__WRITE(X) _Generic(X, \
    float: ulog__write, \
    double: ulog__write, \
    /* Strings need special handling */\
    char *: ulog__write_string_helper, \
    const char *: ulog__write_string_helper, \
    struct ulog_string: ulog__write_htobe,\
    \
    /* Uses basic integers instead of sized integers as sized integers are often typedefs, and we can't have duplicates */\
    /* Integers are simple sized values */\
    signed char: ulog__write,\
    signed short: ulog__write_htobe,\
    signed int: ulog__write_htobe,\
    signed long: ulog__write_htobe,\
    signed long long: ulog__write_htobe,\
    \
    unsigned char: ulog__write,\
    unsigned short: ulog__write_htobe,\
    unsigned int: ulog__write_htobe,\
    unsigned long: ulog__write_htobe,\
    unsigned long long: ulog__write_htobe,\
    \
    /* Bools can be a bit weird in practice with their unused bits */\
    _Bool: ulog__write_bool,\
    char: ulog__write,\
    /* Slices need special handling */\
    struct ulog_slice: ulog__write_slice, \
    struct ulog_slice *: ulog__write_slice \
)(&X, sizeof(X));

// Validate that we aren't blowing through the id cap
static_assert(sizeof(unsigned long long) <= 8, "unsigned long long is larger than 8 bytes");
// We don't encode the size of floats, it needs to be 4 or the decoding will break
// This actually needs to be an ieee single precision float or the decoding will return garbage, but if its 4 bytes it
// at least won't desynchronise the stream
static_assert(sizeof(float) == 4, "float is not 4 bytes");
// We don't encode the size of doubles, it needs to be 8 or the decoding will break
// This actually needs to be an ieee double precision float or the decoding will return garbage, but if its 8 bytes it
// at least won't desynchronise the stream
static_assert(sizeof(double) == 8, "double is not 8 bytes");
// Sanity check? I don't know of any platforms where this isn't true but this is a compile time assert so whatever
static_assert(CHAR_BIT == 8, "char is not 8 bits");

// User provided acquire function, returns whether the lock was acquired. Block at your own risk of deadlocks
bool ulog_external_acquire(void);
// User provided release function, releases the ulog lock
void ulog_external_release(void);
// User provided write function
void ulog_external_write(uint8_t byte);
// User provided flush function
// Should not be assumed to be only sent at the end of a frame
void ulog_external_flush(void);
// User provided system_id function
// Helps identify what system a given log came from. Should be unique as much as possible. Do not set manually, use
// ULOG_SYSTEM_ID() to setup
extern const uint16_t ulog__system_id;


// Internal function to write a string pointer
void ulog__write_string(const char * string);
// Internal function to write data
void ulog__write(const void * data, int size);
// Internal function to write data in a BE format
void ulog__write_htobe(const void * data, int size);
// Internal function called once at the end of every log
void ulog__flush(void);


// Internal helper function to write a string pointer pointer
static void ulog__write_string_helper(void * string_pointer, int _) {
    // Wacky cast because C chokes on const qualifiers in pointers
    ulog__write_string(*(char * *)string_pointer);
}
// Internal function to write a struct ulog_slice
static void ulog__write_slice(const struct ulog_slice * data, const int _) {
    // Write the length first because ulog slices can have an arbitrary length
    ulog__write_htobe(&data->size, sizeof(data->size));
    ulog__write(data->data, data->size);
}
// Internal function to write a bool
static void ulog__write_bool(const bool * boolValue, const int _) {
    // Collapse down the representation to 1 or 0
    const uint8_t value = *boolValue ? 1 : 0;
    ulog__write(&value, sizeof(uint8_t));
}


// Internal macro to define and write out an argument
#define ULOG__LOG_ARGUMENT(format, arg) do {\
    /* __auto_type decays string literals, arrays and pointers into pointers, which is good for us */\
    __auto_type _ulog__value = arg;\
_Pragma("GCC diagnostic push") \
_Pragma("GCC diagnostic ignored \"-Wpedantic\"") \
    /* Export the argument type symbol */\
    ULOG__EXPORT_SYMBOL(arg_symbol, unsigned char, ULOG__SECTION_ARGUMENT, ULOG__BUILD_UNIQUE_SYMBOL_NAME(format) "_arg_" ULOG__XSTRINGIFY(__COUNTER__)) = ULOG__TYPE_OF(_ulog__value);\
_Pragma("GCC diagnostic pop") \
    /* Write out the argument */ \
    ULOG__WRITE(_ulog__value); \
} while(0);


// Internal macro to log a message with arguments. Does the heavy lifting
#define ULOG__LOG(level, format, ...) do { \
_Pragma("GCC diagnostic push") \
_Pragma("GCC diagnostic ignored \"-Wpragmas\"") \
_Pragma("GCC diagnostic ignored \"-Wgnu\"") \
_Pragma("GCC diagnostic ignored \"-Wpointer-to-int-cast\"") \
_Pragma("GCC diagnostic ignored \"-Wint-to-pointer-cast\"") \
    /* Export a definition for the format string */\
    ULOG__EXPORT_MARKER(_ulog__definition, level, ULOG__BUILD_UNIQUE_SYMBOL_NAME(format)); \
    /* If it's greater, then it means we want to ignore it */\
    /* We should also attempt to acquire the lock to prevent reentrancy issues */\
    if ((void *)&_ulog__definition < ulog__level_max_ptr && ulog_external_acquire()) {\
        /* We avoid polluting the scope more than needed */\
        extern uint8_t _sulog_level; \
        /* Casting to uint16 is safe because we assert in the linker script that we don't have more than ~16k logs */\
        /* and the VMA starts at 0 so this will be <=~16k */\
        uint16_t _ulog__definition_id = (uint16_t)(&_ulog__definition - &_sulog_level); \
        /* Write out the system id then the definition(message) id */\
        ULOG__WRITE(ulog__system_id); \
        ULOG__WRITE(_ulog__definition_id); \
        /* Magic macro that expands arguments */\
        ULOG__FOR_EACH1(ULOG__LOG_ARGUMENT, format, __VA_ARGS__) \
        /* We are done logging */\
        ulog__flush();\
        ulog_external_release(); \
    }\
_Pragma("GCC diagnostic pop") \
\
} while(0)


// Helper macro to set the system id, should only be used in top level executables
// Do not use in libraries
#define ULOG_SYSTEM_ID(X) \
ULOG__EXPORT_SYMBOL(ulog__system_id_symbol, uint16_t, ULOG__SECTION_META, "__ulog_sym_system_id") = X; \
const uint16_t ulog__system_id = X;


/* Main macros to be used to log at a given log level */

#define ULOG_EMERGENCY(format, ...) ULOG__LOG(ULOG__SECTION_EMERGENCY, format __VA_OPT__(, __VA_ARGS__))
#define ULOG_EMERG(...) ULOG_EMERGENCY(__VA_ARGS__)
#define ULOG_ALERT(format, ...) ULOG__LOG(ULOG__SECTION_ALERT, format __VA_OPT__(, __VA_ARGS__))
#define ULOG_CRITICAL(format, ...) ULOG__LOG(ULOG__SECTION_CRITICAL, format __VA_OPT__(, __VA_ARGS__))
#define ULOG_CRIT(...) ULOG_CRITICAL(__VA_ARGS__)
#define ULOG_ERROR(format, ...) ULOG__LOG(ULOG__SECTION_ERROR, format __VA_OPT__(, __VA_ARGS__))
#define ULOG_ERR(...) ULOG_ERROR(__VA_ARGS__)
#define ULOG_WARNING(format, ...) ULOG__LOG(ULOG__SECTION_WARNING, format __VA_OPT__(, __VA_ARGS__))
#define ULOG_WARN(...) ULOG_WARNING(__VA_ARGS__)
#define ULOG_NOTICE(format, ...) ULOG__LOG(ULOG__SECTION_NOTICE, format __VA_OPT__(, __VA_ARGS__))
#define ULOG_INFO(format, ...) ULOG__LOG(ULOG__SECTION_INFO, format __VA_OPT__(, __VA_ARGS__))
#define ULOG_DEBUG(format, ...) ULOG__LOG(ULOG__SECTION_DEBUG, format __VA_OPT__(, __VA_ARGS__))
#define ULOG_DBG(...) ULOG_DEBUG(__VA_ARGS__)
#define ULOG_TRACE(format, ...) ULOG__LOG(ULOG__SECTION_TRACE, format __VA_OPT__(, __VA_ARGS__))


/* Log levels to be used with ulog_*_level() */

#define ULOG_LEVEL_EMERGENCY (0)
#define ULOG_LEVEL_EMERG (ULOG_LEVEL_EMERGENCY)
#define ULOG_LEVEL_ALERT (1 + ULOG_LEVEL_EMERGENCY)
#define ULOG_LEVEL_CRITICAL (1 + ULOG_LEVEL_ALERT)
#define ULOG_LEVEL_CRIT (ULOG_LEVEL_CRITICAL)
#define ULOG_LEVEL_ERROR (1 + ULOG_LEVEL_CRITICAL)
#define ULOG_LEVEL_ERR (ULOG_LEVEL_ERROR)
#define ULOG_LEVEL_WARNING (1 + ULOG_LEVEL_ERROR)
#define ULOG_LEVEL_WARN (ULOG_LEVEL_WARNING)
#define ULOG_LEVEL_NOTICE (1 + ULOG_LEVEL_WARNING)
#define ULOG_LEVEL_INFO (1 + ULOG_LEVEL_NOTICE)
#define ULOG_LEVEL_DEBUG (1 + ULOG_LEVEL_INFO)
#define ULOG_LEVEL_DBG (ULOG_LEVEL_DEBUG)
#define ULOG_LEVEL_TRACE (1 + ULOG_LEVEL_DEBUG)


// Set the maximum log level to be actually logged
void ulog_set_level(int level);
// Get the maximum log level that will be logged
int ulog_get_level(void);
// Internal global pointer to the last message definition that should be logged
extern void * ulog__level_max_ptr;

#pragma GCC diagnostic pop
