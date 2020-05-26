

#include "freelist_alloc.h"


u32 fa_init(freelist_alloc_t *fa, u32 ptr, u32 len){

  if(ptr == 0 || len < 12 || ptr & 3 || len & 3 || (ptr + len) < ptr)
    return 0;
  
  fa->freelist = (free_block_t *) ptr;
  fa->freelist->len = len - 8;
  fa->freelist->next = (free_block_t *) 0;

  fa->free = len;
  fa->used = 0;
  fa->frag = 1;

  return 1;
}

u32 fa_alloc(freelist_alloc_t *fa, u32 bytes){

  free_block_t *cur, *prev, *tmp;
  
  if(bytes == 0)
    return 0;
  
  if(bytes & 3)
    bytes = bytes + (4 - (bytes & 3));

  prev = (free_block_t *) 0;
  cur = fa->freelist;

  while(cur){

    if(cur->len == bytes){
      if(prev)
	prev->next = cur->next;
      else
	fa->freelist = cur->next;

      fa->free -= (cur->len + 8);
      fa->used += (cur->len + 8);
      fa->frag--;

      return ((u32) cur) + 8;
    }
    else if(cur->len >= bytes + 12){
      tmp = (free_block_t *) (((u32) cur) + bytes + 8);
      tmp->len = cur->len - bytes - 8;
      tmp->next = cur->next;

      if(prev)
	prev->next = tmp;
      else
	fa->freelist = tmp;

      cur->len = bytes;

      fa->free -= (cur->len + 8);
      fa->used += (cur->len + 8);

      return ((u32) cur) + 8;
    }

    prev = cur;
    cur = cur->next;
  }

  return 0;
}

u32 fa_free(freelist_alloc_t *fa, u32 ptr){

  free_block_t *cur, *prev, *tmp;
  
  if(ptr <= 8 || ptr & 3)
    return 0;

  prev = (free_block_t *) 0;
  cur = fa->freelist;
  tmp = (free_block_t *) (ptr - 8);

  while(cur && (cur < tmp)){
    prev = cur;
    cur = cur->next;
  }

  if(!cur && !prev){
    tmp->next = (free_block_t *) 0;
    fa->freelist = tmp;

    fa->free += (tmp->len + 8);
    fa->used -= (tmp->len + 8);
    fa->frag++;

    return 1;
  }
  else if(!cur){
    if(((u32) prev) + prev->len + 8 > ((u32) tmp))
      return 0;

    if(((u32) prev) + prev->len + 8 == ((u32) tmp)){
      prev->len += (tmp->len + 8);

      fa->free += (tmp->len + 8);
      fa->used -= (tmp->len + 8);

      return 1;
    }
    else{
      tmp->next = (free_block_t *) 0;
      prev->next = tmp;

      fa->free += (tmp->len + 8);
      fa->used -= (tmp->len + 8);
      fa->frag++;

      return 1;
    }
  }
  else if(!prev){
    if(((u32) tmp) + tmp->len + 8 > ((u32) cur))
      return 0;

    if(((u32) tmp) + tmp->len + 8 == ((u32) cur)){
      fa->freelist = tmp;
      tmp->next = cur->next;
      tmp->len += (cur->len + 8);

      fa->free += (tmp->len + 8);
      fa->used -= (tmp->len + 8);

      return 1;
    }
    else{
      fa->freelist = tmp;
      tmp->next = cur;

      fa->free += (tmp->len + 8);
      fa->used -= (tmp->len + 8);
      fa->frag++;

      return 1;
    }
  }
  else{
    if(((u32) prev) + prev->len + 8 > ((u32) tmp) ||
       ((u32) tmp) + tmp->len + 8 > ((u32) cur))
      return 0;

    if(((u32) prev) + prev->len + 8 == ((u32) tmp) &&
       ((u32) tmp) + tmp->len + 8 == ((u32) cur)){
      prev->next = cur->next;
      prev->len += (tmp->len + cur->len + 16);

      fa->free += (tmp->len + 8);
      fa->used -= (tmp->len + 8);
      fa->frag--;

      return 1;
    }
    else if(((u32) prev) + prev->len + 8 == ((u32) tmp)){
      prev->len += (tmp->len + 8);

      fa->free += (tmp->len + 8);
      fa->used -= (tmp->len + 8);

      return 1;
    }
    else if(((u32) tmp) + tmp->len + 8 == ((u32) cur)){
      tmp->len += (cur->len + 8);
      tmp->next = cur->next;
      prev->next = tmp;

      fa->free += (tmp->len + 8);
      fa->used -= (tmp->len + 8);

      return 1;
    }
    else{
      prev->next = tmp;
      tmp->next = cur;

      fa->free += (tmp->len + 8);
      fa->used -= (tmp->len + 8);
      fa->frag++;

      return 1;
    }
  }
}
