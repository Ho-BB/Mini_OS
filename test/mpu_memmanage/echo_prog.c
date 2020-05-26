

#include "echo_prog.h"
#include "inc/stm32h743.h"

extern u32 __AXI_SRAM_START[];

__attribute__((noreturn))
void echo_prog(){

  u8 c = 'a';
  
  h743_gpio_reset(GPIOI, 6);
  open_usart();
  //h743_gpio_set(GPIOI, 6);

  receive_byte();
  //h743_gpio_set(GPIOI, 6);
  c = __AXI_SRAM_START[0];
  h743_gpio_set(GPIOI, 6);
  
  c = 'A';

  while(1){
    receive_byte();
    transmit_byte(c++);
  }
}
