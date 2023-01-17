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
// ---------
static void pm_linked_list_update_element(pm_chunk_t *current);
static int pm_linked_list_combine(pm_chunk_t *first, pm_chunk_t *second)
{
  if (!(second->flags & 1) && !(first->flags & 1))
  {
    if ((size_t)get_physaddr(second) / PAGE_SIZE == (size_t)get_physaddr(first) / PAGE_SIZE)
    {
      pm_linked_list_pop(second);
      if (pm_chunk_len(first) >= PAGE_SIZE)
        first->flags &= ~PM_CHUNK_MAPPED;
    }
    else
    {
      if (!((size_t)get_physaddr(second) % PAGE_SIZE) && !((size_t)get_physaddr(first) % PAGE_SIZE) &&
          !(second->flags & PM_CHUNK_MAPPED) && !(first->flags & PM_CHUNK_MAPPED))
      {
        if (second > first_chunk && second < last_chunk)
        {
          pm_linked_list_pop(first);
          pm_chunk_moveTo(second, get_physaddr(first));
          unmap_page(first);
          return 0;
        }
        else
        {
          pm_linked_list_pop(second);
          unmap_page(second);
        }
      }
    }
    pm_linked_list_update_element(first);
    return 1;
  }
  return 0;
}
static void pm_linked_list_update_element(pm_chunk_t *current)
{
  if (!pm_linked_list_combine((pm_chunk_t *)current->list.prev, current))
    pm_linked_list_combine(current, (pm_chunk_t *)current->list.next);
}
static void pm_linked_list_fullclean()
{
  pm_chunk_t *current = (pm_chunk_t *)first_chunk->list.next;
  while (current)
  {
    pm_linked_list_update_element(current);
    current = (pm_chunk_t *)current->list.next;
  }
  current = (pm_chunk_t *)first_chunk->list.next;
  while (current)
  {
    pm_linked_list_update_element(current);
    current = (pm_chunk_t *)current->list.next;
  }
}

// -------
static void print_chunk_list()
{
  pm_chunk_t *current = first_chunk;
  while (current)
  {
    printf("%d -> ", current->flags);
    current = (pm_chunk_t *)current->list.next;
  }
  printf("%d\n", 0);
  current = first_chunk;
  while (current)
  {
    printf("%p -> ", current);
    current = (pm_chunk_t *)current->list.next;
  }
  printf("%d\n", 0);
  current = first_chunk;
  while (current)
  {
    printf("%p -> ", get_physaddr(current));
    current = (pm_chunk_t *)current->list.next;
  }
  printf("%d\n", 0);
  current = first_chunk;
  while (current)
  {
    printf("%X -> ", pm_chunk_len(current));
    current = (pm_chunk_t *)current->list.next;
  }
  printf("%d\n", 0);
}

void read_physical_memory(uint32_t magic, multiboot_info_t *mbd)
{
  if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
  {
    CRASH("invalid magic number!");
  }
  if (!(mbd->flags >> 6 & 0x1))
  {
    CRASH("invalid memory map given by GRUB bootloader");
  }


  uint32_t virtaddr_offset = (uint32_t)mbd - (uint32_t)get_physaddr(mbd);
  mbd->mmap_addr = (uint32_t)mbd->mmap_addr + virtaddr_offset;
  first_chunk = 0;
  last_chunk = 0;
  for (uint32_t i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t))
  {
    multiboot_memory_map_t *mmmt = (multiboot_memory_map_t *)(mbd->mmap_addr + i);
    if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE && (mmmt->addr + mmmt->len > (size_t)get_physaddr(&_kernel_end)))
    {
      pm_chunk_t *first = _pre_kernel_next_virtaddr();
      pm_chunk_t *post_first = first;
      size_t start = mmmt->addr;

      if (start < (size_t)get_physaddr(&_kernel_end) + sizeof(void *))
        start = (size_t)get_physaddr(&_kernel_end) + sizeof(void *);
      if (start % PAGE_SIZE)
        start += PAGE_SIZE - start % PAGE_SIZE;
      map_page((void *)start, first, 3);
      post_first = (pm_chunk_t *)((size_t)post_first + PM_OFFSET);

      pm_chunk_t *mid_chunk = _pre_kernel_next_virtaddr();
      map_page((void *)(start + PAGE_SIZE), mid_chunk, 3);
      mid_chunk->flags = 0;

      pm_chunk_t *last = _pre_kernel_next_virtaddr();
      pm_chunk_t *pre_last = last;
      size_t end = mmmt->addr + mmmt->len - sizeof(pm_chunk_t);
      if (end % PAGE_SIZE)
        end -= end % PAGE_SIZE;
      map_page((void *)end, last, 3);
      last = (pm_chunk_t *)((size_t)last + PAGE_SIZE - sizeof(pm_chunk_t));

      first->flags = 7;
      last->flags = 7;
      post_first->flags = 6;
      pre_last->flags = 6;

      pm_linked_list_add(first, mid_chunk);
      pm_linked_list_add(mid_chunk, last);
      pm_linked_list_insert(first, post_first);
      pm_linked_list_insert(mid_chunk, pre_last);
      first->list.prev = 0;
      last->list.next = 0;

      if (!first_chunk)
      {
        first_chunk = first;
        last_chunk = last;
      }
      else
      {
        pm_linked_list_add(last_chunk, first);
        last_chunk = last;
      }
    }
  }

  pm_chunk_t *current = first_chunk;
  while (current)
  {
    if (!(current->flags & 3))
    {
      size_t len = pm_chunk_len(current);
      if (len > PAGE_SIZE * 1024)
        break;
    }
    current = (pm_chunk_t *)current->list.next;
  }

  // since the start of a memory sector occupies 2 chunks (1 used, 1 free). Use this to reduce the space used by the pd_reserved chunk as it will act as the header
  pm_chunk_t *pd_reserved = (pm_chunk_t *)current->list.prev;

  pm_linked_list_pop(pd_reserved);
  pd_reserved = (pm_chunk_t *)pd_reserved->list.prev;
  pd_reserved->flags |= 15;
  pm_chunk_move(current, PAGE_SIZE * 1024);
}

void *alloc_page_table(int index)
{
  pm_chunk_t *pd_reserved_chunk = first_chunk;
  while (pd_reserved_chunk)
  {
    if (pd_reserved_chunk->flags & PM_CHUNK_PD_RESERVED)
      break;
    pd_reserved_chunk = (pm_chunk_t *)pd_reserved_chunk->list.next;
  }
  return get_physaddr(pd_reserved_chunk) + PAGE_SIZE * (index + 1);
}

// optimize so that the pages of memory used don't need to be next to one another
void *kmalloc(size_t _size, uint32_t flags)
{
  if (flags & KMALLOC_PHYS_PAGETABLE)
  {
    return alloc_page_table(_size);
  }
  pm_chunk_t *current = first_chunk;
  while (current)
  {
    if (!(current->flags & PM_CHUNK_USED) &&
        (!!(current->flags & PM_CHUNK_KERNEL_ONLY) == !!(flags & KMALLOC_KERNEL)))
    {
      size_t len = get_physaddr(current->list.next) - get_physaddr(current);
      if (len > _size)
        break;
    }
    current = (pm_chunk_t *)current->list.next;
  }
  if (!current || (current->flags & 1))
  {
    CRASH("Out of memory");
    return 0;
  }

  size_t start_addr = (size_t)get_physaddr(current);
  pm_chunk_t *used_chunk;
  // map the chunk
  if (!(current->flags & PM_CHUNK_MAPPED))
  {
    used_chunk = kmalloc_virt_addr(_size, flags);
    map_page((void *)start_addr, used_chunk, 3 | (flags & KMALLOC_KERNEL ? 0 : PTE_USER));

    pm_chunk_move(current, PAGE_SIZE);

    pm_linked_list_insert((pm_chunk_t *)current->list.prev, used_chunk);
  }
  else
  {
    used_chunk = current;
  }

  pm_chunk_t *remaining = (pm_chunk_t *)((size_t *)used_chunk + PM_OFFSET + _size);
  pm_linked_list_insert(used_chunk, remaining);

  used_chunk->flags = PM_CHUNK_USED | PM_CHUNK_MAPPED | (flags & KMALLOC_KERNEL ? PM_CHUNK_KERNEL_ONLY : 0);
  remaining->flags = PM_CHUNK_MAPPED | (flags & KMALLOC_KERNEL ? PM_CHUNK_KERNEL_ONLY : 0);

  return (size_t *)used_chunk + PM_OFFSET;
}

void kfree(void *ptr)
{
  pm_chunk_t *current = (pm_chunk_t *)((size_t *)ptr - PM_OFFSET);
  if (!current || !(current->flags & 1))
  {
    CRASH("memory was not allocated");
  }
  current->flags &= ~PM_CHUNK_USED;
  pm_linked_list_update_element(current);
}
