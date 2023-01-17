#pragma once
#include <stdint.h>
#include <stddef.h>

#ifndef PAGING_H
#define PAGING_H

#ifdef __cplusplus
extern "C"
{
#endif

  extern void *get_physaddr(void *virtualaddr);
  extern void *map_page(void *physaddr, void *virtualaddr, uint32_t flags);
  extern void unmap_page(void*virtaddr);
  extern void* _pre_kernel_next_virtaddr();

#ifdef __cplusplus
}
#endif

#endif