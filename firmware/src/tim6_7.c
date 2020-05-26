

#include "inc/tim6_7.h"


void h743_tim6_7_conf(volatile h743_tim6_7_t *timx,
		      h743_tim6_7_conf_t *conf,
		      u32 tim_clk_mhz){

  u32 clk_ticks, i;

  clk_ticks = tim_clk_mhz * conf->us;

  i = 0;
  while((clk_ticks >> i) > 0x0000FFFF)
    i++;

  timx->PSC = (1 << i) - 1;
  timx->ARR = clk_ticks >> i;
  
  timx->DIER = (timx->DIER & ~((1 << 8) | 1)) | conf->dma_irq_en;
  timx->CR1  = (timx->CR1  & ~(1 << 3)) | (conf->one_pulse << 3);
}
