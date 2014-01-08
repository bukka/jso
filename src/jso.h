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

#define JSO_MAX(value_1, value_2) ((value_1) > (value_2) ? (value_1) : (value_2))
#define JSO_MIN(value_1, value_2) ((value_1) < (value_2) ? (value_1) : (value_2))

/* int types */
typedef unsigned char  jso_bool;
typedef unsigned char  jso_uchar;
typedef unsigned int   jso_uint;
typedef unsigned long  jso_ulong;
typedef unsigned short jso_ushort;

/* character type for scanner */
typedef char jso_ctype;

/* jso value types */
typedef enum {
	JSO_TYPE_NULL,
	JSO_TYPE_LONG,
	JSO_TYPE_DOUBLE,
	JSO_TYPE_STRING
} jso_value_type;

/* jso value data union */
typedef union _jso_value_data {
	long lval;
	double dval;
	struct {
		jso_ctype *val;
		int len;
	} str;
} jso_value_data;

/* jso value structure */
typedef struct _jso_value {
	jso_value_data data;
	jso_value_type type;
} jso_value;

/* accessors for pointer to jso value */
#define JSO_TYPE_P(pjv) (pjv)->type
#define JSO_LVAL_P(pjv) (pjv)->data.lval
#define JSO_DVAL_P(pjv) (pjv)->data.dval
#define JSO_SVAL_P(pjv) (pjv)->data.str.val
#define JSO_SLEN_P(pjv) (pjv)->data.str.len

/* accessors for jso value */
#define JSO_TYPE(jv) JSO_TYPE_P(&(jv))
#define JSO_LVAL(jv) JSO_LVAL_P(&(jv))
#define JSO_DVAL(jv) JSO_DVAL_P(&(jv))
#define JSO_SVAL(jv) JSO_SVAL_P(&(jv))
#define JSO_SLEN(jv) JSO_SLEN_P(&(jv))

/* jso value setters */
#define JSO_VALUE_SET_LONG(jv, lv)   do { JSO_TYPE(jv) = JSO_TYPE_LONG; JSO_LVAL(jv) = lv; } while(0)
#define JSO_VALUE_SET_DOUBLE(jv, dv) do { JSO_TYPE(jv) = JSO_TYPE_DOUBLE; JSO_DVAL(jv) = (dv); } while(0)

/* memeroy functions - just redefinitions for future extending (checking) */
#define jso_malloc malloc
#define jso_realloc realloc
#define jso_calloc calloc
#define jso_strdup strdup
#define jso_free free

#endif /* JSO_H */
