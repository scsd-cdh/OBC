#[[
Inspired by https://github.com/vpetrigo/arm-cmake-toolchains/blob/master/arm-gcc-toolchain.cmake

BSD 3-Clause License

Copyright (c) 2017, Vladimir Petrigo
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
]]

set(CMAKE_SYSTEM_NAME Generic-ELF)
set(CMAKE_SYSTEM_PROCESSOR ARM)

LIST(APPEND CMAKE_PROGRAM_PATH "${CMAKE_CURRENT_LIST_DIR}/../../toolchain/gcc/bin")
LIST(APPEND CMAKE_PROGRAM_PATH "${CMAKE_CURRENT_LIST_DIR}/../../toolchain/openocd/bin")

set(TOOLCHAIN_PREFIX arm-none-eabi-)
find_program(BINUTILS_PATH ${TOOLCHAIN_PREFIX}gcc NO_CACHE)

if (NOT BINUTILS_PATH)
    message(FATAL_ERROR "ARM GCC toolchain not found")
endif ()

get_filename_component(ARM_TOOLCHAIN_DIR ${BINUTILS_PATH} DIRECTORY)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
find_program(CMAKE_AR ${TOOLCHAIN_PREFIX}gcc-ar)
find_program(CMAKE_RANLIB ${TOOLCHAIN_PREFIX}gcc-ranlib)
find_program(CMAKE_OPENOCD openocd)

execute_process(COMMAND ${CMAKE_C_COMPILER} -print-sysroot
        OUTPUT_VARIABLE ARM_GCC_SYSROOT OUTPUT_STRIP_TRAILING_WHITESPACE)

# Option: BOARD_NAME
set(BOARD_NAME SAMV71Q21B CACHE STRING "Name of the board to use")
set_property(CACHE BOARD_NAME PROPERTY STRINGS "SAMV71Q21B")

string(TOUPPER ${BOARD_NAME} BOARD_NAME_UPPER)
string(TOLOWER ${BOARD_NAME} BOARD_NAME_LOWER)

# Set flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "LINKER:-Map='CDH.map' --specs=nano.specs LINKER:--gc-sections -L${CMAKE_CURRENT_LIST_DIR} -T${BOARD_NAME_LOWER}_flash.ld")
set(COMMON_FLAGS "-D__${BOARD_NAME_UPPER}__ -Wall -std=gnu99 -mthumb -mlong-calls -mcpu=cortex-m7 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -ffunction-sections")

# Default C compiler flags
set(CMAKE_C_FLAGS_INIT ${COMMON_FLAGS})
set(CMAKE_C_FLAGS_DEBUG_INIT "${CMAKE_C_FLAGS_INIT} -g3 -Og -pedantic -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_MINSIZEREL_INIT "-Os -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "-O2 -g -DNDEBUG")
# Default C++ compiler flags
set(CMAKE_CXX_FLAGS_INIT ${COMMON_FLAGS})
set(CMAKE_CXX_FLAGS_DEBUG_INIT "${CMAKE_CXX_FLAGS_INIT} -g3 -Og -pedantic -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "${CMAKE_CXX_FLAGS_INIT} -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "${CMAKE_CXX_FLAGS_INIT} -Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "${CMAKE_CXX_FLAGS_INIT} -O2 -g -DNDEBUG")

set(CMAKE_OBJCOPY ${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}objcopy CACHE INTERNAL "objcopy tool")
set(CMAKE_OBJDUMP ${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}objdump CACHE INTERNAL "objdump tool")
set(CMAKE_SIZE_UTIL ${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}size CACHE INTERNAL "size tool")

set(CMAKE_SYSROOT ${ARM_GCC_SYSROOT})
set(CMAKE_FIND_ROOT_PATH ${BINUTILS_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

link_libraries(m)
