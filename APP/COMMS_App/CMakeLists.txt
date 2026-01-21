cmake_minimum_required(VERSION 3.20.0)

set(BOARD sam_v71_xult/samv71q21b)
set(CMAKE_FOLDER "Zephyr")

option(CDH_EVALBOARD "Setup project configuration for the eval board" ON)
if(CDH_EVALBOARD)
    add_compile_definitions(CDH_EVALBOARD)
endif()

find_package(Zephyr)
project(comms_app)


# Add ulog and link it
add_subdirectory("../../MIDDLEWARE/ULOG" "MIDDELEWARE/ULOG")
target_link_libraries(app PRIVATE MIDDLEWARE_ULOG)
target_link_libraries(MIDDLEWARE_ULOG PUBLIC zephyr_interface)
zephyr_linker_sources(SECTIONS "../../MIDDLEWARE/ULOG/ulog_snippet.ld")

target_sources(app
        PRIVATE
        FILE_SET HEADERS
        BASE_DIRS src
        FILES
            src/main.h
            src/logging.h
            src/logging_adapter.h

        PRIVATE
            src/main.c
            src/logging.c
            src/logging_adapter.c
)

