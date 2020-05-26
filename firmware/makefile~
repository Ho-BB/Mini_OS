
.SECONDARY:

.SUFFIXES:

LIB_NAME:=libstm32f10xxx.a

BUILD_DIR:=build/

INCLUDE_DIRS:=-I . -I inc -I /usr/include

AS:=~/arm/2019/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-as
CC:=~/arm/2019/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-gcc
AR:=~/arm/2019/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-ar

CCFLAGS:=-mcpu=cortex-m3 -mthumb -Wall -O0
ASFLAGS:=-mcpu=cortex-m3 -mthumb -Wall

C_FILES:=$(shell ls src/*.c 2> /dev/null)
S_FILES:=$(shell ls src/*.s 2> /dev/null)

C_OBJS:=$(addprefix $(BUILD_DIR), $(C_FILES:.c=.o))
S_OBJS:=$(addprefix $(BUILD_DIR), $(S_FILES:.s=.o))

OBJ_FILES:=$(C_OBJS) $(S_OBJS)

LIB:=$(addprefix $(BUILD_DIR), $(LIB_NAME))


all : $(LIB)


$(LIB) : $(OBJ_FILES)
	ar cr $@ $^

$(S_OBJS) : $(BUILD_DIR)%.o : %.s
	$(AS) $(ASFLAGS) -o $@ -c $<

$(C_OBJS) : $(BUILD_DIR)%.o : %.c
	$(CC) $(CCFLAGS) $(INCLUDE_DIRS) -o $@ -c $<

clean :
	rm -f $(OBJ_FILES) $(LIB)
