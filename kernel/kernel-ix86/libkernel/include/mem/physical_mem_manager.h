#pragma once
#include <stdint.h>
#include <stddef.h>
#include <multiboot.h>

#ifndef PHYSICAL_MEM_MANAGER_H
#define PHYSICAL_MEM_MANAGER_H

#ifdef __cplusplus
extern "C"
{
#endif
  extern void read_physical_memory(multiboot_info_t *mbd_phys);
  extern void *kmalloc(size_t n, uint32_t flags);
  extern void kfree(void*ptr);
#ifdef __cplusplus
}
#endif

#endif
