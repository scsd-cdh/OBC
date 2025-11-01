# HAL library
add_library(MIDDLEWARE)

target_sources(
        MIDDLEWARE
        PRIVATE
        tinyprotocol/tinyprotocol.c
        commsdrive/commsdrive.c
)

target_include_directories(MIDDLEWARE PUBLIC
        tinyprotocol/
        commsdrive/
)

target_link_libraries(MIDDLEWARE PUBLIC DRIVER_SAMV71)