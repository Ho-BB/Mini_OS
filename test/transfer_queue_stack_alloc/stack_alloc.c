

#include "stack_alloc.h"


u32 sa_alloc(stack_alloc_t *alloc, u32 len){

  u32 allocated;
  
  if(alloc->len < len)
    allocated = 0;
  else{
    allocated = alloc->free;
    alloc->len -= len;
    alloc->free += len;
  }

  return allocated;
}

void sa_free(stack_alloc_t *alloc, u32 ptr){

  //stack allocators can't free
}
