.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set VIDEOINFO, 1<<2						# video info
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

.section .multiboot.data, "aw"
	.align 4
	.long MAGIC
	.long FLAGS
	.long CHECKSUM


.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384
stack_top:

.section .bss, "aw", @nobits
	.align 4096
boot_PML5:
	.skip 4096
boot_PML4:
	.skip 4096
boot_page_directory:
	.skip 4096
boot_page_table1:
	.skip 4096
multiboot_data:
	.skip 8

.section .multiboot.text, "a"
.global _start
.type _start, @function
_start:
	movl $(multiboot_data - 0xFFFFFF8000000000), %edi
	movl %eax, (%edi)
	addl $4, %edi
	movl %ebx, (%edi)
	movl $(boot_page_table1 - 0xFFFFFF8000000000), %edi
	movl $0, %esi
	movl $1023, %ecx

1:
	cmpl $_kernel_start, %esi
	jl 2f
	cmpl $(_kernel_end - 0xFFFFFF8000000000), %esi
	jge 3f

	movl %esi, %edx
	orl $0x003, %edx
	movl %edx, (%edi)

2:
	addl $4096, %esi
	addl $4, %edi
	loop 1b

3:
	# movl $(0x000B8000 | 0x003), boot_page_table1 - 0xFFFFFF8000000000 + 1023 * 4

	# movl $(boot_page_table1 - 0xFFFFFF8000000000 + 0x003), boot_page_directory - 0xFFFFFF8000000000 + 0
	# movl $(boot_page_table1 - 0xFFFFFF8000000000 + 0x003), boot_page_directory - 0xFFFFFF8000000000 + 768 * 4

	# movl $(boot_page_directory - 0xFFFFFF8000000000 + 0x003), boot_page_directory - 0xFFFFFF8000000000 + 1023 * 4






# make sure paging is disabled
  mov %cr0, %rcx
  and $(~(1 << 31)), %ecx
  mov %rcx, %cr0

# set PAE bit
  mov %cr4, %rcx
  orl  $(1 << 5), %ecx
  mov %rcx, %cr4
# Set LME (long mode enable)
  mov $(0xC0000080), %rcx
  rdmsr
  or  (1 << 8), %rax
  wrmsr

  mov $(pml4_table - 0xFFFFFF8000000000 + 0x3), %rcx
  mov %rcx, cr3

	lea 4f, %rcx
	jmp *%rcx


.section .text
4:
	movl $0, boot_page_directory + 0

	mov cr3, %rcx
	mov %rcx, cr3

	mov $stack_top, %esp

	mov [multiboot_data], %eax
	mov [multiboot_data+4], %ebx
	push %rbx
	push %rax

	call _kernel_init
	call _init
	call kernel_main
	call _fini

	cli

1:	hlt
	jmp 1b

crash:
	jmp crash

