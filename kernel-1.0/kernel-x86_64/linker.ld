ENTRY (_start)

SECTIONS
{
  . = 0x100000;
	KERNEL_OFFSET = 0xFFFFFF8000000000;

	_kernel_start = .;
	.multiboot.data BLOCK(4K) : ALIGN(4K) {
		*(.multiboot.data)
	}
	.multiboot.text BLOCK(4K) : ALIGN(4K) {
  	*(.multiboot.text)
  }
	.bootstrap ALIGN (4K) : ALIGN(4K) {
		bootstrap_loader.o (.text)
	}

	. += KERNEL_OFFSET;
	.text ALIGN (4K) : AT (ADDR(.text) - KERNEL_OFFSET)
	{
		*(.text) // exclude bootstrap_loader.o
	}
	.rodata ALIGN (4K) : AT (ADDR(.rodata) - KERNEL_OFFSET)
	{
		*(.rodata)
	}
	.data ALIGN (4K) : AT (ADDR(.data) - KERNEL_OFFSET)
	{
		*(.data)
	}
	.bss ALIGN (4K) : AT (ADDR(.bss) - KERNEL_OFFSET)
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}

	_kernel_end = .;
}
