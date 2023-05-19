movabs $(boot_page_table1 - kernel_vadr + 0x003), %eax
movabs %eax, boot_page_directory - kernel_vadr + 0
movabs $(boot_page_directory - kernel_vadr + 0x003), %eax
movabs %eax, boot_PML4 - kernel_vadr + 0

# set the mapping so that kernel_vadr is the base address of the kernel
movabs $(boot_page_directory - kernel_vadr + 0x003), %eax
movabs %eax, boot_PML4 - kernel_vadr + 384 * 8
movabs $(boot_PML4 - kernel_vadr + 0x003), %eax
movabs %eax, boot_PML5 - kernel_vadr
movabs $(boot_PML4 - kernel_vadr + 0x003), %eax
movabs %eax, boot_PML4 - kernel_vadr + nPages * 8
# when pml5 is active, then instead
# mov $(boot_PML5 - kernel_vadr + 0x003), boot_PML5 - kernel_vadr + nPages * 8

movabs $(boot_PML4 - kernel_vadr + 0x003), %rcx
mov %rcx, %cr3

# make sure paging is disabled
mov %cr0, %rcx
and $(~(1 << 31)), %ecx
mov %rcx, %cr0

# set PAE bit
mov %cr4, %rcx
or  $(1 << 5), %ecx
mov %rcx, %cr4

# Set LME (long mode enable)
mov $0xC0000080, %ecx
rdmsr
or  $0x100, %eax
wrmsr

mov %cr0, %rcx
orl $0x80010001, %ecx
mov %rcx, %cr0
