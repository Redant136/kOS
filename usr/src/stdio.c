#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <ctype.h>

int __empty()
{
  return 0;
}

struct FILE
{
  uint64_t flags;
  char *FILE_READ_START;
  char *FILE_READ_END;
  char *FILE_READ_CUR;

  char *FILE_WRITE_START;
  char *FILE_WRITE_END;
  char *FILE_WRITE_CUR;

  int (*flushBuffers)(void);
};
FILE _stdin = {0, 0, 0, 0, 0, 0, 0, 0};
FILE _stdout = {0, 0, 0, 0, 0, 0, 0, 0};
FILE _stderr = {0, 0, 0, 0, 0, 0, 0, 0};
FILE *stdin = &_stdin;
FILE *stdout = &_stdout;
FILE *stderr = &_stderr;

FILE *setFILE(FILE *file, uint64_t flags,
              char *FILE_READ_START,
              char *FILE_READ_END,
              char *FILE_READ_CUR,
              char *FILE_WRITE_START,
              char *FILE_WRITE_END,
              char *FILE_WRITE_CUR,
              int (*flushBuffers)(void))
{
  file->flags = flags;
  file->FILE_READ_START = FILE_READ_START;
  file->FILE_READ_END = FILE_READ_END;
  file->FILE_READ_CUR = FILE_READ_CUR;
  file->FILE_WRITE_START = FILE_WRITE_START;
  file->FILE_WRITE_END = FILE_WRITE_END;
  file->FILE_WRITE_CUR = FILE_WRITE_CUR;
  file->flushBuffers = flushBuffers;
  return file;
}

size_t uint_ndigits(uint64_t i)
{
  if (!i)
    return 1;
  size_t l = 0;
  for (; i > 0; l++, i /= 10)
    ;
  return l;
}

size_t int_ndigits(int64_t i)
{
  return uint_ndigits(i) + (i < 0) ? 1 : 0;
}

size_t oct_ndigits(uint64_t i)
{
  if (!i)
    return 1;
  size_t l = 0;
  for (; i > 0; l++, i >>= 3)
    ;
  return l;
}

size_t hex_ndigits(uint64_t i)
{
  if (!i)
    return 1;
  size_t l = 0;
  for (; i > 0; l++, i >>= 4)
    ;
  return l;
}

int writeUInt(uint64_t integer, char **buffer)
{
  size_t l = uint_ndigits(integer);
  for (int i = l - 1; i >= 0; --i, integer /= 10)
  {
    (*buffer)[i] = integer % 10 + '0';
  }
  (*buffer) += l;
  return 0;
}

int writeInt(int64_t integer, char **buffer)
{
  if (integer < 0)
    *((*buffer)++) = '-';
  writeUInt(integer, buffer);
  return 0;
}

int printfInt(int64_t ival, char **_buffer, int width, uint8_t leftJustified, uint8_t includePosSign, uint8_t posSignSpace)
{
  int l = int_ndigits(ival);
  if (ival > 0)
  {
    if (includePosSign)
    {
      *((*_buffer)++) = '+';
    }
    else if (posSignSpace)
    {
      *((*_buffer)++) = ' ';
    }
  }
  if (!leftJustified)
  {
    int offset = width - l;
    if ((ival > 0) && (includePosSign || posSignSpace))
    {
      offset--;
    }
    offset = offset < 0 ? 0 : offset;
    *_buffer+=offset;
  }
  writeInt(ival, _buffer);
  return 0;
}

int printfUInt(uint64_t ival, char **_buffer, int width, uint8_t leftJustified, uint8_t includePosSign, uint8_t posSignSpace)
{
  int l = uint_ndigits(ival);
  if (includePosSign)
  {
    *((*_buffer)++) = '+';
  }
  else if (posSignSpace)
  {
    *((*_buffer)++) = ' ';
  }
  if(!leftJustified)
  {
    int offset = width - l;
    if (includePosSign || posSignSpace)
    {
      offset--;
    }
    offset = offset < 0 ? 0 : offset;
    *_buffer+=offset;
  }
  writeUInt(ival, _buffer);
  return 0;
}

int printfOct(uint64_t ival, char **_buffer, int width, uint8_t leftJustified, uint8_t hasPound)
{
  int l = oct_ndigits(ival);
  if (hasPound)
  {
    *((*_buffer)++) = '0';
  }
  if (!leftJustified)
  {
    int offset = width - l;
    if (hasPound)
      offset --;
    offset = offset < 0 ? 0 : offset;
    *_buffer += offset;
  }
  for (int i = l - 1; i >= 0; --i, ival >>= 3)
  {
    int val = ival % 0x8;
    (*_buffer)[i] = val + '0';
  }
  (*_buffer) += l;
  return 0;
}

int printfHex(uint64_t ival, char **_buffer, int width, uint8_t leftJustified, uint8_t hasPound, uint8_t capitalLetter)
{
  int l = hex_ndigits(ival);
  if (hasPound)
  {
    *((*_buffer)++) = '0';
    if (capitalLetter)
      *((*_buffer)++) = 'X';
    else
      *((*_buffer)++) = 'x';
  }
  if (!leftJustified)
  {
    int offset = width - l;
    if(hasPound)
      offset-=2;
    offset = offset < 0 ? 0 : offset;
    *_buffer+=offset;
  }
  for (int i = l - 1; i >= 0; --i, ival >>= 4)
  {
    int val = ival % 0x10;
    if (val < 10)
    {
      (*_buffer)[i] = val + '0';
    }
    else
    {
      val -= 10;
      if (capitalLetter)
      {
        (*_buffer)[i] = val + 'A';
      }
      else
      {
        (*_buffer)[i] = val + 'a';
      }
    }
  }
  (*_buffer)+=l;
  return 0;
}

int _fprintf_expand(FILE *outstream, const char *fmt, va_list ap)
{
  const char *p;

  for (p = fmt; *p; p++)
  {
    if (*p != '%')
    {
      *outstream->FILE_WRITE_CUR = *p;
      outstream->FILE_WRITE_CUR++;
      continue;
    }
    int width = 0, precision = 6;
    uint8_t atWidth = 1, hasStar = 0, leftJustified = 0, includePosSign = 0, posSignSpace = 0, zeroPaded = 0, hasPound = 0, capitalLetter = 0;
    unsigned int nL = 0, nH = 0, nZ = 0, nT = 0;
    while (!strchr("diuoxXncsfeEgGaAp%", *++p))
    {
      if (width == 0)
      {
        if (*p == '-')
        {
          leftJustified = 1;
        }
        else if (*p == '+')
        {
          includePosSign = 1;
        }
        else if (*p == ' ')
        {
          posSignSpace = 1;
        }
        else if (*p == '0')
        {
          zeroPaded = 1;
        }
        else if (*p == '#')
        {
          hasPound = 1;
        }
      }
      if (isdigit(*p) && !hasStar)
      {
        if (atWidth)
        {
          width = width * 10 + (*p) - '0';
        }
        else
          precision = precision * 10 + (*p - '0');
      }
      else if (*p == '*')
      {
        if (!atWidth)
          width = va_arg(ap, int);
        else
          precision = va_arg(ap, int);
        hasStar = 1;
      }
      else if (*p == '.')
      {
        precision = 0;
        atWidth = 0;
      }
      else if (*p == 'l' || *p == 'L')
      {
        ++nL;
      }
      else if (*p == 'h')
      {
        ++nH;
      }
      else if (*p == 'z')
      {
        ++nZ;
      }
      else if (*p == 't')
      {
        ++nT;
      }
    }

    char *current = outstream->FILE_WRITE_CUR;
    for (int i = 0; i < width; i++, current++)
    {
      if (zeroPaded)
        *current = '0';
      else
        *current = ' ';
    }

    switch (*p)
    {
    case 'd':
    case 'i':
    {
      int64_t ival;
      if (nL == 0 && nH == 0)
        ival = va_arg(ap, int);
      else if (nL == 1)
        ival = va_arg(ap, long int);
      else if (nL == 2)
        ival = va_arg(ap, long long int);
      else if (nH == 1)
        ival = va_arg(ap, int);
      else if (nH == 2)
        ival = va_arg(ap, int);
      else if (nZ == 1)
        ival = va_arg(ap, size_t);
      else if (nT)
        ival = va_arg(ap, ptrdiff_t);
      else
        ival = va_arg(ap, int);
      printfInt(ival, &(outstream->FILE_WRITE_CUR), width, leftJustified, includePosSign, posSignSpace);
      break;
    }
    case 'u':
    {
      uint64_t ival;
      if (nL == 0 && nH == 0)
        ival = va_arg(ap, unsigned int);
      else if (nL == 1)
        ival = va_arg(ap, unsigned long int);
      else if (nL == 2)
        ival = va_arg(ap, unsigned long long int);
      else if (nH == 1)
        ival = va_arg(ap, unsigned int);
      else if (nH == 2)
        ival = va_arg(ap, unsigned int);
      else if (nZ == 1)
        ival = va_arg(ap, size_t);
      else if (nT)
        ival = va_arg(ap, ptrdiff_t);
      else
        ival = va_arg(ap, unsigned int);
      printfInt(ival, &(outstream->FILE_WRITE_CUR), width, leftJustified, includePosSign, posSignSpace);
      break;
    }
    case 'o':
    {
      uint64_t ival;
      if (nL == 0 && nH == 0)
        ival = va_arg(ap, unsigned int);
      else if (nL == 1)
        ival = va_arg(ap, unsigned long int);
      else if (nL == 2)
        ival = va_arg(ap, unsigned long long int);
      else if (nH == 1)
        ival = va_arg(ap, unsigned int);
      else if (nH == 2)
        ival = va_arg(ap, unsigned int);
      else if (nZ == 1)
        ival = va_arg(ap, size_t);
      else if (nT)
        ival = va_arg(ap, ptrdiff_t);
      else
        ival = va_arg(ap, unsigned int);
      printfOct(ival, &(outstream->FILE_WRITE_CUR), width, leftJustified, hasPound);
      break;
    }
    case 'x':
    case 'X':
    {
      capitalLetter = *p == 'X';
      uint64_t ival;
      if (nL == 0 && nH == 0)
        ival = va_arg(ap, unsigned int);
      else if (nL == 1)
        ival = va_arg(ap, unsigned long int);
      else if (nL == 2)
        ival = va_arg(ap, unsigned long long int);
      else if (nH == 1)
        ival = va_arg(ap, unsigned int);
      else if (nH == 2)
        ival = va_arg(ap, unsigned int);
      else if (nZ == 1)
        ival = va_arg(ap, size_t);
      else if (nT)
        ival = va_arg(ap, ptrdiff_t);
      else
        ival = va_arg(ap, unsigned int);
      printfHex(ival, &(outstream->FILE_WRITE_CUR),width,leftJustified,hasPound,capitalLetter);
      break;
    }
    case 'n':
    {
      if (nL == 0 && nH == 0)
        va_arg(ap, int *);
      else if (nL == 1)
        va_arg(ap, long int *);
      else if (nL == 2)
        va_arg(ap, long long int *);
      else if (nH == 1)
        va_arg(ap, short int *);
      else if (nH == 2)
        va_arg(ap, signed char *);
      else if (nZ == 1)
        va_arg(ap, size_t *);
      else if (nT)
        va_arg(ap, ptrdiff_t *);
      else
        va_arg(ap, int *);
      break;
    }
    case 'c':
    {
      int cval;
      if (nL)
        return 1; // wint_t not implemented
      else
        cval = va_arg(ap, int);

      *outstream->FILE_WRITE_CUR = cval;
      outstream->FILE_WRITE_CUR++;
      break;
    }
    case 's':
    {
      char *sval;
      if (nL)
        return 1; // wchar_t not implemented
      else
        sval = va_arg(ap, char *);

      for (char *c = sval; *c; c++)
      {
        *(outstream->FILE_WRITE_CUR++) = *c;
      }
      break;
    }
    case 'f':
    {
      double dval;
      if (nL)
        dval = va_arg(ap, long double);
      else
        dval = va_arg(ap, double);
      int64_t ival = (int64_t)dval;
      printfInt(ival, &(outstream->FILE_WRITE_CUR), width, leftJustified, includePosSign, posSignSpace);
      *(outstream->FILE_WRITE_CUR++) = '.';

      for (int i = 0; i < precision; i++)
      {
        dval *= 10;
        int num = (int)dval % 10;
        *(outstream->FILE_WRITE_CUR++) = num + '0';
      }
      break;
    }
    case 'e': // TODO(ANT)
    case 'E':
    {
      // double dval = va_arg(ap, double);
      break;
    }
    case 'g': // TODO(ANT)
    case 'G':
    {
      // double dval = va_arg(ap, double);
      break;
    }
    case 'a': // TODO(ANT)
    case 'A':
    {
      break;
    }
    case 'p':
    {
      size_t pval;
      if (nL == 0 && nH == 0)
        pval = va_arg(ap, size_t);
      else if (nL > 0)
        pval = va_arg(ap, uint64_t);
      else if (nH > 0)
        pval = va_arg(ap, uint32_t);
      else
        pval = va_arg(ap, size_t);

      *(outstream->FILE_WRITE_CUR++) = '0';
      *(outstream->FILE_WRITE_CUR++) = 'x';
      width-=2;
      width=width<0?0:width;
      printfHex(pval, &(outstream->FILE_WRITE_CUR), width, 0, 0, 1);
      break;
    }
    case '%':
    {
      *(outstream->FILE_WRITE_CUR++) = '%';
      break;
    }
    default:
      return 1;
    }
  }
  int res = outstream->flushBuffers();
  outstream->FILE_WRITE_CUR = outstream->FILE_WRITE_START;
  return res;
}

int fputc(int ic, FILE *outstream)
{
  char c = (char)ic;
  if (!outstream || !(outstream->flags & FILE_WRITE))
    return 1;
  *outstream->FILE_WRITE_CUR = c;
  outstream->FILE_WRITE_CUR++;
  return 0;
}
int fputs(const char *s, FILE *outstream)
{
  return fprintf(outstream, "%s\n", s);
}
int fprintf(FILE *file, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  int res = _fprintf_expand(file, fmt, ap);
  va_end(ap);
  return res;
}
int sprintf(char *dst, const char *fmt, ...)
{
  FILE strFile = {FILE_WRITE, 0, 0, 0, dst, dst + strlen(dst), dst, __empty};
  va_list ap;
  va_start(ap, fmt);
  int res = _fprintf_expand(&strFile, fmt, ap);
  va_end(ap);
  return res;
}

int scanf(const char *fmt, ...);
int fscanf(FILE *file, const char *fmt, ...);
int sscanf(const char *src, const char *fmt, ...);

int putchar(int ic)
{
  return fputc(ic, stdout);
}

int puts(const char *string)
{
  return printf("%s\n", string);
}

int printf(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  int res = _fprintf_expand(stdout, fmt, ap);
  va_end(ap);
  return res;
}

