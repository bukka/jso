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

#include "jso_schema_error.h"

#include "jso.h"

#include <stdarg.h>
#include <stdio.h>

jso_rc jso_schema_error_set(
		jso_schema *schema, jso_schema_error_type type, const char *message)
{
	size_t message_len = strlen(message);
	char *new_message = jso_malloc(message_len);
	if (new_message == NULL) {
		return JSO_FAILURE;
	}
	memcpy(new_message, message, message_len);

	JSO_SCHEMA_ERROR_MESSAGE(schema) = new_message;
	JSO_SCHEMA_ERROR_TYPE(schema) = type;

	return JSO_SUCCESS;
}

jso_rc jso_schema_error_format(
		jso_schema *schema, jso_schema_error_type type, const char *format, ...)
{
	va_list args;
	char buf[JSO_SCHEMA_ERROR_FORMAT_SIZE + 1];

	va_start(args, format);
	int written = vsnprintf(buf, JSO_SCHEMA_ERROR_FORMAT_SIZE, format, args);
	if (written < 0) {
		return JSO_FAILURE;
	}
	va_end(args);

	if (written == JSO_SCHEMA_ERROR_FORMAT_SIZE) {
		buf[JSO_SCHEMA_ERROR_FORMAT_SIZE] = '\0';
	}

	return jso_schema_error_set(schema, type, buf);
}