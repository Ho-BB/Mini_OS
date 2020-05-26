

#include "syscall.h"


__attribute__((naked))
void syscall_usart_open(u8 *usart){

  asm("\
svc 0\n						\
bx lr\n						\
");
}

__attribute__((naked))
void syscall_usart_close(u8 *usart){

  asm("\
svc 1\n						\
bx lr\n						\
");
}

__attribute__((naked))
void syscall_usart_write(u8 *usart, u8 *buf, u32 len){

  asm("\
svc 2\n						\
bx lr\n						\
");
}

__attribute__((naked))
void syscall_usart_read(u8 *usart, u8 *buf, u32 len, u32 *read){

  asm("\
svc 3\n						\
bx lr\n						\
");
}

__attribute__((naked))
void syscall_sleep(u32 ms){

  asm("\
svc 4\n						\
bx lr\n						\
");
}

__attribute__((naked))
void syscall_launch(u8 *prog_name){

  asm("\
svc 5\n						\
bx lr\n						\
");
}

__attribute__((naked))
void syscall_exit(){

  asm("\
svc 6\n						\
bx lr\n						\
");
}
