

#ifndef __LIST__
#define __LIST__

#include "inc/stm32h743.h"
#include "freelist_alloc.h"

/*
  List to hold arbitrary pointers 
  (care must be taken to have an homogenous collection)
*/

typedef struct list_elem{
  u32 data;
  struct list_elem *next;
} list_elem_t;

typedef struct list{
  list_elem_t *list;
  u32 length;
  freelist_alloc_t *alloc;
} list_t;

void list_init(list_t *l, freelist_alloc_t *alloc);

/* Inserts at head */
u32 list_insert(list_t *l, u32 data);

/* Removes only the first one found if there is duplicates */
u32 list_remove(list_t *l, u32 data);

#endif
