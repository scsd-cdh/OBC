cmake_minimum_required(VERSION 3.20)

# Cross-compiling: CMake tries to link a test executable during compiler
# detection, which fails for bare-metal MSP430. Use a static-library test.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(PROJECT_NAME "PDS" CACHE STRING "Project / executable name")

project(${PROJECT_NAME} C)

set(MCU "msp430fr6989" CACHE STRING "MSP430 device")
set(SUPPORTED_MCUS msp430fr5989 msp430fr5969 msp430fr6989)
set_property(CACHE MCU PROPERTY STRINGS
        msp430fr5989
        msp430fr5969
        msp430fr6989
)
if(NOT MCU IN_LIST SUPPORTED_MCUS)
    message(FATAL_ERROR
            "Unsupported MCU '${MCU}'. Supported MCUs: ${SUPPORTED_MCUS}")
endif()
set(EXTRA_MCU_FLAGS "-mlarge" CACHE STRING "Extra MCU flags, e.g. -mlarge")

# ---------------------------------------------------------------------------
# Root of the repo, derived once, robustly
# ---------------------------------------------------------------------------
get_filename_component(REPO_ROOT
        "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)

# ---------------------------------------------------------------------------
# Manually add include directories
# ---------------------------------------------------------------------------
set(INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${REPO_ROOT}/DRIVERS/MSP430/I2C
        ${REPO_ROOT}/DRIVERS/MSP430/PWM
        ${REPO_ROOT}/DRIVERS/MSP430/PWM/include
        ${REPO_ROOT}/DRIVERS/MSP430/RTC_C
        ${REPO_ROOT}/DRIVERS/MSP430/RTC_B
        ${REPO_ROOT}/DRIVERS/MSP430/UTILS
        ${REPO_ROOT}/DRIVERS/MSP430/ADC
        ${REPO_ROOT}/DRIVERS/MSP430/ADC/include
        ${REPO_ROOT}/DRIVERS/MSP430/GPIO
        ${REPO_ROOT}/MIDDLEWARE
        ${REPO_ROOT}/MIDDLEWARE/ASN1SCC
        ${REPO_ROOT}/MIDDLEWARE/LFP/include
        ${REPO_ROOT}/MIDDLEWARE/LFP/include/lfp
        ${REPO_ROOT}/MIDDLEWARE/LFP_I2C
        ${REPO_ROOT}/MIDDLEWARE/MSP_UTILS
)

# ---------------------------------------------------------------------------
# Manually add source files
# ---------------------------------------------------------------------------
set(SOURCES
        PDS.c
        ${REPO_ROOT}/DRIVERS/MSP430/ADC/ADC_Read.c
        ${REPO_ROOT}/DRIVERS/MSP430/ADC/include/adc12_b.c
        ${REPO_ROOT}/DRIVERS/MSP430/GPIO/gpio.c
        ${REPO_ROOT}/DRIVERS/MSP430/I2C/i2c.c
        ${REPO_ROOT}/DRIVERS/MSP430/PWM/PWM.c
        ${REPO_ROOT}/DRIVERS/MSP430/PWM/include/pmm.c
        ${REPO_ROOT}/DRIVERS/MSP430/PWM/include/timer_a.c
        ${REPO_ROOT}/DRIVERS/MSP430/PWM/include/timer_b.c
        ${REPO_ROOT}/DRIVERS/MSP430/RTC_B/rtc_b.c
        ${REPO_ROOT}/DRIVERS/MSP430/RTC_C/rtc_c.c
        ${REPO_ROOT}/DRIVERS/MSP430/UTILS/utils.c
        ${REPO_ROOT}/MIDDLEWARE/MSP_UTILS/msp_utils.c
        ${REPO_ROOT}/MIDDLEWARE/LFP_I2C/lfp_i2c.c
)

# ---------------------------------------------------------------------------
# This globs a all .c's under a directory and appends it to SOURCES.
# Use if you are lazy to list all .c's under a directory in SOURCES.
# But be careful what you are globbing. Should be used sparingly.
# to avoid mistakes and avoid bloating of the image with unnecessary .c's
# ---------------------------------------------------------------------------
file(GLOB_RECURSE GLOBBED_SOURCES
        ${REPO_ROOT}/MIDDLEWARE/LFP/*.c
        ${REPO_ROOT}/MIDDLEWARE/asn1/*.c
)
list(APPEND SOURCES ${GLOBBED_SOURCES})

message(STATUS "CMAKE_CURRENT_LIST_DIR = ${CMAKE_CURRENT_LIST_DIR}")
message(STATUS "REPO_ROOT              = ${REPO_ROOT}")
message(STATUS "PROJECT_NAME           = ${PROJECT_NAME}")

add_executable(${PROJECT_NAME}
        main.c
        ${SOURCES}
)

# Silence the noisy -Wint-to-pointer-cast warnings from the vendored/legacy
# driver sources when building in the large memory model (-mlarge). Applied
# only to the driver sources; main.c keeps full -Wall -Wextra.
set_source_files_properties(${SOURCES} PROPERTIES
        COMPILE_OPTIONS "-Wno-int-to-pointer-cast"
)

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")

# TI support-files include is always present; your INCLUDE_DIRS are appended.
target_include_directories(${PROJECT_NAME} PRIVATE
        /opt/ti/support-files/include
        ${INCLUDE_DIRS}
)

target_compile_options(${PROJECT_NAME} PRIVATE
        -mmcu=${MCU}
        ${EXTRA_MCU_FLAGS}
        -Os
        -g # IMPORTANT: Keep if u want to debug. greatly bloats image so remove if not debugging
        -Wall
        -Wextra
        -ffunction-sections
        -fdata-sections
)

target_link_options(${PROJECT_NAME} PRIVATE
        -mmcu=${MCU}
        ${EXTRA_MCU_FLAGS}
        -L/opt/ti/support-files/include
        -Wl,--gc-sections
        -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.map
)

find_program(MSP430_OBJCOPY msp430-elf-objcopy REQUIRED)
find_program(MSP430_SIZE msp430-elf-size REQUIRED)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${MSP430_OBJCOPY} -O ihex
        $<TARGET_FILE:${PROJECT_NAME}>
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.hex
        COMMAND ${MSP430_OBJCOPY} -O binary
        $<TARGET_FILE:${PROJECT_NAME}>
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin
        COMMAND ${MSP430_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
        COMMENT "Generating ${PROJECT_NAME}.hex/.bin and size report"
)