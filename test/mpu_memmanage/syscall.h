

#ifndef __SYSCALL__
#define __SYSCALL__

#include "inc/typedef.h"

/*
  open_usart    : svc 0
  receive_byte  : svc 1
  transmit_byte : svc 2
  close_usart   : svc 3

  r0 should contain the return value according to AAPCS
  r0 should contain the argument according to AAPCS

  To retrieve the svc number : ((char *) stacked_pc)[-2]

  r3 will have to be modified on the stack to communicate the return value 
  of receive byte as the compiler generates a mov r0, r3 after the svc
*/

#define SVCN_OPEN     0
#define SVCN_RECEIVE  1
#define SVCN_TRANSMIT 2
#define SVCN_CLOSE    3

void open_usart();

u8 receive_byte();

void transmit_byte(u8 byte);

void close_usart();

#endif
