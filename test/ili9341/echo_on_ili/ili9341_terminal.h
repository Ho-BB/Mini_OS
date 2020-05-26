

#ifndef __ILI9341_TERMINAL__
#define __ILI9341_TERMINAL__

#include "inc/stm32h743.h"

/*
  240 x 320 screen
*/

#define ILI9341_SCREEN_COL    240
#define ILI9341_SCREEN_LIN    320

#define ILI9341_COL_MIN       0
#define ILI9341_COL_MAX       30

#define ILI9341_LIN_MIN       0
#define ILI9341_LIN_MAX       20

#define ILI9341_CMD_READ_STATUS 0x09
#define ILI9341_CMD_SLEEP_OUT   0x11
#define ILI9341_CMD_DISPLAY_ON  0x29
#define ILI9341_CMD_CASET       0x2A
#define ILI9341_CMD_PASET       0x2B
#define ILI9341_CMD_MEM_WRITE   0x2C

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

  volatile h743_gpio_t *gpio_sck;
  u32 pin_sck;
  u32 af_sck;

  volatile h743_gpio_t *gpio_miso;
  u32 pin_miso;
  u32 af_miso;

  volatile h743_gpio_t *gpio_mosi;
  u32 pin_mosi;
  u32 af_mosi;

  volatile h743_gpio_t *gpio_reset;
  u32 pin_reset;

  color_t foreground;
  color_t background;
  bitmap8x16_t *font;
  u32 cur_col;
  u32 cur_lin;
} ili9341_terminal_t;

extern bitmap8x16_t ctrld_font[256];

void ili9341_term_init_struct_default(ili9341_terminal_t *ili);

void ili9341_term_init(ili9341_terminal_t *ili);

u32 ili9341_term_set_cursor(ili9341_terminal_t *ili,
			    u32 col,
			    u32 lin);

void ili9341_term_clear_screen(ili9341_terminal_t *ili,
			       color_t clear_color);

void ili9341_term_print_char(ili9341_terminal_t *ili,
			     u8 c);

void ili9341_term_print_str(ili9341_terminal_t *ili,
			    u8 *str);

void ili9341_term_print_strn(ili9341_terminal_t *ili,
			     u8 *str,
			     u32 n);

void ili9341_term_backspace(ili9341_terminal_t *ili);

#endif
