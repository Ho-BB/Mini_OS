

#include "inc/stm32h743.h"


__attribute__((noreturn))
void h743_reset_isr(){

  h743_gpio_conf_t gpio_conf;

  h743_rcc_enable_clock_ahb4(RCC_AHB4_GPIOI);

  h743_gpio_init_conf(gpio_conf);

  gpio_conf.pin = 6;
  gpio_conf.mode = output;
  h743_gpio_conf(GPIOI, &gpio_conf);

  h743_gpio_reset(GPIOI, 6);

  asm("\
ldr r0, =__AXI_SRAM_END\n				\
msr psp, r0\n						\
mrs r0, control\n					\
orr r0, r0, #3\n					\
msr control, r0\n					\
isb\n							\
b task1							\
");
  
  while(1);
}


__attribute__((noreturn))
void task1(){

  h743_gpio_set(GPIOI, 6);
  
  while(1);
}
