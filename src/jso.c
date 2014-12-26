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
#include <string.h>

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
			if (JSO_SVAL_P(val) && JSO_SLEN_P(val) > 0) {
				jso_free(JSO_SVAL_P(val));
				JSO_SVAL_P(val) = NULL;
			}
			break;
		case JSO_TYPE_ARRAY:
			jso_array_free(JSO_ARRVAL_P(val));
			break;
		case JSO_TYPE_OBJECT:
			jso_object_free(JSO_OBJVAL_P(val));
			break;
		case JSO_TYPE_ERROR:
			jso_error_free(JSO_EVAL_P(val));
			break;
		default:
			break;
	}
}

/* debug print jso value */
JSO_API void jso_value_print_debug_ex(jso_value *val, jso_uint indent)
{
	jso_print_indent(indent);

	switch (JSO_TYPE_P(val)) {
		case JSO_TYPE_NULL:
			fputs("NULL\n", JSO_PRINT_STREAM);
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
				fprintf(JSO_PRINT_STREAM, "STRING: \"%s\" ; LENGTH: %zu\n", JSO_SVAL_P(val), JSO_SLEN_P(val));
			else
				fputs("EMPTY STRING\n", JSO_PRINT_STREAM);
			break;
		case JSO_TYPE_ARRAY:
			if (!JSO_ARRVAL_P(val)) {
				fprintf(stderr, "ERROR: Array allocation failed\n");
			} else {
				fputs("ARRAY:\n", JSO_PRINT_STREAM);
				jso_array_print(JSO_ARRVAL_P(val), indent + 1);
			}
			break;
		case JSO_TYPE_OBJECT:
			if (!JSO_OBJVAL_P(val)) {
				fprintf(stderr, "ERROR: Object allocation failed\n");
			} else {
				fputs("OBJECT:\n", JSO_PRINT_STREAM);
				jso_object_print(JSO_OBJVAL_P(val), indent + 1);
			}
			break;
		case JSO_TYPE_ERROR:
			{
				if (!JSO_EVAL_P(val)) {
					fprintf(stderr, "ERROR: Error allocation failed\n");
				} else {
					const char *emsg;
					switch (JSO_ETYPE_P(val)) {
						case JSO_ERROR_SYNTAX:
							emsg = "SYNTAX";
							break;
						case JSO_ERROR_DEPTH:
							emsg = "DEPTH";
							break;
						case JSO_ERROR_TOKEN:
							emsg = "TOKEN";
							break;
						case JSO_ERROR_CTRL_CHAR:
							emsg = "CONTROL CHARACTER";
							break;
						case JSO_ERROR_ESCAPE:
							emsg = "ESCAPE";
							break;
						case JSO_ERROR_UTF8:
							emsg = "UTF8:";
							break;
						case JSO_ERROR_UTF16:
							emsg = "UTF16";
							break;
						default:
							emsg = "unknown";
							break;
					}
					fprintf(stderr, "ERROR: %s: %zu:%zu-%zu:%zu\n", emsg,
							JSO_ELOC_P(val).first_line, JSO_ELOC_P(val).first_column,
							JSO_ELOC_P(val).last_line, JSO_ELOC_P(val).last_column);
				}
			}
			break;
		default:
			break;
	}
}

/* debug print jso value */
JSO_API void jso_value_print_debug(jso_value *val)
{
	jso_value_print_debug_ex(val, 0);
}

/* pretty print jso value */
JSO_API void jso_value_print_pretty_ex(jso_value *val, jso_uint indent)
{

}

/* pretty print jso value */
JSO_API void jso_value_print_pretty(jso_value *val)
{
	jso_value_print_pretty_ex(val, 0);
}

/* alloc and init error from type and location struct */
JSO_API jso_error *jso_error_new_ex(jso_error_type type, jso_location *loc)
{
	jso_error *err = jso_malloc(sizeof(jso_error));
	if (!err)
		return NULL;

	err->type = type;
	memcpy(&err->loc, loc, sizeof(jso_location));

	return err;
}

/* alloc and init error from type and location params */
JSO_API jso_error *jso_error_new(jso_error_type type,
		size_t first_column, size_t first_line,
		size_t last_column, size_t last_line)
{
	jso_location loc;
	loc.first_column = first_column;
	loc.first_line = first_line;
	loc.last_column = last_column;
	loc.last_line = last_line;

	return jso_error_new_ex(type, &loc);
}

/* free error */
JSO_API void jso_error_free(jso_error *err)
{
	if (err)
		jso_free(err);
}

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

/* allocate and init object */
JSO_API jso_object *jso_object_alloc()
{
	return jso_calloc(1, sizeof(jso_object));
}

/* free object and its elements */
JSO_API void jso_object_free(jso_object *obj)
{
	if (!obj)
		return;
	jso_object_member *tmp, *member = obj->head;
	while (member) {
		jso_value_free(&member->val);
		jso_free(member->key.val);
		tmp = member->next;
		jso_free(member);
		member = tmp;
	}
	jso_free(obj);
}

/* add new element to the object obj */
JSO_API int jso_object_add(jso_object *obj, jso_value *key, jso_value *val)
{
	jso_object_member *member;
	member = jso_malloc(sizeof (jso_object_member));
	if (!member)
		return JSO_FALSE;
	member->key = key->data.str;
	member->val = *val;
	member->next = NULL;
	if (!obj->head) {
		obj->head = obj->tail = member;
	} else {
		obj->tail->next = member;
		obj->tail = member;
	}
	return JSO_TRUE;
}

/* call cbk function for each element in object */
JSO_API void jso_object_foreach(jso_object *obj, jso_object_callback cbk)
{
	jso_object_member *member = obj->head;
	while (member) {
		cbk(&member->key, &member->val);
		member = member->next;
	}
}

/* print object elements */
JSO_API void jso_object_print(jso_object *obj, jso_uint indent)
{
	jso_object_member *member = obj->head;
	while (member) {
		jso_print_indent(indent);
		fprintf(JSO_PRINT_STREAM, " KEY: %s\n", member->key.val);
		jso_value_print_debug_ex(&member->val, indent + 1);
		member = member->next;
	}
}

