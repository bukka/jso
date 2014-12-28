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

#ifndef JSO_TYPES_H
#define JSO_TYPES_H

#include <sys/types.h>

/* exported library functions */
#define JSO_API

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

/* string type */
typedef struct _jso_string {
	jso_ctype *val;
	size_t len;
} jso_string;

/* type definition of the array type */
typedef struct _jso_array jso_array;

/* type definition of the object type */
typedef struct _jso_object jso_object;

/* type definition of the jso value */
typedef struct _jso_value jso_value;

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

#endif /* JSO_TYPES_H */
