####
# Risc-V Toolchain
#
####
# Set system name
set(CMAKE_SYSTEM_NAME "riscv64-unknown-elf")

# Location of pi toolchain
set(RISCV_TOOLCHAIN "$ENV{RISCV_TOOLCHAIN_DIR}")
if ("${RISCV_TOOLCHAIN}" STREQUAL "")
    set(RISCV_TOOLCHAIN "/home/jonathan/Documents/he-arc/ArtyA7SmpLinux/buildroot-build/host")
endif()
# Check toolchain directory exists
IF(NOT EXISTS "${RISCV_TOOLCHAIN}")
    message(FATAL_ERROR "RISCV toolchain not found at ${RISCV_TOOLCHAIN}. Install it, set RISCV_TOOLCHAIN_DIR environment variable, or choose another toolchain")
endif()
message(STATUS "Using RISCV toolchain at: ${RISCV_TOOLCHAIN}")
# specify the cross compiler
set(CMAKE_C_COMPILER "${RISCV_TOOLCHAIN}/bin/riscv64-unknown-elf-gcc")
set(CMAKE_CXX_COMPILER "${RISCV_TOOLCHAIN}/bin/riscv64-unknown-elf-g++")

# where is the target environment
set(CMAKE_FIND_ROOT_PATH  "${RISCV_TOOLCHAIN}")

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
