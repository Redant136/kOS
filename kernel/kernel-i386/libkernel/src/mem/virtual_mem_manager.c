#include <mem/virtual_mem_manager.h>
#include <mem/kmalloc_def.h>
#include <stdlib.h>
#include <stdio.h>
#include <kmacro.h>

#define USER_START (1 << 22)                                           // 0x00400000
#define KERNEL_START (0xC0000000)                                      // 0xC0000000
#define PAGING_MEM_START (0xFFC00000)                                  // 0xFFC00000
#define MEM_END (~((size_t)0))                                         // 0xFFFFFFFF
#define KERNEL_SPACE (MEM_END - KERNEL_START + 1)                      // 0x40000000
#define HEAP_START (KERNEL_START + KERNEL_SPACE / 4)                   // 0xD0000000
#define DRIVER_START (KERNEL_START + KERNEL_SPACE / 2)                 // 0xE0000000
#define MISC_MEM_START (KERNEL_START + KERNEL_SPACE / 4 * 3)           // 0xF0000000
#define LINKED_LIST_RESERVED_START (MISC_MEM_START + KERNEL_SPACE / 8) // 0xF8000000

#define USER_RANGE USER_START, KERNEL_START
#define KERNEL_START_RANGE KERNEL_START, DRIVER_START
#define DRIVER_RANGE DRIVER_START, HEAP_START
#define HEAP_RANGE HEAP_START, MISC_MEM_START
#define MISC_MEM_RANGE MISC_MEM_START, KMALLOC_RESERVED_ADDR
#define LINKED_LIST_RESERVED_RANGE LINKED_LIST_RESERVED_START, PAGING_MEM_START

static void *_next_virtaddr(size_t _n, size_t startAddr, size_t endAddr)
{
  unsigned int nPages = _n / PAGE_SIZE;
  nPages += _n % PAGE_SIZE ? 1 : 0;
  unsigned int n = 0;
  size_t addr = 0;
  size_t *pd = (size_t *)0xFFFFF000;
  for (size_t curAddr = startAddr; curAddr < endAddr; curAddr += PAGE_SIZE)
  {
    int pdindex = curAddr >> 22;
    size_t *pt = ((size_t *)0xFFC00000) + (0x400 * pdindex);
    int ptindex = curAddr >> 12 & 1023;
    if (!(pd[pdindex] & PDE_PRESENT) || !(pt[ptindex] & PTE_PRESENT))
    {
      if (!n)
        addr = (pdindex << 22) + (ptindex << 12);
      n++;
      if (n >= nPages)
      {
        return (void *)addr;
      }
    }
    else
    {
      n = 0;
    }
  }
  CRASH("Ran out of memory addresses");
  abort();
  return 0;
}

void *kmalloc_virt_addr(size_t n, uint32_t flags)
{
  if (flags & KMALLOC_KERNEL)
  {
    return _next_virtaddr(n, HEAP_RANGE);
  }
  else if (flags & KMALLOC_ADDR_CHUNK_RESERVED)
  {
    return _next_virtaddr(PAGE_SIZE, LINKED_LIST_RESERVED_RANGE);
  }
  else
  {
    return _next_virtaddr(n, USER_RANGE);
  }
}
