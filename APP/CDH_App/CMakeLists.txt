add_executable(CDH)

target_link_libraries(CDH PRIVATE DRIVER_CDH_COMMON)
target_link_libraries(CDH PRIVATE COMMSDRIVE)
target_link_libraries(CDH PRIVATE MIDDLEWARE_ULOG)

# Add new c files here
target_sources(CDH
    # Headers, used by IDEs but doesn't really matter beyond that
    PRIVATE
        FILE_SET HEADERS
        BASE_DIRS .
        FILES
            main.h
            logging.h
            temp_ctrl.h

    # Actual source code
    PRIVATE
        main.c
        logging.c
        temp_ctrl.c
)

# Post compile preparation steps
add_custom_command(
    TARGET CDH POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary "CDH.elf" "CDH.bin"
    COMMAND ${CMAKE_OBJCOPY} -O ihex -R .eeprom -R .fuse -R .lock -R .signature  "CDH.elf" "CDH.hex"
    COMMAND ${CMAKE_OBJCOPY} -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O binary "CDH.elf" "CDH.eep" || exit 0
    COMMAND ${CMAKE_OBJDUMP} -h -S "CDH.elf" > "CDH.lss"
    COMMAND ${CMAKE_OBJCOPY} -O srec -R .eeprom -R .fuse -R .lock -R .signature  "CDH.elf" "CDH.srec"
    COMMAND ${CMAKE_SIZE_UTIL} "CDH.elf"
    BYPRODUCTS CDH.bin CDH.hex CDH.eep CDH.lss CDH.srec
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM
)

add_custom_target(CDH_FLASH
    COMMAND ${CMAKE_OPENOCD} -f ${PROJECT_SOURCE_DIR}/ProjectFiles/CDH/openocd.cfg -c "tcl port disabled" -c "gdb port disabled" -c "program CDH.elf verify" -c reset -c shutdown
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS CDH
)
