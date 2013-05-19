/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:  Jakub Zelenka <jakub.php@gmail.com>                        |
   +----------------------------------------------------------------------+
*/

#ifndef JSO_H
#define JSO_H

#include <config.h>
#include <sys/types.h>

/* exported library functions */
#define JSO_API

/* bool values */
#define JSO_TRUE  1
#define JSO_FALSE 0

#define JSO_MAX(_v1, _v2) ((_v1) > (_v2) ? (_v1) : (_v2))
#define JSO_MIN(_v1, _v2) ((_v1) < (_v2) ? (_v1) : (_v2))

/* int types */
typedef unsigned char  jso_bool;
typedef unsigned char  jso_uchar;
typedef unsigned int   jso_uint;
typedef unsigned long  jso_ulong;
typedef unsigned short jso_ushort;

/* character type for scanner */
typedef char jso_ctype;

/* memeroy functions - just redefinitions for future extending (checking) */
#define jso_malloc malloc
#define jso_realloc realloc
#define jso_calloc calloc
#define jso_free free

#endif /* JSO_H */
