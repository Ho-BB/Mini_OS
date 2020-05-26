

#ifndef __H743_USART__
#define __H743_USART__

#include "inc/typedef.h"

typedef struct h743_usart{
  u32 CR1;
  u32 CR2;
  u32 CR3;
  u32 BRR;
  u32 GTPR;
  u32 RTOR;
  u32 RQR;
  u32 ISR;
  u32 ICR;
  u32 RDR;
  u32 TDR;
  u32 PRESC;
} h743_usart_t;

/* 4 USART | 4 UART */
#define USART1                ((volatile h743_usart_t *) 0x40011000)
#define USART2                ((volatile h743_usart_t *) 0x40004400)
#define USART3                ((volatile h743_usart_t *) 0x40004800)
#define UART4                 ((volatile h743_usart_t *) 0x40004C00)
#define UART5                 ((volatile h743_usart_t *) 0x40005000)
#define USART6                ((volatile h743_usart_t *) 0x40011400)
#define UART7                 ((volatile h743_usart_t *) 0x40007800)
#define UART8                 ((volatile h743_usart_t *) 0x40007C00)

/* USART IRQS */
#define USART_IRQ_PE     (1 << 8)
#define USART_IRQ_TXE    (1 << 7)
#define USART_IRQ_TC     (1 << 6)
#define USART_IRQ_RXNE   (1 << 5)

typedef enum h743_usart_word_length
  {
   word_length_7 = (1 << 28),
   word_length_8 = 0,
   word_length_9 = (1 << 12)
  }h743_usart_word_length_e;

typedef enum h743_usart_oversampling
  {
   oversampling_16 = 0,
   oversampling_8  = (1 << 15)
  }h743_usart_oversampling_e;

typedef enum h743_usart_stop_bits
  {
   stop_bit_0P5 = (1 << 12),
   stop_bit_1   = 0,
   stop_bit_1P5 = (3 << 12),
   stop_bit_2   = (2 << 12)
  }h743_usart_stop_bits_e;

typedef struct h743_usart_conf{
  h743_usart_word_length_e word_length;
  h743_usart_oversampling_e oversampling;
  bool parity_en;
  bool parity_even;
  u32 irq_en;
  bool transmitter_en;
  bool receiver_en;
  
  bool msb_first;
  h743_usart_stop_bits_e stop_bits;
  bool synchronous;
  bool polarity_low;
  bool phase_first;

  bool dma_tx_en;
  bool dma_rx_en;

  u32 baud_rate;
} h743_usart_conf_t;


#define h743_usart_init_conf(conf)				\
  conf = (h743_usart_conf_t) { .word_length = word_length_8,	\
			       .oversampling = oversampling_16,	\
			       .parity_en = FALSE,		\
			       .parity_even = TRUE,		\
			       .irq_en = 0,			\
			       .transmitter_en = TRUE,		\
			       .receiver_en = TRUE,		\
			       .msb_first = FALSE,		\
			       .stop_bits = stop_bit_1,		\
			       .synchronous = FALSE,		\
			       .polarity_low = TRUE,		\
			       .phase_first = TRUE,		\
                               .dma_tx_en = FALSE,              \
                               .dma_rx_en = FALSE,		\
                               .baud_rate = 9600	        \
  };

void h743_usart_conf(volatile h743_usart_t *usartx,
		     h743_usart_conf_t *conf,
		     u32 usart_clk_hz);

#define h743_usart_enable(usartx) \
  usartx->CR1 |= 1

#define h743_usart_disable(usartx) \
  usartx->CR1 &= ~1

#define h743_usart_receive(usartx) \
  (usartx->RDR)

#define h743_usart_transmit(usartx, data) \
  usartx->TDR = data

#define h743_usart_txe(usartx) \
  (usartx->ISR & (1 << 7))

#define h743_usart_tc(usartx) \
  (usartx->ISR & (1 << 6))

#define h743_usart_rxne(usartx) \
  (usartx->ISR & (1 << 5))

#define h743_usart_wait_txe(usartx) \
  while(!h743_usart_txe(usartx))

#define h743_usart_wait_tc(usartx) \
  while(!h743_usart_tc(usartx))

#define h743_usart_wait_rxne(usartx) \
  while(!h743_usart_rxne(usartx))

#define h743_usart_enable_irq(usartx, irqs)	\
  usartx->CR1 |= irqs

#define h743_usart_disable_irq(usartx, irqs)	\
  usartx->CR1 &= ~(irqs)

#define h743_usart_enable_dma_tx(usartx)	\
  usartx->CR3 |= (1 << 7)

#define h743_usart_enable_dma_rx(usartx)	\
  usartx->CR3 |= (1 << 6)

#define h743_usart_disable_dma_tx(usartx)	\
  usartx->CR3 &= ~(1 << 7)

#define h743_usart_disable_dma_rx(usartx)	\
  usartx->CR3 &= ~(1 << 6)


void h743_usart_read_strn(volatile h743_usart_t *usartx, u8 *buf, u32 n);
void h743_usart_read_str(volatile h743_usart_t *usartx,
			 u8 *buf, u32 *n, u8 end);

void h743_usart_write_strn(volatile h743_usart_t *usartx, u8 *buf, u32 n);
void h743_usart_write_str(volatile h743_usart_t *usartx, u8 *buf);

#endif
