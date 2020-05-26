

#ifndef __H743_NVIC__
#define __H743_NVIC__

#include "inc/typedef.h"

typedef struct h743_nvic{
  u32 ISER[8];
  u32 not_used1[24];
  u32 ICER[8];
  u32 not_used2[24];
  u32 ISPR[8];
  u32 not_used3[24];
  u32 ICPR[8];
  u32 not_used4[24];
  u32 IABR[8];
  u32 not_used5[56];
  u32 IPR[60];
  u32 not_used6[644];
  u32 STIR;
} h743_nvic_t;

#define NVIC                  ((volatile h743_nvic_t *) 0xE000E100)

/* IRQ numbers */
/* TIMER */
#define H743_IRQ_TIM6         54
#define H743_IRQ_TIM7         55

/* DMA */
#define H743_IRQ_DMA1_STR0    11
#define H743_IRQ_DMA1_STR1    12
#define H743_IRQ_DMA1_STR2    13
#define H743_IRQ_DMA1_STR3    14
#define H743_IRQ_DMA1_STR4    15
#define H743_IRQ_DMA1_STR5    16
#define H743_IRQ_DMA1_STR6    17
#define H743_IRQ_DMA1_STR7    47

#define h743_IRQ_DMA2_STR0    56
#define h743_IRQ_DMA2_STR1    57
#define h743_IRQ_DMA2_STR2    58
#define h743_IRQ_DMA2_STR3    59
#define h743_IRQ_DMA2_STR4    60
#define h743_IRQ_DMA2_STR5    68
#define h743_IRQ_DMA2_STR6    69
#define h743_IRQ_DMA2_STR7    70

/* SPI */
#define H743_IRQ_SPI1         35
#define H743_IRQ_SPI2         36

/* USART */
#define H743_IRQ_USART1       37
#define H743_IRQ_USART2       38
#define H743_IRQ_USART3       39

/* End of IRQ numbers */

/* Enable / Disable IRQs */
#define h743_nvic_enable_irq(irq)		\
  NVIC->ISER[(irq) >> 5] = 1 << ((irq) & 0x1F)

#define h743_nvic_disable_irq(irq)		\
  NVIC->ICER[(irq) >> 5] = 1 << ((irq) & 0x1F)

#define h743_nvic_is_irq_enabled(irq)			\
  ((NVIC->ISER[(irq) >> 5] >> (irq & 0x1F)) & 1)

/* Set pending / Clear pending IRQs */
#define h743_nvic_set_pending_irq(irq)		\
  NVIC->ISPR[(irq) >> 5] = 1 << ((irq) & 0x1F)

#define h743_nvic_clear_pending_irq(irq)	\
  NVIC->ICPR[(irq) >> 5] = 1 << ((irq) & 0x1F)

#define h743_nvic_is_irq_pending(irq)			\
  ((NVIC->ISPR[(irq) >> 5] >> (irq & 0x1F)) & 1)

/* Peripheral interrupt signal active */
#define h743_nvic_is_irq_active(irq)			\
  ((NVIC->IABR[(irq) >> 5] >> (irq & 0x1F)) & 1)

/* Priority of IRQs (0 - 15) */
#define h743_nvic_set_priority_irq(irq, prio)			\
  NVIC->IPR[(irq) >> 2] &=					\
    ~(0x0F0 << (((irq) & 3) << 3))				\
    | (((prio) << 4) << (((irq) & 3) << 3))

#define h743_nvic_get_priority_irq(irq)				\
  ((NVIC->IPR[(irq) >> 2] >> ((((irq) & 3) << 3) + 4)) & 0x0F)

/* Trigger IRQs */
#define h743_nvic_trigger_irq(irq)		\
  NVIC->STIR = irq
  
#endif
