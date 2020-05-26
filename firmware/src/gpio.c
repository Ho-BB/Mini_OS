

#include "inc/gpio.h"

void h743_gpio_conf(volatile h743_gpio_t *gpiox,
		    h743_gpio_conf_t *conf){

  conf->pin &= 0x0F;
  conf->alternate_function &= 0x0F;
  
  gpiox->MODER =
    (gpiox->MODER & ~(3 << (conf->pin << 1))) |
    (conf->mode << (conf->pin << 1));

  gpiox->OTYPER =
    (gpiox->OTYPER & ~(1 << conf->pin)) |
    (conf->output_type << conf->pin);

  gpiox->OSPEEDR =
    (gpiox->OSPEEDR & ~(3 << (conf->pin << 1))) |
    (conf->output_speed << (conf->pin << 1));

  gpiox->PUPDR =
    (gpiox->PUPDR & ~(3 << (conf->pin << 1))) |
    (conf->pull_updown << (conf->pin << 1));

  if(conf->pin & 8)
    gpiox->AFRH =
      (gpiox->AFRH & ~(0x0F << ((conf->pin & 7) << 2))) |
      (conf->alternate_function << ((conf->pin & 7) << 2));
  else
    gpiox->AFRL =
      (gpiox->AFRL & ~(0x0F << (conf->pin << 2))) |
      (conf->alternate_function << (conf->pin << 2));
}
