

#include "inc/stm32h743.h"

extern u32 __DTCM_START[];

__attribute__((noreturn))
void h743_reset_isr(){

  h743_tim6_7_conf_t tim6_7_conf;
  h743_gpio_conf_t gpio_conf;

  __DTCM_START[0] = 1;
  
  h743_rcc_enable_clock_apb1l(RCC_APB1L_TIM7);
  h743_rcc_enable_clock_ahb4(RCC_AHB4_GPIOI);

  ARM_CPSIE;

  h743_nvic_enable_irq(H743_IRQ_TIM7);

  h743_tim6_7_init_conf(tim6_7_conf);
  h743_gpio_init_conf(gpio_conf);

  /* GPIOI6 conf */
  gpio_conf.pin = 6;
  gpio_conf.mode = output;
  h743_gpio_conf(GPIOI, &gpio_conf);

  h743_gpio_set(GPIOI, 6);
  
  /* TIM7 conf */
  tim6_7_conf.us = 1000000;
  h743_tim6_7_conf(TIM7, &tim6_7_conf, 64);
  h743_tim6_7_enable(TIM7);

  while(1);
}


void h743_tim7_isr(){

  if(__DTCM_START[0] == 1){
    __DTCM_START[0] = 0;
    h743_gpio_reset(GPIOI, 6);
  }
  else{
    __DTCM_START[0] = 1;
    h743_gpio_set(GPIOI, 6);
  }

  h743_tim6_7_eoi(TIM7);
  ARM_DSB;
}
