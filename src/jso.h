/*
 * Copyright (c) 2012-2014 Jakub Zelenka. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
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

/* min max value funcs */
#define JSO_MAX(value_1, value_2) ((value_1) > (value_2) ? (value_1) : (value_2))
#define JSO_MIN(value_1, value_2) ((value_1) < (value_2) ? (value_1) : (value_2))

/* bool type */
typedef char  jso_bool;

/* int type */
typedef long  jso_int;

/* double type */
typedef double jso_double;

/* character type for scanner */
typedef char jso_ctype;

/* jso value types */
typedef enum {
	JSO_TYPE_NULL,
	JSO_TYPE_BOOL,
	JSO_TYPE_INT,
	JSO_TYPE_DOUBLE,
	JSO_TYPE_STRING
} jso_value_type;

/* jso value data union */
typedef union _jso_value_data {
	jso_int ival;
	jso_double dval;
	struct {
		jso_ctype *val;
		size_t len;
	} str;
} jso_value_data;

/* jso value structure */
typedef struct _jso_value {
	jso_value_data data;
	jso_value_type type;
} jso_value;

/* accessors for pointer to jso value */
#define JSO_TYPE_P(pjv) (pjv)->type
#define JSO_IVAL_P(pjv) (pjv)->data.ival
#define JSO_DVAL_P(pjv) (pjv)->data.dval
#define JSO_SVAL_P(pjv) (pjv)->data.str.val
#define JSO_SLEN_P(pjv) (pjv)->data.str.len

/* accessors for jso value */
#define JSO_TYPE(jv) JSO_TYPE_P(&(jv))
#define JSO_IVAL(jv) JSO_IVAL_P(&(jv))
#define JSO_DVAL(jv) JSO_DVAL_P(&(jv))
#define JSO_SVAL(jv) JSO_SVAL_P(&(jv))
#define JSO_SLEN(jv) JSO_SLEN_P(&(jv))

/* jso value setters */
#define JSO_VALUE_SET_NULL(jv) \
	do { \
		JSO_TYPE(jv) = JSO_TYPE_NULL; \
	} while(0)
#define JSO_VALUE_SET_BOOL(jv, bv) \
	do { \
		JSO_TYPE(jv) = JSO_TYPE_BOOL; \
		JSO_IVAL(jv) = bv; \
	} while(0)
#define JSO_VALUE_SET_INT(jv, lv) \
	do { \
		JSO_TYPE(jv) = JSO_TYPE_INT; \
		JSO_IVAL(jv) = lv; \
	} while(0)
#define JSO_VALUE_SET_DOUBLE(jv, dv) \
	do { \
		JSO_TYPE(jv) = JSO_TYPE_DOUBLE; \
		JSO_DVAL(jv) = (dv); \
	} while(0)
#define JSO_VALUE_SET_STRING(jv, sv, sl) \
	do { \
		JSO_TYPE(jv) = JSO_TYPE_STRING; \
		JSO_SVAL(jv) = (sv); \
		JSO_SLEN(jv) = (sl); \
	} while(0)
#define JSO_VALUE_SET_EMPTY_STRING(jv) \
	do { \
		JSO_TYPE(jv) = JSO_TYPE_STRING; \
		JSO_SVAL(jv) = NULL; \
		JSO_SLEN(jv) = 0; \
	} while(0)

/* free jso_value data */
#define JSO_VALUE_FREE(jv) \
	if (JSO_TYPE(jv) == JSO_TYPE_STRING && JSO_SVAL(jv) && JSO_SLEN(jv) > 0) \
		jso_free(JSO_SVAL(jv))

/* memeroy functions - just redefinitions for future extending (checking) */
#define jso_malloc malloc
#define jso_realloc realloc
#define jso_calloc calloc
#define jso_strdup strdup
#define jso_free free

#endif /* JSO_H */
