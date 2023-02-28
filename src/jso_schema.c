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
#include "jso_schema.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static jso_rc jso_schema_error_set(
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

static inline bool jso_schema_error_is_set(jso_schema *schema)
{
	return JSO_SCHEMA_ERROR_TYPE(schema) != JSO_SCHEMA_ERROR_NONE;
}

#define JSO_SCHEMA_ERROR_FORMAT_SIZE 512

static jso_rc jso_schema_error_format(
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

static jso_value *jso_schema_data_get(
		jso_schema *schema, jso_value *data, const char *key, jso_value_type type)
{
	jso_value *val;
	jso_string str_key;

	JSO_STRING_ASSIGN_STATIC_CSTR(str_key, key);

	if (jso_ht_get(JSO_OBJHVAL_P(data), &str_key, &val) == JSO_FAILURE) {
		return NULL;
	}

	if ((JSO_TYPE_P(val) & type) == 0) {
		jso_schema_error_format(
				schema, JSO_SCHEMA_ERROR_VALUE_DATA_TYPE, "Invalid type for %s", key);
		return NULL;
	}

	return val;
}

static inline jso_string *jso_schema_data_get_str(
		jso_schema *schema, jso_value *data, const char *key)
{
	jso_value *val = jso_schema_data_get(schema, data, key, JSO_TYPE_STRING);

	return val == NULL ? NULL : &JSO_STR_P(val);
}

static inline jso_bool jso_schema_data_get_bool(
		jso_schema *schema, jso_value *data, const char *key)
{
	jso_value *val = jso_schema_data_get(schema, data, key, JSO_TYPE_BOOL);

	return val == NULL ? false : (jso_bool) JSO_IVAL_P(val);
}

static inline jso_int jso_schema_data_get_int(jso_schema *schema, jso_value *data, const char *key)
{
	jso_value *val = jso_schema_data_get(schema, data, key, JSO_TYPE_INT | JSO_TYPE_DOUBLE);

	if (val == NULL) {
		return 0;
	}

	if (JSO_TYPE_P(val) == JSO_TYPE_INT) {
		return JSO_IVAL_P(val);
	}

	return (jso_int) JSO_DVAL_P(val);
}

JSO_API jso_schema *jso_schema_alloc()
{
	return jso_calloc(1, sizeof(jso_schema));
}

JSO_API void jso_schema_init(jso_schema *schema)
{
	memset(schema, 0, sizeof(jso_schema));
}

static jso_rc jso_schema_parse_value(jso_schema *schema, jso_value *data, jso_value *parent)
{
	return JSO_SUCCESS;
}

static jso_rc jso_schema_version_set(jso_schema *schema, jso_value *data)
{
	jso_string *version = jso_schema_data_get_str(schema, data, "$schema");

	if (version == NULL || JSO_STRING_EQUALS_CSTR_P(version, "http://json-schema.org/schema#")
			|| JSO_STRING_EQUALS_CSTR_P(version, "http://json-schema.org/draft-04/schema#")) {
		schema->version = JSO_SCHEMA_VERSION_DRAFT_04;
		return JSO_SUCCESS;
	}

	return jso_schema_error_set(
			schema, JSO_SCHEMA_ERROR_VERSION, "Only draft 4 is currently supported");
}

JSO_API jso_rc jso_schema_parse(jso_schema *schema, jso_value *data)
{
	if (JSO_TYPE_P(data) != JSO_TYPE_OBJECT) {
		return jso_schema_error_set(
				schema, JSO_SCHEMA_ERROR_ROOT_DATA_TYPE, "Root data type is not an object");
	}

	if (jso_schema_version_set(schema, data) == JSO_FAILURE) {
		return JSO_FAILURE;
	}

	jso_string *id = jso_schema_data_get_str(schema, data, "id");
	if (id) {
		JSO_STRING_COPY(*id, schema->id);
	}

	return jso_schema_parse_value(schema, data, NULL);
}

JSO_API void jso_schema_clear(jso_schema *schema)
{
}

JSO_API void jso_schema_free(jso_schema *schema)
{
	jso_schema_clear(schema);
	jso_free(schema);
}
