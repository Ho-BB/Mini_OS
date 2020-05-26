

#include "list.h"


void list_init(list_t *l, freelist_alloc_t *alloc){

  l->list = (list_elem_t *) 0;
  l->length = 0;
  l->alloc = alloc;
}

/* Inserts at head */
u32 list_insert(list_t *l, u32 data){

  list_elem_t *new_elem =
    (list_elem_t *) fa_alloc(l->alloc, sizeof(list_elem_t));

  if(!new_elem)
    return 0;

  new_elem->data = data;
  new_elem->next = l->list;

  l->list = new_elem;
  l->length++;

  return 1;
}

/* Removes only the first one found if there is duplicates */
u32 list_remove(list_t *l, u32 data){
  
  list_elem_t *cur = l->list;
  list_elem_t *prev = (list_elem_t *) 0;

  while(cur){
    if(cur->data == data)
      break;

    prev = cur;
    cur = cur->next;
  }

  if(!cur)
    return 0;

  if(prev)
    prev->next = cur->next;
  else
    l->list = cur->next;

  fa_free(l->alloc, (u32) cur);

  l->length--;

  return 1;
}
