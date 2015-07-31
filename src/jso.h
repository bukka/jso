/*
 * Copyright (c) 2012-2015 Jakub Zelenka. All rights reserved.
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

/**
 * @file jso.h
 * @brief Main API header
 */

#ifndef JSO_H
#define JSO_H

#include <config.h>
#include "jso_types.h"
#include "jso_array.h"
#include "jso_object.h"

/**
 * Get max of two values
 * @param value_1 first value
 * @param value_2 second value
 * @return `value_1` if `value_1` > `value_2`, otherwise `value_2`
 */
#define JSO_MAX(value_1, value_2) \
	((value_1) > (value_2) ? (value_1) : (value_2))

/**
 * Get max of two values
 * @param value_1 first value
 * @param value_2 second value
 * @return `value_1` if `value_1` < `value_2`, otherwise `value_2`
 */
#define JSO_MIN(value_1, value_2) \
	((value_1) < (value_2) ? (value_1) : (value_2))

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


/**
 * Suppresses warnings about unused variable
 * @param unused_variable the variable that is not used
 */
#define JSO_USE(unused_variable) ((void) (unused_variable))

/**
 * Suppresses warnings about unused variable and 1 extra parameter
 * @param uv0 the variable that is not used
 * @param uv1 the parameter that is not used
 */
#define JSO_USE_1(uv0, uv1) JSO_USE(uv0); JSO_USE(uv1)

/**
 * Suppresses warnings about unused variable and 2 extra parameters
 * @param uv0 the variable that is not used
 * @param uv1 the first parameter that is not used
 * @param uv1 the second parameter that is not used
 */
#define JSO_USE_2(uv0, uv1, uv2) JSO_USE(uv0); JSO_USE(uv1); JSO_USE(uv2)


/**
 * Get string length for string pointer
 * @param _str pointer to string
 * @return The string length
 */
#define JSO_STRING_LEN_P(_str) (_str)->len

/**
 * Get string value for string pointer
 * @param _str pointer to string
 * @return The string value (char array)
 */
#define JSO_STRING_VAL_P(_str) (_str)->val

/**
 * Get string length
 * @param _str string
 * @return The string length
 */
#define JSO_STRING_LEN(_str)   (_str).len

/**
 * Get string value
 * @param _str string
 * @return The string value (char array)
 */
#define JSO_STRING_VAL(_str)   (_str).val


/* accessors for pointer to jso value */
#define JSO_TYPE_P(pjv) (pjv)->type
#define JSO_IVAL_P(pjv) (pjv)->data.ival
#define JSO_DVAL_P(pjv) (pjv)->data.dval
#define JSO_STR_P(pjv)  (pjv)->data.str
#define JSO_SVAL_P(pjv) JSO_STR_P(pjv).val
#define JSO_SLEN_P(pjv) JSO_STR_P(pjv).len
#define JSO_ARRVAL_P(pjv) (pjv)->data.arr
#define JSO_OBJVAL_P(pjv) (pjv)->data.obj
#define JSO_EVAL_P(pjv) (pjv)->data.err
#define JSO_ELOC_P(pjv) (pjv)->data.err->loc
#define JSO_ETYPE_P(pjv) (pjv)->data.err->type

/* accessors for jso value */
#define JSO_TYPE(jv) JSO_TYPE_P(&(jv))
#define JSO_IVAL(jv) JSO_IVAL_P(&(jv))
#define JSO_DVAL(jv) JSO_DVAL_P(&(jv))
#define JSO_STR(jv)  JSO_STR_P(&(jv))
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

/* jso value free function */
JSO_API void jso_value_free(jso_value *val);
/* jso value print function */
JSO_API void jso_value_print_debug_ex(jso_value *val, jso_uint indent);
JSO_API void jso_value_print_debug(jso_value *val);
JSO_API void jso_value_print_pretty_ex(jso_value *val, jso_uint indent);
JSO_API void jso_value_print_pretty(jso_value *val);

/* error functions */
JSO_API jso_error *jso_error_new_ex(jso_error_type type, jso_location *loc);
JSO_API jso_error *jso_error_new(jso_error_type type,
		size_t first_column, size_t first_line,
		size_t last_column, size_t last_line);
JSO_API void jso_error_free(jso_error *err);


/* memeroy functions - just redefinitions for future extending (checking) */

/**
 * @brief Memory allocation
 */
#define jso_malloc malloc

/**
 * @brief Memory reallocation
 */
#define jso_realloc realloc

/**
 * @brief Memory allocation with zeroing all bytes
 */
#define jso_calloc calloc

/**
 * @brief String duplication
 */
#define jso_strdup strdup

/**
 * @brief Memory freeing
 */
#define jso_free free

/* print stream */
#define JSO_PRINT_STREAM stderr

#endif /* JSO_H */
