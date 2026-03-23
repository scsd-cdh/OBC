cmake_minimum_required(VERSION 4.0)
project("ulog_test" C)

add_link_options("-T${CMAKE_CURRENT_LIST_DIR}/../../ProjectFiles/CDH/ulog.ld")
add_subdirectory("../../MIDDLEWARE/ULOG" ulog)
add_executable(ulog_test main.c)
target_compile_options(ulog_test PRIVATE "-save-temps")
target_link_libraries(ulog_test MIDDLEWARE_ULOG)

