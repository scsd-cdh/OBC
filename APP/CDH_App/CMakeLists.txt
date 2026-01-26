cmake_minimum_required(VERSION 3.20.0)

include("../../settings.cmake")

find_package(Zephyr)
project(CDH)

target_sources(app
        PRIVATE
        FILE_SET HEADERS
        BASE_DIRS src
        FILES
        src/main.h
#        src/logging.h

        PRIVATE
        src/main.c
#        src/logging.c
)

