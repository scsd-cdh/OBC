#pragma once

#ifndef __GNUC__
#error uLog is only compatible with GCC.
#endif

// Different platforms name the file in different ways
#if __has_include (<sys/endian.h>)
#   include <sys/endian.h>
#elif __has_include (<endian.h>)
#   include <endian.h>
#else
#   error "cant find endian.h"
#endif

// Different platforms use a different amount of underscores
#ifdef BYTE_ORDER
#elifdef __BYTE_ORDER
#   define BYTE_ORDER __BYTE_ORDER
#elifdef _BYTE_ORDER
#   define BYTE_ORDER _BYTE_ORDER
#endif
#ifdef BIG_ENDIAN
#elifdef __BIG_ENDIAN
#   define BIG_ENDIAN __BIG_ENDIAN
#elifdef _BIG_ENDIAN
#   define BIG_ENDIAN _BIG_ENDIAN
#endif
#ifdef LITTLE_ENDIAN
#elifdef __LITTLE_ENDIAN
#   define LITTLE_ENDIAN __LITTLE_ENDIAN
#elifdef _LITTLE_ENDIAN
#   define LITTLE_ENDIAN _LITTLE_ENDIAN
#endif


// Internal macro to stringify the input
#define ULOG__STRINGIFY(X) #X
// Internal macro to force expansion before stringifying the input
#define ULOG__XSTRINGIFY(X) ULOG__STRINGIFY(X)

// The following section is taken from https://www.scs.stanford.edu/~dm/blog/va-opt.html
// It's basically a set of internal magic macros to have for loops in the preprocessor

#define ULOG__PARENS ()

#define ULOG__EXPAND(...) ULOG__EXPAND4(ULOG__EXPAND4(ULOG__EXPAND4(ULOG__EXPAND4(__VA_ARGS__))))
#define ULOG__EXPAND4(...) ULOG__EXPAND3(ULOG__EXPAND3(ULOG__EXPAND3(ULOG__EXPAND3(__VA_ARGS__))))
#define ULOG__EXPAND3(...) ULOG__EXPAND2(ULOG__EXPAND2(ULOG__EXPAND2(ULOG__EXPAND2(__VA_ARGS__))))
#define ULOG__EXPAND2(...) ULOG__EXPAND1(ULOG__EXPAND1(ULOG__EXPAND1(ULOG__EXPAND1(__VA_ARGS__))))
#define ULOG__EXPAND1(...) __VA_ARGS__

#define ULOG__FOR_EACH0(macro, ...)                                    \
    __VA_OPT__(ULOG__EXPAND(ULOG__FOR_EACH_HELPER0(macro, __VA_ARGS__)))
#define ULOG__FOR_EACH_HELPER0(macro, a0, ...)                         \
    macro(a0)                                                     \
    __VA_OPT__(ULOG__FOR_EACH_AGAIN0 ULOG__PARENS (macro, __VA_ARGS__))
#define ULOG__FOR_EACH_AGAIN0() ULOG__FOR_EACH_HELPER0

#define ULOG__FOR_EACH1(macro, sarg1, ...)                                    \
    __VA_OPT__(ULOG__EXPAND(ULOG__FOR_EACH_HELPER1(macro, sarg1, __VA_ARGS__)))
#define ULOG__FOR_EACH_HELPER1(macro, sarg1, a1, ...)                         \
    macro(sarg1, a1)                                                     \
    __VA_OPT__(ULOG__FOR_EACH_AGAIN1 ULOG__PARENS (macro, sarg1, __VA_ARGS__))
#define ULOG__FOR_EACH_AGAIN1() ULOG__FOR_EACH_HELPER1

#define ULOG__FOR_EACH2(macro, sarg1, sarg2, ...)                                    \
    __VA_OPT__(ULOG__EXPAND(ULOG__FOR_EACH_HELPER2(macro, sarg1, sarg2, __VA_ARGS__)))
#define ULOG__FOR_EACH_HELPER2(macro, sarg1, sarg2, a1, ...)                         \
    macro(sarg1, sarg2, a1)                                                     \
    __VA_OPT__(ULOG__FOR_EACH_AGAIN2 ULOG__PARENS (macro, sarg1, sarg2, __VA_ARGS__))
#define ULOG__FOR_EACH_AGAIN2() ULOG__FOR_EACH_HELPER2

// Internal macro to explicitly mark a symbol as used and not allow the compiler to remove it
#define ULOG__ATTR_USED __attribute__((used))
// Internal macro to mark what section a symbol should be placed in
#define ULOG__ATTR_SECTION(NAME) __attribute__((section(NAME)))

// Internal macro to construct a unique symbol name. Note that if the macro is used twice with the same input in a single line
// Then the symbol may not actually be unique. See __COUNTER__ if a truly unique name is required
#define ULOG__BUILD_UNIQUE_SYMBOL_NAME(NAME) "__ulog_sym_" ULOG__XSTRINGIFY(__FILE__)  "_" ULOG__XSTRINGIFY(__LINE__)  "_" ULOG__XSTRINGIFY(NAME)

// Internal macro to export an arbitrary symbol in a section
#define ULOG__EXPORT_SYMBOL(LOCAL_NAME, TYPE, SECTION, EXPORTED_NAME) \
    const static TYPE \
    ULOG__ATTR_SECTION(SECTION) \
    ULOG__ATTR_USED \
    LOCAL_NAME \
    __asm__( \
        ULOG__XSTRINGIFY(EXPORTED_NAME)\
    )
// Internal macro to export a 1 byte symbol in a section
#define ULOG__EXPORT_MARKER(LOCAL_NAME, SECTION, EXPORTED_NAME) ULOG__EXPORT_SYMBOL(LOCAL_NAME, unsigned char, SECTION, EXPORTED_NAME)

