#pragma once

#ifndef STRING_H
#define STRING_H
#include <stddef.h>
#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C"
{
#endif

  static void *memcpy(void *dst, void *src, size_t n)
  {
    for (size_t i = 0; i < n; i++)
    {
      ((unsigned char *)dst)[i] = ((unsigned char *)src)[i];
    }
    return dst;
  }
  static void *memccpy(void *dst, void *src, size_t c, size_t n)
  {
    for (size_t i = 0; i < n; i++)
    {
      unsigned char byte = ((unsigned char *)src)[i];
      ((unsigned char *)dst)[i] = byte;
      if (byte == c)
        return ((unsigned char *)src) + i + 1;
    }
    return NULL;
  }
  static void *memset(void *dst, int c, size_t n)
  {
    for (size_t i = 0; i < n / sizeof(int); i++)
    {
      ((int *)dst)[i] = c;
    }
    return dst;
  }
  static void *memchr(void *s, int c, size_t n)
  {
    for (size_t i = 0; i < n; i++)
    {
      int cmp = *(((char *)s) + i);
      if (cmp == c)
        return ((char *)s) + i;
    }
    return NULL;
  }
  static int memcmp(const void *aptr, const void *bptr, size_t size)
  {
    const unsigned char *a = (const unsigned char *)aptr;
    const unsigned char *b = (const unsigned char *)bptr;
    for (size_t i = 0; i < size; i++)
    {
      if (a[i] < b[i])
        return -1;
      else if (b[i] < a[i])
        return 1;
    }
    return 0;
  }

  static char *strcpy(char *dst, char *src)
  {
    size_t i;
    for (i = 0; src[i] != 0; i++)
    {
      dst[i] = src[i];
    }
    dst[i + 1] = 0;
    return dst;
  }
  static char *strncpy(char *dst, char *src, size_t n)
  {
    for (size_t i = 0; src[i] != 0 && i < n; i++)
    {
      dst[i] = src[i];
    }
    return dst;
  }
  static size_t strlen(const char *str)
  {
    size_t len = 0;
    while (str[len])
      len++;
    return len;
  }
  static char *strcat(char *dst, char *src)
  {
    size_t l = strlen(dst);
    for (size_t i = 0; src[i] != 0; i++)
    {
      dst[l + i] = src[i];
    }
    return dst;
  }
  static char *strncat(char *dst, char *src, size_t n)
  {
    size_t l = strlen(dst);
    for (size_t i = 0; src[i] != 0 && i < n; i++)
    {
      dst[l + i] = src[i];
    }
    return dst;
  }
  static int strcmp(const char *s1, const char *s2)
  {
    while (*s1 != 0)
    {
      if (*s1 != *s2)
        break;
      s1++;
      s2++;
    }
    return (int)((const unsigned char *)s1 - *(const unsigned char *)s2);
  }
  static int strncmp(const char *s1, const char *s2, size_t n)
  {
    for (size_t i = 0; *s1 != 0 && i < n; i++, s1++, s2++)
    {
      if (*s1 != *s2)
        break;
    }
    return (int)((const unsigned char *)s1 - *(const unsigned char *)s2);
  }
  static char *strchr(char *s, char c)
  {
    return (char *)memchr(s, c, strlen(s));
  }

  static char *strerror(int errno)
  {
    if (errno)
      return "";
    return "";
  }

#ifdef __cplusplus
}
#endif

#endif
