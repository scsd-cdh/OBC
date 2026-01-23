if (CONFIG_ULOG)
    zephyr_library()

    zephyr_library_sources(
        PUBLIC
            FILE_SET HEADERS
            BASE_DIRS ../include
            FILES
            ../include/ulog/ulog.h
            ../include/ulog/macros.h

        PRIVATE
            FILE_SET private_headers
            TYPE HEADERS
            BASE_DIRS ..
            FILES
            ../rzcobs.h
            ../linker_symbols.h

        PRIVATE
            ../ulog.c
            ../rzcobs.c
            ./logging_adapter.c
    )

    zephyr_include_directories(../include)
    zephyr_linker_sources(SECTIONS "../ulog_snippet.ld")
endif ()