

MEMORY
{
	FLASH       : ORIGIN = 0x08000000, LENGTH = 2M

	ITCM        : ORIGIN = 0x00000000, LENGTH = 64K
	DTCM        : ORIGIN = 0x20000000, LENGTH = 128K
	AXI_SRAM    : ORIGIN = 0x24000000, LENGTH = 512K
	SRAM1       : ORIGIN = 0x30000000, LENGTH = 128K
	SRAM2       : ORIGIN = 0x30020000, LENGTH = 128K
	SRAM3       : ORIGIN = 0x30040000, LENGTH = 32K
	SRAM4       : ORIGIN = 0x38000000, LENGTH = 64K
	BACKUP_SRAM : ORIGIN = 0x38800000, LENGTH = 4K
}

/* Necessary so that LD doesn't discard the IVT */
EXTERN(h743_ivt h743_default_reset_isr h743_default_isr);

__FLASH_START       = ORIGIN(FLASH);
__FLASH_END         = ORIGIN(FLASH) + LENGTH(FLASH);

__ITCM_START        = ORIGIN(ITCM);
__ITCM_END          = ORIGIN(ITCM) + LENGTH(ITCM);

__DTCM_START        = ORIGIN(DTCM);
__DTCM_END          = ORIGIN(DTCM) + LENGTH(DTCM);

__AXI_SRAM_START    = ORIGIN(AXI_SRAM);
__AXI_SRAM_END      = ORIGIN(AXI_SRAM) + LENGTH(AXI_SRAM);

__SRAM1_START       = ORIGIN(SRAM1);
__SRAM1_END         = ORIGIN(SRAM1) + LENGTH(SRAM1);

__SRAM2_START       = ORIGIN(SRAM2);
__SRAM2_END         = ORIGIN(SRAM2) + LENGTH(SRAM2);

__SRAM3_START       = ORIGIN(SRAM3);
__SRAM3_END         = ORIGIN(SRAM3) + LENGTH(SRAM3);

__SRAM4_START       = ORIGIN(SRAM4);
__SRAM4_END         = ORIGIN(SRAM4) + LENGTH(SRAM4);

__BACKUP_SRAM_START = ORIGIN(BACKUP_SRAM);
__BACKUP_SRAM_END   = ORIGIN(BACKUP_SRAM) + LENGTH(BACKUP_SRAM);

/* IVT symbols */
/* ARM Exceptions */
__main_stack_pointer = ORIGIN(DTCM) + LENGTH(DTCM);
PROVIDE(h743_reset_isr = h743_default_reset_isr + 1);
PROVIDE(h743_nmi_isr = h743_default_isr + 1);
PROVIDE(h743_hardfault_isr = h743_default_isr + 1);

PROVIDE(h743_memmanage_isr = h743_default_isr + 1);
PROVIDE(h743_busfault_isr = h743_default_isr + 1);
PROVIDE(h743_usagefault_isr = h743_default_isr + 1);
/* Reserved */

/* Reserved */
/* Reserved */
/* Reserved */
PROVIDE(h743_svcall_isr = h743_default_isr + 1);

PROVIDE(h743_debugmonitor_isr = h743_default_isr + 1);
/* Reserved */
PROVIDE(h743_pendsv_isr = h743_default_isr + 1);
PROVIDE(h743_systick_isr = h743_default_isr + 1);

/* External Interrupts */
/* TIMER */
PROVIDE(h743_tim6_isr = h743_default_isr + 1);
PROVIDE(h743_tim7_isr = h743_default_isr + 1);
/* DMA */
PROVIDE(h743_dma1_str0_isr = h743_default_isr + 1);
PROVIDE(h743_dma1_str1_isr = h743_default_isr + 1);
PROVIDE(h743_dma1_str2_isr = h743_default_isr + 1);
PROVIDE(h743_dma1_str3_isr = h743_default_isr + 1);
PROVIDE(h743_dma1_str4_isr = h743_default_isr + 1);
PROVIDE(h743_dma1_str5_isr = h743_default_isr + 1);
PROVIDE(h743_dma1_str6_isr = h743_default_isr + 1);
PROVIDE(h743_dma1_str7_isr = h743_default_isr + 1);

PROVIDE(h743_dma2_str0_isr = h743_default_isr + 1);
PROVIDE(h743_dma2_str1_isr = h743_default_isr + 1);
PROVIDE(h743_dma2_str2_isr = h743_default_isr + 1);
PROVIDE(h743_dma2_str3_isr = h743_default_isr + 1);
PROVIDE(h743_dma2_str4_isr = h743_default_isr + 1);
PROVIDE(h743_dma2_str5_isr = h743_default_isr + 1);
PROVIDE(h743_dma2_str6_isr = h743_default_isr + 1);
PROVIDE(h743_dma2_str7_isr = h743_default_isr + 1);

/* SPI */
PROVIDE(h743_spi1_isr = h743_default_isr + 1);
PROVIDE(h743_spi2_isr = h743_default_isr + 1);

/* USART */
PROVIDE(h743_usart1_isr = h743_default_isr + 1);
PROVIDE(h743_usart2_isr = h743_default_isr + 1);
PROVIDE(h743_usart3_isr = h743_default_isr + 1);


SECTIONS
{
	.text :
	{
		KEEP( *(.h743_ivt) )
		KEEP( *(.text) )
		KEEP( *(.data) )
		KEEP( *(.rodata) )
	} > FLASH
}

