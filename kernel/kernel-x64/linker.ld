OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:x86-64)

ENTRY (_start)

SECTIONS
{
  . = 0x100000;
	_kernel_start = .;
	KERNEL_OFFSET = 0xC00000000000;

	.multiboot.data BLOCK(4K) : ALIGN(4K) {
		*(.multiboot.data)
	}
	.multiboot.text BLOCK(4K) : ALIGN(4K) {
  	*(.multiboot.text)
  }
	.bootstrap.data BLOCK (4K) : ALIGN(4K) {
		*(.bootstrap.data)
	}
	.bootstrap.bss BLOCK (4K) : ALIGN(4K) {
		*(.bootstrap.bss)
	}


	. += KERNEL_OFFSET;
	.text ALIGN (4K) : AT (ADDR(.text) - KERNEL_OFFSET)
	{
		*(.text)
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
	_kernel_end = . - KERNEL_OFFSET;
}
