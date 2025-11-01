cmake_minimum_required(VERSION 4.0.2)
project(OBC)

# Load drivers
include_directories("${PROJECT_SOURCE_DIR}/APP/CDH_App/Config")
add_subdirectory("${PROJECT_SOURCE_DIR}/DRIVERS/CMSIS/5.4.0")
add_subdirectory("${PROJECT_SOURCE_DIR}/DRIVERS/SAMV71_DFP/2.4.182")
add_subdirectory("${PROJECT_SOURCE_DIR}/DRIVERS/SAMV71")
add_subdirectory("${PROJECT_SOURCE_DIR}/MIDDLEWARE")

# Link HAL driver
if("${BOARD_NAME_UPPER}" STREQUAL "SAMV71Q21B")
    link_libraries(DRIVER_SAMV71)
    link_libraries(MIDDLEWARE)
endif ()

# Load app
add_subdirectory("${PROJECT_SOURCE_DIR}/APP/CDH_App")
