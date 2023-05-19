#pragma once
#include <stdint.h>
#include <stddef.h>

#ifndef KMALLOC_DEF_H
#define KMALLOC_DEF_H

// 4MB
#define PAGE_SIZE 4096

#define PML4_5_PRESENT (1)
#define PML4_5_READ_WRITE (1 << 1)
#define PML4_5_USER (1 << 2)
#define PML4_5_WRITE_THROUGH (1 << 3)
#define PML4_5_CACHE_DISABLE (1 << 4)
#define PML4_5_ACCESSED (1 << 5)
#define PML4_5_AVL0 (1 << 6)
#define PML4_5_RSVD (1 << 7)
#define PML4_5_AVL1 (0xF00)
#define PML4_5_FRAME (0xFFFFFFFFFF << 12)
#define PML4_5_AVL2 (0x7FF << 52)
#define PML4_5_XD (1 << 63)

#define PDPE_PRESENT (1)
#define PDPE_READ_WRITE (1 << 1)
#define PDPE_USER (1 << 2)
#define PDPE_WRITE_THROUGH (1 << 3)
#define PDPE_CACHE_DISABLE (1 << 4)
#define PDPE_ACCESSED (1 << 5)
#define PDPE_DIRTY (1 << 6)
#define PDPE_PAGE_SIZE (1 << 7)
#define PDPE_GLOBAL (1 << 8)
#define PDPE_AVL1 (0xE00)
#define PDPE_PS_PAT (1 << 12)
#define PDPE_PS_RSVD (0x1FFFF << 13)
#define PDPE_FRAME (0x3FFFFF << 30)
#define PDPE_AVL2 (0x7F << 52)
#define PDPE_PS_PK (0xF << 59)
#define PDPE_XD (1 << 63)

#define PDE_PRESENT (1)
#define PDE_READ_WRITE (1 << 1)
#define PDE_USER (1 << 2)
#define PDE_WRITE_THROUGH (1 << 3)
#define PDE_CACHE_DISABLE (1 << 4)
#define PDE_ACCESSED (1 << 5)
#define PDE_DIRTY (1 << 6)
#define PDE_PAGE_SIZE (1 << 7)
#define PDE_GLOBAL (1 << 8)
#define PDE_AVL1 (0xE00)
#define PDE_PS_PAT (1 << 12)
#define PDE_PS_RSVD (0xFF << 13)
#define PDE_FRAME (0xFFFFFFFE << 21)
#define PDE_AVL2 (0x7F << 52)
#define PDE_PS_PK (0xF << 59)
#define PDE_XD (1 << 63)

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
#define PTE_FRAME (0xFFFFFFFFFF << 12) // bits 12+
#define PTE_AVL2 (0x7F << 52)
#define PTE_PK (0xF << 59)
#define PTE_XD (1 << 63)

#define KMALLOC_KERNEL 1
#define KMALLOC_PHYS_PAGETABLE 2
#define KMALLOC_ADDR_CHUNK_RESERVED 2
#define KMALLOC_ADDR_DRIVER 4

#endif