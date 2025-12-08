add_library(DRIVER_CDH_COMMON
        src/I2C0_wrapper.c)
target_sources(DRIVER_CDH_COMMON
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/CDH/SPI0_wrapper.h
            include/CDH/pins.h
            include/CDH/driver_init.h
        include/CDH/I2C0_wrapper.h

    PRIVATE
        FILE_SET private_headers
        TYPE HEADERS
        BASE_DIRS src
        FILES
            src/driver_init_private.h


    PRIVATE
        src/SPI0_wrapper.c
        src/Device_Startup/startup_${BOARD_NAME_LOWER}.c
        src/Device_Startup/system_${BOARD_NAME_LOWER}.c
        src/driver_init.c
        src/I2C0_wrapper.c
)

target_include_directories(DRIVER_CDH_COMMON PUBLIC include)
target_link_libraries(DRIVER_CDH_COMMON PUBLIC DRIVER_SAMV71)

# Fucky, but the driver needs the config file
target_include_directories(DRIVER_SAMV71 PUBLIC Config)
target_sources(DRIVER_SAMV71
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS Config
        FILES
            Config/RTE_Components.h
            Config/peripheral_clk_config.h
            Config/hpl_xdmac_config.h
            Config/hpl_uart_config.h
            Config/hpl_usart_config.h
            Config/hpl_twihs_config.h
            Config/hpl_spi_config.h
            Config/hpl_pmc_config.h
)