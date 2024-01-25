# This file is Copyright (c) 2017-2021 MicroPython Developers
# This file is Copyright (c) 2021-2024 Victor Suarez Rovere <suarezvictor@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause


# Include the core environment definitions; this will set $(TOP).
include ../../py/mkenv.mk

# Include py core make definitions
include $(TOP)/py/py.mk

BUILD ?= ./build
CPU_FLAGS = -march=rv64gcv0p7_zfh_xtheadc -mabi=lp64d -mtune=c906 -mcmodel=medlow -fno-stack-protector -mstrict-align

# Set Cross-Compiler.
CROSS_COMPILE ?= riscv64-unknown-linux-gnu-
MACH_DIRECTORY := /media/vsuarez/elocaldata/SCRATCH/gpu2d-notabug.org/target-cpu/f133-bare

# Set ASFLAGS / CFLAGS.
DEFINES		+=	-D__RISCV64__

ASFLAGS		+=	-std=gnu99 $(DEFINES) -ffunction-sections -fdata-sections -ffreestanding 
CFLAGS		+=	-std=gnu99 $(DEFINES) -ffunction-sections -fdata-sections -ffreestanding
CPPFLAGS	+=	 $(DEFINES) -ffunction-sections -fdata-sections -ffreestanding
LDFLAGS		+=	-Wl,-gc-sections


CFLAGS += $(CPUFLAGS)
#CFLAGS += $(INC) -Wall -Werror -std=gnu11 -ggdb $(COPT)
CFLAGS += $(INC) -Wall -Wno-error -std=gnu11 $(COPT)
CFLAGS += -Og -Wdouble-promotion

# Set Include directories.
INC += -I. -I../..
INC += -I$(BUILD)
INC += -I$(MACH_DIRECTORY)
LINKER_FILE = linker.ld

# Set LDFLAGS. NOTE: **** lack of -static makes the project hangs ****
LDFLAGS += -nostartfiles -static -T $(LINKER_FILE) -Wl,-Map=$@.map -Wl,--cref

# Set Libraries.
LIBS = -Wl,-lm

# Set Sources.
SRC_C = \
	main.c \
	startup.c \
	mpmain.c \
	modmachine.c \
	modutime.c \
	modf133.c \
	lib/utils/stdout_helpers.c \
	lib/utils/pyexec.c \
	lib/timeutils/timeutils.c \
	lib/mp-readline/readline.c

#lib sources with no need of QSTR extraction
SRC_MACH = mach-f133.c


DRIVERS_SRC_C = # $(addprefix drivers/, bus/softspi.c)
EXTMOD_SRC_C = # $(addprefix extmod/,	machine_spi.c machine_i2c.c)

CRT0_S = crt0.S
CRT0_O = $(BUILD)/crt0.o

SRC_S = 
OBJ =  $(CRT0_O) $(addprefix $(BUILD)/, $(SRC_S:.S=.o)) $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(DRIVERS_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(EXTMOD_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MACH:.c=.o))

#$(info $$OBJ is: ${OBJ})


all: $(BUILD)/firmware.bin
	xfel ddr f133
	xfel write 0x40000000 $(BUILD)/firmware.bin
	xfel exec 0x40000000

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SRC_LIB)

# CRT0 override for normal assembler rule.
$(CRT0_O): $(CRT0_S) $(LINKER_FILE)
	$(CC) -c $(ASFLAGS) $(CFLAGS) $(CRT0FLAGS) -o $@ $<

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O binary $^ $(BUILD)/firmware.bin

include ../../py/mkrules.mk
