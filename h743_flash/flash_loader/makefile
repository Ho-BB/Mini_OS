
LIB:=
LDSCRIPT:=stm32h743i.ld
INCLUDE:=

AS:=~/arm/2019/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-as
CC:=~/arm/2019/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-gcc
LD:=~/arm/2019/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-ld
OBJCOPY:=~/arm/2019/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-objcopy

CSRC:=$(shell ls *.c 2> /dev/null)
SSRC:=$(shell ls *.s 2> /dev/null)

COBJ:=$(CSRC:.c=.o)
SOBJ:=$(SSRC:.s=.o)

PROG:=prog

all : $(PROG).bin

$(PROG).bin : $(PROG).elf
	$(OBJCOPY) $< $@ -O binary

$(PROG).elf : $(COBJ) $(SOBJ)
	$(LD) $(LDSCRIPT) -o $@ $^ $(LIB)

$(COBJ) : %.o : %.c
	$(CC) -o $@ -c $< -mcpu=cortex-m7 -mthumb -Wall -O0 $(INCLUDE)

$(SOBJ) : %.o : %.s
	$(AS) -mcpu=cortex-m7 -o $@ $<
