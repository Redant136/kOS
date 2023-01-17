#pragma once
#include <stdint.h>
#include <stddef.h>
#include <mem/kmalloc_def.h>

#ifndef VIRTUAL_MEM_MANGER_H
#define VIRTUAL_MEM_MANGER_H

#ifdef __cplusplus
extern "C"
{
#endif

  extern void *kmalloc_virt_addr(size_t n,uint32_t flags);

#ifdef __cplusplus
}
#endif

#endif
