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

asn1scc_generate_c(app bms.asn1)