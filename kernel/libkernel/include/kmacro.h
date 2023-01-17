#pragma once

#include <stdio.h>

#ifndef KMACRO_H
#define KMACRO_H

#define CRASH1(MSG, ...) fprintf(stderr, MSG, __VA_ARGS__);fprintf(stderr,"%s:%d\n",__FILE__,__LINE__);abort();

#define CRASH(...) CRASH1(__VA_ARGS__,0)

#endif