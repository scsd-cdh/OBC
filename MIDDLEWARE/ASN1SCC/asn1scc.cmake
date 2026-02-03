# Save curren list dir because we lose it when calling a function
if(NOT ASN1SCC_DIR_NAME)
    set(ASN1SCC_DIR_NAME ${CMAKE_CURRENT_LIST_DIR})
endif()

# Download asn1scc if missing
if(NOT ASN1SCC_EXECUTABLE)
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        set(ZIP_LOCATION "${ASN1SCC_DIR_NAME}/asn1scc.windows.x86_64.zip")
        file(DOWNLOAD
                "https://github.com/scsd-cdh/asn1scc/releases/download/4.6.0.17/asn1scc.windows.x86_64.zip"
                "${CMAKE_BINARY_DIR}/asn1scc.windows.x86_64.zip"
                SHOW_PROGRESS EXPECTED_HASH SHA256=2e741c9bbf7b8854f2e7e6b01962965e7121be9feff8d03bfa604a7f541f8367
        )
        file(ARCHIVE_EXTRACT
                INPUT "${CMAKE_BINARY_DIR}/asn1scc.windows.x86_64.zip"
                DESTINATION "${CMAKE_BINARY_DIR}/_asn1scc_tool"
        )
        file(REMOVE "${CMAKE_BINARY_DIR}/asn1scc.windows.x86_64.zip")
        SET(ASN1SCC_EXECUTABLE "${CMAKE_BINARY_DIR}/_asn1scc_tool/asn1scc.exe" CACHE INTERNAL "" FORCE)
    else()
        file(DOWNLOAD
                "https://github.com/scsd-cdh/asn1scc/releases/download/4.6.0.17/asn1scc.linux.x86_64.zip"
                "${CMAKE_BINARY_DIR}/asn1scc.linux.x86_64.zip"
                SHOW_PROGRESS EXPECTED_HASH SHA256=e81aea0ef9d16a7a7206c5241f01af6f5d612c747906c556921781464211a24f
        )
        file(ARCHIVE_EXTRACT
                INPUT "${CMAKE_BINARY_DIR}/asn1scc.linux.x86_64.zip"
                DESTINATION "${CMAKE_BINARY_DIR}/_asn1scc_tool"
        )
        file(REMOVE "${CMAKE_BINARY_DIR}/asn1scc.linux.x86_64.zip")
        SET(ASN1SCC_EXECUTABLE "${CMAKE_BINARY_DIR}/_asn1scc_tool/asn1scc" CACHE INTERNAL "" FORCE)
    endif()
endif()

# usage: asn1scc_generate_c(<target> [INCLUDE_DIR <include_dir>] [FILES] <files...> [EXTRA_OPTS <options...>])
# params:
#   target -> target to add the generated c files to
#   include_dir -> what to name the include dir used for the generated header files. Default: asn1
#   files... -> .asn1 files to include in the compilation
#   options... -> extra arguments to pass to asn1scc, see asn1scc --help
# example: asn1scc_generate_c(app bms.asn1 cdh.asn1 EXTRA_OPTS --slim -sm)
function(asn1scc_generate_c ASN1SCC_TARGET)
    cmake_parse_arguments(ARG "" "INCLUDE_DIR" "EXTRA_OPTS;FILES" ${ARGN})

    if(NOT ARG_INCLUDE_DIR)
        set(ARG_INCLUDE_DIR "asn1")
    endif()
    if(NOT ARG_EXTRA_OPTS)
        set(ARG_EXTRA_OPTS "")
    endif()

    # Resolved full path to asn1 files
    set(ASN_FILES "")
    # Generated source files
    set(SRCS "")
    # Generated header files
    set(HDRS "")

    # Parse all extra arguments as files
    list(APPEND ARG_FILES ${ARG_UNPARSED_ARGUMENTS})

    # Parse file arguments
    foreach(FIL ${ARG_FILES})
        get_filename_component(FIL_WLE ${FIL} NAME_WLE)
        get_filename_component(FIL_ABS ${FIL} ABSOLUTE BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
        list(APPEND ASN_FILES ${FIL_ABS})

        list(APPEND SRCS "${CMAKE_CURRENT_BINARY_DIR}/${ARG_INCLUDE_DIR}/${ARG_INCLUDE_DIR}/${FIL_WLE}.c")
        list(APPEND HDRS "${CMAKE_CURRENT_BINARY_DIR}/${ARG_INCLUDE_DIR}/${ARG_INCLUDE_DIR}/${FIL_WLE}.h")
    endforeach()

    # Add the always generated files to the generated lists
    list(APPEND SRCS "${CMAKE_CURRENT_BINARY_DIR}/${ARG_INCLUDE_DIR}/${ARG_INCLUDE_DIR}/asn1crt.c")
    list(APPEND HDRS "${CMAKE_CURRENT_BINARY_DIR}/${ARG_INCLUDE_DIR}/${ARG_INCLUDE_DIR}/asn1crt.h")
    list(APPEND SRCS "${CMAKE_CURRENT_BINARY_DIR}/${ARG_INCLUDE_DIR}/${ARG_INCLUDE_DIR}/asn1crt_encoding.c")
    list(APPEND HDRS "${CMAKE_CURRENT_BINARY_DIR}/${ARG_INCLUDE_DIR}/${ARG_INCLUDE_DIR}/asn1crt_encoding.h")
    list(APPEND SRCS "${CMAKE_CURRENT_BINARY_DIR}/${ARG_INCLUDE_DIR}/${ARG_INCLUDE_DIR}/asn1crt_encoding_uper.c")
    list(APPEND HDRS "${CMAKE_CURRENT_BINARY_DIR}/${ARG_INCLUDE_DIR}/${ARG_INCLUDE_DIR}/asn1crt_encoding_uper.h")

    # Build argument list for asn1scc
    set(ASN1SCC_ARGS "")
    list(APPEND ASN1SCC_ARGS ${ARG_EXTRA_OPTS})
    list(APPEND ASN1SCC_ARGS "-o;${CMAKE_CURRENT_BINARY_DIR}/${ARG_INCLUDE_DIR}/${ARG_INCLUDE_DIR}")
    list(APPEND ASN1SCC_ARGS "-c")
    list(APPEND ASN1SCC_ARGS "-uPER")
    list(APPEND ASN1SCC_ARGS "--icdacn;icd.html")
    list(APPEND ASN1SCC_ARGS ${ASN_FILES})

    # Run asn1scc
    add_custom_command(
            OUTPUT ${SRCS} ${HDRS}
            COMMAND ${ASN1SCC_EXECUTABLE}
            ARGS ${ASN1SCC_ARGS}
            DEPENDS ${ASN_FILES}
            COMMAND_EXPAND_LISTS
            DEPENDS_EXPLICIT_ONLY
    )

    # Link the target to the genned files
    target_sources(${ASN1SCC_TARGET} PRIVATE ${SRCS} ${HDRS})
    target_include_directories(${ASN1SCC_TARGET} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/${ARG_INCLUDE_DIR}/")
    target_include_directories(${ASN1SCC_TARGET} PRIVATE "${ASN1SCC_DIR_NAME}")
endfunction()

