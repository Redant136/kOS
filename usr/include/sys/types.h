/* Copyright (C) 1991-2022 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

/*
 *	POSIX Standard: 2.6 Primitive System Data Types	<sys/types.h>
 */

#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H 1

#include <features.h>
#include <bits/types.h>

__BEGIN_DECLS

#ifndef __dev_t_defined
typedef unsigned long int dev_t;
#define __dev_t_defined
#endif

#ifndef __gid_t_defined
typedef unsigned int gid_t;
#define __gid_t_defined
#endif

#ifndef __mode_t_defined
typedef unsigned int mode_t;
#define __mode_t_defined
#endif

#ifndef __nlink_t_defined
typedef unsigned long int nlink_t;
#define __nlink_t_defined
#endif

#ifndef __uid_t_defined
typedef unsigned int uid_t;
#define __uid_t_defined
#endif

#ifndef __pid_t_defined
typedef int pid_t;
#define __pid_t_defined
#endif

#if (defined __USE_XOPEN || defined __USE_XOPEN2K8) && !defined __id_t_defined
typedef unsigned int id_t;
#define __id_t_defined
#endif

#ifndef __ssize_t_defined
typedef long int ssize_t;
#define __ssize_t_defined
#endif

#ifdef __USE_MISC
#ifndef __daddr_t_defined
typedef int daddr_t;
typedef char* caddr_t;
#define __daddr_t_defined
#endif
#endif

#if (defined __USE_MISC || defined __USE_XOPEN) && !defined __key_t_defined
typedef int key_t;
#define __key_t_defined
#endif

#ifdef __USE_XOPEN
#ifndef __useconds_t_defined
typedef __useconds_t useconds_t;
#define __useconds_t_defined
#endif
#ifndef __suseconds_t_defined
typedef __suseconds_t suseconds_t;
#define __suseconds_t_defined
#endif
#endif

#define __need_size_t
#include <stddef.h>

#ifdef __USE_MISC
/* Old compatibility names for C types.  */
typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;
#endif

/* These were defined by ISO C without the first `_'.  */
typedef __uint8_t u_int8_t;
typedef __uint16_t u_int16_t;
typedef __uint32_t u_int32_t;
typedef __uint64_t u_int64_t;

#if __GNUC_PREREQ(2, 7)
typedef int register_t __attribute__((__mode__(__word__)));
#else
typedef int register_t;
#endif

/* Some code from BIND tests this macro to see if the types above are
   defined.  */
#define __BIT_TYPES_DEFINED__ 1

#ifdef __USE_MISC
/* In BSD <sys/types.h> is expected to define BYTE_ORDER.  */
#include <endian.h>

#endif /* Use misc.  */

#if (defined __USE_UNIX98 || defined __USE_XOPEN2K8) && !defined __blksize_t_defined
typedef __blksize_t blksize_t;
#define __blksize_t_defined
#endif

/* Types from the Large File Support interface.  */
#ifndef __USE_FILE_OFFSET64
#ifndef __blkcnt_t_defined
typedef __blkcnt_t blkcnt_t; /* Type to count number of disk blocks.  */
#define __blkcnt_t_defined
#endif
#ifndef __fsblkcnt_t_defined
typedef __fsblkcnt_t fsblkcnt_t; /* Type to count file system blocks.  */
#define __fsblkcnt_t_defined
#endif
#ifndef __fsfilcnt_t_defined
typedef __fsfilcnt_t fsfilcnt_t; /* Type to count file system inodes.  */
#define __fsfilcnt_t_defined
#endif
#else
#ifndef __blkcnt_t_defined
typedef __blkcnt64_t blkcnt_t;     /* Type to count number of disk blocks.  */
#define __blkcnt_t_defined
#endif
#ifndef __fsblkcnt_t_defined
typedef __fsblkcnt64_t fsblkcnt_t; /* Type to count file system blocks.  */
#define __fsblkcnt_t_defined
#endif
#ifndef __fsfilcnt_t_defined
typedef __fsfilcnt64_t fsfilcnt_t; /* Type to count file system inodes.  */
#define __fsfilcnt_t_defined
#endif
#endif

#ifdef __USE_LARGEFILE64
typedef __blkcnt64_t blkcnt64_t;     /* Type to count number of disk blocks. */
typedef __fsblkcnt64_t fsblkcnt64_t; /* Type to count file system blocks.  */
typedef __fsfilcnt64_t fsfilcnt64_t; /* Type to count file system inodes.  */
#endif

__END_DECLS

#endif /* sys/types.h */
