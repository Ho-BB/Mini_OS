

MEMORY
{
	RAM : ORIGIN = 0x20000000, LENGTH = 1M
	FLASH : ORIGIN = 0x08000000, LENGTH = 2M
}

SECTIONS
{
	.text :
	{
		*(.text)
		*(.data)
	} > RAM
}
