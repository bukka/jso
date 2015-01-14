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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jso.h"

/* ERROR */

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

/* FREEING */

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

/* PRINTING */

/* print indentation */
static void jso_value_print_indent(jso_uint indent)
{
	jso_uint i;
	for (i = 0; i < indent; i++) {
		fprintf(JSO_PRINT_STREAM, " ");
	}
}

/* print error */
static void jso_value_print_error(jso_value *val)
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

/* debug print array callback */
static int jso_value_print_debug_array_callback(size_t idx, jso_value *val, void *arg)
{
	jso_value_print_debug_ex(val, (jso_uint) arg);

	return 0;
}

/* debug print object callback */
static int jso_value_print_debug_object_callback(jso_string *key, jso_value *val, void *arg)
{
	jso_uint indent = (jso_uint) arg;

	jso_value_print_indent(indent);
	fprintf(JSO_PRINT_STREAM, " KEY: %s\n", key->val);
	jso_value_print_debug_ex(val, indent + 1);

	return 0;
}

/* debug print jso value */
JSO_API void jso_value_print_debug_ex(jso_value *val, jso_uint indent)
{
	jso_value_print_indent(indent);

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
				fprintf(JSO_PRINT_STREAM, "STRING: \"%s\" ; LENGTH: %zu\n",
						JSO_SVAL_P(val), JSO_SLEN_P(val));
			else
				fputs("EMPTY STRING\n", JSO_PRINT_STREAM);
			break;
		case JSO_TYPE_ARRAY:
			if (!JSO_ARRVAL_P(val)) {
				fprintf(stderr, "ERROR: Array allocation failed\n");
			} else {
				fputs("ARRAY:\n", JSO_PRINT_STREAM);
				jso_array_apply_with_arg(JSO_ARRVAL_P(val),
						jso_value_print_debug_array_callback, (void *) (indent + 1));
			}
			break;
		case JSO_TYPE_OBJECT:
			if (!JSO_OBJVAL_P(val)) {
				fprintf(stderr, "ERROR: Object allocation failed\n");
			} else {
				fputs("OBJECT:\n", JSO_PRINT_STREAM);
				jso_object_apply_with_arg(JSO_OBJVAL_P(val),
						jso_value_print_debug_object_callback, (void *) (indent + 1));
			}
			break;
		case JSO_TYPE_ERROR:
			jso_value_print_error(val);
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
