

#ifndef __ILI9341_TERMINAL__
#define __ILI9341_TERMINAL__

#include "inc/stm32h743.h"

/*
  240 x 320 screen
*/

#define ILI9341_SCREEN_COL    240
#define ILI9341_SCREEN_LIN    320

#define ILI9341_COL_MIN       0
#define ILI9341_COL_MAX       29

#define ILI9341_LIN_MIN       0
#define ILI9341_LIN_MAX       19

__attribute__((packed))
typedef struct color{
  u8 r;
  u8 g;
  u8 b;
} color_t;

typedef struct bitmap8x16{
  u8 data[16];
} bitmap8x16_t;

typedef struct ili9341_terminal{
  volatile h743_spi_t *spi;
  
  volatile h743_gpio_t *gpio_dcx;
  u32 pin_dcx;

  volatile h743_gpio_t *gpio_csn;
  u32 pin_csn;
} ili9341_terminal_t;

void ili9341_term_clear_screen(ili9341_terminal_t *ili,
			       color_t clear_color);

void ili9341_term_print_char(ili9341_terminal_t *ili,
			     color_t foreground,
			     color_t background,
			     bitmap8x16_t *font,
			     u32 col,
			     u32 lin,
			     u8 c);

#endif
