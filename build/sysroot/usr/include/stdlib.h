#pragma once

#include <stddef.h>

#ifndef K_STDLIB_H
#define K_STDLIB_H

#define RAND_MAX __INT_MAX__


#ifdef __cplusplus
extern "C"
{
#endif

double atof(const char* s);
int atoi(const char* s);
long int atol(const char* s);
long long int atoll(const char* s);
long int random(void);
int rand(void);
void srandom(unsigned int __seed);
void srand(unsigned int __seed);

void*malloc(size_t size);
void*calloc(size_t _n,size_t size);
void*realloc(void* ptr,size_t size);
void free(void* ptr);

void abort(void);
void exit(int status);

char* getenv(const char*name);
char* secure_getenv(const char*name);
int putenv(const char*name);
int setenv(const char *name, const char *value, int replaces);
int unsetenv(const char *name);

char *realpath(const char *name,char *resolved);


#ifdef __cplusplus
}
#endif
#endif