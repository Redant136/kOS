#pragma once

#ifndef _CDEFS_H
#define _CDEFS_H

#define _kOS 1
#define __CONCAT(x, y) x##y
#define __STRING(x) #x
#define __EVAL8(...) __EVAL4(EVAL4(__VA_ARGS__))
#define __EVAL4(...) __EVAL2(EVAL2(__VA_ARGS__))
#define __EVAL2(...) __EVAL1(EVAL1(__VA_ARGS__))
#define __EVAL1(...) __VA_ARGS__
#define OS_NAME "kOS"

#ifdef __cplusplus
#define __BEGIN_DECLS \
  extern "C"          \
  {
#define __END_DECLS }
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif

#endif