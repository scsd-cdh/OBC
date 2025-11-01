# HAL library
add_library(DRIVER_SAMV71)

target_sources(
    DRIVER_SAMV71
    PRIVATE
        hal/src/hal_atomic.c
        hal/src/hal_delay.c
        hal/src/hal_gpio.c
        hal/src/hal_i2c_m_sync.c
        hal/src/hal_init.c
        hal/src/hal_io.c
        hal/src/hal_sleep.c
        hal/src/hal_spi_m_sync.c
        hal/src/hal_usart_sync.c
        hal/utils/src/utils_assert.c
        hal/utils/src/utils_event.c
        hal/utils/src/utils_list.c
        hal/utils/src/utils_ringbuffer.c
        hal/utils/src/utils_syscalls.c
        hpl/core/hpl_core_m7_base.c
        hpl/core/hpl_init.c
        hpl/pmc/hpl_pmc.c
        hpl/pmc/hpl_sleep.c
        hpl/spi/hpl_spi.c
        hpl/twihs/hpl_twihs.c
        hpl/uart/hpl_uart.c
        hpl/xdmac/hpl_xdmac.c
)

target_include_directories(DRIVER_SAMV71 PUBLIC
    ./
    hal/include
    hal/utils/include
    hpl/core
    hpl/pio
    hpl/pmc
    hpl/spi
    hpl/twihs
    hpl/uart
    hpl/xdmac
    hri
)

# Needs to link to the header files (and will reexport them to consumers)
target_link_libraries(DRIVER_SAMV71 PUBLIC DRIVER_SAMV71_DFP)