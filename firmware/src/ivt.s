

	@The linker script provides default adresses for isr symbols the library
	@user didn't define
	
	.code 16
	
	.section .h743_ivt

	.global h743_ivt
	.global h743_default_reset_isr
	.global h743_default_isr

	@ARM Exceptions

h743_ivt:	
	.word __main_stack_pointer      @Main stack pointer loaded at reset
	.word h743_reset_isr            @Reset
	.word h743_nmi_isr              @Non Maskable Interrupt
	.word h743_hardfault_isr        @Hard Fault
	
	.word h743_memmanage_isr        @Mem Manage
	.word h743_busfault_isr         @Bus Fault
	.word h743_usagefault_isr       @Usage Fault
	.word 0                         @Reserved (see ARM)

	.word 0                         @Reserved (see ARM)
	.word 0                         @Reserved (see ARM)
	.word 0                         @Reserved (see ARM)
	.word h743_svcall_isr           @SVCall

	.word h743_debugmonitor_isr     @Debug Monitor
	.word 0                         @Reserved (see ARM)
	.word h743_pendsv_isr           @PendSV
	.word h743_systick_isr          @SysTick

	@External Interrupts (150 for h743)

	.word 0                         @0
	.word 0
	.word 0
	.word 0

	.word 0                         @4
	.word 0
	.word 0
	.word 0

	.word 0                         @8
	.word 0
	.word 0
	.word h743_dma1_str0_isr        @11

	.word h743_dma1_str1_isr        @12
	.word h743_dma1_str2_isr        @13
	.word h743_dma1_str3_isr        @14
	.word h743_dma1_str4_isr        @15

	.word h743_dma1_str5_isr        @16
	.word h743_dma1_str6_isr        @17
	.word 0
	.word 0

	.word 0                         @20
	.word 0
	.word 0
	.word 0

	.word 0                         @24
	.word 0
	.word 0
	.word 0

	.word 0                         @28
	.word 0
	.word 0
	.word 0

	.word 0                         @32
	.word 0
	.word 0
	.word h743_spi1_isr

	.word h743_spi2_isr             @36
	.word h743_usart1_isr
	.word h743_usart2_isr
	.word h743_usart3_isr

	.word 0                         @40
	.word 0
	.word 0
	.word 0

	.word 0                         @44
	.word 0
	.word 0
	.word h743_dma1_str7_isr        @47

	.word 0                         @48
	.word 0
	.word 0
	.word 0

	.word 0                         @52
	.word 0
	.word h743_tim6_isr             @54 : TIM6
	.word h743_tim7_isr             @55 : TIM7

	.word h743_dma2_str0_isr        @56
	.word h743_dma2_str1_isr        @57
	.word h743_dma2_str2_isr        @58
	.word h743_dma2_str3_isr        @59

	.word h743_dma2_str4_isr        @60
	.word 0
	.word 0
	.word 0

	.word 0                         @64
	.word 0
	.word 0
	.word 0

	.word h743_dma2_str5_isr        @68
	.word h743_dma2_str6_isr        @69
	.word h743_dma2_str7_isr        @70
	.word 0

	.word 0                         @72
	.word 0
	.word 0
	.word 0

	.word 0                         @76
	.word 0
	.word 0
	.word 0

	.word 0                         @80
	.word 0
	.word 0
	.word 0

	.word 0                         @84
	.word 0
	.word 0
	.word 0

	.word 0                         @88
	.word 0
	.word 0
	.word 0

	.word 0                         @92
	.word 0
	.word 0
	.word 0

	.word 0                         @96
	.word 0
	.word 0
	.word 0

	.word 0                         @100
	.word 0
	.word 0
	.word 0

	.word 0                         @104
	.word 0
	.word 0
	.word 0

	.word 0                         @108
	.word 0
	.word 0
	.word 0

	.word 0                         @112
	.word 0
	.word 0
	.word 0

	.word 0                         @116
	.word 0
	.word 0
	.word 0

	.word 0                         @120
	.word 0
	.word 0
	.word 0

	.word 0                         @124
	.word 0
	.word 0
	.word 0

	.word 0                         @128
	.word 0
	.word 0
	.word 0

	.word 0                         @132
	.word 0
	.word 0
	.word 0

	.word 0                         @136
	.word 0
	.word 0
	.word 0

	.word 0                         @140
	.word 0
	.word 0
	.word 0

	.word 0                         @144
	.word 0
	.word 0
	.word 0

	.word 0                         @148
	.word 0                         @149
	
	@End of External Interrupts

	.text
	
	@Default Reset ISR

h743_default_reset_isr:	
	b h743_default_reset_isr

	
	@Default ISR for all other Exceptions / Interrupts

h743_default_isr:
	bx lr
