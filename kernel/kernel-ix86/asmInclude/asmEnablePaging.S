# Map the page table to both virtual addresses 0x00000000 and 0xC0000000.
movl $(boot_page_table1 - kernel_vadr + 0x003), boot_page_directory - kernel_vadr + 0
movl $(boot_page_table1 - kernel_vadr + 0x003), boot_page_directory - kernel_vadr + 768 * 4
# map the boot directory to the last page in the boot directory
movl $(boot_page_directory - kernel_vadr + 0x003), boot_page_directory - kernel_vadr + 1023 * 4
# Set cr3 to the address of the boot_page_directory.
mov $(boot_page_directory - kernel_vadr), %ecx
mov %ecx, %cr3
# Enable paging and the write-protect bit.
mov %cr0, %ecx
or $0x80010000, %ecx
mov %ecx, %cr0