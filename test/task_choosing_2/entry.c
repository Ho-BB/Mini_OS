

#include "inc/stm32h743.h"

/*
  __AXI_SRAM_START[0] : scheduler_t *
*/

extern u32 __AXI_SRAM_START[];
extern u32 __AXI_SRAM_END[];

typedef struct task{
  u32 sp;
  u32 entry;
} task_t;

void init_task_stack(task_t *task){

  task->sp -= 64;

  ((u32 *) task->sp)[0]  = 0;  //R4
  ((u32 *) task->sp)[1]  = 0;  //R5
  ((u32 *) task->sp)[2]  = 0;  //R6
  ((u32 *) task->sp)[3]  = 0;  //R7

  ((u32 *) task->sp)[4]  = 0;  //R8
  ((u32 *) task->sp)[5]  = 0;  //R9
  ((u32 *) task->sp)[6]  = 0;  //R10
  ((u32 *) task->sp)[7]  = 0;  //R11

  ((u32 *) task->sp)[8]  = 0;  //R0
  ((u32 *) task->sp)[9]  = 0;  //R1
  ((u32 *) task->sp)[10] = 0;  //R2
  ((u32 *) task->sp)[11] = 0;  //R3

  ((u32 *) task->sp)[12] = 0;            //R12
  ((u32 *) task->sp)[13] = 0;            //LR (R14)
  ((u32 *) task->sp)[14] = task->entry;  //PC (R15)
  ((u32 *) task->sp)[15] = 0x01000000;   //xPSR
}

typedef struct scheduler{
  u32 num_task;
  task_t *queue[10];
  task_t *elected;
  task_t *idle;
} scheduler_t;

void scheduler_init(scheduler_t *sched, task_t *idle){

  sched->num_task = 0;
  sched->elected  = idle;
  sched->idle     = idle;
}

task_t *scheduler_elect(scheduler_t *sched){

  task_t *tmp;
  u32 i;
  
  if(sched->num_task > 1){
    tmp = sched->elected;
    sched->elected = sched->queue[0];
    
    for(i = 1; i < sched->num_task - 1; i++)
      sched->queue[i - 1] = sched->queue[i];

    sched->queue[sched->num_task - 2] = tmp;
  }

  return sched->elected;
}

task_t *scheduler_add(scheduler_t *sched, task_t *new_task){

  if(sched->num_task == 0)
    sched->elected = new_task;
  else
    sched->queue[sched->num_task - 1] = new_task;

  sched->num_task++;

  return sched->elected;
}

task_t *scheduler_rmv(scheduler_t *sched, task_t *rmv_task){

  u32 i;
  
  if(sched->num_task != 0){
    if(sched->elected == rmv_task){
      if(sched->num_task == 1)
	sched->elected = sched->idle;
      else{
	sched->elected = sched->queue[0];
	for(i = 1; i < sched->num_task - 1; i++)
	  sched->queue[i - 1] = sched->queue[i];
      }
      
      sched->num_task--;
    }
    else{
      for(i = 0; i < sched->num_task - 1; i++)
	if(sched->queue[i] == rmv_task)
	  break;

      if(i != (sched->num_task - 1)){
	while(i < (sched->num_task - 2)){
	  sched->queue[i] = sched->queue[i + 1];
	  i++;
	}
	sched->num_task--;
      }
    }
  }
  
  return sched->elected;
}

__attribute__((noreturn, naked))
void launch_first_task(u32 sp){

  asm("\
msr psp, r0\n						\
mrs r0, control\n					\
orr r0, r0, #3\n					\
msr control, r0\n					\
isb\n							\
ldmia sp!, {r4-r11}\n					\
ldmia sp!, {r0-r3,r12,lr}\n				\
add sp, sp, #8\n					\
ldr pc, [sp, #-8]\n					\
");
}

void idle();
void task1();
void task2();

__attribute__((noreturn))
void h743_reset_isr(){

  h743_gpio_conf_t gpio_conf;
  h743_tim6_7_conf_t tim6_7_conf;
  h743_usart_conf_t usart_conf;

  task_t task_idle, task_task1, task_task2;
  scheduler_t sched;
  task_t *elected;

  task_idle.sp = (u32) __AXI_SRAM_END;
  task_idle.entry = (u32) idle;

  task_task1.sp = ((u32) __AXI_SRAM_END) - 4096;
  task_task1.entry = (u32) task1;

  task_task2.sp = ((u32) __AXI_SRAM_END) - 8192;
  task_task2.entry = (u32) task2;

  init_task_stack(&task_idle);
  init_task_stack(&task_task1);
  init_task_stack(&task_task2);

  scheduler_init(&sched, &task_idle);
  elected = scheduler_add(&sched, &task_task1);
  elected = scheduler_add(&sched, &task_task2);

  __AXI_SRAM_START[0] = (u32) &sched;

  h743_rcc_enable_clock_apb1l(RCC_APB1L_TIM7);
  h743_rcc_enable_clock_ahb4(RCC_AHB4_GPIOI);
  h743_rcc_enable_clock_ahb4(RCC_AHB4_GPIOD);
  h743_rcc_enable_clock_apb1l(RCC_APB1L_USART2);

  ARM_CPSIE;

  h743_nvic_enable_irq(H743_IRQ_TIM7);

  h743_tim6_7_init_conf(tim6_7_conf);
  h743_gpio_init_conf(gpio_conf);
  h743_usart_init_conf(usart_conf);

  gpio_conf.pin = 6;
  gpio_conf.mode = output;
  h743_gpio_conf(GPIOI, &gpio_conf);

  h743_gpio_reset(GPIOI, 6);

  gpio_conf.pin = 5;
  gpio_conf.mode = alternate_function;
  gpio_conf.alternate_function = 7;

  h743_gpio_conf(GPIOD, &gpio_conf);  //TX

  gpio_conf.pin = 6;
  gpio_conf.mode = alternate_function;
  gpio_conf.alternate_function = 7;

  h743_gpio_conf(GPIOD, &gpio_conf);  //RX

  h743_usart_conf(USART2, &usart_conf, 64000000);
  h743_usart_enable(USART2);

  TIM7->CNT = 0;
  tim6_7_conf.us = 1000000;
  h743_tim6_7_conf(TIM7, &tim6_7_conf, 64);
  h743_tim6_7_enable(TIM7);

  launch_first_task(elected->sp);
}

void idle(){

  while(1);
}

__attribute__((noreturn))
void task1(){

  u8 c;
  
  while(1){
    h743_usart_wait_rxne(USART2);
    h743_usart_wait_txe(USART2);
    c = h743_usart_receive(USART2);
    h743_usart_transmit(USART2, '1');
  }
}

void task2(){

  u8 c;
  
  while(1){
    h743_usart_wait_rxne(USART2);
    h743_usart_wait_txe(USART2);
    c = h743_usart_receive(USART2);
    h743_usart_transmit(USART2, '2');
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

  task_t *new_elected;
  
  ((scheduler_t *) __AXI_SRAM_START[0])->elected->sp = old_sp;

  new_elected = scheduler_elect((scheduler_t *) __AXI_SRAM_START[0]);
  
  h743_tim6_7_eoi(TIM7);
  ARM_DSB;
  return new_elected->sp;
}
