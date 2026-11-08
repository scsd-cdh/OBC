cmake_minimum_required(VERSION 3.20.0)

include("../../settings.cmake")

find_package(Zephyr)
project(CDH)

list(APPEND DTS_ROOT "${CMAKE_CURRENT_LIST_DIR}")

target_sources(app
        PRIVATE FILE_SET HEADERS BASE_DIRS src/include FILES
        src/include/main.h
        src/include/pins.h
        src/include/cdh_pds_example.h
        src/include/lfp_i2c_single.h
        src/include/main.h
        src/include/threads.h

        PRIVATE
        src/cdh_pds_example.c
        src/lfp_i2c_single.c
        src/main.c
        src/pins.c
        ../../MIDDLEWARE/tinyprotocol/tinyprotocol.c
)


include("../../MIDDLEWARE/ASN1SCC/asn1scc.cmake")

asn1scc_generate_c(app ../_asn1/_systems.asn1 ../_asn1/pds.asn1 EXTRA_OPTS --slim -sm)

add_custom_command(
        TARGET app
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/icd.html ${CMAKE_CURRENT_LIST_DIR}/../_asn1/icd.html
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/icd.css ${CMAKE_CURRENT_LIST_DIR}/../_asn1/icd.css
)
target_include_directories(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../MIDDLEWARE/shared_types
)
