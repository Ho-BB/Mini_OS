

#include "inc/stm32h743.h"

extern u32 __DTCM_START[];
extern u8 __AXI_SRAM_START[];

void h743_reset_isr(){

  h743_gpio_conf_t gpio_conf;
  h743_usart_conf_t usart_conf;
  h743_dma_conf_t dma_conf;
  h743_dmamux_conf_t dmamux_conf;
  u32 num_data;

  h743_rcc_enable_clock_ahb4(RCC_AHB4_GPIOD);
  h743_rcc_enable_clock_ahb4(RCC_AHB4_GPIOI);
  h743_rcc_enable_clock_apb1l(RCC_APB1L_USART2);
  h743_rcc_enable_clock_ahb1(RCC_AHB1_DMA1);
  //No clock to enable for dmamux1

  ARM_CPSIE;

  h743_nvic_enable_irq(H743_IRQ_DMA1_STR0);

  h743_gpio_init_conf(gpio_conf);
  h743_usart_init_conf(usart_conf);
  h743_dma_init_conf(dma_conf);
  h743_dmamux_init_conf(dmamux_conf);

  gpio_conf.pin = 5;
  gpio_conf.mode = alternate_function;
  gpio_conf.alternate_function = 7;
  h743_gpio_conf(GPIOD, &gpio_conf); //TX

  gpio_conf.pin = 6;
  gpio_conf.mode = alternate_function;
  gpio_conf.alternate_function = 7;
  h743_gpio_conf(GPIOD, &gpio_conf); //RX

  gpio_conf.pin = 6;
  gpio_conf.mode = output;
  h743_gpio_conf(GPIOI, &gpio_conf);

  h743_usart_conf(USART2, &usart_conf, 64000000);
  h743_usart_enable(USART2);

  h743_usart_wait_rxne(USART2);
  num_data = h743_usart_receive(USART2);

  h743_usart_wait_rxne(USART2);
  num_data = num_data | (h743_usart_receive(USART2) << 8);

  __DTCM_START[0] = num_data;
  
  /*
    Can't write that otherwise the data to receive will be lost
  h743_usart_write_str(USART2, (u8 *) "Preparing to receive ");
  h743_usart_write_u32(USART2, num_data);
  h743_usart_write_str(USART2, (u8 *) " bytes\n");
  */

  dma_conf.trbuff = TRUE;
  dma_conf.pinc = FALSE;
  dma_conf.dir = per_to_mem;
  dma_conf.irq_en = DMA_IRQ_TC | DMA_IRQ_TE | DMA_IRQ_FE | DMA_IRQ_DME;
  dma_conf.num_data = num_data;
  dma_conf.per_address = (u32) &USART2->RDR;
  dma_conf.mem_address_0 = (u32) __AXI_SRAM_START;
  dma_conf.fifo_mode = FALSE;
  h743_dma_conf(DMA1, 0, &dma_conf);

  dmamux_conf.request = DMAMUX_USART2_RX;
  h743_dmamux_conf(DMAMUX1, 0, &dmamux_conf);
  h743_dmamux_enable(DMAMUX1, 0);

  h743_dma_enable(DMA1, 0);

  h743_usart_enable_dma_rx(USART2);

  while(1);
}


void h743_dma1_str0_isr(){

  u32 pending_irq;

  h743_gpio_set(GPIOI, 6);

  pending_irq = h743_dma_pending_irq(DMA1, 0);

  if(pending_irq & DMA_IRQ_TC){
    h743_usart_write_str(USART2, (u8 *) "Transfer Complete, received : ");
    h743_usart_write_strn(USART2,
			  __AXI_SRAM_START,
			  __DTCM_START[0]);

    h743_dma_eoi(DMA1, 0, DMA_IRQ_TC);
  }
  else if(pending_irq & DMA_IRQ_TE){
    h743_usart_write_str(USART2, (u8 *) "Transfer Error\n");
    h743_dma_eoi(DMA1, 0, DMA_IRQ_TE);
  }
  else if(pending_irq & DMA_IRQ_FE){
    h743_usart_write_str(USART2, (u8 *) "Fifo Error\n");
    h743_dma_eoi(DMA1, 0, DMA_IRQ_FE);
  }
  else if(pending_irq & DMA_IRQ_DME){
    h743_usart_write_str(USART2, (u8 *) "Direct Mode Error\n");
    h743_dma_eoi(DMA1, 0, DMA_IRQ_DME);
  }
  else{
    h743_usart_write_str(USART2, (u8 *) "Half Transfer\n");
    h743_dma_eoi(DMA1, 0, DMA_IRQ_HT);
  }

  ARM_DSB;
}
