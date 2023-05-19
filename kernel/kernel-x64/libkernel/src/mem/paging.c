#include <mem/paging.h>
#include <mem/kmalloc_def.h>
#include <mem/physical_mem_manager.h>
#include <kmacro.h>
#include <stdio.h>
#include <stdlib.h>

static void reload_TLB()
{
  __asm__ volatile("movl %cr3, %rcx \n\t"
                   "movl %rcx, %cr3");
}

void *get_physaddr(void *virtualaddr)
{
}

// TODO(ANT) complete
void *map_page(void *physaddr, void *virtualaddr, uint32_t flags)
{
}

void unmap_page(void *virtualaddr)
{
}

