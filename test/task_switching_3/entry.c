

#include "inc/stm32h743.h"

/*
  __AXI_SRAM_START[0] : (0 = task1) | (1 = task2)
  __AXI_SRAM_START[1] : task1 sp
  __AXI_SRAM_START[2] : task2 sp
*/

extern u32 __AXI_SRAM_START[];
extern u32 __AXI_SRAM_END[];

void task2();

__attribute__((noreturn))
void h743_reset_isr(){

  h743_gpio_conf_t gpio_conf;
  h743_tim6_7_conf_t tim6_7_conf;

  __AXI_SRAM_START[0] = 0;
  __AXI_SRAM_START[2] = ((u32) __AXI_SRAM_END) - 4096 - 32;
  //We do not save any state other than what is automatically saved atm
  //task2 stack init
  ((u32 *) __AXI_SRAM_START[2])[0] = 0; // R0
  ((u32 *) __AXI_SRAM_START[2])[1] = 0; // R1
  ((u32 *) __AXI_SRAM_START[2])[2] = 0; // R2
  ((u32 *) __AXI_SRAM_START[2])[3] = 0; // R3

  ((u32 *) __AXI_SRAM_START[2])[4] = 0; // R12
  ((u32 *) __AXI_SRAM_START[2])[5] = 0; // LR
  ((u32 *) __AXI_SRAM_START[2])[6] = (u32) task2; // PC
  ((u32 *) __AXI_SRAM_START[2])[7] = 0x01000000; // xPSR

  __AXI_SRAM_START[2] -= 32;

  h743_rcc_enable_clock_apb1l(RCC_APB1L_TIM7);
  h743_rcc_enable_clock_ahb4(RCC_AHB4_GPIOI);

  ARM_CPSIE;

  h743_nvic_enable_irq(H743_IRQ_TIM7);

  h743_tim6_7_init_conf(tim6_7_conf);
  h743_gpio_init_conf(gpio_conf);

  gpio_conf.pin = 6;
  gpio_conf.mode = output;
  h743_gpio_conf(GPIOI, &gpio_conf);

  h743_gpio_reset(GPIOI, 6);

  TIM7->CNT = 0;
  tim6_7_conf.us = 1000000;
  h743_tim6_7_conf(TIM7, &tim6_7_conf, 64);
  h743_tim6_7_enable(TIM7);

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

  while(1){
    h743_gpio_set(GPIOI, 6);
  }
}

void task2(){

  while(1){
    h743_gpio_reset(GPIOI, 6);
  }
}

__attribute__((naked))
void h743_tim7_isr(){

  asm("\
mrs r0, psp\n					\
stmdb r0!, {r4-r11}\n				\
sub lr, lr, #4\n				\
str lr, [sp]\n					\
bl switch_task_main\n				\
ldr lr, [sp]\n					\
add lr, lr, #4\n				\
ldmia r0!, {r4-r11}\n				\
msr psp, r0\n					\
bx lr\n						\
");
}

u32 switch_task_main(u32 old_sp){

  u32 new_sp;
  
  if(__AXI_SRAM_START[0] == 0){
    __AXI_SRAM_START[0] = 1;
    __AXI_SRAM_START[1] = old_sp;
    new_sp = __AXI_SRAM_START[2];
  }
  else{
    __AXI_SRAM_START[0] = 0;
    __AXI_SRAM_START[2] = old_sp;
    new_sp = __AXI_SRAM_START[1];
  }
  
  h743_tim6_7_eoi(TIM7);
  ARM_DSB;
  return new_sp;
}
