if(NOT ASN1SCC_DIR_NAME)
    set(CMAKE_BUILD_TYPE Release)
endif()

function(asn1scc_generate_c ASN1SCC_TARGET)
    cmake_parse_arguments(ARG "" "INCLUDE_DIR" "" ${ARGN})

    if(NOT ARG_INCLUDE_DIR)
        set(ARG_INCLUDE_DIR asn1scc)
    endif()

    set(ASN1SCC_EXECUTABLE "/home/alexkar598/Desktop/Repositories/asn1scc/asn1scc/bin/Release/net9.0/asn1scc")


    set(ARG_ASN_FILES)
    set(SRCS)
    set(HDRS)

    foreach(FIL ${ARGN})
        get_filename_component(FIL_WLE ${FIL} NAME_WLE)
        get_filename_component(FIL_ABS ${FIL} ABSOLUTE BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
        list(APPEND ARG_ASN_FILES ${FIL_ABS})

        list(APPEND SRCS "${CMAKE_CURRENT_BINARY_DIR}/${ARG_INCLUDE_DIR}/${ARG_INCLUDE_DIR}/${FIL_WLE}.c")
        list(APPEND HDRS "${CMAKE_CURRENT_BINARY_DIR}/${ARG_INCLUDE_DIR}/${ARG_INCLUDE_DIR}/${FIL_WLE}.h")
    endforeach()

    list(APPEND SRCS "${CMAKE_CURRENT_BINARY_DIR}/${ARG_INCLUDE_DIR}/${ARG_INCLUDE_DIR}/asn1crt.c")
    list(APPEND HDRS "${CMAKE_CURRENT_BINARY_DIR}/${ARG_INCLUDE_DIR}/${ARG_INCLUDE_DIR}/asn1crt.h")
    list(APPEND SRCS "${CMAKE_CURRENT_BINARY_DIR}/${ARG_INCLUDE_DIR}/${ARG_INCLUDE_DIR}/asn1crt_encoding.c")
    list(APPEND HDRS "${CMAKE_CURRENT_BINARY_DIR}/${ARG_INCLUDE_DIR}/${ARG_INCLUDE_DIR}/asn1crt_encoding.h")
    list(APPEND SRCS "${CMAKE_CURRENT_BINARY_DIR}/${ARG_INCLUDE_DIR}/${ARG_INCLUDE_DIR}/asn1crt_encoding_uper.c")
    list(APPEND HDRS "${CMAKE_CURRENT_BINARY_DIR}/${ARG_INCLUDE_DIR}/${ARG_INCLUDE_DIR}/asn1crt_encoding_uper.h")

    add_custom_command(
            OUTPUT ${SRCS} ${HDRS}
            COMMAND ${ASN1SCC_EXECUTABLE}
            ARGS -o "${CMAKE_CURRENT_BINARY_DIR}/${ARG_INCLUDE_DIR}/${ARG_INCLUDE_DIR}" -c -uPER ${ARG_ASN_FILES}
            DEPENDS ${ARG_ASN_FILES}
            DEPENDS_EXPLICIT_ONLY
    )

    target_sources(${ASN1SCC_TARGET} PRIVATE ${SRCS} ${HDRS})
    target_include_directories(${ASN1SCC_TARGET} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/${ARG_INCLUDE_DIR}/")
endfunction()

