

#include <stdio.h>
#include <stdlib.h>

#include "circular_buffer.h"

#define BUFSIZE     16

int main(int argc, char **argv){

  circular_buffer_t cb;
  u8 buf[BUFSIZE];
  u8 rbuf[BUFSIZE + 1];
  u32 overwritten, read;

  if(!cb_init(&cb, buf, BUFSIZE)){
    printf("Error cb init\n");
  }

  overwritten = cb_write(&cb, "hello", 5);
  printf("overwritten : %d\n", overwritten); //0
  read = cb_read(&cb, rbuf, BUFSIZE + 1);
  rbuf[read] = '\0';
  printf("read : %s\n", rbuf); // "hello"

  overwritten = cb_write(&cb, "hello yo this is too big", 24);
  printf("overwritten : %d\n", overwritten); // 8
  read = cb_read(&cb, rbuf, BUFSIZE + 1);
  rbuf[read] = '\0';
  printf("read : %s\n", rbuf); // " this is too big"

  printf("len : %d\n", cb.len); //0

  overwritten = cb_write(&cb, "hello yo this is too big", 24);
  printf("overwritten : %d\n", overwritten); // 8
  read = cb_read(&cb, rbuf, 5);
  rbuf[read] = '\0';
  printf("read : %s\n", rbuf); // " this"

  printf("len : %d\n", cb.len); //11

  overwritten = cb_write(&cb, " big", 4);
  printf("overwritten : %d\n", overwritten); // 0
  read = cb_read(&cb, rbuf, BUFSIZE + 1);
  rbuf[read] = '\0';
  printf("read : %s\n", rbuf); // " is too big big"

  printf("len : %d\n", cb.len); //0
  
  return 0;
}
