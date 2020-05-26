

#include <libusb-1.0/libusb.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define STLINK_CMD_SIZE 16
#define STLINK_GET_VERSION 0xF1
#define ENDPOINT_REP 1 | LIBUSB_ENDPOINT_IN
#define ENDPOINT_REQ 2 | LIBUSB_ENDPOINT_OUT
#define STLINK_GET_CURRENT_MODE 0xF5
#define STLINK_DEV_DFU_MODE 0x00
#define STLINK_DEV_MASS_MODE 0x01
#define STLINK_DEV_DEBUG_MODE 0x02
#define STLINK_SWD_ENTER 0x30
#define STLINK_SWD_READCOREID 0x32
#define STLINK_DEBUG_COMMAND 0xF2
#define STLINK_DFU_COMMAND 0xF3
#define STLINK_DFU_EXIT 0x07
#define STLINK_SWDCLK 1
#define STLINK_DEBUG_APIV2_SWD_SET_FREQ 0x43
#define STLINK_DEBUG_APIV2_ENTER 0x30
#define STLINK_DEBUG_ENTER_SWD 0xA3
#define STLINK_JTAG_DRIVE_NRST 0x3C
#define STLINK_DEBUG_RESETSYS 0x03
#define STLINK_DEBUG_READCOREID 0x22
#define STLINK_DEBUG_FORCEDEBUG 0x02
#define STLINK_DEBUG_GETSTATUS 0x01
#define STLINK_DEBUG_WRITEMEM_32BIT 0x08
#define STLINK_DEBUG_RUNCORE 0x09
#define STLINK_DEBUG_WRITEDEBUGREG 0x0F
#define STLINK_REG_DHCSR 0xE000EDF0
#define STLINK_REG_DHCSR_DBGKEY 0xA05F0000
#define STLINK_JTAG_WRITEDEBUG_32BIT 0x35
#define STLINK_JTAG_READDEBUG_32BIT 0x36
#define STLINK_DEBUG_WRITEREG 0x06

#define FLASH_CR_KEY1 0x45670123
#define FLASH_CR_KEY2 0xCDEF89AB

#define FLASH_ADDRESS 0x52002000
#define FLASH_KEYR1_OFFSET 0x04
#define FLASH_CR1_OFFSET 0x0C
#define FLASH_SR1_OFFSET 0x10

#define FLASH_KEYR1 (FLASH_ADDRESS + FLASH_KEYR1_OFFSET)
#define FLASH_CR1 (FLASH_ADDRESS + FLASH_CR1_OFFSET)
#define FLASH_SR1 (FLASH_ADDRESS + FLASH_SR1_OFFSET)

int stlink_communicate(libusb_device_handle *dev_handle,
		       unsigned char *tx_buf,
		       unsigned int tx_len,
		       unsigned char *rx_buf,
		       unsigned int rx_len);
int stlink_get_current_mode(libusb_device_handle *dev_handle);
int stlink_exit_dfu_mode(libusb_device_handle *dev_handle);
int stlink_enter_swd_mode(libusb_device_handle *dev_handle);
int stlink_force_debug(libusb_device_handle *dev_handle);
int stlink_status(libusb_device_handle *dev_handle);

int stlink_write_mem(libusb_device_handle *dev_handle,
		     unsigned int addr,
		     unsigned char *buf,
		     unsigned short buf_len){//buf_len in BYTES!!

  unsigned char cmd[8];

  cmd[0] = STLINK_DEBUG_COMMAND;
  cmd[1] = STLINK_DEBUG_WRITEMEM_32BIT;
  cmd[2] = ((unsigned char *) &addr)[0];
  cmd[3] = ((unsigned char *) &addr)[1];
  cmd[4] = ((unsigned char *) &addr)[2];
  cmd[5] = ((unsigned char *) &addr)[3];
  cmd[6] = ((unsigned char *) &buf_len)[0];
  cmd[7] = ((unsigned char *) &buf_len)[1];

  if(stlink_communicate(dev_handle, cmd, 8, NULL, 0))
    return -1;

  if(stlink_communicate(dev_handle, buf, buf_len, NULL, 0))
    return -1;

  return 0;
}

int stlink_write_reg(libusb_device_handle *dev_handle,
		     unsigned int val,
		     unsigned char reg_number){

  unsigned char cmd[7];
  unsigned char answer[2];

  cmd[0] = STLINK_DEBUG_COMMAND;
  cmd[1] = STLINK_DEBUG_WRITEREG;
  cmd[2] = reg_number;
  cmd[3] = ((unsigned char *) &val)[0];
  cmd[4] = ((unsigned char *) &val)[1];
  cmd[5] = ((unsigned char *) &val)[2];
  cmd[6] = ((unsigned char *) &val)[3];

  if(stlink_communicate(dev_handle, cmd, 7, answer, 2))
    return -1;

  return 0;
}

int stlink_run(libusb_device_handle *dev_handle){

  unsigned char cmd[2];
  unsigned char answer[2];

  cmd[0] = STLINK_DEBUG_COMMAND;
  cmd[1] = STLINK_DEBUG_RUNCORE;

  if(stlink_communicate(dev_handle, cmd, 2, answer, 2))
    return -1;

  return 0;
}

int stlink_status(libusb_device_handle *dev_handle){

  unsigned char cmd[2];
  unsigned char answer[2];

  cmd[0] = STLINK_DEBUG_COMMAND;
  cmd[1] = STLINK_DEBUG_GETSTATUS;

  if(stlink_communicate(dev_handle, cmd, 2, answer, 2))
    return -1;

  return 0;
}

int stlink_force_debug(libusb_device_handle *dev_handle){

  unsigned char cmd[2];
  unsigned char answer[2];

  cmd[0] = STLINK_DEBUG_COMMAND;
  cmd[1] = STLINK_DEBUG_FORCEDEBUG;

  if(stlink_communicate(dev_handle, cmd, 2, answer, 2))
    return -1;

  return 0;
}

int stlink_enter_swd_mode(libusb_device_handle *dev_handle){

  unsigned char cmd[3];
  unsigned char answer[2];

  cmd[0] = STLINK_DEBUG_COMMAND;
  cmd[1] = STLINK_DEBUG_APIV2_ENTER;
  cmd[2] = STLINK_DEBUG_ENTER_SWD;

  if(stlink_communicate(dev_handle, cmd, 3, answer, 2))
    return -1;

  return 0;
}

int stlink_exit_dfu_mode(libusb_device_handle *dev_handle){

  unsigned char cmd[2];

  cmd[0] = STLINK_DFU_COMMAND;
  cmd[1] = STLINK_DFU_EXIT;

  if(stlink_communicate(dev_handle, cmd, 2, NULL, 0))
    return -1;

  return 0;
}

int stlink_get_current_mode(libusb_device_handle *dev_handle){

  unsigned char cmd = STLINK_GET_CURRENT_MODE;
  unsigned char answer[2];

  if(stlink_communicate(dev_handle, &cmd, 1, answer, 2))
    return -1;

  return answer[0];
}

int stlink_communicate(libusb_device_handle *dev_handle,
		       unsigned char *tx_buf,
		       unsigned int tx_len,
		       unsigned char *rx_buf,
		       unsigned int rx_len){

  int transferred;

  if(libusb_bulk_transfer(dev_handle,
			  ENDPOINT_REQ,
			  tx_buf,
			  tx_len,
			  &transferred,
			  3000)){
    printf("Error sending command (usb)\n");
    return -1;
  }

  if(transferred != tx_len){
    printf("Error didn't transfer full command (usb)\n");
    return -1;
  }

  if(rx_len != 0){
    if(libusb_bulk_transfer(dev_handle,
			    ENDPOINT_REP,
			    rx_buf,
			    rx_len,
			    &transferred,
			    3000)){
      printf("Error retrieving answer (usb)\n");
      return -1;
    }

    if(transferred != rx_len){
      printf("Error didn't retrive full answer (usb)\n");
      return -1;
    }
  }

  return 0;
}

#define FLASH_LOADER_START 0x20000000
#define WRITE_MAX_LEN 0x0000FFFF

int h743_flash(libusb_device_handle *dev_handle,
	       unsigned char *program_path){

  struct stat st;
  int fd;
  unsigned char *buf;
  ssize_t rd;
  unsigned int flash_loader_size, program_size;
  int i, remaining;

  fd = open("flash_loader/prog.bin", O_RDONLY);
  if(fd == -1){
    printf("Error opening flash_loader file\n");
    return -1;
  }

  if(fstat(fd, &st)){
    printf("Error fstat\n");
    return -1;
  }

  flash_loader_size = st.st_size;
  printf("Flash_loader is %d bytes in size\n", flash_loader_size);

  buf = (unsigned char *) malloc(flash_loader_size);
  if(!buf){
    printf("Error malloc\n");
    return -1;
  }

  rd = read(fd, buf, flash_loader_size);
  if(rd != flash_loader_size){
    printf("Error : read only %d bytes\n", rd);
    return -1;
  }
  
  if(stlink_get_current_mode(dev_handle) == STLINK_DEV_DFU_MODE){
    printf("Exiting dfu mode\n");
    if(stlink_exit_dfu_mode(dev_handle)){
      printf("Error exiting dfu mode\n");
      return -1;
    }
  }

  if(stlink_get_current_mode(dev_handle) != STLINK_DEV_DEBUG_MODE){
    printf("Entering SWD mode\n");
    if(stlink_enter_swd_mode(dev_handle)){
      printf("Error entering SWD mode\n");
      return -1;
    }
  }
  
  if(stlink_force_debug(dev_handle)){
    printf("Error forcing debug (halting core)\n");
    return -1;
  }

  if(stlink_status(dev_handle)){
    printf("Error getting status\n");
    return -1;
  }

  if(stlink_write_mem(dev_handle, FLASH_LOADER_START, buf, flash_loader_size)){
    printf("Error writing flash_loader to address 0x%08x\n",
	   FLASH_LOADER_START);
    return -1;
  }

  free(buf);
  close(fd);

  fd = open(program_path, O_RDONLY);
  if(fd == -1){
    printf("Error opening program : %s\n", program_path);
    return -1;
  }

  if(fstat(fd, &st)){
    printf("Error fstat\n");
    return -1;
  }

  program_size = st.st_size;
  
  if(program_size & 31)
    program_size += (32 - (program_size & 31));
  
  if(program_size > 0x0000FFFF){
    printf("Error program too big\n");
  }
  printf("Program is %d bytes in size, adjusting to flash word boundary (%d)\n",
	 st.st_size,
	 program_size);

  buf = (unsigned char *) malloc(program_size);
  if(!buf){
    printf("Error malloc\n");
    return -1;
  }

  rd = read(fd, buf, st.st_size);
  if(rd != st.st_size){
    printf("Error : read only %d bytes\n", rd);
    return -1;
  }

  remaining = program_size;
  for(i = 0; i < program_size; i += 200){
    if(stlink_write_mem(dev_handle,
			FLASH_LOADER_START + flash_loader_size + i,
			buf + i,
			((remaining < 200) ? remaining : 200))){
      printf("Error writing program to address 0x%08x\n",
	     FLASH_LOADER_START + flash_loader_size + i);
      return -1;
    }
    
    printf("Wrote %d bytes at address 0x%08x\n",
	   ((remaining < 200) ? remaining : 200),
	   FLASH_LOADER_START + flash_loader_size + i);
    
    remaining -= 200;
  }

  free(buf);
  close(fd);

  if(stlink_write_reg(dev_handle, FLASH_LOADER_START, 15)){//PC
    printf("Error setting PC\n");
    return -1;
  }

  if(stlink_write_reg(dev_handle, FLASH_LOADER_START + flash_loader_size, 3)){
    printf("Error setting R3 (source)\n");
    return -1;
  }

  if(stlink_write_reg(dev_handle, program_size >> 2, 4)){
    printf("Error setting R4 (length)\n");
    return -1;
  }

  if(stlink_run(dev_handle)){
    printf("Error restarting the processor\n");
    return -1;
  }

  return 0;
}

int main(int argc, char **argv){
  
  libusb_context *ctx;
  libusb_device_handle *dev_handle;
  int config;

  if(argc != 2){
    printf("usage %s <program_to_flash>\n", *argv);
    exit(0);
  }

  if(libusb_init(&ctx)){
    printf("Error initialising libusb\n");
    exit(0);
  }

  dev_handle = libusb_open_device_with_vid_pid(ctx, 1155, 14152);
  if(!dev_handle){
    printf("Error opening/finding stlink usb device\n");
    goto on_exit;
  }

  if(libusb_kernel_driver_active(dev_handle, 0) == 1){
    printf("INFO : Kernel driver active on interface 0, detaching\n");
    if(libusb_detach_kernel_driver(dev_handle, 0)){
      printf("Error detaching kernel driver on interface 0\n");
      goto on_exit_close;
    }
  }

  if(libusb_get_configuration(dev_handle, &config)){
    printf("Error getting usb configuration of stlink_device\n");
    goto on_exit_close;
  }

  if(config !=1){
    printf("INFO : Changing stlink usb configuration from %d to 1\n", config);
    if(libusb_set_configuration(dev_handle, 1)){
      printf("Error setting new usb configuration for stlink device\n");
      goto on_exit_close;
    }
  }

  if(libusb_claim_interface(dev_handle, 0)){
    printf("Error claiming stlink's usb interface 0\n");
    goto on_exit_close;
  }

  if(h743_flash(dev_handle, argv[1]))
    goto on_exit_close;

 on_exit_close:
  libusb_close(dev_handle);

 on_exit:
  libusb_exit(ctx);
  printf("Exiting\n");
  exit(0);
  return 0;
}
