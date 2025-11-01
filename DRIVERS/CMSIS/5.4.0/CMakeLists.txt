# Dummy container for header files
add_library(DRIVER_CMSIS INTERFACE)

target_include_directories(DRIVER_CMSIS INTERFACE
    CMSIS/Core/Include
)