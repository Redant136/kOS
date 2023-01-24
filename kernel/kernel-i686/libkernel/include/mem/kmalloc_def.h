#pragma once
#include <stdint.h>
#include <stddef.h>

#ifndef KMALLOC_DEF_H
#define KMALLOC_DEF_H

// 4MB
#define PAGE_SIZE 4096

#define PDE_PRESENT (1)
#define PDE_READ_WRITE (1 << 1)
#define PDE_USER (1 << 2)
#define PDE_WRITE_THROUGH (1 << 3)
#define PDE_CACHE_DISABLE (1 << 4)
#define PDE_ACCESSED (1 << 5)
#define PDE_DIRTY (1 << 6)     // 4MB entry only
#define PDE_PAGE_SIZE (1 << 7) // 0 = 4KB page, 1 = 4MB page
#define PDE_GLOBAL (1 << 8)    // 4MB entry only
#define PDE_AVL1 (0xE00)
#define PDE_PS_PAT (1 << 12)   // 4MB entry only
#define PDE_FRAME (0xFFFFF000) // bits 12+

#define PTE_PRESENT (1)
#define PTE_READ_WRITE (1 << 1)
#define PTE_USER (1 << 2)
#define PTE_WRITE_THROUGH (1 << 3)
#define PTE_CACHE_DISABLE (1 << 4)
#define PTE_ACCESSED (1 << 5)
#define PTE_DIRTY (1 << 6)
#define PTE_PAT (1 << 7)
#define PTE_GLOBAL (1 << 8)
#define PTE_AVL1 (0xE00)
#define PTE_FRAME (0xFFFFF000) // bits 12+

#define KMALLOC_KERNEL 1
#define KMALLOC_PHYS_PAGETABLE 2
#define KMALLOC_ADDR_CHUNK_RESERVED 2
#define KMALLOC_ADDR_DRIVER 4

#endif