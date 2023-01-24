# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set VIDEOINFO, 1<<2						# video info
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a multiboot header that marks the program as a kernel.
.section .multiboot.data, "aw"
	.align 4
	.long MAGIC
	.long FLAGS
	.long CHECKSUM


# Allocate the initial stack.
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# Preallocate pages used for paging. Don't hard-code addresses and assume they
# are available, as the bootloader might have loaded its multiboot structures or
# modules there. This lets the bootloader know it must avoid the addresses.
.section .bss, "aw", @nobits
	.align 4096
# 1024 int array
boot_page_directory:
	.skip 4096
# 1024 int array
boot_page_table1:
	.skip 4096
multiboot_data:
	.skip 8

# The kernel entry point.
.section .multiboot.text, "a"
.global _start
.type _start, @function
_start:
	movl $(multiboot_data - 0xC0000000), %edi
	movl %eax, (%edi)
	addl $4, %edi
	movl %ebx, (%edi)
	mov $(boot_page_table1 - 0xC0000000), %edi
	# First address to map is address 0.
	mov $0, %esi
	# Map 1023 pages. The 1024th will be the VGA text buffer.
	mov $1023, %ecx

1:
	# Only map the kernel.
	cmpl $_kernel_start, %esi
	jl 2f
	cmpl $(_kernel_end - 0xC0000000), %esi
	jge 3f

	# Map physical address as "present, writable". Note that this maps
	# .text and .rodata as writable. Mind security and map them as non-writable.
	mov %esi, %edx
	or $0x003, %edx
	mov %edx, (%edi)

2:
	# map the page table
	# Size of page is 4096 bytes.
	addl $4096, %esi
	# Size of entries in boot_page_table1 is 4 bytes.
	addl $4, %edi
	# Loop to the next entry if we haven't finished.
	loop 1b

3:
	# Map VGA video memory to 0xC03FF000 as "present, writable".
	movl $(0x000B8000 | 0x003), boot_page_table1 - 0xC0000000 + 1023 * 4

	# The page table is used at both page directory entry 0 (virtually from 0x0
	# to 0x003FFFFF) (thus identity mapping the kernel) and page directory entry
	# 768 (virtually from 0xC0000000 to 0xC03FFFFF) (thus mapping it in the
	# higher half). The kernel is identity mapped because enabling paging does
	# not change the next instruction, which continues to be physical. The CPU
	# would instead page fault if there was no identity mapping.

	# Map the page table to both virtual addresses 0x00000000 and 0xC0000000.
	movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 0
	movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 768 * 4

	# map the boot directory to the last page in the boot directory
	movl $(boot_page_directory - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 1023 * 4

	# Set cr3 to the address of the boot_page_directory.
	mov $(boot_page_directory - 0xC0000000), %ecx
	mov %ecx, %cr3

	# Enable paging and the write-protect bit.
	mov %cr0, %ecx
	or $0x80010000, %ecx
	mov %ecx, %cr0

	# Jump to higher half with an absolute jump. 
	lea 4f, %ecx
	jmp *%ecx

.section .text
4:
	# At this point, paging is fully set up and enabled.
	# Unmap the identity mapping as it is now unnecessary. 
	movl $0, boot_page_directory + 0

	# Reload crc3 to force a TLB flush so the changes to take effect.
	mov %cr3, %ecx
	mov %ecx, %cr3

	# Set up the stack.
	mov $stack_top, %esp

	mov [multiboot_data], %eax
	mov [multiboot_data+4], %ebx
	push %ebx
	push %eax
	

	# Enter the high-level kernel.
	call _kernel_init
	call _init
	call kernel_main
	call _fini

	cli

1:	hlt
	jmp 1b

crash:
	jmp crash

