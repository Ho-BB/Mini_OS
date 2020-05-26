

#ifndef __H743_UTILS__
#define __H743_UTILS__

#include "inc/typedef.h"
#include "inc/usart.h"

#define qtoa(q)					\
  (((q) < 10) ? ((q) + 48) : ((q) + 55))

void h743_usart_write_u8(volatile h743_usart_t *usartx, u8 byte);
void h743_usart_write_u32(volatile h743_usart_t *usartx, u32 word);

#endif
