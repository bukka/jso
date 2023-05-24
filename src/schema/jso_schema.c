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

#include "jso_schema_data.h"
#include "jso_schema_error.h"
#include "jso_schema_value.h"

#include "jso_schema.h"

#include "jso.h"

JSO_API jso_schema *jso_schema_alloc()
{
	return jso_calloc(1, sizeof(jso_schema));
}

JSO_API void jso_schema_init(jso_schema *schema)
{
	memset(schema, 0, sizeof(jso_schema));
}

static jso_rc jso_schema_version_set(jso_schema *schema, jso_value *data)
{
	jso_string *version = jso_schema_data_get_str(schema, data, "$schema");

	if (version == NULL || jso_string_equals_to_cstr(version, "http://json-schema.org/schema#")
			|| jso_string_equals_to_cstr(version, "http://json-schema.org/draft-04/schema#")) {
		schema->version = JSO_SCHEMA_VERSION_DRAFT_04;
		return JSO_SUCCESS;
	}

	return jso_schema_error_set(
			schema, JSO_SCHEMA_ERROR_VERSION, "Only draft 4 is currently supported");
}

JSO_API jso_rc jso_schema_parse(jso_schema *schema, jso_value *data)
{
	if (JSO_TYPE_P(data) != JSO_TYPE_OBJECT) {
		jso_schema_error_set(
				schema, JSO_SCHEMA_ERROR_ROOT_DATA_TYPE, "Root data type is not an object");
		return JSO_FAILURE;
	}

	if (jso_schema_version_set(schema, data) == JSO_FAILURE) {
		return JSO_FAILURE;
	}

	// Parse ID.
	jso_string *id = jso_schema_data_get_str(schema, data, "id");
	if (jso_schema_error_is_set(schema)) {
		return JSO_FAILURE;
	}
	if (id) {
		schema->id = jso_string_copy(id);
	}

	// Parse root value.
	jso_schema_value *root = jso_schema_value_parse(schema, data, NULL);
	if (root == NULL) {
		return JSO_FAILURE;
	}
	schema->root = root;

	return JSO_SUCCESS;
}

static inline void jso_schema_empty(jso_schema *schema)
{
	jso_schema_value_free(schema->root);
	jso_string_free(schema->id);
	jso_schema_error_free(schema);
}

JSO_API void jso_schema_clear(jso_schema *schema)
{
	jso_schema_empty(schema);
	jso_schema_init(schema);
}

JSO_API void jso_schema_free(jso_schema *schema)
{
	if (schema->is_initialized) {
		jso_schema_empty(schema);
	}
	jso_free(schema);
}