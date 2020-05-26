

#include "echo_prog.h"
#include "inc/stm32h743.h"

__attribute__((noreturn))
void echo_prog(){

  u8 c = 'a';
  
  h743_gpio_reset(GPIOI, 6);
  open_usart();
  h743_gpio_set(GPIOI, 6);

  while(1){
    receive_byte();
    transmit_byte(c++);
  }
}
