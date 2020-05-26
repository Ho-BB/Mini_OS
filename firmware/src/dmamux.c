

#include "inc/dmamux.h"


void h743_dmamux_conf(volatile h743_dmamux_t *dmamuxx,
		      u32 streamx,
		      h743_dmamux_conf_t *conf){

  dmamuxx->CCR[streamx] = conf->request;
}
