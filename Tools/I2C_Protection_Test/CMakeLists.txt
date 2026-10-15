cmake_minimum_required(VERSION 3.20)

# Cross-compiling: CMake tries to link a test executable during compiler
# detection, which fails for bare-metal MSP430. Use a static-library test.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(PROJECT_NAME "I2C_Protection_Test" CACHE STRING "Project / executable name")

project(${PROJECT_NAME} C)

set(MCU "msp430fr5989" CACHE STRING "MSP430 device")
set(SUPPORTED_MCUS msp430fr5989)
set_property(CACHE MCU PROPERTY STRINGS
        msp430fr5989
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
        ${REPO_ROOT}/MIDDLEWARE/MSP_UTILS
        ${REPO_ROOT}/DRIVERS/MSP430/RTC_C
        ${REPO_ROOT}/DRIVERS/MSP430/UTILS
)

set(SOURCES
        ${REPO_ROOT}/MIDDLEWARE/MSP_UTILS/msp_utils.c
)

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
