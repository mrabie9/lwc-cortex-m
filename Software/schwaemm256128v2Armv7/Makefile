# Jonas Gava 
# updated Luciano Ost
# 2023-24 update by Mohamed Rabie

##### Project setup #####
# Name of your current project
PROJ_NAME = $(shell basename "$(CURDIR)")
include ../board.mk
#~ PROJ_NAME = ascon128

# Linker script, drivers, CPU flags: see ../board.mk (BOARD=m7|m55)


##### Arm Toolchain #####
TRIPLE  = arm-none-eabi
CC      = ${TRIPLE}-gcc
CLANG	= /soft64/cross/llvm/mitigation/clang_6.0.1/bin/clang
LD      = ${TRIPLE}-ld
AS      = ${TRIPLE}-as
GDB 	= ${TRIPLE}-gdb
OBJCOPY = ${TRIPLE}-objcopy
SZ 		= $(TRIPLE)-size

##### Compiler options #####
CFLAGS = -g -T$(LINKER_SCRIPT) -w
CFLAGS += -mlittle-endian -mthumb $(CPU_FLAGS)
CFLAGS += $(FLOAT_ABI) --specs=nosys.specs

##### Project specific libraries #####
SRC_FILES += $(wildcard Src/*.c)
CFLAGS += -IInc

##### Drivers libraries and source code #####
CFLAGS += -I$(CMSIS_INC)
CFLAGS += -I$(DEVICE_INC)

SRC_FILES += $(STARTUP)
SRC_FILES += $(HAL_DIR)/Src/*
SRC_FILES += $(BOARD_SRC)
CFLAGS += -I$(HAL_DIR)/Inc
CFLAGS += $(BOARD_INC)

all: $(PROJ_NAME).elf 
#$(PROJ_NAME).bin $(PROJ_NAME).hex

##### Flash code to board #####
flash:
	STM32_Programmer_CLI -c port=SWD -w $(shell pwd)/$(PROJ_NAME).hex -v -ob displ -rst

##### Print out disassembly of specified function using GDB #####
##### USAGE EXAMPLE: 	make disass FUNC=main 		    #####
disass: $(PROJ_NAME).elf
	$(GDB) $^ -batch -ex 'disass /r $(FUNC)'

clean:
	rm -f $(BUILD_DIR)/$(PROJ_NAME)_Pwr.bin $(BUILD_DIR)/$(PROJ_NAME)_Pwr.hex $(BUILD_DIR)/$(PROJ_NAME)_Pwr.elf $(BUILD_DIR)/$(PROJ_NAME)_Pwr.o

$(PROJ_NAME).elf: $(SRC_FILES) 
	mkdir -p $(BUILD_DIR)
	$(CC) -O3 $(CFLAGS)  -o $(BUILD_DIR)/$(PROJ_NAME)_Pwr_O3.elf $^ Src/sparkle384f_v7m.S $(DEVICE_DEF) -Wl,--print-memory-usage #-fstack-usage
	$(CC) -O2 $(CFLAGS)  -o $(BUILD_DIR)/$(PROJ_NAME)_Pwr_O2.elf $^ Src/sparkle384f_v7m.S $(DEVICE_DEF) -Wl,--print-memory-usage #-fstack-usage
	$(CC) -Os $(CFLAGS)  -o $(BUILD_DIR)/$(PROJ_NAME)_Pwr_Os.elf $^ Src/sparkle384f_v7m.S  $(DEVICE_DEF) -Wl,--print-memory-usage #-fstack-usage
	$(SZ) $(BUILD_DIR)/$(PROJ_NAME)_Pwr_O3.elf
	$(SZ) $(BUILD_DIR)/$(PROJ_NAME)_Pwr_O2.elf
	$(SZ) $(BUILD_DIR)/$(PROJ_NAME)_Pwr_Os.elf


#$(PROJ_NAME).elf: $(SRC_FILES) $(PROJ_NAME).o
#	$(CC) -O3 $(CFLAGS) -o $@ $^ #Src/$(PROJ_NAME)-$(SUFIX).o

#$(PROJ_NAME).o: $(PROJ_NAME).c
#	$(CLANG) --target=arm-none-eabi -mcpu=cortex-m4 -mfloat-abi=soft -mthumb -mlittle-endian -IInc -I/soft64/cross/gcc/gcc-arm-none-eabi-10.3/arm-none-eabi/include/ -fshort-enums -DKEY=$(KEY) -DDEBUG $(EXTRA_FLAGS) -c -o $@ $<

#~ $(PROJ_NAME).hex: $(PROJ_NAME).elf
#~ 	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $@

#~ $(PROJ_NAME).bin: $(PROJ_NAME).elf
#~ 	$(OBJCOPY) -O binary $^ $@

