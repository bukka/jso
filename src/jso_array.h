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

#ifndef JSO_ARRAY_H
#define JSO_ARRAY_H

#include "jso_types.h"

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

/* callback function for iterating array */
typedef int (*jso_array_callback)(size_t idx, jso_value *val);
typedef int (*jso_array_with_arg_callback)(size_t idx, jso_value *val, void *arg);

/* array functions */
JSO_API jso_array *jso_array_alloc();
JSO_API void jso_array_free(jso_array *arr);
JSO_API int jso_array_append(jso_array *arr, jso_value *val);
JSO_API int jso_array_push(jso_array *arr, jso_value *val);
JSO_API int jso_array_pop(jso_array *arr);
JSO_API void jso_array_apply(jso_array *arr, jso_array_callback cbk);
JSO_API void jso_array_apply_with_arg(jso_array *arr, jso_array_with_arg_callback cbk, void *arg);
JSO_API void jso_array_print(jso_array *arr, jso_uint indent);

#define JSO_ARRAY_FOREACH(_arr, _val) \
	do { \
		struct _jso_array_element *_array_el; \
		for (_array_el = _arr->head; _array_el; _array_el = _array_el->next) { \
			_val = &_array_el->val;

#define JSO_ARRAY_FOREACH_END } } while(0)

#endif /* JSO_ARRAY_H */
