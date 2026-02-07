cmake_minimum_required(VERSION 3.20.0)

include("../../settings.cmake")

find_package(Zephyr)
project(COMMS)

target_sources(app
        PRIVATE FILE_SET HEADERS BASE_DIRS src FILES
        src/main.h
        src/test_client.h
        src/server.h

        PRIVATE
        src/main.c
        src/test_client.c
        src/server.c
)
