if (CONFIG_ULOG)
    zephyr_library()

    zephyr_library_sources(
        PUBLIC FILE_SET HEADERS BASE_DIRS ../include FILES
        ../include/ulog/ulog.h
        ../include/ulog/macros.h

        PRIVATE FILE_SET private_headers TYPE HEADERS BASE_DIRS .. FILES
        ../rzcobs.h
        ../linker_symbols.h
        logging.h

        PRIVATE
        ../ulog.c
        ../rzcobs.c
    )

    zephyr_include_directories(../include)
    zephyr_linker_sources(SECTIONS "../ulog_snippet.ld")

    zephyr_library_sources_ifdef(CONFIG_ULOG_LOG_BACKEND logging_adapter.c)
    zephyr_library_sources_ifdef(CONFIG_ULOG_USE_UART logging.c)

endif ()