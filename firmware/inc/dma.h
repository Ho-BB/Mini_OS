

#ifndef __H743_DMA__
#define __H743_DMA__

#include "inc/typedef.h"

typedef struct h743_dma{
  u32 ISR[2];
  u32 IFCR[2];
  
  struct h743_dma_stream{
    u32 CR;
    u32 NDTR;
    u32 PAR;
    u32 M0AR;
    u32 M1AR;
    u32 FCR;
  } STREAM[8];
} h743_dma_t;

/* 2 DMA */
#define DMA1                  ((volatile h743_dma_t *) 0x40020000)
#define DMA2                  ((volatile h743_dma_t *) 0x40020400)

/* DMA IRQs */
#define DMA_IRQ_HT            (1 << 4)
#define DMA_IRQ_TC            (1 << 5)
#define DMA_IRQ_TE            (1 << 3)
#define DMA_IRQ_FE            (1 << 0)
#define DMA_IRQ_DME           (1 << 2)

typedef enum h743_dma_stream_priority
  {
   dma_priority_low = (0 << 16),
   dma_priority_medium = (1 << 16),
   dma_priority_high = (2 << 16),
   dma_priority_very_high = (3 << 16)
  }h743_dma_stream_priority_e;

typedef enum h743_dma_data_size
  {
   byte = 0,
   half_word = 1,
   word = 2
  }h743_dma_data_size_e;

/* 
   per_to_mem : source = per_address   | destination = mem_address_0
   mem_to_per : source = mem_address_0 | destination = per_address
   mem_to_mem : source = per_address   | destination = mem_address_0
*/
typedef enum h743_dma_direction
  {
   per_to_mem = (0 << 6),
   mem_to_per = (1 << 6),
   mem_to_mem = (2 << 6)
  }h743_dma_direction_e;

typedef enum h743_dma_fifo_threshold
  {
   fifo_1_quarter = 0,
   fifo_half = 1,
   fifo_3_quarter = 2,
   fifo_full = 3
  }h743_dma_fifo_threshold_e;

typedef struct h743_dma_conf{
  bool trbuff;
  bool double_buffer_mode;
  h743_dma_stream_priority_e priority;
  h743_dma_data_size_e msize;
  h743_dma_data_size_e psize;
  bool minc;
  bool pinc;
  bool circular_mode;
  h743_dma_direction_e dir;
  u32 irq_en;

  u16 num_data;

  u32 per_address;
  u32 mem_address_0;
  u32 mem_address_1;

  bool fifo_mode;
  h743_dma_fifo_threshold_e fifo_threshold;
} h743_dma_conf_t;

#define h743_dma_init_conf(conf)		\
  conf = (h743_dma_conf_t)			\
    { .trbuff = TRUE,				\
      .double_buffer_mode = FALSE,		\
      .priority = dma_priority_very_high,	\
      .msize = byte,				\
      .psize = byte,				\
      .minc = TRUE,				\
      .pinc = FALSE,				\
      .circular_mode = FALSE,			\
      .dir = mem_to_per,			\
      .irq_en = DMA_IRQ_TC,			\
      .num_data = 1,				\
      .per_address = 0,				\
      .mem_address_0 = 0,			\
      .mem_address_1 = 0,			\
      .fifo_mode = FALSE,			\
      .fifo_threshold = fifo_full,		\
    };

void h743_dma_conf(volatile h743_dma_t *dmax,
		   u32 streamx,
		   h743_dma_conf_t *conf);

#define h743_dma_enable(dmax, streamx)		\
  dmax->STREAM[streamx].CR |= 1

#define h743_dma_disable(dmax, streamx)		\
  dmax->STREAM[streamx].CR &= ~1

#define h743_dma_wait_disabled(dmax, streamx)	\
  while(dmax->STREAM[streamx].CR & 1)

/* Only word access is allowed */
#define h743_dma_pending_irq(dmax, streamx)			\
  ((dmax->ISR[streamx >> 2] >> ((streamx & 2) << 3)) >>		\
   (((streamx & 1) << 2) | ((streamx & 1) << 1)))

#define h743_dma_eoi(dmax, streamx, irq)			\
  dmax->IFCR[streamx >> 2] = (irq << ((streamx & 2) << 3)) <<	\
    (((streamx & 1) << 2) | ((streamx & 1) << 1))

#endif
