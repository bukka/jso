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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "jso.h"
#include "jso_io.h"
#include "jso_encoder.h"

static int jso_encoder_output(jso_encoder *encoder, const char *str, size_t len)
{
	return JSO_IO_WRITE(encoder->output, (jso_ctype *) str, len);
}

static int jso_encoder_encode_int(jso_encoder *encoder, jso_int val)
{
	return 0;
}

static int jso_encoder_encode_double(jso_encoder *encoder, jso_double val)
{
	return 0;
}

static int jso_encoder_encode_string(jso_encoder *encoder, jso_string *str)
{
	return 0;
}

static int jso_encoder_encode_array(jso_encoder *encoder, jso_array *arr)
{
	jso_value *val;
	jso_bool is_first = JSO_TRUE;

	JSO_ARRAY_FOREACH(arr, val) {
		if (is_first) {
			is_first = JSO_FALSE;
		} else {
			jso_encoder_output(encoder, ",", 1);
		}
		jso_encoder_encode(encoder, val);
	} JSO_ARRAY_FOREACH_END;

	return 0;
}

static int jso_encoder_encode_object(jso_encoder *encoder, jso_object *obj)
{
	jso_string *key;
	jso_value *val;
	jso_bool is_first = JSO_TRUE;

	JSO_OBJECT_FOREACH(obj, key, val) {
		if (is_first) {
			is_first = JSO_FALSE;
		} else {
			jso_encoder_output(encoder, ",", 1);
		}
		jso_encoder_encode_string(encoder, key);
		jso_encoder_output(encoder, ":", 1);
		jso_encoder_encode(encoder, val);
	} JSO_OBJECT_FOREACH_END;

	return 0;
}


JSO_API void jso_encoder_init(jso_encoder *encoder, jso_io *output, jso_encoder_options *options)
{
	encoder->output = output;
	memcpy(&encoder->options, options, sizeof(jso_encoder_options));
}

JSO_API int jso_encoder_encode(jso_encoder *encoder, jso_value *val)
{
	switch (JSO_TYPE_P(val)) {
		case JSO_TYPE_NULL:
			return jso_encoder_output(encoder, "null", sizeof("null") - 1);
		case JSO_TYPE_BOOL:
			if (JSO_IVAL_P(val)) {
				return jso_encoder_output(encoder, "true", sizeof("true") - 1);
			}
			return jso_encoder_output(encoder, "false", sizeof("false") - 1);
		case JSO_TYPE_INT:
			return jso_encoder_encode_int(encoder, JSO_IVAL_P(val));
		case JSO_TYPE_DOUBLE:
			return jso_encoder_encode_double(encoder, JSO_DVAL_P(val));
		case JSO_TYPE_STRING:
			return jso_encoder_encode_string(encoder, &val->data.str);
		case JSO_TYPE_ARRAY:
			return jso_encoder_encode_array(encoder, JSO_ARRVAL_P(val));
		case JSO_TYPE_OBJECT:
			return jso_encoder_encode_object(encoder, JSO_OBJVAL_P(val));
		default:
			/* error */
			return -1;
	}
}

JSO_API int jso_encode(jso_value *val, jso_io *output, jso_encoder_options *options)
{
	jso_encoder encoder;

	jso_encoder_init(&encoder, output, options);
	return jso_encoder_encode(&encoder, val);
}
