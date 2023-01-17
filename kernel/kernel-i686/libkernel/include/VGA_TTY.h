#pragma once

#ifndef VGA_TTY_H
#define VGA_TTY_H

#include <sys/cdefs.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

  enum vga_color
  {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
  };

  extern char* write_buf_start,*write_buf_end,*read_buf_start,*read_buf_end;

  int terminal_initialize(void);

  int terminal_setcolor(uint8_t color);

  int terminal_putentryat(char c, uint8_t color, size_t x, size_t y);

  int terminal_putchar(char c);

  int terminal_write(const char *data, size_t size);

  int terminal_writestring(const char *data);

  int terminal_writeBuffers(void);
  
  int terminal_writeErrBuffers(void);

#ifdef __cplusplus
}
#endif

#endif