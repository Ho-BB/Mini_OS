

#include "inc/stm32h743.h"
#include "ili9341_terminal.h"

/*
  USART2 : TX = D5 (AF7)
         | RX = D6 (AF7)

  ILI9341 : SPI2 : CSN  = B12 (Output push/pull)
                 | SCK  = B13 (AF5)
                 | MISO = B14 (AF5)
                 | MOSI = B15 (AF5)
          | RESET = H12 (Output push/pull)
          | D/CX  = D8  (Output push/pull)
*/

__attribute__((noreturn))
void h743_reset_isr(){

  h743_gpio_conf_t gpio_conf;
  h743_usart_conf_t usart_conf;
  ili9341_terminal_t ili;

  /* Enable pll1 for spi2 */
  h743_rcc_enable_pll(RCC_PLL1);
  h743_rcc_wait_pll_locked(RCC_PLL1);

  /* Enable peripheral clock */
  h743_rcc_enable_clock_ahb4(RCC_AHB4_GPIOB);
  h743_rcc_enable_clock_ahb4(RCC_AHB4_GPIOD);
  h743_rcc_enable_clock_ahb4(RCC_AHB4_GPIOH);
  h743_rcc_enable_clock_apb1l(RCC_APB1L_USART2);
  h743_rcc_enable_clock_apb1l(RCC_APB1L_SPI2);

  /* Init peripheral conf struct */
  h743_gpio_init_conf(gpio_conf);
  h743_usart_init_conf(usart_conf);
  ili9341_term_init_struct_default(&ili);

  /* Configure Usart IOs */
  gpio_conf.pin = 5;
  gpio_conf.mode = alternate_function;
  gpio_conf.alternate_function = 7;
  h743_gpio_conf(GPIOD, &gpio_conf);

  gpio_conf.pin = 6;
  gpio_conf.mode = alternate_function;
  gpio_conf.alternate_function = 7;
  h743_gpio_conf(GPIOD, &gpio_conf);

  ili9341_term_init(&ili);

  /* Configure and start usart */
  h743_usart_conf(USART2, &usart_conf, 64000000);
  h743_usart_enable(USART2);

  /* Wait for data received on usart before continuing */
  u32 c;
  h743_usart_wait_rxne(USART2);
  c = h743_usart_receive(USART2);

  ili9341_term_clear_screen(&ili, ili.background);

  while(1){
    h743_usart_wait_rxne(USART2);
    c = h743_usart_receive(USART2);

    ili9341_term_print_char(&ili, c);
  }
}
