

#ifndef __QUEUE__
#define __QUEUE__

#include "inc/stm32h743.h"

/*
  This queue is supposed to hold arbitrary pointers, hence the u32 type
  (0 is not a valid value to put in the queue as it is the same as a null
  pointer)
*/

typedef struct queue{
  u32 *queue;         //buffer
  u32 size;           //size in words of the buffer
  u32 len;            //number of elem in the queue
  u32 ihd;            //index of the head elem
  u32 itl;            //index of the first free space at tail
} queue_t;

void queue_init(queue_t *q, u32 *buf, u32 size);

u32 queue_peek(queue_t *q);

u32 queue_remove(queue_t *q);

u32 queue_insert(queue_t *q, u32 e);

#endif
