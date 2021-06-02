####
# FreeRTOS.cmake: CMake to compile F' on Posix/Linux Simulator for FreeRTOS
# 
# Can be used on Linux (tested on Ubuntu 18.04) or cross compiled for Raspberry
#
# In order to cross compile, the [raspberry pi build tools](https://github.com/raspberrypi/tools)
# should be cloned in `/opt/rpi/`. If they are cloned elsewhere, the user must set the 
# environment variable `RPI_TOOLCHAIN_DIR` to the full path to the 
# `arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf` directory before running CMake or
# `fprime-util generate`.
#  Use `fprime-util generate [raspberrypi]` for cross compilation
#
####
add_definitions(-DTGT_OS_TYPE_FREERTOS_SIM)

# In our case the Posix/Linux simulator from FreeRTOS will use Posix
set(FPRIME_USE_POSIX ON)
# Requires threading library, use cmake to find appropriate library
message(STATUS "Requiring thread library")
FIND_PACKAGE ( Threads REQUIRED )

# FreeRTOS repository has to be manually cloned somewhere 
# https://github.com/FreeRTOS/FreeRTOS.gitcd ..
# Tested version : FreeRTOS tag 202012.00-LTS
# FreeRTOS/Sources -> bug with V10.4.3, use V10.4.2 (@todo debug ?)
# FreeRTOS has not been added as a git submodule because it caused lag
# 
# Current config:
# 05_FS     <- Chess FS repository
#   ├── App
#   ├── fprime
#   └── FreeRTOS  <- FreeRTOS main repository
#       ├── FreeRTOS-Plus
#       ├── FreeRTOS
#       └── ...
# 
# Relative FreeRTOS from current CMake source directory (05_FS/App/)
set(FREERTOS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../FreeRTOS/FreeRTOS")

include_directories(
    ${FPRIME_FRAMEWORK_PATH}/Os/FreeRTOSSim                 # FreeRTOSConfig.h
    ${FREERTOS_DIR}/Source/include                          # FreeRTOS.h
    ${FREERTOS_DIR}/Source/portable/ThirdParty/GCC/Posix        # FreeRTOS Posix simulator
    ${FREERTOS_DIR}/Source/portable/ThirdParty/GCC/Posix/utils
    ${FREERTOS_DIR}/Demo/Common/include
    ${FREERTOS_DIR}/Demo/Posix_GCC
)

# Generic Code
FILE(GLOB FREERTOS_SOURCES
    ${FREERTOS_DIR}/Source/*.c
)

# Memory manager (use malloc() / free() wraps for thread safety)
list(APPEND FREERTOS_SOURCES "${FREERTOS_DIR}/Source/portable/MemMang/heap_3.c")

# Plaform Port Specific Code
list(APPEND FREERTOS_SOURCES "${FREERTOS_DIR}/Source/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c")
list(APPEND FREERTOS_SOURCES "${FREERTOS_DIR}/Source/portable/ThirdParty/GCC/Posix/port.c")
list(APPEND FREERTOS_SOURCES "${FPRIME_FRAMEWORK_PATH}/Os/FreeRTOSSim/assert.c")

# C flags
add_definitions(-DprojCOVERAGE_TEST=0)
add_definitions(-D_WINDOWS_)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")

add_library(freertos STATIC ${FREERTOS_SOURCES})

include_directories(SYSTEM "${FPRIME_FRAMEWORK_PATH}/Fw/Types/FreeRTOSSim")
