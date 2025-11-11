add_library(MIDDLEWARE_ULOG OBJECT)

target_sources(MIDDLEWARE_ULOG
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/ulog/ulog.h
            include/ulog/macros.h

    PRIVATE
        FILE_SET private_headers
        TYPE HEADERS
        BASE_DIRS .
        FILES
            rzcobs.h
            linker_symbols.h

    PRIVATE
        ulog.c
        rzcobs.c
)

target_include_directories(MIDDLEWARE_ULOG PUBLIC include)
#target_link_libraries(MIDDLEWARE_ULOG PRIVATE DRIVER_CDH_COMMON)
target_compile_options(MIDDLEWARE_ULOG PRIVATE "-save-temps")