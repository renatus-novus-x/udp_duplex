cmake_minimum_required(VERSION 3.15)

project(udp_duplex C)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)

# Single-source demo: cross-platform UDP ping-pong sample.
# Builds on:
#   - Windows (MSVC / MinGW)
#   - Linux
#   - macOS
#   - X68000 via elf2x68k (using the provided toolchain file)

add_executable(udp_duplex
  src/udp_duplex.c
)

# Windows-specific settings
if(WIN32)
  # Link with Winsock2
  target_link_libraries(udp_duplex PRIVATE ws2_32)
endif()
