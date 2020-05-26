

#include "task.h"


void task_init_stack(task_t *task){

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

__attribute__((noreturn, naked))
void task_launch_first(u32 sp){

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

void task_init_stack_wrapper(task_t *task){

  task->sp -= 64;

  ((u32 *) task->sp)[0]  = 0;  //R4
  ((u32 *) task->sp)[1]  = 0;  //R5
  ((u32 *) task->sp)[2]  = 0;  //R6
  ((u32 *) task->sp)[3]  = 0;  //R7

  ((u32 *) task->sp)[4]  = 0;  //R8
  ((u32 *) task->sp)[5]  = 0;  //R9
  ((u32 *) task->sp)[6]  = 0;  //R10
  ((u32 *) task->sp)[7]  = 0;  //R11

  ((u32 *) task->sp)[8]  = task->argc;   //R0
  ((u32 *) task->sp)[9]  = task->argv;   //R1
  ((u32 *) task->sp)[10] = task->entry;  //R2
  ((u32 *) task->sp)[11] = 0;            //R3

  ((u32 *) task->sp)[12] = 0;                    //R12
  ((u32 *) task->sp)[13] = 0;                    //LR (R14)
  ((u32 *) task->sp)[14] = task->wrapper_entry;  //PC (R15)
  ((u32 *) task->sp)[15] = 0x01000000;           //xPSR
}
