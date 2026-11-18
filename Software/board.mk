##### Target board selection #####
# Included by each benchmark Makefile.
#   make              -> STM32F767 (Cortex-M7), ELFs in Build/
#   make BOARD=m55    -> NUCLEO-N657X0-Q (Cortex-M55), ELFs in Build/m55/
BOARD ?= m7
FLOAT_ABI ?= -mfloat-abi=soft

ifeq ($(BOARD),m7)
LINKER_SCRIPT = ../../Board/STM32F767ZITx_FLASH.ld
CMSIS_INC     = ../../Drivers/CMSIS/Include
DEVICE_INC    = ../../Drivers/CMSIS/Device/ST/STM32F7xx/Include
HAL_DIR       = ../../Drivers/STM32F7xx_HAL_Driver
CPU_FLAGS     = -mcpu=cortex-m7
DEVICE_DEF    = -DSTM32F767xx
STARTUP       = ../../Startup/startup_stm32f767xx.s
BOARD_SRC     = ../common/Src/*
BOARD_INC     = -I../common/Inc
BUILD_DIR     = ../../Build

else ifeq ($(BOARD),m55)
# STM32N657 runs from on-chip AXISRAM2 (no internal flash). The ELF is loaded
# by the debugger with the board in development boot mode (BOOT1 = 2-3).
LINKER_SCRIPT = ../../Board/STM32N657X0HXQ_AXISRAM2_fsbl.ld
CMSIS_INC     = ../../Drivers/CMSIS_6/Include
DEVICE_INC    = ../../Drivers/CMSIS/Device/ST/STM32N6xx/Include
HAL_DIR       = ../../Drivers/STM32N6xx_HAL_Driver
# No Helium (MVE) and no FPU, to match the soft-float M7 builds.
# For Helium/FPU results: make BOARD=m55 M55_CPU=-mcpu=cortex-m55 FLOAT_ABI=-mfloat-abi=hard
M55_CPU      ?= -mcpu=cortex-m55+nomve+nofp
CPU_FLAGS     = $(M55_CPU)
DEVICE_DEF    = -DSTM32N657xx
STARTUP       = ../../Startup/m55/startup_stm32n657x0hxq_fsbl.s
BOARD_SRC     = ../common/Src/main.c ../common_m55/Src/*
BOARD_INC     = -I../common_m55/Inc -I../common/Inc
BUILD_DIR     = ../../Build/m55

else
$(error Unknown BOARD '$(BOARD)' (expected m7 or m55))
endif
