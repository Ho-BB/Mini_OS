

#ifndef __SYSCALL__
#define __SYSCALL__

#include "inc/stm32h743.h"

#define SYSCALL_USART_OPEN         0
#define SYSCALL_USART_CLOSE        1
#define SYSCALL_USART_WRITE        2
#define SYSCALL_USART_READ         3
#define SYSCALL_SLEEP              4
#define SYSCALL_LAUNCH             5

__attribute__((naked))
void syscall_usart_open();

__attribute__((naked))
void syscall_usart_close();

__attribute__((naked))
void syscall_usart_write(u8 *buf, u32 len);

__attribute__((naked))
void syscall_usart_read(u8 *buf, u32 len, u32 *read);

__attribute__((naked))
void syscall_sleep(u32 ms);

__attribute__((naked))
void syscall_launch(u8 *prog_name);

#endif
