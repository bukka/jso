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

/* jso int limits */
#if SIZEOF_LONG == 4
#define JSO_INT_MAX_LENGTH 10
#define JSO_INT_MAX_DIGITS "2147483648"
#elif SIZEOF_LONG == 8
#define JSO_INT_MAX_LENGTH 19
#define JSO_INT_MAX_DIGITS "9223372036854775808"
#else
#error "Unknown SIZEOF_LONG"
#endif


/* this macro suppresses warnings about unused variable */
#define JSO_USE(unused_variable) ((void) (unused_variable))
#define JSO_USE_1(uv0, uv1) JSO_USE(uv0); JSO_USE(uv1)
#define JSO_USE_2(uv0, uv1, uv2) JSO_USE(uv0); JSO_USE(uv1); JSO_USE(uv2)

/* bool type */
typedef char  jso_bool;

/* int type */
typedef long  jso_int;

/* unsigned int type */
typedef unsigned long jso_uint;

/* double type */
typedef double jso_double;

/* character type for scanner */
typedef unsigned char jso_ctype;

/* pre-definition of array type */
typedef struct _jso_array jso_array;

/* pre-definition of object type */
typedef struct _jso_object jso_object;

/* string type */
typedef struct _jso_string {
	jso_ctype *val;
	size_t len;
} jso_string;

/* jso value types */
typedef enum {
	JSO_TYPE_ERROR,
	JSO_TYPE_NULL,
	JSO_TYPE_BOOL,
	JSO_TYPE_INT,
	JSO_TYPE_DOUBLE,
	JSO_TYPE_STRING,
	JSO_TYPE_ARRAY,
	JSO_TYPE_OBJECT
} jso_value_type;

/* jso error types */
typedef enum {
	JSO_ERROR_SYNTAX,
	JSO_ERROR_DEPTH,
	JSO_ERROR_TOKEN,
	JSO_ERROR_CTRL_CHAR,
	JSO_ERROR_UTF16,
	JSO_ERROR_UTF8,
	JSO_ERROR_ESCAPE
} jso_error_type;

/* jso error location */
typedef struct {
	size_t first_column;
	size_t first_line;
	size_t last_column;
	size_t last_line;
} jso_location;

/* jso error structure */
typedef struct {
	jso_error_type type;
	jso_location loc;
} jso_error;

/* jso value data union */
typedef union _jso_value_data {
	jso_int ival;
	jso_double dval;
	jso_string str;
	jso_array *arr;
	jso_object *obj;
	jso_error *err;
} jso_value_data;

/* jso value structure */
typedef struct _jso_value {
	jso_value_data data;
	jso_value_type type;
} jso_value;

/* jso array element */
typedef struct _jso_array_element {
	jso_value val;
	struct _jso_array_element *next;
} jso_array_element;

/* jso array structure */
struct _jso_array {
	jso_array_element *head;
	jso_array_element *tail;
};

/* jso object member */
typedef struct _jso_object_member {
	jso_string key;
	jso_value val;
	struct _jso_object_member *next;
} jso_object_member;

/* jso object structure */
struct _jso_object {
	jso_object_member *head;
	jso_object_member *tail;
};

/* accessors for pointer to jso value */
#define JSO_TYPE_P(pjv) (pjv)->type
#define JSO_IVAL_P(pjv) (pjv)->data.ival
#define JSO_DVAL_P(pjv) (pjv)->data.dval
#define JSO_SVAL_P(pjv) (pjv)->data.str.val
#define JSO_SLEN_P(pjv) (pjv)->data.str.len
#define JSO_ARRVAL_P(pjv) (pjv)->data.arr
#define JSO_OBJVAL_P(pjv) (pjv)->data.obj
#define JSO_EVAL_P(pjv) (pjv)->data.err
#define JSO_ELOC_P(pjv) (pjv)->data.err->loc
#define JSO_ETYPE_P(pjv) (pjv)->data.err->type

/* accessors for jso value */
#define JSO_TYPE(jv) JSO_TYPE_P(&(jv))
#define JSO_IVAL(jv) JSO_IVAL_P(&(jv))
#define JSO_DVAL(jv) JSO_DVAL_P(&(jv))
#define JSO_SVAL(jv) JSO_SVAL_P(&(jv))
#define JSO_SLEN(jv) JSO_SLEN_P(&(jv))
#define JSO_ARRVAL(jv) JSO_ARRVAL_P(&(jv))
#define JSO_OBJVAL(jv) JSO_OBJVAL_P(&(jv))
#define JSO_EVAL(jv) JSO_EVAL_P(&(jv))
#define JSO_ELOC(jv) JSO_ELOC_P(&(jv))
#define JSO_ETYPE(jv) JSO_ETYPE_P(&(jv))

/* jso value setters */
#define JSO_VALUE_SET_ERROR(jv, eval) \
	do { \
		JSO_TYPE(jv) = JSO_TYPE_ERROR; \
		JSO_EVAL(jv) = eval; \
	} while(0)
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
#define JSO_VALUE_SET_ARRAY(jv, av) \
	do { \
		JSO_TYPE(jv) = JSO_TYPE_ARRAY; \
		JSO_ARRVAL(jv) = (av); \
	} while(0)
#define JSO_VALUE_SET_OBJECT(jv, ov) \
	do { \
		JSO_TYPE(jv) = JSO_TYPE_OBJECT; \
		JSO_OBJVAL(jv) = (ov); \
	} while(0)

/* jso value functions */
JSO_API void jso_value_free(jso_value *val);
JSO_API void jso_value_print(jso_value *val, jso_uint indent);

/* callback function for iterating array */
typedef int (*jso_array_callback)(size_t idx, jso_value *val);

/* error functions */
JSO_API jso_error *jso_error_new_ex(jso_error_type type, jso_location *loc);
JSO_API jso_error *jso_error_new(jso_error_type type,
		size_t first_column, size_t first_line,
		size_t last_column, size_t last_line);
JSO_API void jso_error_free(jso_error *err);

/* array functions */
JSO_API jso_array *jso_array_alloc();
JSO_API void jso_array_free(jso_array *arr);
JSO_API int jso_array_append(jso_array *arr, jso_value *val);
JSO_API int jso_array_push(jso_array *arr, jso_value *val);
JSO_API int jso_array_pop(jso_array *arr);
JSO_API void jso_array_foreach(jso_array *arr, jso_array_callback cbk);
JSO_API void jso_array_print(jso_array *arr, jso_uint indent);

/* callback function for iterating object */
typedef int (*jso_object_callback)(jso_string *key, jso_value *val);

/* object functions */
JSO_API jso_object *jso_object_alloc();
JSO_API void jso_object_free(jso_object *obj);
JSO_API int jso_object_add(jso_object *obj, jso_value *key, jso_value *val);
JSO_API void jso_object_foreach(jso_object *obj, jso_object_callback cbk);
JSO_API void jso_object_print(jso_object *obj, jso_uint indent);

/* memeroy functions - just redefinitions for future extending (checking) */
#define jso_malloc malloc
#define jso_realloc realloc
#define jso_calloc calloc
#define jso_strdup strdup
#define jso_free free

#endif /* JSO_H */
