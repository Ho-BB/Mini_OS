

#ifndef __STACK_ALLOC__
#define __STACK_ALLOC__

#include "inc/stm32h743.h"

typedef struct stack_alloc{
  u32 free;
  u32 len;
} stack_alloc_t;

u32 sa_alloc(stack_alloc_t *alloc, u32 len);

void sa_free(stack_alloc_t *alloc, u32 ptr);

#endif
