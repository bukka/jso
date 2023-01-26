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
#include "jso_string.h"

/* FREEING */

/* free jso value */
JSO_API void jso_value_free(jso_value *val)
{
	switch (val->type) {
		case JSO_TYPE_STRING:
			JSO_STRING_CLEAR(JSO_STR_P(val));
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
static void jso_value_print_indent(jso_uint indent, jso_io *io)
{
	jso_uint i;
	for (i = 0; i < indent; i++) {
		JSO_IO_PRINTF(io, " ");
	}
}

/* print error */
static void jso_value_print_error(jso_value *val, jso_io *io)
{
	if (!JSO_EVAL_P(val)) {
		JSO_IO_PRINTF(io, "ERROR: Error allocation failed\n");
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
		JSO_IO_PRINTF(io, "ERROR: %s: %zu:%zu-%zu:%zu\n", emsg,
				JSO_ELOC_P(val).first_line, JSO_ELOC_P(val).first_column,
				JSO_ELOC_P(val).last_line, JSO_ELOC_P(val).last_column);
	}
}

/**
 * Callback argument
 */
typedef struct _jso_value_dump_callback_arg {
	/** output IO handle */
	jso_io *io;
	/** indention */
	jso_uint indent;
} jso_value_dump_callback_arg;

/* debug print array callback */
static int jso_value_dump_array_callback(size_t idx, jso_value *val, void *varg)
{
	jso_value_dump_callback_arg *arg = (jso_value_dump_callback_arg *) varg;

	jso_value_dump_ex(val, arg->io, arg->indent + 1);

	return 0;
}

/* debug print object callback */
static int jso_value_dump_object_callback(jso_string *key, jso_value *val, void *varg)
{
	jso_value_dump_callback_arg *arg = (jso_value_dump_callback_arg *) varg;

	jso_value_print_indent(arg->indent, arg->io);
	JSO_IO_PRINTF(arg->io, " KEY: %s\n", key->val);
	jso_value_dump_ex(val, arg->io, arg->indent + 1);

	return 0;
}

/* debug print jso value */
JSO_API void jso_value_dump_ex(jso_value *val, jso_io *io, jso_uint indent)
{
	jso_value_print_indent(indent, io);

	switch (JSO_TYPE_P(val)) {
		case JSO_TYPE_NULL:
			JSO_IO_PRINTF(io, "NULL\n");
			break;
		case JSO_TYPE_BOOL:
			JSO_IO_PRINTF(io, "BOOL: %s\n", JSO_IVAL_P(val) ? "TRUE" : "FALSE");
			break;
		case JSO_TYPE_INT:
			JSO_IO_PRINTF(io, "INT: %ld\n", JSO_IVAL_P(val));
			break;
		case JSO_TYPE_DOUBLE:
			JSO_IO_PRINTF(io, "DOUBLE: %f\n", JSO_DVAL_P(val));
			break;
		case JSO_TYPE_STRING:
			if (JSO_SVAL_P(val))
				JSO_IO_PRINTF(io, "STRING: \"%s\" ; LENGTH: %zu\n",
						JSO_SVAL_P(val), JSO_SLEN_P(val));
			else
				JSO_IO_PRINTF(io, "EMPTY STRING\n");
			break;
		case JSO_TYPE_ARRAY:
			if (!JSO_ARRVAL_P(val)) {
				fprintf(stderr, "ERROR: Array allocation failed\n");
			} else {
				jso_value_dump_callback_arg arg = { io, indent };

				JSO_IO_PRINTF(io, "ARRAY:\n");
				jso_array_apply_with_arg(JSO_ARRVAL_P(val),
						jso_value_dump_array_callback, &arg);
			}
			break;
		case JSO_TYPE_OBJECT:
			if (!JSO_OBJVAL_P(val)) {
				JSO_IO_PRINTF(io, "ERROR: Object allocation failed\n");
			} else {
				jso_value_dump_callback_arg arg = { io, indent };

				JSO_IO_PRINTF(io, "OBJECT:\n");
				jso_object_apply_with_arg(JSO_OBJVAL_P(val),
						jso_value_dump_object_callback, &arg);
			}
			break;
		case JSO_TYPE_ERROR:
			jso_value_print_error(val, io);
			break;
		default:
			break;
	}
}

/* debug print jso value */
JSO_API void jso_value_dump(jso_value *val, jso_io *io)
{
	jso_value_dump_ex(val, io, 0);
}
