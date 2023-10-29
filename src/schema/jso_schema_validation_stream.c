/*
 * Copyright (c) 2023 Jakub Zelenka. All rights reserved.
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

#include "jso.h"

JSO_API jso_rc jso_schema_validation_stream_init(
		jso_schema *schema, jso_schema_validation_stream *stream)
{

	return JSO_SUCCESS;
}

JSO_API void jso_schema_validation_stream_clear(jso_schema_validation_stream *stream)
{
}

JSO_API jso_rc jso_schema_validation_stream_object_start(jso_schema_validation_stream *context)
{

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_object_key(
		jso_schema_validation_stream *context, jso_value *str)
{

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_array_start(jso_schema_validation_stream *context)
{

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_value(
		jso_schema_validation_stream *context, jso_value *value)
{

	return JSO_SUCCESS;
}