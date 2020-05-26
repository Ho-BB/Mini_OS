

#include "inc/utils.h"


void h743_usart_write_u8(volatile h743_usart_t *usartx, u8 byte){

  h743_usart_wait_txe(usartx);
  h743_usart_transmit(usartx, qtoa(byte >> 4));
  h743_usart_wait_txe(usartx);
  h743_usart_transmit(usartx, qtoa(byte & 0x0F));
}

void h743_usart_write_u32(volatile h743_usart_t *usartx, u32 word){

  h743_usart_wait_txe(usartx);
  h743_usart_transmit(usartx, qtoa(word >> 28));
  h743_usart_wait_txe(usartx);
  h743_usart_transmit(usartx, qtoa((word >> 24) & 0x0F));
  
  h743_usart_wait_txe(usartx);
  h743_usart_transmit(usartx, qtoa((word >> 20) & 0x0F));
  h743_usart_wait_txe(usartx);
  h743_usart_transmit(usartx, qtoa((word >> 16) & 0x0F));
  
  h743_usart_wait_txe(usartx);
  h743_usart_transmit(usartx, qtoa((word >> 12) & 0x0F));
  h743_usart_wait_txe(usartx);
  h743_usart_transmit(usartx, qtoa((word >> 8) & 0x0F));
  
  h743_usart_wait_txe(usartx);
  h743_usart_transmit(usartx, qtoa((word >> 4) & 0x0F));
  h743_usart_wait_txe(usartx);
  h743_usart_transmit(usartx, qtoa(word & 0x0F));
}
