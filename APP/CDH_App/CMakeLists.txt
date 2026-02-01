cmake_minimum_required(VERSION 3.20.0)

include("../../settings.cmake")

find_package(Zephyr)
project(CDH)

target_sources(app
        PRIVATE FILE_SET HEADERS BASE_DIRS src FILES
        src/main.h

        PRIVATE
        src/main.c
        ../../MIDDLEWARE/tinyprotocol/tinyprotocol.c
)

target_include_directories(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../MIDDLEWARE/tinyprotocol
        ${CMAKE_CURRENT_SOURCE_DIR}/../../MIDDLEWARE/shared_types
)
