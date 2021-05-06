##
# riscv64-unknown-elf.cmake:
#
# A platform file supporting the riscv. This is the F prime platform support file that combines with the
# toolchain file found at [../toolchain/riscv.cmake](../toolchain/riscv.cmake).
##

# RiscV is Linux based, so include the common Linux items
include("${CMAKE_CURRENT_LIST_DIR}/Linux-common.cmake")
