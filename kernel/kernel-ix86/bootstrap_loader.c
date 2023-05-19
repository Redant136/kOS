#include <sys/elf32.h>
#include <sys/elf_parser.h>
#include <multiboot.h>
#include <stdint.h>
#include <string.h>

char *kernel_elf_space[sizeof(elf_file_data_t)];
elf_file_data_t *kernel_elf = (elf_file_data_t *)kernel_elf_space; /* Pointer to elf file structure (remember there is no memory management yet) */

/* This function parses the ELF file and returns the entry point */
void *load_elf_module(multiboot_uint32_t mod_start, multiboot_uint32_t mod_end)
{
  // unsigned long err = parse_elf_executable((void *)mod_start, sizeof(elf_file_data_t), kernel_elf); /* Parses ELF file and returns an error code */
  // if (err == 0)
  // { /* No errors occurred while parsing the file */
  //   for (int i = 0; i < kernel_elf->numSegments; i++)
  //   {
  //     elf_file_segment_t seg = kernel_elf->segments[i];          /* Load all the program segments into memory */
  //                                                                /*  if you want to do relocation you should do so here, */
  //     const void *src = (const void *)(mod_start + seg.foffset); /*  though that would require some changes to parse_elf_executable */
  //     memcpy((void *)seg.address, src, seg.flength);
  //   }
  //   return (void *)kernel_elf->entryAddr; /* Finally we can return the entry address */
  // }
  // return NULL;
}

void lmain(void *mbd)
{
  // setup memory here
  int a =0;
  const multiboot_info_t *mb_info = mbd; /* Make pointer to multiboot_info_t struct */
  multiboot_uint32_t mb_flags = mb_info->flags;       /* Get flags from mb_info */

  void *kentry = 0; /* Pointer to the kernel entry point */

  if (mb_flags & MULTIBOOT_INFO_MODS)
  {                                                      /* Check if modules are available */
    multiboot_uint32_t mods_count = mb_info->mods_count; /* Get the amount of modules available */
    multiboot_uint32_t mods_addr = mb_info->mods_addr;   /* And the starting address of the modules */

    for (uint32_t mod = 0; mod < mods_count; mod++)
    {
      multiboot_module_t *module = (multiboot_module_t *)(mods_addr + (mod * sizeof(multiboot_module_t))); /* Loop through all modules */
      const char *module_string = (const char *)module->cmdline;
      /* Here I check if module_string is equals to the one i assigned my kernel
         you could skip this check if you had a way of determining the kernel module */
      // try "kOS_bin" if not work
      if (strcmp(module_string, OS_NAME))
      {
        kentry = load_elf_module(module->mod_start, module->mod_end);
      }
    }
  }

  

  return;  
}