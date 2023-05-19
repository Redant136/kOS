#include <stdio.h>
#include <mem/kmalloc_def.h>
#include <mem/physical_mem_manager.h>
#include <mem/virtual_mem_manager.h>
#include <mem/paging.h>
#include <stdlib.h>
#include <string.h>
#include <kmacro.h>

extern void *_kernel_start;
extern void *_kernel_end;

static void print_chunk_list();

typedef struct pm_linked_list pm_linked_list_t;
struct pm_linked_list
{
  pm_linked_list_t *prev;
  pm_linked_list_t *next;
};

typedef struct pm_chunk pm_chunk_t;
struct pm_chunk
{
  pm_linked_list_t list;
  size_t flags;
#define PM_CHUNK_USED 1
#define PM_CHUNK_MAPPED 2
#define PM_CHUNK_KERNEL_ONLY 4
#define PM_CHUNK_PD_RESERVED 8

  union
  {
    char data[0];
    void *_free;
  };
};

#define PM_OFFSET offsetof(pm_chunk_t, data)

pm_chunk_t *first_chunk;
pm_chunk_t *last_chunk;

static size_t pm_chunk_len(pm_chunk_t *chunk)
{
  size_t len;
  if (!chunk->list.next)
    len = sizeof(pm_chunk_t);
  else
    len = get_physaddr(chunk->list.next) - get_physaddr(chunk);
  return len;
}
static void pm_chunk_moveTo(pm_chunk_t *chunk, void *addr)
{
  pm_chunk_t tmp = *chunk;
  map_page(addr, chunk, 3);
  *chunk = tmp;
}
static void pm_chunk_move(pm_chunk_t *chunk, size_t offset)
{
  pm_chunk_t tmp = *chunk;
  map_page(get_physaddr(chunk) + offset, chunk, 3);
  *chunk = tmp;
}

static void pm_linked_list_add(pm_chunk_t *prev, pm_chunk_t *next)
{
  prev->list.next = (pm_linked_list_t *)next;
  next->list.prev = (pm_linked_list_t *)prev;
}
static void pm_linked_list_insert(pm_chunk_t *current, pm_chunk_t *x)
{
  x->list.prev = (pm_linked_list_t *)current;
  x->list.next = (pm_linked_list_t *)current->list.next;
  current->list.next->prev = (pm_linked_list_t *)x;
  current->list.next = (pm_linked_list_t *)x;
}
static void pm_linked_list_pop(pm_chunk_t *x)
{
  x->list.prev->next = x->list.next;
  x->list.next->prev = x->list.prev;
}
static void pm_linker_list_replace(pm_chunk_t *current, pm_chunk_t *x)
{
  current->list.prev->next = (pm_linked_list_t *)x;
  current->list.next->prev = (pm_linked_list_t *)x;
}
