

#ifndef __CIRCULAR_BUFFER__
#define __CIRCULAR_BUFFER__

#include "inc/stm32h743.h"

typedef struct circular_buffer{
  u8 *buf;          //buffer
  u32 size;         //size of buffer
  u32 len;          //number of bytes in buffer
  u32 ihd;          //index of the first byte
  u32 itl;          //index of the first free space
} circular_buffer_t;

/*
  inits the circular buffer, return 0 if it fails. The only fail cases is if
  size == 0 or buf == 0.
*/
u32 cb_init(circular_buffer_t *cb, u8 *buf, u32 size);

/*
  writes len bytes from buf into cb and returns the number of bytes overwritten
*/
u32 cb_write(circular_buffer_t *cb, u8 *buf, u32 len);

/*
  reads up to len bytes from cb into buf and returns the number of bytes
  actually read.
*/
u32 cb_read(circular_buffer_t *cb, u8 *buf, u32 len);

#endif
