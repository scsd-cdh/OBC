cmake_minimum_required(VERSION 3.20.0)

include("../../settings.cmake")

# Receiver-specific devicetree overlay: UART2 only (see app.overlay).
# Must be set BEFORE find_package(Zephyr).
set(DTC_OVERLAY_FILE "${CMAKE_CURRENT_LIST_DIR}/app.overlay")

find_package(Zephyr)
project(CDH_RX)

target_sources(app
        PRIVATE
        src/main.c
        ../../MIDDLEWARE/UART/uart_transport.c
)

# Same ASN.1 schemas as CDH_App so both ends of the link always agree on the
# wire format. (No POST_BUILD icd.html copy here; CDH_App already does that.)
include("../../MIDDLEWARE/ASN1SCC/asn1scc.cmake")

asn1scc_generate_c(app
        ../../APP/_asn1/_systems.asn1
        ../../APP/_asn1/bms.asn1
        ../../APP/_asn1/pds.asn1
        ../../APP/_asn1/comms.asn1
        EXTRA_OPTS --slim -sm
)

target_include_directories(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../MIDDLEWARE/UART/include
)
