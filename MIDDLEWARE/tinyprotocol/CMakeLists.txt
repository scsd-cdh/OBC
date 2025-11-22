add_library(TINYPROTOCOL)

target_sources(
    TINYPROTOCOL
    PRIVATE
        tinyprotocol.c
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS .
        FILES
            tinyprotocol.h
)