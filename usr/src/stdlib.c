#include <stdlib.h>
#include <ctype.h>

static unsigned long long int __rand_next=1;

double atof(const char *s){
  double a=0,b=0;
  int pastPer=0;
  while(*s!=0){
    if(!pastPer){
      a *= 10;
      if (isdigit(*s))
        a += (*s - '0');
    }else{
      b *= 10;
      if (*s <= '9' && *s >= '0')
        a += (*s - '0')/b;
    }
    if(*s=='.'){
      pastPer=1;
    }
  }
  return (s[0] == '-' ? -1 : 1) * a;
}
int atoi(const char *s){
  return atoll(s);
}
long int atol(const char *s){
  return atoll(s);
}
long long int atoll(const char *s){
  long long int a = 0;
  while (*s != 0 && *s != '.')
  {
    a *= 10;
    if (isdigit(*s))
      a += (*s - '0');
  }
  return (s[0] == '-' ? -1 : 1) * a;
}

int rand(void){
  __rand_next = __rand_next * 1103515245 + 12345;
  return (unsigned int)(__rand_next / 65536) % RAND_MAX;
}
void srand(unsigned int seed)
{
  __rand_next = seed;
}
long int random(void){
  return rand();
}
void srandom(unsigned int seed){
  srand(seed);
}

__attribute__((__noreturn__)) void abort(void)
{
  asm volatile("hlt");
  while (1)
  {
  }
  __builtin_unreachable();
}
// would need to be reimplemented
void exit(int status)
{
  status &= 0xff;
  abort();
}

#if 0
void *malloc(size_t size){

}
void *calloc(size_t _n, size_t size){

}
void *realloc(void *ptr, size_t size){

}
void free(void *ptr){

}

char* getenv(const char *name){

}
char* secure_getenv(const char *name){

}
int putenv(const char *name){

}
int setenv(const char *name, const char *value, int replaces){

}
int unsetenv(const char *name){

}

char *realpath(const char *name, char *resolved){
}

#endif