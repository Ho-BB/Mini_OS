

#include "inc/stm32h743.h"

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

  u32 i;
  u8 buf[4];
  h743_gpio_conf_t gpio_conf;
  h743_usart_conf_t usart_conf;
  h743_spi_conf_t spi_conf;

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
  h743_spi_init_conf(spi_conf);

  /* Configure Usart IOs */
  gpio_conf.pin = 5;
  gpio_conf.mode = alternate_function;
  gpio_conf.alternate_function = 7;
  h743_gpio_conf(GPIOD, &gpio_conf);

  gpio_conf.pin = 6;
  gpio_conf.mode = alternate_function;
  gpio_conf.alternate_function = 7;
  h743_gpio_conf(GPIOD, &gpio_conf);

  /* Configure Spi IOs */
  gpio_conf.pin = 12;
  gpio_conf.mode = output;
  gpio_conf.output_type = push_pull;
  gpio_conf.output_speed = high;
  h743_gpio_conf(GPIOB, &gpio_conf);

  gpio_conf.pin = 13;
  gpio_conf.mode = alternate_function;
  gpio_conf.alternate_function = 5;
  h743_gpio_conf(GPIOB, &gpio_conf);

  gpio_conf.pin = 14;
  gpio_conf.mode = alternate_function;
  gpio_conf.alternate_function = 5;
  h743_gpio_conf(GPIOB, &gpio_conf);

  gpio_conf.pin = 15;
  gpio_conf.mode = alternate_function;
  gpio_conf.alternate_function = 5;
  h743_gpio_conf(GPIOB, &gpio_conf);

  /* Configure D/CX IO */
  gpio_conf.pin = 8;
  gpio_conf.mode = output;
  gpio_conf.output_type = push_pull;
  gpio_conf.output_speed = high;
  h743_gpio_conf(GPIOD, &gpio_conf);

  /* Configure RESET IO */
  gpio_conf.pin = 12;
  gpio_conf.mode = output;
  gpio_conf.output_type = push_pull;
  gpio_conf.output_speed = high;
  h743_gpio_conf(GPIOH, &gpio_conf);

  /* Configure and start usart */
  h743_usart_conf(USART2, &usart_conf, 64000000);
  h743_usart_enable(USART2);

  /* Configure and start spi */
  h743_spi_conf(SPI2, &spi_conf);
  h743_spi_enable(SPI2);

  /* Set CSN high (inactive) */
  h743_gpio_set(GPIOB, 12);

  /* Reset ili9341 */
  h743_gpio_reset(GPIOH, 12);

  i = 3000000;
  while(i--);

  h743_gpio_set(GPIOH, 12);

  /* Wait for data received on usart before continuing */
  h743_usart_wait_rxne(USART2);
  i = h743_usart_receive(USART2);
  
  /* Send read status cmd */
  h743_gpio_reset(GPIOB, 12);
  h743_gpio_reset(GPIOD, 8);
  h743_spi_transmit_byte(SPI2, 0x09);
  h743_spi_wait_rxp(SPI2);
  i = h743_spi_receive_byte(SPI2); // Discard what's received

  h743_gpio_set(GPIOD, 8);
  
  /* Insert dummy clock cycle */

  gpio_conf.pin = 13;
  gpio_conf.mode = output;
  gpio_conf.output_type = push_pull;
  gpio_conf.output_speed = high;
  h743_gpio_conf(GPIOB, &gpio_conf);
  
  h743_gpio_set(GPIOB, 13);

  i = 20000;
  while(i--);

  h743_gpio_reset(GPIOB, 13);

  i = 20000;
  while(i--);

  gpio_conf.pin = 13;
  gpio_conf.mode = alternate_function;
  gpio_conf.alternate_function = 5;
  h743_gpio_conf(GPIOB, &gpio_conf);
  
  /* End of dummy clock cycle */

  /* Read status */
  for(i = 0; i < 4; i++){
    h743_spi_transmit_byte(SPI2, 0x00);
    h743_spi_wait_rxp(SPI2);
    buf[i] = h743_spi_receive_byte(SPI2);
  }

  h743_gpio_set(GPIOB, 12);

  /* Print status with usart */
  for(i = 0; i < 4; i++){
    h743_usart_wait_txe(USART2);
    h743_usart_transmit(USART2, qtoa(buf[i] >> 4));
    h743_usart_wait_txe(USART2);
    h743_usart_transmit(USART2, qtoa(buf[i] & 0x0F));
  }

  /* Sleep out mode */
  h743_gpio_reset(GPIOB, 12);
  h743_gpio_reset(GPIOD, 8);
  h743_spi_transmit_byte(SPI2, 0x11);
  h743_spi_wait_rxp(SPI2);
  i = h743_spi_receive_byte(SPI2);

  h743_gpio_set(GPIOB, 12);

  /* Wait at least 5ms for sleep out mode to take effect */
  i = 1000000;
  while(i--);

  h743_usart_write_str(USART2, (u8 *) "SLEEP OUT\n");

  /* Display ON */
  h743_gpio_reset(GPIOB, 12);
  h743_gpio_reset(GPIOD, 8);
  h743_spi_transmit_byte(SPI2, 0x29);
  h743_spi_wait_rxp(SPI2);
  i = h743_spi_receive_byte(SPI2);

  h743_gpio_set(GPIOB, 12);

  h743_usart_write_str(USART2, (u8 *) "DISPLAY ON\n");

  h743_gpio_reset(GPIOB, 12);
  h743_gpio_reset(GPIOD, 8);
  h743_spi_transmit_byte(SPI2, 0x2C);
  h743_spi_wait_rxp(SPI2);
  i = h743_spi_receive_byte(SPI2);

  h743_gpio_set(GPIOD, 8);
  
  while(1){
    h743_spi_transmit_byte(SPI2, 0xFC);//B
    h743_spi_wait_txp(SPI2);
    h743_spi_transmit_byte(SPI2, 0x00);//G
    h743_spi_wait_txp(SPI2);
    h743_spi_transmit_byte(SPI2, 0x00);//R
    h743_spi_wait_txp(SPI2);
  }
}
