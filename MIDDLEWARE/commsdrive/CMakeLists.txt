add_library(COMMSDRIVE)

target_link_libraries(COMMSDRIVE PRIVATE DRIVER_SAMV71)
target_link_libraries(COMMSDRIVE PRIVATE DRIVER_CDH_COMMON)
target_link_libraries(COMMSDRIVE PRIVATE TINYPROTOCOL)
target_sources(
    COMMSDRIVE
    PRIVATE
        commsdrive.c
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS .
        FILES
            commsdrive.h
)