ENTRY (_start)

SECTIONS
{
  . = 0x100000;
	.multiboot.data BLOCK(4K) : ALIGN(4K) {
		*(.multiboot.data)
	}
	.multiboot.text BLOCK(4K) : ALIGN(4K) {
  	*(.multiboot.text)
  }
	.bootstrap ALIGN (4K) : ALIGN(4K) {
		build/bootstrap_loader.o (.text)
	}

	.text ALIGN (4K) : AT (ADDR(.text))
	{
		*(EXCLUDE_FILE(*build/bootstrap_loader.o) .text)
	}
	.rodata ALIGN (4K) : AT (ADDR(.rodata))
	{
		*(.rodata)
	}
	.data ALIGN (4K) : AT (ADDR(.data))
	{
		*(.data)
	}
	.bss ALIGN (4K) : AT (ADDR(.bss))
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}
}
