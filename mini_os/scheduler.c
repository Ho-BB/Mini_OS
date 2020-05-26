

#include "scheduler.h"


void sched_init(scheduler_t *sched,
		u32 *queue_buf,
		u32 queue_size,
		task_t *idle){

  sched->num_task = 0;
  queue_init(&sched->queue, queue_buf, queue_size);
  sched->elected = idle;
  sched->idle = idle;
}

task_t *sched_elect(scheduler_t *sched){

  if(sched->num_task > 1){
    queue_insert(&sched->queue, (u32) sched->elected);
    sched->elected = (task_t *) queue_remove(&sched->queue);
  }

  return sched->elected;
}

task_t *sched_add(scheduler_t *sched, task_t *new_task, u32 *changed){

  if(sched->num_task == 0){
    sched->elected = new_task;
    *changed = 1;
  }
  else{
    queue_insert(&sched->queue, (u32) new_task);
    *changed = 0;
  }

  sched->num_task++;

  return sched->elected;
}

task_t *sched_rmv(scheduler_t *sched, task_t *rmv_task, u32 *changed){

  u32 n;
  task_t *tmp;
  
  *changed = 0;
  
  if(sched->num_task != 0){
    if(sched->elected == rmv_task){
      if(sched->num_task == 1)
	sched->elected = sched->idle;
      else
	sched->elected = (task_t *) queue_remove(&sched->queue);

      *changed = 1;
      sched->num_task--;
    }
    else{
      n = sched->queue.len;
      while(n--){
	tmp = (task_t *) queue_remove(&sched->queue);
	if(tmp == rmv_task)
	  sched->num_task--;
	else
	  queue_insert(&sched->queue, (u32) tmp);
      }
    }
  }

  return sched->elected;
}
