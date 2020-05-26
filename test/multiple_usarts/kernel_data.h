

#ifndef __KERNEL_DATA__
#define __KERNEL_DATA__

#include "inc/stm32h743.h"
#include "freelist_alloc.h"
#include "circular_buffer.h"
#include "list.h"
#include "queue.h"
#include "scheduler.h"
#include "task.h"

typedef struct usart_write_request{
  task_t *task;
  u8 *buf;
  u32 len;
} usart_write_request_t;

typedef struct usart_read_request{
  u8 *buf;
  u32 len;
  u32 *read;
} usart_read_request_t;

typedef struct reading_task{
  circular_buffer_t receive_buffer;
  task_t *task;
  u32 read_request_pending;
  usart_read_request_t read_request;
} reading_task_t;

typedef struct program{
  u8 *name;
  u32 entry;
} program_t;

typedef struct sleeping_task{
  task_t *task;
  u32 rem_ms;
} sleeping_task_t;


extern u32 __AXI_SRAM_START[];

typedef struct kernel_data{
  freelist_alloc_t alloc;
  scheduler_t sched;

  queue_t usart2_transmit_queue;
  queue_t usart3_transmit_queue;

  circular_buffer_t usart2_receive_buffer;
  circular_buffer_t usart3_receive_buffer;

  list_t usart2_reading_tasks;
  list_t usart3_reading_tasks;

  list_t task_to_add;

  list_t program_list;

  list_t sleeping_list;

  u32 cur_pid;

  h743_gpio_conf_t gpio_conf;
  h743_tim6_7_conf_t tim6_7_conf;
  h743_usart_conf_t usart_conf;
  h743_spi_conf_t spi_conf;
  h743_dma_conf_t dma_conf;
  h743_dmamux_conf_t dmamux_conf;
} kernel_data_t;

#define KERNEL_DATA           ((kernel_data_t *) __AXI_SRAM_START[0])

#define QUANTUM_US            10000

#endif
