add_library(LFP)

target_sources(LFP
    PUBLIC FILE_SET HEADERS BASE_DIRS include FILES
        include/lfp.h
        include/lfp/body.h
        include/lfp/cobs.h
        include/lfp/header.h
        include/lfp/stream.h

    PRIVATE FILE_SET private_headers TYPE HEADERS BASE_DIRS . FILES
        cobs_private.h
        crc.h

    PRIVATE
        body.c
        cobs.c
        crc.c
        header.c
        lfp.c
        stream.c
)

target_include_directories(LFP PUBLIC include)
