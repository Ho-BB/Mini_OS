# Mini OS

This is the start of a mini operating system.
So far dynamic memory allocation, task switching and system calls are implemented.
There are 3 user space programs, one that prints its arguments at regular intervals to demonstrate task switching, one that just echoes its arguments, and a little shell to launch these two programs or another shell.
This mini OS works on a STM32H743II chip.

Folders :
* doc : The documentation I used.
* firmware : C code interfacing with the chip's peripherals, and startup code (IVT)
* h743_flash : utility program I made to flash the chip, it is based on the code of [st-flash](https://github.com/stlink-org/stlink) (I couldn't use st-flash since it doesn't have support for the STM32H743II chip).
* mini_os : The code for the mini OS.
  * circular_buffer : circular buffer used by the OS to receive data via usart on behalf of requesting programs.
  * freelist_alloc : contains the dynamic memory allocator implementation, based on a list of free blocks.
  * kernel_data : contains the declaration of the structures used by the kernel.
  * list : contains an implementation of lists.
  * queue : contains an implementation of queues.
  * scheduler : contains the scheduler.
  * syscall : contains the list of system calls.
  * task : contains the structure representing a task, as well as code to initialise the stack of a new task and the code to launch the very first task when the OS is started (all the other tasks are launched by changing the SP and doing an exception return).
  