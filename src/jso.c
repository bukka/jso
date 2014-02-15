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

#define JSO_PRINT_STREAM stderr

/* print indentation */
static void jso_print_indent(jso_uint indent)
{
	jso_uint i;
	for (i = 0; i < indent; i++) {
		fprintf(JSO_PRINT_STREAM, " ");
	}
}

/* free jso value */
JSO_API void jso_value_free(jso_value *val)
{
	switch (val->type) {
		case JSO_TYPE_STRING:
			if (JSO_SVAL_P(val) && JSO_SLEN_P(val) > 0)
				jso_free(JSO_SVAL_P(val));
			break;
		case JSO_TYPE_ARRAY:
			jso_array_free(JSO_ARRVAL_P(val));
			jso_free(JSO_ARRVAL_P(val));
			break;
		default:
			break;
	}
}

/* print jso value */
JSO_API void jso_value_print(jso_value *val, jso_uint indent)
{
	jso_print_indent(indent);

	switch (JSO_TYPE_P(val)) {
		case JSO_TYPE_NULL:
			fputs("NULL", JSO_PRINT_STREAM);
			break;
		case JSO_TYPE_BOOL:
			fprintf(JSO_PRINT_STREAM, "BOOL: %s\n", JSO_IVAL_P(val) ? "TRUE" : "FALSE");
			break;
		case JSO_TYPE_INT:
			fprintf(JSO_PRINT_STREAM, "INT: %ld\n", JSO_IVAL_P(val));
			break;
		case JSO_TYPE_DOUBLE:
			fprintf(JSO_PRINT_STREAM, "DOUBLE: %f\n", JSO_DVAL_P(val));
			break;
		case JSO_TYPE_STRING:
			if (JSO_SVAL_P(val))
				printf("STRING: \"%s\" ; LENGTH: %zu\n", JSO_SVAL_P(val), JSO_SLEN_P(val));
			else
				fputs("EMPTY STRING", JSO_PRINT_STREAM);
			break;
		case JSO_TYPE_ARRAY:
			fputs("ARRAY:", JSO_PRINT_STREAM);
			jso_array_print(JSO_ARRVAL_P(val), indent + 1);
		default:
			break;
	}
}

/* alloc and init new array */
JSO_API jso_array *jso_array_alloc()
{
	return jso_calloc(1, sizeof(jso_array));
}

/* free array and its elements */
JSO_API void jso_array_free(jso_array *arr)
{
	jso_array_element *tmp, *el = arr->head;
	while (el) {
		jso_value_free(el->val);
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
	el->val = val;
	el->next = NULL;
	if (!arr->head) {
		arr->head = arr->tail = el;
	} else {
		arr->tail->next = el;
		arr->tail = el;
	}
	return JSO_TRUE;
}

/* prepend element to the array */
JSO_API int jso_array_prepend(jso_array *arr, jso_value *val)
{
	jso_array_element *el = jso_malloc(sizeof (jso_array_element));
	if (!el)
		return JSO_FALSE;
	el->val = val;
	el->next = arr->head;
	arr->head = el;
	if (!arr->tail)
		arr->tail = el;
	return JSO_TRUE;
}

/* call cbk function for each element in array */
JSO_API void jso_array_foreach(jso_array *arr, jso_array_callback cbk)
{
	size_t pos = 0;
	jso_array_element *el = arr->head;
	while (el) {
		cbk(pos++, el->val);
		el = el->next;
	}
}

/* print array elements */
JSO_API void jso_array_print(jso_array *arr, jso_uint indent)
{
	jso_array_element *el = arr->head;
	while (el) {
		jso_value_print(el->val, indent);
		el = el->next;
	}
}
