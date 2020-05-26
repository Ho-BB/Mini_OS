

#ifndef __TASK__
#define __TASK__

#include "inc/stm32h743.h"

typedef struct task{
  u32 entry;
  u32 sp;
  u32 pid;

  u32 wrapper_entry;
  u32 argc;
  u32 argv;

  u32 initial_sp;
} task_t;

void task_init_stack(task_t *task);

__attribute__((noreturn, naked))
void task_launch_first(u32 sp);

void task_init_stack_wrapper(task_t *task);

#endif
