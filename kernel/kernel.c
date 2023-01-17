/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

#include <VGA_TTY.h>
#include <stdio.h>
#include <mem_manager.h>
#include <kmacro.h>

void _kernel_init(uint32_t magic,uint32_t mbd)
{
  /* Initialize terminal interface */
  terminal_initialize();
  setFILE(stdout, FILE_WRITE, 0, 0, 0, write_buf_start, write_buf_end, write_buf_start, terminal_writeBuffers);
  setFILE(stdin, FILE_READ, read_buf_start, read_buf_end, read_buf_start, 0, 0, 0, NULL);
  setFILE(stderr, FILE_WRITE | FILE_READ, read_buf_start, read_buf_end, read_buf_start, write_buf_start, write_buf_end, write_buf_start, terminal_writeErrBuffers);

  read_physical_memory(magic, map_page((void *)mbd, _pre_kernel_next_virtaddr(), 3));

}

void kernel_main(void)
{
  int *test = kmalloc(sizeof(int) * 4, 0);
  int*t2=kmalloc(4,0);
  kfree(test);
  kfree(t2);
  printf("test %03d test\n", 1);
}
