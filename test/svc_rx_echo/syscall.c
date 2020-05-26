

#include "syscall.h"


__attribute__((naked))
void syscall_usart_open(){

  asm("\
svc 0\n						\
bx lr\n						\
");
}

__attribute__((naked))
void syscall_usart_close(){

  asm("\
svc 1\n						\
bx lr\n						\
");
}

__attribute__((naked))
void syscall_usart_write(u8 *buf, u32 len){

  asm("\
svc 2\n						\
bx lr\n						\
");
}

__attribute__((naked))
void syscall_usart_read(u8 *buf, u32 len, u32 *read){

  asm("\
svc 3\n						\
bx lr\n						\
");
}