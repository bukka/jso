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

#include <stdio.h>
#include <stdlib.h>
#include "jso.h"

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

/* alloc and init new array */
JSO_API jso_array *jso_array_alloc()
{
	return jso_calloc(1, sizeof(jso_array));
}

/* free array and its elements */
JSO_API void jso_array_free(jso_array *arr)
{
	if (!arr)
		return;
	jso_array_element *tmp, *el = arr->head;
	while (el) {
		jso_value_free(&el->val);
		tmp = el->next;
		jso_free(el);
		el = tmp;
	}
	jso_free(arr);
}

/* append element to the array */
JSO_API int jso_array_append(jso_array *arr, jso_value *val)
{
	jso_array_element *el = jso_malloc(sizeof (jso_array_element));
	if (!el)
		return JSO_FALSE;
	el->val = *val;
	el->next = NULL;
	if (!arr->head) {
		arr->head = arr->tail = el;
	} else {
		arr->tail->next = el;
		arr->tail = el;
	}
	return JSO_TRUE;
}

/* push element to the array */
JSO_API int jso_array_push(jso_array *arr, jso_value *val)
{
	jso_array_element *el = jso_malloc(sizeof (jso_array_element));
	if (!el)
		return JSO_FALSE;
	el->val = *val;
	el->next = arr->head;
	arr->head = el;
	if (!arr->tail)
		arr->tail = el;
	return JSO_TRUE;
}

/* pop the head element from the array */
JSO_API int jso_array_pop(jso_array *arr)
{
	jso_array_element *el = arr->head;
	if (!el)
		return JSO_FALSE;
	if (el == arr->tail)
		arr->head = arr->tail = NULL;
	else
		arr->head = arr->head->next;
	jso_free(el);
	return JSO_TRUE;
}

/* call cbk function for each element in array */
JSO_API void jso_array_foreach(jso_array *arr, jso_array_callback cbk)
{
	size_t pos = 0;
	jso_array_element *el = arr->head;
	while (el) {
		cbk(pos++, &el->val);
		el = el->next;
	}
}

/* print array elements */
JSO_API void jso_array_print(jso_array *arr, jso_uint indent)
{
	jso_array_element *el = arr->head;
	while (el) {
		jso_value_print_debug_ex(&el->val, indent);
		el = el->next;
	}
}
