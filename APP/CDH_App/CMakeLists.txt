cmake_minimum_required(VERSION 3.20.0)

include("../../settings.cmake")

find_package(Zephyr)
project(CDH)

target_sources(app
        PRIVATE FILE_SET HEADERS BASE_DIRS src FILES
        src/main.h

        PRIVATE
        src/main.c
)


include("../../MIDDLEWARE/ASN1SCC/asn1scc.cmake")

asn1scc_generate_c(app ../_asn1/_systems.asn1 ../_asn1/bms.asn1 EXTRA_OPTS --slim -sm)

add_custom_command(
        TARGET app
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/icd.html ${CMAKE_CURRENT_LIST_DIR}/../_asn1/icd.html
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/icd.css ${CMAKE_CURRENT_LIST_DIR}/../_asn1/icd.css
)