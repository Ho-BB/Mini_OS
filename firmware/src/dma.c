

#include "inc/dma.h"


void h743_dma_conf(volatile h743_dma_t *dmax,
		   u32 streamx,
		   h743_dma_conf_t *conf){

  dmax->STREAM[streamx].CR =
    (conf->trbuff << 20) |
    (conf->double_buffer_mode << 18) |
    conf->priority |
    (conf->msize << 13) |
    (conf->psize << 11) |
    (conf->minc << 10) |
    (conf->pinc << 9) |
    (conf->circular_mode << 8) |
    conf->dir |
    (conf->irq_en >> 1);

  dmax->STREAM[streamx].NDTR = conf->num_data;

  dmax->STREAM[streamx].PAR = conf->per_address;
  dmax->STREAM[streamx].M0AR = conf->mem_address_0;
  dmax->STREAM[streamx].M1AR = conf->mem_address_1;

  dmax->STREAM[streamx].FCR =
    ((conf->irq_en & 1) << 7) |
    (conf->fifo_mode << 2) |
    conf->fifo_threshold;
}
