

#ifndef __SCHEDULER__
#define __SCHEDULER__

#include "inc/stm32h743.h"
#include "queue.h"
#include "task.h"

typedef struct scheduler{
  u32 num_task;
  queue_t queue;
  task_t *elected;
  task_t *idle;
} scheduler_t;

void sched_init(scheduler_t *sched,
		u32 *queue_buf,
		u32 queue_size,
		task_t *idle);

task_t *sched_elect(scheduler_t *sched);

task_t *sched_add(scheduler_t *sched, task_t *new_task, u32 *changed);

task_t *sched_rmv(scheduler_t *sched, task_t *rmv_task, u32 *changed);

#endif
