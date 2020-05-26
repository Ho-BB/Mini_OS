

#include "inc/stm32h743.h"


__attribute__((noreturn))
void h743_reset_isr(){

  h743_gpio_conf_t gpio_conf;
  h743_usart_conf_t usart_conf;
  
  h743_rcc_enable_clock_ahb4(RCC_AHB4_GPIOD);
  h743_rcc_enable_clock_apb1l(RCC_APB1L_USART2);

  h743_gpio_init_conf(gpio_conf);
  h743_usart_init_conf(usart_conf);

  gpio_conf.pin = 5;
  gpio_conf.mode = alternate_function;
  gpio_conf.alternate_function = 7;

  h743_gpio_conf(GPIOD, &gpio_conf);  //TX

  gpio_conf.pin = 6;
  gpio_conf.mode = alternate_function;
  gpio_conf.alternate_function = 7;

  h743_gpio_conf(GPIOD, &gpio_conf);  //RX

  //usart_conf.irq_en = USART_IRQ_RX;
  h743_usart_conf(USART2, &usart_conf, 64000000);
  h743_usart_enable(USART2);

  h743_usart_wait_rxne(USART2);

  ARM_CPSIE;

  h743_nvic_enable_irq(H743_IRQ_USART2);

  h743_usart_enable_irq(USART2, USART_IRQ_RXNE);

  while(1);
}


void h743_usart2_isr(){

  u8 c;

  c = h743_usart_receive(USART2);
  h743_usart_transmit(USART2, c);
}
