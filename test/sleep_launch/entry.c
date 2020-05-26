

#include "inc/stm32h743.h"
#include "kernel_data.h"
#include "syscall.h"

#define SCHED_QUEUE_SIZE      64
#define TRANSMIT_QUEUE_SIZE   16
#define RECEIVE_BUF_SIZE      256

extern u32 __AXI_SRAM_END[];

void idle();
void shell();
void hello();

__attribute__((noreturn))
void h743_reset_isr(){

  kernel_data_t kernel_data;
  u32 sched_queue_buf[SCHED_QUEUE_SIZE];
  u32 transmit_queue_buf[TRANSMIT_QUEUE_SIZE];
  u8 receive_buf[RECEIVE_BUF_SIZE];
  task_t idle_task;
  task_t shell_task;
  task_t *elected;
  u32 changed;
  program_t hello_program;

  /* Init kernel_data */
  __AXI_SRAM_START[0] = (u32) &kernel_data;

  h743_gpio_init_conf(kernel_data.gpio_conf);
  h743_tim6_7_init_conf(kernel_data.tim6_7_conf);
  h743_usart_init_conf(kernel_data.usart_conf);
  h743_spi_init_conf(kernel_data.spi_conf);
  h743_dma_init_conf(kernel_data.dma_conf);
  h743_dmamux_init_conf(kernel_data.dmamux_conf);

  idle_task.pid = 0;
  idle_task.entry = (u32) idle;
  idle_task.sp = (u32) __AXI_SRAM_END;

  shell_task.pid = 1;
  shell_task.entry = (u32) shell;
  shell_task.sp = ((u32) __AXI_SRAM_END) - 4096;

  task_init_stack(&idle_task);
  task_init_stack(&shell_task);

  fa_init(&kernel_data.alloc, ((u32) __AXI_SRAM_START) + 4096, 1 << 18);

  sched_init(&kernel_data.sched, sched_queue_buf, SCHED_QUEUE_SIZE, &idle_task);

  elected = sched_add(&kernel_data.sched, &shell_task, &changed);

  queue_init(&kernel_data.usart_transmit_queue,
	     transmit_queue_buf,
	     TRANSMIT_QUEUE_SIZE);

  cb_init(&kernel_data.usart_receive_buffer, receive_buf, RECEIVE_BUF_SIZE);

  list_init(&kernel_data.usart_reading_tasks, &kernel_data.alloc);
  list_init(&kernel_data.task_to_add, &kernel_data.alloc);
  list_init(&kernel_data.program_list, &kernel_data.alloc);
  list_init(&kernel_data.sleeping_list, &kernel_data.alloc);

  hello_program.name = (u8 *) "hello";
  hello_program.entry = (u32) hello;

  list_insert(&kernel_data.program_list, (u32) &hello_program);

  kernel_data.cur_pid = 2;

  /* Enable peripheral clocks */
  h743_rcc_enable_clock_apb1l(RCC_APB1L_TIM7);
  h743_rcc_enable_clock_ahb4(RCC_AHB4_GPIOI);
  h743_rcc_enable_clock_ahb4(RCC_AHB4_GPIOD);
  h743_rcc_enable_clock_apb1l(RCC_APB1L_USART2);
  h743_rcc_enable_clock_ahb1(RCC_AHB1_DMA1);

  /* Enable IRQs */
  ARM_CPSIE;

  h743_nvic_enable_irq(H743_IRQ_TIM7);
  h743_nvic_enable_irq(H743_IRQ_DMA1_STR0);
  h743_nvic_enable_irq(H743_IRQ_USART2);

  /* Configure LED */
  kernel_data.gpio_conf.pin = 6;
  kernel_data.gpio_conf.mode = output;
  h743_gpio_conf(GPIOI, &kernel_data.gpio_conf);

  h743_gpio_reset(GPIOI, 6);

  /* Configure USART */
  kernel_data.gpio_conf.pin = 5;
  kernel_data.gpio_conf.mode = alternate_function;
  kernel_data.gpio_conf.alternate_function = 7;

  h743_gpio_conf(GPIOD, &kernel_data.gpio_conf);  //TX

  kernel_data.gpio_conf.pin = 6;
  kernel_data.gpio_conf.mode = alternate_function;
  kernel_data.gpio_conf.alternate_function = 7;

  h743_gpio_conf(GPIOD, &kernel_data.gpio_conf);  //RX

  //kernel_data.usart_conf.irq_en = USART_IRQ_RXNE;
  h743_usart_conf(USART2, &kernel_data.usart_conf, 64000000);
  h743_usart_enable(USART2);

  /* Wait before proceding */
  h743_usart_wait_rxne(USART2);
  //changed = h743_usart_receive(USART2);

  /* Enable Usart RX IRQ */
  h743_usart_enable_irq(USART2, USART_IRQ_RXNE);

  /* Enable scheduling timer */
  kernel_data.tim6_7_conf.us = QUANTUM_US;
  h743_tim6_7_conf(TIM7, &kernel_data.tim6_7_conf, 64);
  h743_tim6_7_enable(TIM7);

  /* Launch first task */
  task_launch_first(elected->sp);
}


void setup_usart_tx_dma(u8 *buf, u32 len){

  KERNEL_DATA->dma_conf.trbuff = TRUE;
  KERNEL_DATA->dma_conf.pinc = FALSE;
  KERNEL_DATA->dma_conf.dir = mem_to_per;
  KERNEL_DATA->dma_conf.irq_en =
    DMA_IRQ_TC | DMA_IRQ_TE | DMA_IRQ_FE | DMA_IRQ_DME;
  KERNEL_DATA->dma_conf.num_data = len;
  KERNEL_DATA->dma_conf.per_address = (u32) &USART2->TDR;
  KERNEL_DATA->dma_conf.mem_address_0 = (u32) buf;
  KERNEL_DATA->dma_conf.fifo_mode = FALSE;
  h743_dma_conf(DMA1, 0, &KERNEL_DATA->dma_conf);

  KERNEL_DATA->dmamux_conf.request = DMAMUX_USART2_TX;
  h743_dmamux_conf(DMAMUX1, 0, &KERNEL_DATA->dmamux_conf);
  h743_dmamux_enable(DMAMUX1, 0);

  h743_usart_enable_dma_tx(USART2);

  h743_dma_enable(DMA1, 0);
}


void idle(){

  while(1);
}

int strn_cmp(u8 *str1, u8 *str2, u32 n){

  while(n--)
    if(*str1++ != *str2++)
      return 0;

  return 1;
}

void shell(){

  u8 buf[64];
  u32 read;
  
  syscall_usart_open();
  
  while(1){
    syscall_usart_read(buf, 64, &read);
    if(strn_cmp(buf, (u8 *) "hello\n", 6)){
      syscall_usart_write((u8 *) "Launching hello\n", 16);
      syscall_launch((u8 *) "hello");
    }
    else{
      syscall_usart_write((u8 *) "Unknown program : ", 18);
      syscall_usart_write(buf, read);
    }
  }

  syscall_usart_close();
}

void hello(){

  while(1){
    syscall_usart_write((u8 *) "hello world\n", 12);
    syscall_sleep(2000);
  }
}


void h743_usart2_isr(){

  u8 c;
  u8 buf[RECEIVE_BUF_SIZE];
  u32 len;
  list_elem_t *cur;
  
  c = h743_usart_receive(USART2);

  cb_write(&KERNEL_DATA->usart_receive_buffer, &c, 1);
  if(c == '\n'){
    len = cb_read(&KERNEL_DATA->usart_receive_buffer, buf, RECEIVE_BUF_SIZE);

    cur = KERNEL_DATA->usart_reading_tasks.list;
    while(cur){
      cb_write(&((reading_task_t *) cur->data)->receive_buffer, buf, len);

      if(((reading_task_t *) cur->data)->read_request_pending){
	*((reading_task_t *) cur->data)->read_request.read =
	  cb_read(&((reading_task_t *) cur->data)->receive_buffer,
		  ((reading_task_t *) cur->data)->read_request.buf,
		  ((reading_task_t *) cur->data)->read_request.len);

	list_insert(&KERNEL_DATA->task_to_add,
		    (u32) ((reading_task_t *) cur->data)->task);

	((reading_task_t *) cur->data)->read_request_pending = 0;
      }

      cur = cur->next;
    }
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


void h743_dma1_str0_isr(){

  u32 pending_irq;
  usart_write_request_t *tmp;

  pending_irq = h743_dma_pending_irq(DMA1, 0);

  if(pending_irq & DMA_IRQ_TC){
    h743_dma_eoi(DMA1, 0, DMA_IRQ_TC);

    tmp = (usart_write_request_t *)
      queue_remove(&KERNEL_DATA->usart_transmit_queue);
    list_insert(&KERNEL_DATA->task_to_add, (u32) tmp->task);
    fa_free(&KERNEL_DATA->alloc, (u32) tmp);

    if(KERNEL_DATA->usart_transmit_queue.len != 0){
      tmp = (usart_write_request_t *)
	queue_peek(&KERNEL_DATA->usart_transmit_queue);
      setup_usart_tx_dma(tmp->buf, tmp->len);
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


u32 svchandler(u32 old_sp){

  u8 syscall_number = ((u8 *) ((u32 *) old_sp)[14])[-2];
  reading_task_t *reading_task;
  u8 *buf;
  list_elem_t *cur;
  usart_write_request_t *write_request;
  u32 changed;
  sleeping_task_t *sleeping_task;
  task_t *new_task;

  KERNEL_DATA->sched.elected->sp = old_sp;

  switch(syscall_number){
  case SYSCALL_USART_OPEN :
    reading_task = (reading_task_t *) fa_alloc(&KERNEL_DATA->alloc,
					       sizeof(reading_task_t));
    buf = (u8 *) fa_alloc(&KERNEL_DATA->alloc, RECEIVE_BUF_SIZE);
    
    cb_init(&reading_task->receive_buffer, buf, RECEIVE_BUF_SIZE);
    reading_task->task = KERNEL_DATA->sched.elected;
    reading_task->read_request_pending = 0;

    list_insert(&KERNEL_DATA->usart_reading_tasks, (u32) reading_task);
    break;
  case SYSCALL_USART_CLOSE :
    cur = KERNEL_DATA->usart_reading_tasks.list;
    while(cur){
      if(((reading_task_t *) cur->data)->task == KERNEL_DATA->sched.elected){
	reading_task = (reading_task_t *) cur->data;
	list_remove(&KERNEL_DATA->usart_reading_tasks, cur->data);
	fa_free(&KERNEL_DATA->alloc, (u32) reading_task->receive_buffer.buf);
	fa_free(&KERNEL_DATA->alloc, (u32) reading_task);
	break;
      }

      cur = cur->next;
    }
    break;
  case SYSCALL_USART_WRITE :
    write_request =
      (usart_write_request_t *) fa_alloc(&KERNEL_DATA->alloc,
					 sizeof(usart_write_request_t));

    write_request->task = KERNEL_DATA->sched.elected;
    write_request->buf = (u8 *) ((u32 *) old_sp)[8];
    write_request->len = ((u32 *) old_sp)[9];

    sched_rmv(&KERNEL_DATA->sched, KERNEL_DATA->sched.elected, &changed);

    queue_insert(&KERNEL_DATA->usart_transmit_queue, (u32) write_request);

    if(KERNEL_DATA->usart_transmit_queue.len == 1)
      setup_usart_tx_dma(write_request->buf, write_request->len);
    break;
  case SYSCALL_USART_READ :
    cur = KERNEL_DATA->usart_reading_tasks.list;

    while(cur){
      if(((reading_task_t *) cur->data)->task == KERNEL_DATA->sched.elected){
	if(((reading_task_t *) cur->data)->receive_buffer.len != 0){
	  *((u32 **) old_sp)[10] =
	    cb_read(&((reading_task_t *) cur->data)->receive_buffer,
		    (u8 *) ((u32 *) old_sp)[8],
		    ((u32 *) old_sp)[9]);
	}
	else{
	  reading_task = (reading_task_t *) cur->data;
	  reading_task->read_request_pending = 1;
	  reading_task->read_request.buf = (u8 *) ((u32 *) old_sp)[8];
	  reading_task->read_request.len = ((u32 *) old_sp)[9];
	  reading_task->read_request.read = ((u32 **) old_sp)[10];

	  sched_rmv(&KERNEL_DATA->sched, KERNEL_DATA->sched.elected, &changed);
	}
	break;
      }
      
      cur = cur->next;
    }
    break;
  case SYSCALL_SLEEP :
    sleeping_task =
      (sleeping_task_t *) fa_alloc(&KERNEL_DATA->alloc,
				   sizeof(sleeping_task_t));

    sleeping_task->task = KERNEL_DATA->sched.elected;
    sleeping_task->rem_ms = ((u32 *) old_sp)[8];

    sched_rmv(&KERNEL_DATA->sched, KERNEL_DATA->sched.elected, &changed);

    list_insert(&KERNEL_DATA->sleeping_list, (u32) sleeping_task);
    break;
  case SYSCALL_LAUNCH :
    new_task = (task_t *) fa_alloc(&KERNEL_DATA->alloc, sizeof(task_t));
    
    new_task->sp = fa_alloc(&KERNEL_DATA->alloc, 4096) + 4096;
    new_task->pid = KERNEL_DATA->cur_pid++;

    //to change
    new_task->entry = (u32) hello;

    task_init_stack(new_task);

    sched_add(&KERNEL_DATA->sched, new_task, &changed);
    break;
  }

  return KERNEL_DATA->sched.elected->sp;
}


u32 switch_task_main(u32 old_sp){

  task_t *new_elected;
  task_t *cur;
  u32 changed;
  u32 was_changed = 0;
  list_elem_t *le_cur;
  sleeping_task_t *sleeping_task;

  KERNEL_DATA->sched.elected->sp = old_sp;

  while(KERNEL_DATA->task_to_add.length){
    cur = (task_t *) KERNEL_DATA->task_to_add.list->data;
    new_elected = sched_add(&KERNEL_DATA->sched, cur, &changed);
    list_remove(&KERNEL_DATA->task_to_add, (u32) cur);
    was_changed |= changed;
  }

  le_cur = KERNEL_DATA->sleeping_list.list;
  while(le_cur){
    sleeping_task = (sleeping_task_t *) le_cur->data;

    if(sleeping_task->rem_ms <= (QUANTUM_US / 1000)){
      sched_add(&KERNEL_DATA->sched, sleeping_task->task, &changed);

      fa_free(&KERNEL_DATA->alloc, (u32) sleeping_task);

      le_cur = le_cur->next;
      
      list_remove(&KERNEL_DATA->sleeping_list, (u32) sleeping_task);

      was_changed |= changed;
    }
    else{
      sleeping_task->rem_ms -= (QUANTUM_US / 1000);
      le_cur = le_cur->next;
    }
  }

  if(!was_changed || was_changed)
    new_elected = sched_elect(&KERNEL_DATA->sched);

  h743_tim6_7_eoi(TIM7);
  ARM_DSB;
  return new_elected->sp;
}
