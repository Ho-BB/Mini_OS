

#include "inc/usart.h"


void h743_usart_conf(volatile h743_usart_t *usartx,
		     h743_usart_conf_t *conf,
		     u32 usart_clk_hz){

  u32 brr, presc;
  
  brr = (usart_clk_hz << (conf->oversampling == oversampling_8))
    / conf->baud_rate;

  presc = brr >> 16;
  if(presc){
    presc = 8; // prescaler divides by 32 (enough for all practical cases)
    brr = brr >> 5;
  }

  if(conf->oversampling == oversampling_8)
    brr = (brr & ~0x0F) | ((brr & 0x0F) >> 1);

  usartx->CR1 &= ~1; //disable usartx

  usartx->CR1 =
    (usartx->CR1 &
     ~((1 << 28) |
       (1 << 12) |
       (1 << 15) |
       (1 << 10) |
       (1 << 9)  |
       (15 << 5) |
       (1 << 3)  |
       (1 << 2))
     ) |
    ((u32) conf->word_length) |
    ((u32) conf->oversampling) |
    (conf->parity_en << 10) |
    (!conf->parity_even << 9) |
    conf->irq_en |
    (conf->transmitter_en << 3) |
    (conf->receiver_en << 2);

  usartx->CR2 =
    (usartx->CR2 &
     ~((1 << 19) |
       (3 << 12) |
       (1 << 11) |
       (1 << 10) |
       (1 << 9))
     ) |
    (conf->msb_first << 19) |
    ((u32) conf->stop_bits) |
    (conf->synchronous << 11) |
    (!conf->polarity_low << 10) |
    (!conf->phase_first << 9);

  usartx->CR3 = (conf->dma_tx_en << 7) | (conf->dma_rx_en << 6);

  usartx->PRESC = presc;
  usartx->BRR = brr;
}

void h743_usart_read_strn(volatile h743_usart_t *usartx, u8 *buf, u32 n){

  while(n--){
    h743_usart_wait_rxne(usartx);
    *buf++ = h743_usart_receive(usartx);
  }
}

void h743_usart_read_str(volatile h743_usart_t *usartx,
			 u8 *buf, u32 *n, u8 end){

  u32 i = 0;
  while(i < *n){
    h743_usart_wait_rxne(usartx);
    *buf = h743_usart_receive(usartx);
    if(*buf == end)
      break;

    buf++;
    i++;
  }

  *n = i;
}

void h743_usart_write_strn(volatile h743_usart_t *usartx, u8 *buf, u32 n){

  while(n--){
    h743_usart_wait_txe(usartx);
    h743_usart_transmit(usartx, *buf++);
  }
}

void h743_usart_write_str(volatile h743_usart_t *usartx, u8 *buf){

  while(*buf != '\0'){
    h743_usart_wait_txe(usartx);
    h743_usart_transmit(usartx, *buf++);
  }
}
