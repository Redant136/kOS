#include <mem/paging.h>
#include <mem/kmalloc_def.h>
#include <mem/physical_mem_manager.h>
#include <kmacro.h>
#include <stdio.h>
#include <stdlib.h>

// page directory virt mem address: 0xFFFFF000
// page table virt mem address: 0xFFC00000

static void reload_TLB()
{
  __asm__ volatile("movl %cr3, %ecx \n\t"
                   "movl %ecx, %cr3");
}

void *get_physaddr(void *virtualaddr)
{
  uint32_t pdindex = (uint32_t)virtualaddr >> 22;
  uint32_t ptindex = (uint32_t)virtualaddr >> 12 & 0x03FF;

  uint32_t *pd = (uint32_t *)0xFFFFF000;
  if (!(pd[pdindex] & PDE_PRESENT))
  {
    CRASH("Page directory entry is not present: %p %p\n", virtualaddr, pd[pdindex]);
    
    // TODO(ANT) crash
    return 0;
  }

  uint32_t *pt = ((uint32_t *)0xFFC00000) + (0x400 * pdindex);
  if (!(pt[ptindex] & PTE_PRESENT))
  {
    CRASH("Page table entry is not present: %p %p\n", virtualaddr, pt[ptindex]);
    // TODO(ANT) crash
    return 0;
  }

  return (void *)((pt[ptindex] & ~0xFFF) + ((uint32_t)virtualaddr & 0xFFF));
}

// TODO(ANT) complete
void *map_page(void *physaddr, void *virtualaddr, uint32_t flags)
{
  if ((uint32_t)physaddr % PAGE_SIZE || (uint32_t)virtualaddr % PAGE_SIZE)
  {
    // TODO(ANT) crash
    CRASH("addresses are not multiple of 4096.\nphys_addr remainder: %d virt_addr remainder: %d\n", (uint32_t)physaddr % PAGE_SIZE, (uint32_t)virtualaddr % PAGE_SIZE);
    return 0;
  }

  uint32_t pdindex = (uint32_t)virtualaddr >> 22;
  uint32_t ptindex = (uint32_t)virtualaddr >> 12 & 0x3FF;

  uint32_t *pd = (uint32_t *)0xFFFFF000;
  if (!(pd[pdindex] & PDE_PRESENT))
  {
    pd[pdindex] = (size_t)kmalloc(pdindex, KMALLOC_KERNEL | KMALLOC_PHYS_PAGETABLE);
    pd[pdindex] |= PDE_PRESENT | PDE_READ_WRITE;
    reload_TLB();
  }

  // Here you need to check whether the PD entry is present.
  // When it is not present, you need to create a new empty PT and
  // adjust the PDE accordingly.
  uint32_t *pt = ((uint32_t *)0xFFC00000) + (0x400 * pdindex);

  // Here you need to check whether the PT entry is present.
  // When it is, then there is already a mapping present. What do you do now?

  pt[ptindex] = ((uint32_t)physaddr) | (flags & 0xFFF) | 0x01; // Present

  reload_TLB();
  return virtualaddr;
}

void unmap_page(void *virtualaddr)
{
  if ((uint32_t)virtualaddr % PAGE_SIZE)
  {
    // TODO(ANT) crash
    CRASH( "addresses are not multiple of 4096.\nvirt_addr remainder: %d\n", (uint32_t)virtualaddr % PAGE_SIZE);
    return;
  }
  uint32_t pdindex = (uint32_t)virtualaddr >> 22;
  uint32_t ptindex = (uint32_t)virtualaddr >> 12 & 0x03FF;
  uint32_t *pd = (uint32_t *)0xFFFFF000;
  if (!(pd[pdindex] & PDE_PRESENT))
  {
    CRASH("address is invalid");
    return;
  }
  uint32_t *pt = ((uint32_t *)0xFFC00000) + (0x400 * pdindex);
  if (!(pt[ptindex] & PTE_PRESENT))
  {
    CRASH("address is invalid");
    return;
  }
  pt[ptindex] = ((uint32_t)0) & ~PTE_PRESENT; // Present
  reload_TLB();
  return;
}

void *_pre_kernel_next_virtaddr()
{
  uint32_t *pd = (uint32_t *)0xFFFFF000;
  for (int pdindex = 0; pdindex < 1024; pdindex++)
  {
    if (!pd[pdindex] & PDE_PRESENT)
      continue;
    uint32_t *pt = ((uint32_t *)0xFFC00000) + (0x400 * pdindex);

    for (int ptindex = 0; ptindex < 1024; ptindex++)
    {
      if (!pt[ptindex] & PTE_PRESENT)
      {
        return (void *)((pdindex << 22) + (ptindex << 12));
      }
    }
  }

  CRASH("Ran out of memory addresses");
  return 0;
}

