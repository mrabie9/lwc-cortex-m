# Author: Jonas Gava
# Author: Luciano Ost

##### Project setup #####
# Name of your current project
PROJ_NAME = $(shell basename $(CURDIR))

# R = reference version (without protection)
SUFIX ?= R

# Linker script for STM32xx Device 
LINKER_SCRIPT = ../../Board/STM32F767ZITx_FLASH.ld

# Directory containing drivers source code
CMSIS_DIR = ../../Drivers/CMSIS
HAL_DIR = ../../Drivers/STM32F7xx_HAL_Driver

##### Arm Toolchain #####
TRIPLE  = arm-none-eabi
CC      = ${TRIPLE}-gcc
#~ CC      = ${TRIPLE}-g++
LD      = ${TRIPLE}-ld
AS      = ${TRIPLE}-as
GDB 	= ${TRIPLE}-gdb
OBJCOPY = ${TRIPLE}-objcopy

CLANG_FOLDER = /soft64/cross/llvm/mitigation/clang_6.0.1/bin
CLANG = $(CLANG_FOLDER)/clang
LLC = $(CLANG_FOLDER)/llc
CLANG_CFLAGS = -mlittle-endian -mthumb -mcpu=cortex-m7 -mfloat-abi=soft -O2


##### Compiler options #####
CCFLAGS = -g -T$(LINKER_SCRIPT) -Wall
CCFLAGS += --specs=nosys.specs

##### Project specific libraries #####
SRC_FILES += $(wildcard Src/*.c)
SRC_FILES += $(wildcard Src/*.cpp)
CFLAGS = -IInc

##### Drivers libraries and source code #####
CFLAGS += -I$(CMSIS_DIR)/Include
CFLAGS += -I$(CMSIS_DIR)/Device/ST/STM32F7xx/Include

SRC_FILES += ../../Startup/*
SRC_FILES += $(HAL_DIR)/Src/*
CFLAGS += -I$(HAL_DIR)/Inc

CLANG_CFLAGS += $(CFLAGS) -IInc/CMSIS_5/CMSIS/DSP/Include -IInc/CMSIS_5/CMSIS/NN/Include -I/usr/lib/arm-none-eabi/include -fshort-enums
CCFLAGS += $(CLANG_CFLAGS)

#~ all: $(PROJ_NAME).bin $(PROJ_NAME).hex
all: $(PROJ_NAME).elf

##### Flash code to board #####
flash:
	STM32_Programmer_CLI -c port=SWD -w $(shell pwd)/$(PROJ_NAME).hex -v -ob displ -rst

##### Print out disassembly of specified function using GDB #####
##### USAGE EXAMPLE: 	make disass FUNC=main 		    #####
disass: $(PROJ_NAME).elf
	$(GDB) $^ -batch -ex 'disass /r $(FUNC)'

clean:
	rm -f *.elf *.o *.ll *.s

$(PROJ_NAME).elf: $(SRC_FILES) $(PROJ_NAME).c
	$(CC) $(CCFLAGS) -o $@ $^ -DSTM32F767xx -Wl,--print-memory-usage
	cp $(PROJ_NAME).elf ../

#~ $(PROJ_NAME)-$(SUFIX).s: $(PROJ_NAME).ll
#~ 	$(LLC) -O2 $^ -o $@ $(EXTRA_FLAGS)

#~ $(PROJ_NAME).ll: $(PROJ_NAME).c
#~ 	$(CLANG) --target=$(TRIPLE) $(CLANG_CFLAGS) -S -emit-llvm $^ -o $@

$(PROJ_NAME).hex: $(PROJ_NAME).elf
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $@

$(PROJ_NAME).bin: $(PROJ_NAME).elf
	$(OBJCOPY) -O binary $^ $@

