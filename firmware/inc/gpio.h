

#ifndef __H743_GPIO__
#define __H743_GPIO__

#include "inc/typedef.h"

typedef struct h743_gpio{
  u32 MODER;
  u32 OTYPER;
  u32 OSPEEDR;
  u32 PUPDR;
  u32 IDR;
  u32 ODR;
  u32 BSRR;
  u32 LCKR;
  u32 AFRL;
  u32 AFRH;
} h743_gpio_t;

/* 11 GPIO */
#define GPIOA            ((volatile h743_gpio_t *) 0x58020000)
#define GPIOB            ((volatile h743_gpio_t *) 0x58020400)
#define GPIOC            ((volatile h743_gpio_t *) 0x58020800)
#define GPIOD            ((volatile h743_gpio_t *) 0x58020C00)
#define GPIOE            ((volatile h743_gpio_t *) 0x58021000)
#define GPIOF            ((volatile h743_gpio_t *) 0x58021400)
#define GPIOG            ((volatile h743_gpio_t *) 0x58021800)
#define GPIOH            ((volatile h743_gpio_t *) 0x58021C00)
#define GPIOI            ((volatile h743_gpio_t *) 0x58022000)
#define GPIOJ            ((volatile h743_gpio_t *) 0x58022400)
#define GPIOK            ((volatile h743_gpio_t *) 0x58022800)

typedef enum h743_gpio_mode
  {
   input = 0,
   output = 1,
   alternate_function = 2,
   analog = 3
  }h743_gpio_mode_e;

typedef enum h743_gpio_output_type
  {
   push_pull = 0,
   open_drain = 1
  }h743_gpio_output_type_e;

typedef enum h743_gpio_output_speed
  {
   low = 0,
   medium = 1,
   high = 2,
   very_high = 3
  }h743_gpio_output_speed_e;

typedef enum h743_gpio_pull_updown
  {
   no_pull_updown = 0,
   pull_up = 1,
   pull_down = 2
  }h743_gpio_pull_updown_e;

typedef struct h743_gpio_conf{
  u8 pin;
  h743_gpio_mode_e mode;
  h743_gpio_output_type_e output_type;
  h743_gpio_output_speed_e output_speed;
  h743_gpio_pull_updown_e pull_updown;
  u8 alternate_function;
} h743_gpio_conf_t;

#define h743_gpio_init_conf(conf)				\
  conf = (h743_gpio_conf_t) { .pin = 0,				\
			      .mode = analog,			\
			      .output_type = push_pull,		\
			      .output_speed = low,		\
			      .pull_updown = no_pull_updown,	\
			      .alternate_function = 0		\
  };

void h743_gpio_conf(volatile h743_gpio_t *gpiox,
		    h743_gpio_conf_t *conf);

#define h743_gpio_read(gpiox, pin)		\
  ((gpiox->IDR & (1 << (pin & 0x0F))) != 0)

#define h743_gpio_set(gpiox, pin)		\
  gpiox->BSRR = 1 << (pin & 0x0F)

#define h743_gpio_reset(gpiox, pin)		\
  gpiox->BSRR = (1 << 16) << (pin & 0x0F)

#define h743_gpio_toggle(gpiox, pin)		\
  if(h743_gpio_read(gpiox, pin))				\
    h743_gpio_reset(gpiox, pin);				\
  else								\
    h743_gpio_set(gpiox, pin)

#endif
