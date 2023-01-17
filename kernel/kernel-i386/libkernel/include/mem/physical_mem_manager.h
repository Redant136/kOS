#pragma once
#include <stdint.h>
#include <stddef.h>
#include <mem/multiboot.h>

#ifndef PHYSICAL_MEM_MANAGER_H
#define PHYSICAL_MEM_MANAGER_H

#ifdef __cplusplus
extern "C"
{
#endif
  extern void read_physical_memory(uint32_t magic_num, multiboot_info_t *mbd_phys);
  extern void *kmalloc(size_t n, uint32_t flags);
  extern void kfree(void*ptr);
#ifdef __cplusplus
}
#endif

#endif
