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
 * @file jso_types.h
 * @brief Type definitions
 */

#ifndef JSO_TYPES_H
#define JSO_TYPES_H

#include <sys/types.h>

/**
 * @brief Exported library functions
 */
#define JSO_API

/**
 * @brief Boolean type
 */
typedef char  jso_bool;

/**
 * @brief Integer type
 */
typedef long  jso_int;

/**
 * @brief Unsigned integer type
 */
typedef unsigned long jso_uint;

/**
 * @brief Double type
 */
typedef double jso_double;

/**
 * @brief Character type for scanner
 */
typedef unsigned char jso_ctype;

/**
 * String type
 */
typedef struct _jso_string {
	/** string characters */
	jso_ctype *val;
	/** string length */
	size_t len;
} jso_string;

/**
 * @brief Array type
 */
typedef struct _jso_array jso_array;

/**
 * @brief Object type
 */
typedef struct _jso_object jso_object;

/**
 * @brief Value type
 */
typedef enum {
	/** error value type */
	JSO_TYPE_ERROR,
	/** null value type */
	JSO_TYPE_NULL,
	/** boolean value type */
	JSO_TYPE_BOOL,
	/** integer value type */
	JSO_TYPE_INT,
	/** double value type */
	JSO_TYPE_DOUBLE,
	/** string value type */
	JSO_TYPE_STRING,
	/** array value type */
	JSO_TYPE_ARRAY,
	/** object value type */
	JSO_TYPE_OBJECT
} jso_value_type;

/**
 * @brief Error type
 */
typedef enum {
	/** syntax error */
	JSO_ERROR_SYNTAX,
	/** maximal depth exceeded error */
	JSO_ERROR_DEPTH,
	/** invalid token error */
	JSO_ERROR_TOKEN,
	/** control character error */
	JSO_ERROR_CTRL_CHAR,
	/** invalid unicode escape error */
	JSO_ERROR_UTF16,
	/** UTF-8 encoding error */
	JSO_ERROR_UTF8,
	/** invalid escape error */
	JSO_ERROR_ESCAPE
} jso_error_type;

/**
 * @brief Error location
 *
 * It identifies a place where the error was found. The structure
 * is designed for Bison parser.
 */
typedef struct _jso_error_location {
	/** first column of the error */
	size_t first_column;
	/** first line of the error */
	size_t first_line;
	/** last column of the error */
	size_t last_column;
	/** last line of the error */
	size_t last_line;
} jso_error_location;

/**
 * @brief Error information structure
 */
typedef struct _jso_error {
	/** error type */
	jso_error_type type;
	/** error location */
	jso_error_location loc;
} jso_error;

/**
 * @brief Value data
 *
 * The current size of the value data is equal to the largest
 * element which is @ref jso_string with size 128 bits on 64bit
 * platform.
 */
typedef union _jso_value_data {
	/** integer value */
	jso_int ival;
	/** double value */
	jso_double dval;
	/** string value (embedded) */
	jso_string str;
	/** array value (reference) */
	jso_array *arr;
	/** object value (reference) */
	jso_object *obj;
	/** error value (reference) */
	jso_error *err;
} jso_value_data;

/**
 * @brief Value data and type
 */
typedef struct _jso_value {
	/** value data */
	jso_value_data data;
	/** value type */
	jso_value_type type;
} jso_value;

/**
 * @brief Return code
 */
typedef enum _jso_rc {
	/** success return code */
	JSO_SUCCESS = 0,
	/** failure return code */
	JSO_FAILURE = -1
} jso_rc;


/**
 * Boolean TRUE value
 */
#define JSO_TRUE  1

/**
 * Boolean FALSE value
 */
#define JSO_FALSE 0

#endif /* JSO_TYPES_H */
