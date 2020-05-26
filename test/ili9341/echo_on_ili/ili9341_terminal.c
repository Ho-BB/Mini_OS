

#include "ili9341_terminal.h"

static void cpaset(ili9341_terminal_t *ili, u32 caset, u32 start, u32 end){

  u32 cmd;

  if(caset)
    cmd = ILI9341_CMD_CASET;
  else
    cmd = ILI9341_CMD_PASET;
  
  h743_gpio_reset(ili->gpio_csn, ili->pin_csn);
  h743_gpio_reset(ili->gpio_dcx, ili->pin_dcx);
  
  h743_spi_transmit_byte(ili->spi, cmd);
  h743_spi_wait_rxp(ili->spi);
  cmd = h743_spi_receive_byte(ili->spi);

  h743_gpio_set(ili->gpio_dcx, ili->pin_dcx);

  h743_spi_transmit_byte(ili->spi, start >> 8);
  h743_spi_wait_rxp(ili->spi);
  cmd = h743_spi_receive_byte(ili->spi);

  h743_spi_transmit_byte(ili->spi, start & 0x0FF);
  h743_spi_wait_rxp(ili->spi);
  cmd = h743_spi_receive_byte(ili->spi);

  h743_spi_transmit_byte(ili->spi, end >> 8);
  h743_spi_wait_rxp(ili->spi);
  cmd = h743_spi_receive_byte(ili->spi);

  h743_spi_transmit_byte(ili->spi, end & 0x0FF);
  h743_spi_wait_rxp(ili->spi);
  cmd = h743_spi_receive_byte(ili->spi);

  h743_gpio_set(ili->gpio_csn, ili->pin_csn);
}

static void mem_write(ili9341_terminal_t *ili){

  u32 discard;
  
  h743_gpio_reset(ili->gpio_csn, ili->pin_csn);
  h743_gpio_reset(ili->gpio_dcx, ili->pin_dcx);

  h743_spi_transmit_byte(ili->spi, ILI9341_CMD_MEM_WRITE);
  h743_spi_wait_rxp(ili->spi);
  discard = h743_spi_receive_byte(ili->spi);

  h743_gpio_set(ili->gpio_dcx, ili->pin_dcx);
}

void ili9341_term_init_struct_default(ili9341_terminal_t *ili){

  ili->spi = SPI2;
  
  ili->gpio_dcx = GPIOD;
  ili->pin_dcx = 8;

  ili->gpio_csn = GPIOB;
  ili->pin_csn = 12;

  ili->gpio_sck = GPIOB;
  ili->pin_sck = 13;
  ili->af_sck = 5;

  ili->gpio_miso = GPIOB;
  ili->pin_miso = 14;
  ili->af_miso = 5;

  ili->gpio_mosi = GPIOB;
  ili->pin_mosi = 15;
  ili->af_mosi = 5;

  ili->gpio_reset = GPIOH;
  ili->pin_reset = 12;

  ili->foreground.r = 0xFF;
  ili->foreground.g = 0xFF;
  ili->foreground.b = 0xFF;

  ili->background.r = 0x00;
  ili->background.g = 0x00;
  ili->background.b = 0x00;

  ili->font = ctrld_font;

  ili->cur_col = ILI9341_COL_MIN;
  ili->cur_lin = ILI9341_LIN_MAX - 1;
}

void ili9341_term_init(ili9341_terminal_t *ili){

  h743_gpio_conf_t gpio_conf;
  h743_spi_conf_t spi_conf;

  h743_gpio_init_conf(gpio_conf);
  h743_spi_init_conf(spi_conf);

  /* Configure Spi IOs */
  gpio_conf.pin = ili->pin_csn;
  gpio_conf.mode = output;
  gpio_conf.output_type = push_pull;
  gpio_conf.output_speed = high;
  h743_gpio_conf(ili->gpio_csn, &gpio_conf);

  gpio_conf.pin = ili->pin_sck;
  gpio_conf.mode = alternate_function;
  gpio_conf.alternate_function = ili->af_sck;
  h743_gpio_conf(ili->gpio_sck, &gpio_conf);

  gpio_conf.pin = ili->pin_miso;
  gpio_conf.mode = alternate_function;
  gpio_conf.alternate_function = ili->af_miso;
  h743_gpio_conf(ili->gpio_miso, &gpio_conf);

  gpio_conf.pin = ili->pin_mosi;
  gpio_conf.mode = alternate_function;
  gpio_conf.alternate_function = ili->af_mosi;
  h743_gpio_conf(ili->gpio_mosi, &gpio_conf);

  /* Configure D/CX IO */
  gpio_conf.pin = ili->pin_dcx;
  gpio_conf.mode = output;
  gpio_conf.output_type = push_pull;
  gpio_conf.output_speed = high;
  h743_gpio_conf(ili->gpio_dcx, &gpio_conf);

  /* Configure RESET IO */
  gpio_conf.pin = ili->pin_reset;
  gpio_conf.mode = output;
  gpio_conf.output_type = push_pull;
  gpio_conf.output_speed = high;
  h743_gpio_conf(ili->gpio_reset, &gpio_conf);

  spi_conf.mbr = clock_div_8;
  h743_spi_conf(ili->spi, &spi_conf);
  h743_spi_enable(ili->spi);

  h743_gpio_set(ili->gpio_csn, ili->pin_csn);

  h743_gpio_reset(ili->gpio_reset, ili->pin_reset);

  u32 i;
  i = 3000000;
  while(i--);

  h743_gpio_set(ili->gpio_reset, ili->pin_reset);

  i = 100000;
  while(i--);

  /* Sleep Out mode */
  h743_gpio_reset(ili->gpio_csn, ili->pin_csn);
  h743_gpio_reset(ili->gpio_dcx, ili->pin_dcx);
  h743_spi_transmit_byte(ili->spi, ILI9341_CMD_SLEEP_OUT);
  h743_spi_wait_rxp(ili->spi);
  i = h743_spi_receive_byte(ili->spi);

  h743_gpio_set(ili->gpio_csn, ili->pin_csn);

  /* Wait atleast 5ms */
  i = 1000000;
  while(i--);

  /* Display On*/
  h743_gpio_reset(ili->gpio_csn, ili->pin_csn);
  h743_gpio_reset(ili->gpio_dcx, ili->pin_dcx);
  h743_spi_transmit_byte(ili->spi, ILI9341_CMD_DISPLAY_ON);
  h743_spi_wait_rxp(ili->spi);
  i = h743_spi_receive_byte(ili->spi);

  h743_gpio_set(ili->gpio_csn, ili->pin_csn);
}

u32 ili9341_term_set_cursor(ili9341_terminal_t *ili,
			    u32 col,
			    u32 lin){

  if(col >= ILI9341_COL_MAX || lin >= ILI9341_LIN_MAX)
    return 0;

  ili->cur_col = col;
  ili->cur_lin = lin;

  return 1;
}

void ili9341_term_clear_screen(ili9341_terminal_t *ili,
			       color_t clear_color){

  u32 i;
  u32 discard;
  
  cpaset(ili, 1, 0, ILI9341_SCREEN_COL - 1);
  cpaset(ili, 0, 0, ILI9341_SCREEN_LIN - 1);

  mem_write(ili);

  for(i = 0; i < ILI9341_SCREEN_COL * ILI9341_SCREEN_LIN; i++){
    h743_spi_transmit_byte(ili->spi, clear_color.b);
    h743_spi_wait_rxp(ili->spi);
    discard = h743_spi_receive_byte(ili->spi);

    h743_spi_transmit_byte(ili->spi, clear_color.g);
    h743_spi_wait_rxp(ili->spi);
    discard = h743_spi_receive_byte(ili->spi);

    h743_spi_transmit_byte(ili->spi, clear_color.r);
    h743_spi_wait_rxp(ili->spi);
    discard = h743_spi_receive_byte(ili->spi);
  }

  h743_gpio_set(ili->gpio_csn, ili->pin_csn);
}

void ili9341_term_print_char(ili9341_terminal_t *ili,
			     u8 c){

  int i, j;
  bitmap8x16_t glyph;
  color_t cur_color;
  u32 discard;

  if(c == '\n'){
    if(ili->cur_lin == ILI9341_LIN_MIN)
      ili->cur_lin = ILI9341_LIN_MAX - 1;
    else
      ili->cur_lin--;

    ili->cur_col = ILI9341_COL_MIN;
  }
  else{
    cpaset(ili, 1, ili->cur_col << 3, (ili->cur_col << 3) + 7);
    cpaset(ili, 0, ili->cur_lin << 4, (ili->cur_lin << 4) + 15);

    mem_write(ili);
    
    glyph = ili->font[c];
    
    for(i = 15; i >= 0; i--){
      for(j = 7; j >= 0; j--){
	if((glyph.data[i] >> j) & 1)
	  cur_color = ili->foreground;
	else
	  cur_color = ili->background;
	
	h743_spi_transmit_byte(ili->spi, cur_color.b);
	h743_spi_wait_rxp(ili->spi);
	discard = h743_spi_receive_byte(ili->spi);

	h743_spi_transmit_byte(ili->spi, cur_color.g);
	h743_spi_wait_rxp(ili->spi);
	discard = h743_spi_receive_byte(ili->spi);
	
	h743_spi_transmit_byte(ili->spi, cur_color.r);
	h743_spi_wait_rxp(ili->spi);
	discard = h743_spi_receive_byte(ili->spi);
      }
    }
    
    h743_gpio_set(ili->gpio_csn, ili->pin_csn);

    if(ili->cur_col == ILI9341_COL_MAX - 1){
      ili->cur_col = ILI9341_COL_MIN;
      
      if(ili->cur_lin == ILI9341_LIN_MIN)
	ili->cur_lin = ILI9341_LIN_MAX - 1;
      else
	ili->cur_lin--;
    }
    else
      ili->cur_col++;
  }
}

void ili9341_term_print_str(ili9341_terminal_t *ili,
			    u8 *str){

  while(*str)
    ili9341_term_print_char(ili, *str++);
}

void ili9341_term_print_strn(ili9341_terminal_t *ili,
			     u8 *str,
			     u32 n){

  while(n--)
    ili9341_term_print_char(ili, *str++);
}

void ili9341_term_backspace(ili9341_terminal_t *ili){

  u32 new_col, new_lin;
  
  if(ili->cur_col == ILI9341_COL_MIN){
    new_col = ILI9341_COL_MAX - 1;
    
    if(ili->cur_lin == ILI9341_LIN_MAX - 1)
      new_lin = ILI9341_LIN_MIN;
    else
      new_lin = ili->cur_lin + 1;
  }
  else{
    new_col = ili->cur_col - 1;
    new_lin = ili->cur_lin;
  }

  ili->cur_col = new_col;
  ili->cur_lin = new_lin;
  
  ili9341_term_print_char(ili, ' ');

  ili->cur_col = new_col;
  ili->cur_lin = new_lin;
}
