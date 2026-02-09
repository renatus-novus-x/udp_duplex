# Toolchain file for building udp_duplex for X68000
# using elf2x68k (m68k-xelf-gcc) and libsocket.
#
# Usage (out-of-source build recommended):
#   mkdir build-x68k
#   cd build-x68k
#   cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-elf2x68k.cmake ..
#   cmake --build .
#
# This assumes that:
#   - m68k-xelf-gcc is in your PATH
#   - libsocket for elf2x68k is installed in the default search path
#     (so that -lsocket is enough)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR m68k)

# Compiler
set(CMAKE_C_COMPILER m68k-xelf-gcc)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Link with libsocket (for BSD-like socket API on X68000)
set(CMAKE_EXE_LINKER_FLAGS "-lsocket ${CMAKE_EXE_LINKER_FLAGS}")
