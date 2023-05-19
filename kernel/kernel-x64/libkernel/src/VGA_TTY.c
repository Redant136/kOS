#include <VGA_TTY.h>
#include <string.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define OUTPUT_POINTER 0xFFFFFC00001FF000

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
  return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
  return (uint16_t)uc | (uint16_t)color << 8;
}

static char _write_buf_start[VGA_WIDTH * VGA_HEIGHT] = {0};
static char _read_buf_start[VGA_WIDTH * VGA_HEIGHT] = {0};
char *write_buf_start = _write_buf_start;
char *write_buf_end = _write_buf_start + VGA_WIDTH * VGA_HEIGHT - 1;
char *read_buf_start = _read_buf_start;
char *read_buf_end = _read_buf_start + VGA_WIDTH * VGA_HEIGHT - 1;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t *terminal_buffer;

int terminal_initialize(void)
{
  terminal_row = 0;
  terminal_column = 0;
  terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
  terminal_buffer = (uint16_t *)OUTPUT_POINTER;
  for (size_t y = 0; y < VGA_HEIGHT; y++)
  {
    for (size_t x = 0; x < VGA_WIDTH; x++)
    {
      const size_t index = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
  }

  return 0;
}

int terminal_setcolor(uint8_t color)
{
  terminal_color = color < 16 ? color : 15;
  return !(color < 16);
}

int terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
  const size_t index = y * VGA_WIDTH + x;
  terminal_buffer[index] = vga_entry(c, color);
  return 0;
}

int terminal_putchar(char c)
{
  switch (c)
  {
  case '\n':
    terminal_column = 0;
    if (++terminal_row == VGA_HEIGHT)
      terminal_row = 0;
    return 0;
  case '\t':
    while ((++terminal_column) % 8)
      ;
    return 0;
  case '\r':
    terminal_column = 0;
    return 0;
  default:
    break;
  }
  if (c < 0x20)
    return 1;
  else if (c == 0x7f)
    if (terminal_column != 0)
      --terminal_column;
  terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
  if (++terminal_column == VGA_WIDTH)
  {
    terminal_column = 0;
    if (++terminal_row == VGA_HEIGHT)
      terminal_row = 0;
  }
  return 0;
}

int terminal_write(const char *data, size_t size)
{
  int res = 0;
  for (size_t i = 0; i < size; i++)
    res |= terminal_putchar(data[i]);
  return res;
}

int terminal_writestring(const char *data)
{
  return terminal_write(data, strlen(data));
}

int terminal_writestringColor(const char *data, uint8_t color)
{
  int res = 0;
  for (size_t i = 0; i < strlen(data); i++)
  {
    res |= terminal_setcolor(color);
    res |= terminal_putchar(data[i]);
  }
  return res;
}

int terminal_writeBuffers(void)
{
  int res = terminal_writestring(write_buf_start);
  memset(write_buf_start, 0, write_buf_end - write_buf_start);
  return res;
}

int terminal_writeErrBuffers(void)
{
  int res = terminal_writestringColor(write_buf_start, VGA_COLOR_RED);
  memset(write_buf_start, 0, write_buf_end - write_buf_start);
  return res;
}
