cmake_minimum_required(VERSION 3.20)

# Cross-compiling: CMake tries to link a test executable during compiler
# detection, which fails for bare-metal MSP430. Use a static-library test.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(BMS C)

set(MCU "msp430fr6989" CACHE STRING "MSP430 device")
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
        ${CMAKE_CURRENT_SOURCE_DIR}../../DRIVERS/MSP430/I2C
        ${CMAKE_CURRENT_SOURCE_DIR}../../DRIVERS/MSP430/PWM
        ${CMAKE_CURRENT_SOURCE_DIR}../../DRIVERS/MSP430/PWM/include
        ${CMAKE_CURRENT_SOURCE_DIR}../../DRIVERS/MSP430/RTC_C
        ${CMAKE_CURRENT_SOURCE_DIR}../../DRIVERS/MSP430/RTC_B
        ${CMAKE_CURRENT_SOURCE_DIR}../../DRIVERS/MSP430/UTILS
        ${CMAKE_CURRENT_SOURCE_DIR}../../DRIVERS/MSP430/ADC
        ${CMAKE_CURRENT_SOURCE_DIR}../../DRIVERS/MSP430/ADC/include
        ${CMAKE_CURRENT_SOURCE_DIR}../../DRIVERS/MSP430/GPIO
        ${CMAKE_CURRENT_SOURCE_DIR}../../DRIVERS/MSP430/SWI2C
        ${CMAKE_CURRENT_SOURCE_DIR}../../MIDDLEWARE/ADS7138
        ${CMAKE_CURRENT_SOURCE_DIR}../../MIDDLEWARE/asn1
        ${CMAKE_CURRENT_SOURCE_DIR}../../MIDDLEWARE/ASN1SCC
        ${CMAKE_CURRENT_SOURCE_DIR}../../MIDDLEWARE/LFP/include
        ${CMAKE_CURRENT_SOURCE_DIR}../../MIDDLEWARE/LFP/include/lfp
        ${CMAKE_CURRENT_SOURCE_DIR}../../MIDDLEWARE/LFP_I2C
        ${CMAKE_CURRENT_SOURCE_DIR}../../MIDDLEWARE/MSP_UTILS
)

# ---------------------------------------------------------------------------
# Manually add source files
# ---------------------------------------------------------------------------
set(SOURCES
        BMS.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../../DRIVERS/MSP430/ADC/ADC_Read.c
        ${CMAKE_CURRENT_SOURCE_DIR}../../DRIVERS/MSP430/ADC/include/adc12_b.c
        ${CMAKE_CURRENT_SOURCE_DIR}../../DRIVERS/MSP430/GPIO/gpio.c
        ${CMAKE_CURRENT_SOURCE_DIR}../../DRIVERS/MSP430/I2C/i2c.c
        ${CMAKE_CURRENT_SOURCE_DIR}../../DRIVERS/MSP430/PWM/PWM.c
        ${CMAKE_CURRENT_SOURCE_DIR}../../DRIVERS/MSP430/PWM/include/pmm.c
        ${CMAKE_CURRENT_SOURCE_DIR}../../DRIVERS/MSP430/PWM/include/timer_a.c
        ${CMAKE_CURRENT_SOURCE_DIR}../../DRIVERS/MSP430/PWM/include/timer_b.c
        ${CMAKE_CURRENT_SOURCE_DIR}../../DRIVERS/MSP430/RTC_B/rtc_b.c
        ${CMAKE_CURRENT_SOURCE_DIR}../../DRIVERS/MSP430/RTC_C/rtc_c.c
        ${CMAKE_CURRENT_SOURCE_DIR}../../DRIVERS/MSP430/SWI2C/swi2c.c
        ${CMAKE_CURRENT_SOURCE_DIR}../../DRIVERS/MSP430/UTILS/utils.c
        ${CMAKE_CURRENT_SOURCE_DIR}../../MIDDLEWARE/ADS7138/ads7138irter.c
        ${CMAKE_CURRENT_SOURCE_DIR}../../MIDDLEWARE/MSP_UTILS/msp_utils.c
        ${CMAKE_CURRENT_SOURCE_DIR}../../MIDDLEWARE/LFP_I2C/lfp_i2c.c
        ${CMAKE_CURRENT_SOURCE_DIR}../../MIDDLEWARE/ASN1SCC/example.c
)

# ---------------------------------------------------------------------------
# This globs a all .c's under a directory and appends it to SOURCES.
# Use if you are lazy to list all .c's under a directory in SOURCES.
# But be careful what you are globbing. Should be used sparingly.
# to avoid mistakes and avoid bloating of the image with unnecessary .c's
# ---------------------------------------------------------------------------
file(GLOB_RECURSE GLOBBED_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}../../MIDDLEWARE/LFP/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}../../MIDDLEWARE/asn1/*.c
)
list(APPEND SOURCES ${GLOBBED_SOURCES})

add_executable(BMS
        main.c
        ${SOURCES}
)

# Silence the noisy -Wint-to-pointer-cast warnings from the vendored/legacy
# driver sources when building in the large memory model (-mlarge). Applied
# only to the driver sources; main.c keeps full -Wall -Wextra.
set_source_files_properties(${SOURCES} PROPERTIES
        COMPILE_OPTIONS "-Wno-int-to-pointer-cast"
)

set_target_properties(BMS PROPERTIES SUFFIX ".elf")

# TI support-files include is always present; your INCLUDE_DIRS are appended.
target_include_directories(BMS PRIVATE
        /opt/ti/support-files/include
        ${INCLUDE_DIRS}
)

target_compile_options(BMS PRIVATE
        -mmcu=${MCU}
        ${EXTRA_MCU_FLAGS}
        -Os
        -g # IMPORTANT: Keep if u want to debug. greatly bloats image so remove if not debugging
        -Wall
        -Wextra
        -ffunction-sections
        -fdata-sections
)

target_link_options(BMS PRIVATE
        -mmcu=${MCU}
        ${EXTRA_MCU_FLAGS}
        -L/opt/ti/support-files/include
        -Wl,--gc-sections
        -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.map
)

find_program(MSP430_OBJCOPY msp430-elf-objcopy REQUIRED)
find_program(MSP430_SIZE msp430-elf-size REQUIRED)

add_custom_command(TARGET BMS POST_BUILD
        COMMAND ${MSP430_OBJCOPY} -O ihex
        $<TARGET_FILE:BMS>
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.hex
        COMMAND ${MSP430_OBJCOPY} -O binary
        $<TARGET_FILE:BMS>
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin
        COMMAND ${MSP430_SIZE} $<TARGET_FILE:BMS>
        COMMENT "Generating ${PROJECT_NAME}.hex/.bin and size report"
)