# Dummy container for header files
add_library(DRIVER_SAMV71_DFP INTERFACE)

target_include_directories(DRIVER_SAMV71_DFP INTERFACE
    samv71b/include
)

target_link_libraries(DRIVER_SAMV71_DFP INTERFACE DRIVER_CMSIS)
