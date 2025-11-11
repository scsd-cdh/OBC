cmake_minimum_required(VERSION 4.0.2)
project(OBC)

# Load drivers
add_subdirectory("${PROJECT_SOURCE_DIR}/DRIVERS/CMSIS/5.4.0")
add_subdirectory("${PROJECT_SOURCE_DIR}/DRIVERS/SAMV71_DFP/2.4.182")
add_subdirectory("${PROJECT_SOURCE_DIR}/DRIVERS/SAMV71")
add_subdirectory("${PROJECT_SOURCE_DIR}/DRIVERS/CDH_COMMON")

# Load middleware
add_subdirectory("${PROJECT_SOURCE_DIR}/MIDDLEWARE/commsdrive")
add_subdirectory("${PROJECT_SOURCE_DIR}/MIDDLEWARE/tinyprotocol")

# Load app
add_subdirectory("${PROJECT_SOURCE_DIR}/APP/CDH_App")

# Example module, don't actually include it in the build lol. Feel free to uncomment it if you wanna play around with
# the example.
#add_subdirectory("${PROJECT_SOURCE_DIR}/MIDDLEWARE/EXAMPLE")
add_subdirectory("${PROJECT_SOURCE_DIR}/MIDDLEWARE/ULOG")
