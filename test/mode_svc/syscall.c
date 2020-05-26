

#include "syscall.h"

#define SVC_OPEN     asm("svc 0")
#define SVC_RECEIVE  asm("svc 1")
#define SVC_TRANSMIT asm("svc 2")
#define SVC_CLOSE    asm("svc 3")

__attribute__((naked))
void open_usart(){

  SVC_OPEN;
  asm("bx lr");
}

__attribute__((naked))
u8 receive_byte(){

  SVC_RECEIVE;
  asm("bx lr");
}

__attribute__((naked))
void transmit_byte(u8 byte){

  SVC_TRANSMIT;
  asm("bx lr");
}

__attribute__((naked))
void close_usart(){

  SVC_CLOSE;
  asm("bx lr");
}
