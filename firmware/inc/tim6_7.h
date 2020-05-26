

#ifndef __H743_TIM6_7__
#define __H743_TIM6_7__

#include "inc/typedef.h"

typedef struct h743_tim6_7{
  u32 CR1;
  u32 CR2;
  u32 reserved1;
  u32 DIER;
  u32 SR;
  u32 EGR;
  u32 reserved2[3];
  u32 CNT;
  u32 PSC;
  u32 ARR;
} h743_tim6_7_t;

/* 2 TIMER */
#define TIM6                  ((volatile h743_tim6_7_t *) 0x40001000)
#define TIM7                  ((volatile h743_tim6_7_t *) 0x40001400)

#define TIM6_7_REQ_DMA        (1 << 8)
#define TIM6_7_IRQ_UPD        (1 << 0)

typedef struct h743_tim6_7_conf{
  bool one_pulse;
  u32 dma_irq_en;
  u32 us;
} h743_tim6_7_conf_t;

#define h743_tim6_7_init_conf(conf)				\
  conf = (h743_tim6_7_conf_t) { .one_pulse = FALSE,		\
				.dma_irq_en = TIM6_7_IRQ_UPD,	\
				.us = 1				\
  };

void h743_tim6_7_conf(volatile h743_tim6_7_t *timx,
		      h743_tim6_7_conf_t *conf,
		      u32 tim_clk_mhz);

#define h743_tim6_7_enable(timx)		\
  timx->CR1 |= 1

#define h743_tim6_7_disable(timx)		\
  timx->CR1 &= ~1

#define h743_tim6_7_eoi(timx)			\
  timx->SR = 0

#endif
