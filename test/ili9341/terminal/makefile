
.SECONDARY:

.SUFFIXES:

LIB:=~/src/stm32h743i/firmware_v1/build/libstm32h743ii.a
LDSCRIPT:=~/src/stm32h743i/firmware_v1/ld/stm32h743ii.ld
INCLUDE:=-I ~/src/stm32h743i/firmware_v1/

AS:=~/arm/2019/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-as
CC:=~/arm/2019/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-gcc
LD:=~/arm/2019/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-ld
OBJCOPY:=~/arm/2019/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-objcopy

CCFLAGS:=-mcpu=cortex-m7 -mthumb -Wall -O0
ASFLAGS:=-mcpu=cortex-m7 -mthumb -Wall

C_FILES:=$(shell ls *.c 2> /dev/null)
S_FILES:=$(shell ls *.s 2> /dev/null)

C_OBJS:=$(C_FILES:.c=.o)
S_OBJS:=$(S_FILES:.s=.o)

OBJ_FILES:=$(C_OBJS) $(S_OBJS)

PROG:=prog

all : $(PROG).bin

$(PROG).bin : $(PROG).elf
	$(OBJCOPY) $< $@ -O binary

$(PROG).elf : $(OBJ_FILES) ../font/ctrld_font.o
	$(LD) -T $(LDSCRIPT) -o $@ $^ $(LIB)

$(C_OBJS) : %.o : %.c
	$(CC) -o $@ -c $< $(CCFLAGS) $(INCLUDE)

$(S_OBJS) : %.o : %.s
	$(AS) -o $@ $< $(ASFLAGS)

