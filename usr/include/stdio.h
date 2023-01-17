#pragma once

#include <stdint.h>

#ifndef K_STDIO_H
#define K_STDIO_H

#define EOF -1


#ifdef __cplusplus
extern "C"
{
#endif

  enum FILE_FLAGS{
    FILE_READ=1<<1,
    FILE_WRITE=1<<2
  };
  typedef struct FILE FILE;
  extern FILE*stdout;
  extern FILE*stdin;
  extern FILE*stderr;

  FILE *setFILE(FILE *file, uint64_t flags,
                char *FILE_READ_START,
                char *FILE_READ_END,
                char *FILE_READ_CUR,
                char *FILE_WRITE_START,
                char *FILE_WRITE_END,
                char *FILE_WRITE_CUR,
                int (*flushBufs)(void));

  int putchar(int);
  int puts(const char *);
  int printf(const char *fmt, ...);

  int fputc(int,FILE*file);
  int fputs(const char *, FILE *file);
  int fprintf(FILE*file,const char*fmt,...);
  int sprintf(char*dst,const char*fmt,...);

  int scanf(const char*fmt,...);
  int fscanf(FILE*file,const char*fmt,...);
  int sscanf(const char*src,const char*fmt,...);


#ifdef __cplusplus
}
#endif

#endif