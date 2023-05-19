#pragma once

#ifndef _SYS_ELF_PARSER_H_
#define _SYS_ELF_PARSER_H_ 1

#if defined(__ix86__)
#include <sys/elf32.h>

typedef Elf32_Phdr elf_file_data_t;
typedef int elf_file_segment_t;
int parse_elf_executable(){

}

#elif defined(__x64__)
#include <sys/elf64.h>
#endif

#endif