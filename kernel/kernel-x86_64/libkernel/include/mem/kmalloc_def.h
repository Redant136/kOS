#pragma once
#include <stdint.h>
#include <stddef.h>

#ifndef KMALLOC_DEF_H
#define KMALLOC_DEF_H

// 4MB
#define PAGE_SIZE 4096

enum PAGE_DIR_FLAGS
{
  PDE_PRESENT = 1 << 0,
  PDE_READ_WRITE = 1 << 1,
  PDE_USER = 1 << 2,
  PDE_WRITE_THROUGH = 1 << 3,
  PDE_CACHE_DISABLE = 1 << 4,
  PDE_ACCESSED = 1 << 5,
  PDE_DIRTY = 1 << 6,     // 4MB entry only
  PDE_PAGE_SIZE = 1 << 7, // 0 = 4KB page, 1 = 4MB page
  PDE_GLOBAL = 1 << 8,    // 4MB entry only
  PDE_PAT = 1 << 12,      // 4MB entry only
  PDE_FRAME = 0xFFFFF000, // bits 12+
};

enum PAGE_TABLE_FLAGS
{
  PTE_PRESENT = 1 << 0,
  PTE_READ_WRITE = 1 << 1,
  PTE_USER = 1 << 2,
  PTE_WRITE_THROUGH = 1 << 3,
  PTE_CACHE_DISABLE = 1 << 4,
  PTE_ACCESSED = 1 << 5,
  PTE_DIRTY = 1 << 6,
  PTE_PAT = 1 << 7,
  PTE_GLOBAL = 1 << 8,
  PTE_FRAME = 0xFFFFF000, // bits 12+
};

#define KMALLOC_KERNEL 1
#define KMALLOC_PHYS_PAGETABLE 2
#define KMALLOC_ADDR_CHUNK_RESERVED 2
#define KMALLOC_ADDR_DRIVER 4

#endif