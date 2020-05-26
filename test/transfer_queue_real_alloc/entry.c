

#include "inc/stm32h743.h"
#include "freelist_alloc.h"

/*
  __AXI_SRAM_START[0] : scheduler_t *
  __AXI_SRAM_START[1] : freelist_alloc_t *
  __AXI_SRAM_START[4] : transfer_queue_t *
  __AXI_SRAM_START[5 - 10] : transfer_queue buffer
  __AXI_SRAM_START[12] : dma_conf_t *
  __AXI_SRAM_START[13] : dmamux_conf_t *
*/

#define ALLOC(a, s)      fa_alloc((a), (s))
#define FREE(a, p)       fa_free((a), (p))

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

typedef struct transfer{
  u8 *buf;
  u32 len;
  task_t *task;
} transfer_t;

typedef struct transfer_queue{
  transfer_t **queue; //buffer of transfer
  u32 size;           //size in words of the buffer
  u32 len;            //number of transfer in the queue
  u32 ihd;            //index of the head transfer
  u32 itl;            //index of the first free space at tail
} transfer_queue_t;

void tq_init(transfer_queue_t *tq, transfer_t **buf, u32 size){

  tq->queue = buf;
  tq->size = size;
  tq->len = 0;
  tq->ihd = 0;
  tq->itl = 0;
}

transfer_t *tq_peek(transfer_queue_t *tq){

  if(tq->len)
    return tq->queue[tq->ihd];
  else
    return (transfer_t *) 0;
}

transfer_t *tq_remove(transfer_queue_t *tq){

  transfer_t *tmp;
  
  if(tq->len){
    tmp = tq->queue[tq->ihd];
    
    if(tq->ihd == (tq->size - 1))
      tq->ihd = 0;
    else
      tq->ihd++;

    tq->len--;
    
    return tmp;
  }
  else
    return (transfer_t *) 0;
}

u32 tq_insert(transfer_queue_t *tq, transfer_t *t){

  if(tq->len == tq->size)
    return 0;

  tq->queue[tq->itl] = t;

  if(tq->itl == (tq->size - 1))
    tq->itl = 0;
  else
    tq->itl++;

  tq->len++;

  return 1;
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
void task3();

__attribute__((noreturn))
void h743_reset_isr(){

  h743_gpio_conf_t gpio_conf;
  h743_tim6_7_conf_t tim6_7_conf;
  h743_usart_conf_t usart_conf;
  h743_dma_conf_t dma_conf;
  h743_dmamux_conf_t dmamux_conf;

  task_t task_idle, task_task1, task_task2, task_task3;
  scheduler_t sched;
  task_t *elected;
  transfer_queue_t tq;
  freelist_alloc_t fa;

  task_idle.sp = (u32) __AXI_SRAM_END;
  task_idle.entry = (u32) idle;

  task_task1.sp = ((u32) __AXI_SRAM_END) - 4096;
  task_task1.entry = (u32) task1;

  task_task2.sp = ((u32) __AXI_SRAM_END) - 8192;
  task_task2.entry = (u32) task2;

  task_task3.sp = ((u32) __AXI_SRAM_END) - 12288;
  task_task3.entry = (u32) task3;

  init_task_stack(&task_idle);
  init_task_stack(&task_task1);
  init_task_stack(&task_task2);
  init_task_stack(&task_task3);

  scheduler_init(&sched, &task_idle);
  elected = scheduler_add(&sched, &task_task1);
  elected = scheduler_add(&sched, &task_task2);
  elected = scheduler_add(&sched, &task_task3);

  tq_init(&tq, ((transfer_t **) &__AXI_SRAM_START[5]), 6);

  fa_init(&fa, ((u32) __AXI_SRAM_START) + 4096, 1 << 18);
  
  __AXI_SRAM_START[0] = (u32) &sched;
  __AXI_SRAM_START[1] = (u32) &fa;
  __AXI_SRAM_START[4] = (u32) &tq;
  __AXI_SRAM_START[12] = (u32) &dma_conf;
  __AXI_SRAM_START[13] = (u32) &dmamux_conf;

  h743_rcc_enable_clock_apb1l(RCC_APB1L_TIM7);
  h743_rcc_enable_clock_ahb4(RCC_AHB4_GPIOI);
  h743_rcc_enable_clock_ahb4(RCC_AHB4_GPIOD);
  h743_rcc_enable_clock_apb1l(RCC_APB1L_USART2);
  h743_rcc_enable_clock_ahb1(RCC_AHB1_DMA1);

  ARM_CPSIE;

  h743_nvic_enable_irq(H743_IRQ_TIM7);
  h743_nvic_enable_irq(H743_IRQ_DMA1_STR0);

  h743_tim6_7_init_conf(tim6_7_conf);
  h743_gpio_init_conf(gpio_conf);
  h743_usart_init_conf(usart_conf);
  h743_dma_init_conf(dma_conf);
  h743_dmamux_init_conf(dmamux_conf);

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

  h743_usart_wait_rxne(USART2);
  
  tim6_7_conf.us = 1000000;
  h743_tim6_7_conf(TIM7, &tim6_7_conf, 64);
  h743_tim6_7_enable(TIM7);

  launch_first_task(elected->sp);
}

#define DMA_CONF    ((h743_dma_conf_t *) __AXI_SRAM_START[12])
#define DMAMUX_CONF ((h743_dmamux_conf_t *) __AXI_SRAM_START[13])

void setup_dma_transfer(u8 *buf, u32 len){

  DMA_CONF->trbuff = TRUE;
  DMA_CONF->pinc = FALSE;
  DMA_CONF->dir = mem_to_per;
  DMA_CONF->irq_en = DMA_IRQ_TC | DMA_IRQ_TE | DMA_IRQ_FE | DMA_IRQ_DME;
  DMA_CONF->num_data = len;
  DMA_CONF->per_address = (u32) &USART2->TDR;
  DMA_CONF->mem_address_0 = (u32) buf;
  DMA_CONF->fifo_mode = FALSE;
  h743_dma_conf(DMA1, 0, DMA_CONF);

  DMAMUX_CONF->request = DMAMUX_USART2_TX;
  h743_dmamux_conf(DMAMUX1, 0, DMAMUX_CONF);
  h743_dmamux_enable(DMAMUX1, 0);

  h743_usart_enable_dma_tx(USART2);

  h743_dma_enable(DMA1, 0);
}

__attribute__((naked))
void write_term(u8 *buf, u32 len){

  asm("\
svc 0\n						\
bx lr\n						\
");
}

void idle(){

  while(1);
}

__attribute__((noreturn))
void task1(){

  while(1){
    write_term((u8 *) "task1\n", 6);
  }
}

void task2(){

  while(1){
    write_term((u8 *) "task2\n", 6);
  }
}

void task3(){

  while(1){
    write_term((u8 *) "task3\n", 6);
  }
}

__attribute__((naked))
void h743_svcall_isr(){

  asm("\
mrs r0, psp\n					\
stmdb r0!, {r4-r11}\n				\
sub sp, sp, #4\n				\
str lr, [sp]\n					\
bl svchandler\n					\
ldr lr, [sp]\n					\
add sp, sp, #4\n				\
ldmia r0!, {r4-r11}\n				\
msr psp, r0\n					\
bx lr\n						\
");
}

#define SCHED       ((scheduler_t *) __AXI_SRAM_START[0])
#define FA          ((freelist_alloc_t *) __AXI_SRAM_START[1])
#define TQ          ((transfer_queue_t *) __AXI_SRAM_START[4])

u32 svchandler(u32 old_sp){

  task_t *new_elected;
  task_t *old_elected;
  transfer_t *transfer;
  
  SCHED->elected->sp = old_sp;

  old_elected = SCHED->elected;
  new_elected = scheduler_rmv(SCHED, SCHED->elected);

  transfer = (transfer_t *) ALLOC(FA, sizeof(transfer_t));

  transfer->buf = (u8 *) ((u32 *) old_sp)[8];
  transfer->len = ((u32 *) old_sp)[9];
  transfer->task = old_elected;

  tq_insert(TQ, transfer);

  if(TQ->len == 1){
    setup_dma_transfer(transfer->buf, transfer->len);
  }

  return new_elected->sp;
}

__attribute__((naked))
void h743_dma1_str0_isr(){

  asm("\
mrs r0, psp\n					\
stmdb r0!, {r4-r11}\n				\
sub sp, sp, #4\n				\
str lr, [sp]\n					\
bl dma_handler\n				\
ldr lr, [sp]\n					\
add sp, sp, #4\n				\
ldmia r0!, {r4-r11}\n				\
msr psp, r0\n					\
bx lr\n						\
");
}

u32 dma_handler(u32 old_sp){

  u32 pending_irq;
  task_t *new_elected = SCHED->elected;
  transfer_t *tmp;

  SCHED->elected->sp = old_sp;

  pending_irq = h743_dma_pending_irq(DMA1, 0);

  if(pending_irq & DMA_IRQ_TC){
    h743_dma_eoi(DMA1, 0, DMA_IRQ_TC);

    h743_gpio_set(GPIOI, 6);
    
    tmp = tq_remove(TQ);
    new_elected = scheduler_add(SCHED, tmp->task);
    FREE(FA, ((u32) tmp));
    
    if(TQ->len != 0){
      tmp = tq_peek(TQ);
      setup_dma_transfer(tmp->buf, tmp->len);
    }
  }
  else if(pending_irq & DMA_IRQ_TE){
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
  return new_elected->sp;
}

__attribute__((naked))
void h743_tim7_isr(){

  asm("\
mrs r0, psp\n					\
stmdb r0!, {r4-r11}\n				\
sub sp, sp, #4\n				\
str lr, [sp]\n					\
bl switch_task_main\n				\
ldr lr, [sp]\n					\
add sp, sp, #4\n				\
ldmia r0!, {r4-r11}\n				\
msr psp, r0\n					\
bx lr\n						\
");
}

u32 switch_task_main(u32 old_sp){

  task_t *new_elected;

  SCHED->elected->sp = old_sp;

  new_elected = scheduler_elect(SCHED);
  
  h743_tim6_7_eoi(TIM7);
  ARM_DSB;
  return new_elected->sp;
}
