

	.equ FLASH_BASE, 0x52002000
	.equ FLASH_KEYR1, FLASH_BASE + 0x04
	.equ FLASH_CR1, FLASH_BASE + 0x0C
	.equ FLASH_SR1, FLASH_BASE + 0x010
	
	.equ FLASH_CR_KEY1, 0x45670123
	.equ FLASH_CR_KEY2, 0xCDEF89AB

	.equ FLASH_ERASE_SECTOR_0, 0x000000B4
	.equ FLASH_PROGRAM, 0x00000022
	.equ FLASH_CR_LOCK, 0x00000001
	.equ FLASH_DESTINATION, 0x08000000

	.code 16
	.text

entry:
	cpsid f                         @Disable all irqs
	
	ldr r0, =FLASH_KEYR1            @Unlock flash
	ldr r1, =FLASH_CR_KEY1
	str r1, [r0]

	ldr r1, =FLASH_CR_KEY2
	str r1, [r0]

	ldr r0, =FLASH_CR1              @Erase sector 0
	ldr r1, =FLASH_ERASE_SECTOR_0
	str r1, [r0]

	ldr r0, =FLASH_SR1              @Poll SR1 until erase done
	mov r1, #4
poll:
	ldr r2, [r0]
	and r2, r2, r1
	cmp r2, r1
	beq poll

	ldr r0, =FLASH_CR1              @Enable flash writes
	ldr r1, =FLASH_PROGRAM
	str r1, [r0]

	ldr r0, =FLASH_DESTINATION      @Write flash
	mov r1, #0                      @r3 : Source pointer
write:                                  @r4 : Length of source in words
	cmp r4, r1
	beq lock_cr

	ldr r2, [r3]
	str r2, [r0]
	add r3, r3, #4
	add r0, r0, #4
	sub r4, r4, #1
	b write

lock_cr:
	ldr r0, =FLASH_SR1              @Poll SR1 until write done
	mov r1, #4
poll_2:
	ldr r2, [r0]
	and r2, r2, r1
	cmp r2, r1
	beq poll_2
	
	ldr r0, =FLASH_CR1              @set PG to 0
	mov r1, #0
	str r1, [r0]

	ldr r1, =FLASH_CR_LOCK          @Lock CR1 up
	str r1, [r0]

loop:
	b loop
