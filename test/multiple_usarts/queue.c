

#include "queue.h"


void queue_init(queue_t *q, u32 *buf, u32 size){

  q->queue = buf;
  q->size = size;
  q->len = 0;
  q->ihd = 0;
  q->itl = 0;
}

u32 queue_peek(queue_t *q){

  if(q->len)
    return q->queue[q->ihd];
  else
    return 0;
}

u32 queue_remove(queue_t *q){

  u32 tmp;
  
  if(q->len){
    tmp = q->queue[q->ihd];
    
    if(q->ihd == (q->size - 1))
      q->ihd = 0;
    else
      q->ihd++;

    q->len--;
    
    return tmp;
  }
  else
    return 0;
}

u32 queue_insert(queue_t *q, u32 e){

  if(q->len == q->size)
    return 0;

  q->queue[q->itl] = e;

  if(q->itl == (q->size - 1))
    q->itl = 0;
  else
    q->itl++;

  q->len++;

  return 1;
}

