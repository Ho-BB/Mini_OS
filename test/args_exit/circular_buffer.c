

#include "circular_buffer.h"


/*
  inits the circular buffer, return 0 if it fails. The only fail cases is if
  size == 0 or buf == 0.
*/
u32 cb_init(circular_buffer_t *cb, u8 *buf, u32 size){

  if(buf == (u8 *) 0 || size == 0)
    return 0;

  cb->buf = buf;
  cb->size = size;
  cb->len = 0;
  cb->ihd = 0;
  cb->itl = 0;

  return 1;
}

/*
  writes len bytes from buf into cb and returns the number of bytes overwritten
*/
u32 cb_write(circular_buffer_t *cb, u8 *buf, u32 len){

  u32 overwritten = 0;
  u32 new_itl, new_ihd, new_len, copy_1, copy_2, i;

  if(!len)
    return 0;
  
  if(len > cb->size){
    overwritten = (len - cb->size);
    buf += (len - cb->size);
    len = cb->size;
  }

  if((cb->itl + len) >= cb->size){
    new_itl = cb->itl + len - cb->size;
    copy_1 = cb->size - cb->itl;
    copy_2 = len - copy_1;
  }
  else{
    new_itl = cb->itl + len;
    copy_1 = len;
    copy_2 = 0;
  }
  
  if(len > (cb->size - cb->len)){
    overwritten += (len - (cb->size - cb->len));
    new_len = cb->size;
    new_ihd = new_itl;
  }
  else{
    new_len = len + cb->len;
    new_ihd = cb->ihd;
  }

  i = cb->itl;
  while(copy_1--)
    cb->buf[i++] = *buf++;

  i = 0;
  while(copy_2--)
    cb->buf[i++] = *buf++;

  cb->len = new_len;
  cb->itl = new_itl;
  cb->ihd = new_ihd;

  return overwritten;
}

/*
  reads up to len bytes from cb into buf and returns the number of bytes
  actually read.
*/
u32 cb_read(circular_buffer_t *cb, u8 *buf, u32 len){

  u32 i;
  u32 new_ihd, new_len, copy_1, copy_2;

  if(len > cb->len)
    len = cb->len;

  new_len = cb->len - len;

  if((cb->ihd + len) >= cb->size){
    copy_1 = cb->size - cb->ihd;
    copy_2 = len - copy_1;
    new_ihd = copy_2;
  }
  else{
    copy_1 = len;
    copy_2 = 0;
    new_ihd = cb->ihd + len;
  }

  i = cb->ihd;
  while(copy_1--)
    *buf++ = cb->buf[i++];

  i = 0;
  while(copy_2--)
    *buf++ = cb->buf[i++];

  cb->len = new_len;
  cb->ihd = new_ihd;

  return len;
}
