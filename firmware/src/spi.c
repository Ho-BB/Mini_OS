

#include "inc/spi.h"


void h743_spi_conf(volatile h743_spi_t *spix,
		   h743_spi_conf_t *conf){

  spix->CR1 |= (1 << 12);
  
  spix->CFG1 =
    (spix->CFG1 & ~((7 << 28) | (15 << 5) | 31)) |
    conf->mbr |
    (((conf->fifo_threshold - 1) & 15) << 5) |
    ((conf->frame_size - 1) & 31);

  spix->CFG2 =
    (spix->CFG2 & ~((1 << 25) | (1 << 24) | (1 << 23) | (1 << 22))) |
    (1 << 26) |
    (conf->clock_idle_high << 25) |
    (conf->clock_phase_second << 24) |
    (conf->lsb_first << 23) |
    (conf->master << 22);

  spix->IER = (spix->IER & ~3) | conf->irq_en;
}
