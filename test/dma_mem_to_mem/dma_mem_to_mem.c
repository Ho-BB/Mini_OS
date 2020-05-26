

#include "inc/stm32h743.h"

/*
  I learned that the DMA1 and DMA2 can't access DTCM (or ITCM), resulting in
  a transfer error if you try.
*/

extern u8 __DTCM_START[];
extern u8 __AXI_SRAM_START[];

#define DMA_SOURCE       256
#define DMA_SOURCE_LEN   5

#define DMA_DESTINATION  512

__attribute__((noreturn))
void h743_reset_isr(){

  h743_dma_conf_t dma_conf;
  h743_gpio_conf_t gpio_conf;
  h743_usart_conf_t usart_conf;
  
  h743_rcc_enable_clock_ahb1(RCC_AHB1_DMA1);
  h743_rcc_enable_clock_ahb4(RCC_AHB4_GPIOD);
  h743_rcc_enable_clock_ahb4(RCC_AHB4_GPIOI);
  h743_rcc_enable_clock_apb1l(RCC_APB1L_USART2);

  ARM_CPSIE;

  h743_nvic_enable_irq(H743_IRQ_DMA1_STR0);

  __AXI_SRAM_START[DMA_SOURCE + 0] = 'h';
  __AXI_SRAM_START[DMA_SOURCE + 1] = 'e';
  __AXI_SRAM_START[DMA_SOURCE + 2] = 'l';
  __AXI_SRAM_START[DMA_SOURCE + 3] = 'l';
  __AXI_SRAM_START[DMA_SOURCE + 4] = 'o';

  h743_dma_init_conf(dma_conf);
  h743_gpio_init_conf(gpio_conf);
  h743_usart_init_conf(usart_conf);

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

  dma_conf.trbuff = TRUE;
  dma_conf.pinc = TRUE;
  dma_conf.dir = mem_to_mem;
  dma_conf.irq_en = DMA_IRQ_TC | DMA_IRQ_TE;
  dma_conf.num_data = DMA_SOURCE_LEN;
  dma_conf.per_address = (u32) (__AXI_SRAM_START + DMA_SOURCE);
  dma_conf.mem_address_0 = (u32) (__AXI_SRAM_START + DMA_DESTINATION);
  dma_conf.fifo_mode = TRUE;
  dma_conf.fifo_threshold = fifo_full;
  h743_dma_conf(DMA1, 0, &dma_conf);

  h743_usart_wait_rxne(USART2);
  h743_dma_enable(DMA1, 0);
  
  while(1);
}


void h743_dma1_str0_isr(){

  u32 pending_irq;
  
  h743_gpio_set(GPIOI, 6);

  pending_irq = h743_dma_pending_irq(DMA1, 0);

  if(pending_irq & DMA_IRQ_TC){
    h743_usart_write_strn(USART2,
			  __AXI_SRAM_START + DMA_DESTINATION,
			  DMA_SOURCE_LEN);

    h743_dma_eoi(DMA1, 0, DMA_IRQ_TC);
  }
  else if(pending_irq & DMA_IRQ_TE){
    h743_usart_write_strn(USART2,
			  (u8 *) "Transfer error\n",
			  15);
    h743_dma_eoi(DMA1, 0, DMA_IRQ_TE);
  }
  else if(pending_irq & DMA_IRQ_FE){
    h743_dma_eoi(DMA1, 0, DMA_IRQ_FE);
  }
  else if(pending_irq & DMA_IRQ_DME){
    h743_dma_eoi(DMA1, 0, DMA_IRQ_DME);
  }
  else{
    h743_dma_eoi(DMA1, 0, DMA_IRQ_HT);
  }

  ARM_DSB;
}
