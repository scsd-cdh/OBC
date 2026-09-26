cmake_minimum_required(VERSION 3.20)

# Cross-compiling: CMake tries to link a test executable during compiler
# detection, which fails for bare-metal MSP430. Use a static-library test.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(BMS C)

# ---------------------------------------------------------------------------
# NOTE ON MEMORY MODEL:
# The CCS Debug config this was ported from (target MSP430FR6989, which is
# what MCU defaults to below) used DATA_MODEL "restricted" in TI's compiler,
# i.e. the small/16-bit model — NOT -mlarge. Verify which model your linker
# script (lnk_msp430fr6989.cmd) actually expects before flashing; the
# -mlarge default below was inherited from the original template and may
# not match the original CCS Debug build.
# ---------------------------------------------------------------------------
set(MCU "msp430fr6989" CACHE STRING "MSP430 device")
set(EXTRA_MCU_FLAGS "-mlarge" CACHE STRING "Extra MCU flags, e.g. -mlarge")

# Repo root: this file is assumed to sit alongside APP/, DRIVERS/, MIDDLEWARE/
# (i.e. where the CCS project's PARENT-2-PROJECT_LOC linked resources point).
set(REPO_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# ---------------------------------------------------------------------------
# Include directories, ported from the CCS Debug configuration's
# INCLUDE_PATH option in _cproject. CCS-internal paths
# (${CCS_BASE_ROOT}/msp430/include, ${CG_TOOL_ROOT}/include) were dropped
# since those are TI-compiler-specific and not applicable to msp430-elf-gcc.
# (The TI support-files include is added automatically below; do not add it
# here.)
#
# MIDDLEWARE/LFP_I2C and MIDDLEWARE/MSP_UTILS were linked in _project via
# absolute paths on the original developer's machine
# (/home/brendan/dev/zephyr_obc/application/MIDDLEWARE/...) rather than
# relative ones -- update these two to wherever they actually live in your
# checkout.
# ---------------------------------------------------------------------------
set(INCLUDE_DIRS
        APP/BMS
        DRIVERS/MSP430/I2C
        DRIVERS/MSP430/PWM
        DRIVERS/MSP430/RTC_C
        DRIVERS/MSP430/RTC_B
        DRIVERS/MSP430/UTILS
        DRIVERS/MSP430/ADC
        DRIVERS/MSP430/GPIO
        DRIVERS/MSP430/SWI2C
        ${REPO_ROOT}/MIDDLEWARE
        MIDDLEWARE/ADS7138
        MIDDLEWARE/asn1
        MIDDLEWARE/ASN1SCC
        MIDDLEWARE/LFP/include
        MIDDLEWARE/LFP_I2C
        MIDDLEWARE/MSP_UTILS
)

# ---------------------------------------------------------------------------
# Source files explicitly enumerated via linked resources in _project.
# ---------------------------------------------------------------------------
set(SOURCES
        APP/BMS/BMS.c
        DRIVERS/MSP430/ADC/ADC_Read.c
        DRIVERS/MSP430/ADC/include/adc12_b.c
        DRIVERS/MSP430/GPIO/gpio.c
        DRIVERS/MSP430/I2C/i2c.c
        DRIVERS/MSP430/PWM/PWM.c
        DRIVERS/MSP430/PWM/include/pmm.c
        DRIVERS/MSP430/PWM/include/timer_a.c
        DRIVERS/MSP430/PWM/include/timer_b.c
        DRIVERS/MSP430/RTC_B/rtc_b.c
        DRIVERS/MSP430/RTC_C/rtc_c.c
        DRIVERS/MSP430/SWI2C/swi2c.c
        DRIVERS/MSP430/UTILS/utils.c
        MIDDLEWARE/ADS7138/ads7138irter.c
        MIDDLEWARE/tinyprotocol/tinyprotocol.c
)

# ---------------------------------------------------------------------------
# CCS's sourceEntries scanned the WHOLE project folder for *.c (excluding
# only the two .cmd linker scripts), so folders that were linked as whole
# directories rather than per-file in _project -- and whose contents
# therefore aren't individually listed there -- are picked up here via glob
# instead of being silently dropped. Review this list once the build is
# working and replace with an explicit list if you'd rather avoid globbing.
# ---------------------------------------------------------------------------
file(GLOB_RECURSE GLOBBED_SOURCES
        MIDDLEWARE/LFP/*.c
        MIDDLEWARE/asn1/*.c
        MIDDLEWARE/ASN1SCC/*.c
        MIDDLEWARE/LFP_I2C/*.c
        MIDDLEWARE/MSP_UTILS/*.c
)
list(APPEND SOURCES ${GLOBBED_SOURCES})

add_executable(BMS
        APP/BMS/main.c
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

# Preprocessor defines, ported from the CCS Debug configuration's DEFINE
# option in _cproject.
target_compile_definitions(BMS PRIVATE
        _MPU_ENABLE
        MOCK_POWER
        __MSP430FR6989__
)

target_compile_options(BMS PRIVATE
        -mmcu=${MCU}
        ${EXTRA_MCU_FLAGS}
        -Os
        -g #Keep if u want to debug. greatly bloats image so remove if not debugging
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