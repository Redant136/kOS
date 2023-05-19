#define ASM_FILE
#include <multiboot.h>
#include <sys/cdefs.h>
#if defined(__i386__)
#define nPages 1023
#else
#error wrong assembly file
#endif

#if defined(MULTIBOOT2_HEADER_MAGIC)
.set CHECKSUM, -(MULTIBOOT2_HEADER_MAGIC + 0 + multiboot_header_end - multiboot_header_start)
.section .multiboot.data, "aw"
multiboot_header_start:
	.align MULTIBOOT_HEADER_ALIGN
	.long MULTIBOOT2_HEADER_MAGIC
	.long 0
	.long multiboot_header_end - multiboot_header_start
	.long CHECKSUM
multiboot_header_end:
#elif defined(MULTIBOOT_HEADER_MAGIC)
.set FLAGS,    MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO  # this is the Multiboot 'flag' field
.set CHECKSUM, -(MULTIBOOT_HEADER_MAGIC + FLAGS) # checksum of above, to prove we are multiboot
.section .multiboot.data, "aw"
	.align MULTIBOOT_HEADER_ALIGN
	.long MULTIBOOT_HEADER_MAGIC
	.long FLAGS
	.long CHECKSUM
#endif


.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384
stack_top:

# .section .bootstrap.data
.section .bss, "aw", @nobits
	.align 4096
.section .bootstrap.bss, "aw", @nobits
	.align 4096
boot_page_directory:
	.skip 4096
boot_page_table1:
	.skip 4096




.section .multiboot.text, "a"
gdt:
  .quad 0
gdt_code_seg:
  .quad (1<<44) | (1<<47) | (1<<41) | (1<<43) | (1<<53)
gdt_data_seg: 
  .quad (1<<44) | (1<<47) | (1<<41)
gdt_ptr:
  .word . - gdt - 1
#ifdef __x64__
	.quad gdt
#else
	.long gdt
#endif
CODE_SEL = gdt_code_seg-gdt
DATA_SEL = gdt_data_seg-gdt

.global _start
.type _start, @function
_start:
	movl $(boot_page_table1), %edi
	movl $0, %esi
	movl $nPages, %ecx
1:
	cmpl $_kernel_start, %esi
	jl 2f
	cmpl $(_kernel_end), %esi
	jge 3f

	movl %esi, %edx
	orl $0x3, %edx
	movl %edx, (%edi)
2:
	addl $4096, %esi
	addl $4, %edi
	loop 1b
3:
	movl $(0x000B8000 | 0x003), boot_page_table1 + nPages * 4

	movl $(boot_page_table1 + 0x003), boot_page_directory + 0
	movl $(boot_page_table1 + 0x003), boot_page_directory + 768 * 4
  movl $(boot_page_directory + 0x003), boot_page_directory + nPages * 4

	movl $(boot_page_directory + 0x003), %ecx
	movl %ecx, %cr3

	# this crashes in 64 bit
	movl %cr0, %ecx
	orl $0x80010000, %ecx
	movl %ecx, %cr0

	# TODO(ANT) FIX GDT TABLE THINGY HERE
	# jmp .
	# lgdt (gdt_ptr)
	# mov $DATA_SEL, %eax
  # mov %eax, %ds
  # mov %eax, %es
  # mov %eax, %fs
  # mov %eax, %gs
  # mov %eax, %ss
	# ljmp $CODE_SEL, $start64
	lea 4f, %ecx
	jmp *%ecx



.section .text
4:
	# add $8, %esp
	# At this point, paging is fully set up and enabled.
	# Unmap the identity mapping as it is now unnecessary. 
	movl $0, boot_page_directory + 0

	# Reload crc3 to force a TLB flush so the changes to take effect.
	movl %cr3, %ecx
	movl %ecx, %cr3

	# Set up the stack.
	mov $stack_top, %esp


	# give multiboot object
	push %ebx


	# Enter the high-level kernel.
	call _kernel_init
	# call _init
	call kernel_main
	# call _fini

	cli


hang: hlt
	jmp hang



