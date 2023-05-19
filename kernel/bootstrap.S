.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set VIDEOINFO, 1<<2						# video info
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384
stack_top:

.global _start
.type _start, @function
_start:
	cli
	mov stack_top, %esp
  mov stack_top, %ebp
  push %ebx
	# boot loader main 
	call lmain
