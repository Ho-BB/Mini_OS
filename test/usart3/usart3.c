

#include "inc/stm32h743.h"

/*
  USART3 : RX = C11 (AF 7)
         | TX = C10 (AF 7)
*/

__attribute__((noreturn))
void h743_reset_isr(){

  h743_gpio_conf_t gpio_conf;
  h743_usart_conf_t usart_conf;
  
  h743_rcc_enable_clock_ahb4(RCC_AHB4_GPIOC);
  h743_rcc_enable_clock_apb1l(RCC_APB1L_USART3);

  h743_gpio_init_conf(gpio_conf);
  h743_usart_init_conf(usart_conf);

  gpio_conf.pin = 10;
  gpio_conf.mode = alternate_function;
  gpio_conf.alternate_function = 7;

  h743_gpio_conf(GPIOC, &gpio_conf);  //TX

  gpio_conf.pin = 11;
  gpio_conf.mode = alternate_function;
  gpio_conf.alternate_function = 7;

  h743_gpio_conf(GPIOC, &gpio_conf);  //RX

  h743_usart_conf(USART3, &usart_conf, 64000000);
  h743_usart_enable(USART3);
  
  while(1){
    h743_usart_wait_rxne(USART3);
    h743_usart_wait_txe(USART3);
    h743_usart_transmit(USART3, h743_usart_receive(USART3));
  }
}
