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

#include "jso_schema_array.h"
#include "jso_schema_data.h"
#include "jso_schema_error.h"
#include "jso_schema_keyword.h"
#include "jso_schema_keyword_array.h"
#include "jso_schema_keyword_object.h"
#include "jso_schema_keyword_regexp.h"
#include "jso_schema_keyword_scalar.h"
#include "jso_schema_keyword_types.h"
#include "jso_schema_value.h"

#include "jso.h"

#include <math.h>

typedef jso_schema_keyword *(*jso_schema_keyword_get_callback)(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent);

static const jso_schema_keyword_get_callback schema_keyword_get_callbacks[] = {
	[JSO_SCHEMA_KEYWORD_TYPE_ANY] = jso_schema_keyword_get_any,
	[JSO_SCHEMA_KEYWORD_TYPE_NULL] = jso_schema_keyword_get_null,
	[JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN] = jso_schema_keyword_get_bool,
	[JSO_SCHEMA_KEYWORD_TYPE_INTEGER] = jso_schema_keyword_get_int,
	[JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER] = jso_schema_keyword_get_uint,
	[JSO_SCHEMA_KEYWORD_TYPE_NUMBER] = jso_schema_keyword_get_number,
	[JSO_SCHEMA_KEYWORD_TYPE_STRING] = jso_schema_keyword_get_string,
	[JSO_SCHEMA_KEYWORD_TYPE_REGEXP] = jso_schema_keyword_get_regexp,
	[JSO_SCHEMA_KEYWORD_TYPE_ARRAY] = jso_schema_keyword_get_array,
	[JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_STRINGS] = jso_schema_keyword_get_array_of_strings,
	[JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS]
	= jso_schema_keyword_get_array_of_schema_objects,
	[JSO_SCHEMA_KEYWORD_TYPE_OBJECT] = jso_schema_keyword_get_object,
	[JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT] = jso_schema_keyword_get_schema_object,
	[JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS]
	= jso_schema_keyword_get_object_of_schema_objects,
	[JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS_OR_ARRAY_OF_STRINGS]
	= jso_schema_keyword_get_object_of_schema_objects_or_array_of_strings,
	[JSO_SCHEMA_KEYWORD_TYPE_REGEXP_OBJECT_OF_SCHEMA_OBJECTS]
	= jso_schema_keyword_get_regexp_object_of_schema_objects,
};

static inline jso_schema_keyword *jso_schema_keyword_get_ex(jso_schema *schema, jso_value *data,
		const char *key, jso_schema_value *parent, jso_schema_keyword_type keyword_type,
		jso_uint32 keyword_flags, jso_bool error_on_invalid_type,
		jso_schema_keyword *schema_keyword, jso_value *val)
{
	JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword) = keyword_type;
	return schema_keyword_get_callbacks[keyword_type](
			schema, data, key, error_on_invalid_type, keyword_flags, schema_keyword, val, parent);
}

static inline jso_schema_keyword *jso_schema_keyword_get(jso_schema *schema, jso_value *data,
		const char *key, jso_schema_value *parent, jso_schema_keyword_type keyword_type,
		jso_uint32 keyword_flags, jso_bool error_on_invalid_type,
		jso_schema_keyword *schema_keyword)
{
	return jso_schema_keyword_get_ex(schema, data, key, parent, keyword_type, keyword_flags,
			error_on_invalid_type, schema_keyword, NULL);
}

#define JSO_SCHEMA_KW_VALUE_TYPES_SIZE 8

static inline jso_schema_keyword *jso_schema_keyword_get_union_of_2_types(jso_schema *schema,
		jso_value *data, const char *key, jso_schema_value *parent,
		jso_schema_keyword_type keyword_union_type_1, jso_schema_keyword_type keyword_union_type_2,
		jso_uint32 keyword_flags, jso_schema_keyword *schema_keyword)
{
	jso_value *val = jso_schema_data_get_value_fast(schema, data, key, keyword_flags);
	if (val == NULL) {
		return NULL;
	}

	// Try first type.
	jso_schema_keyword *schema_keyword_result = jso_schema_keyword_get_ex(
			schema, data, key, parent, keyword_union_type_1, 0, false, schema_keyword, val);
	if (schema_keyword_result != NULL || jso_schema_error_is_set(schema)) {
		return schema_keyword_result;
	}

	// Try second type.
	schema_keyword_result = jso_schema_keyword_get_ex(schema, data, key, parent,
			keyword_union_type_2, keyword_flags, false, schema_keyword, val);
	if (schema_keyword_result != NULL || jso_schema_error_is_set(schema)) {
		return schema_keyword_result;
	}

	// Trigger type error if types do not match.
	jso_value_type value_types[JSO_SCHEMA_KW_VALUE_TYPES_SIZE];
	size_t types_size = jso_schema_keyword_get_types(
			schema, key, keyword_union_type_1, value_types, JSO_SCHEMA_KW_VALUE_TYPES_SIZE);
	types_size += jso_schema_keyword_get_types(schema, key, keyword_union_type_2,
			&value_types[types_size], JSO_SCHEMA_KW_VALUE_TYPES_SIZE - types_size);
	jso_schema_data_type_error(schema, key, val, value_types, types_size);

	return NULL;
}

static inline jso_rc jso_schema_keyword_check(jso_schema *schema, jso_schema_keyword *keyword)
{
	return keyword == NULL || jso_schema_error_is_set(schema) ? JSO_FAILURE : JSO_SUCCESS;
}

/* Set union keyword. */
jso_rc jso_schema_keyword_set_union_of_2_types(jso_schema *schema, jso_value *data, const char *key,
		jso_schema_value *value, jso_schema_keyword *schema_keyword,
		jso_schema_keyword_type keyword_union_type_1, jso_schema_keyword_type keyword_union_type_2,
		jso_uint32 keyword_flags)
{
	jso_schema_keyword *schema_keyword_ptr = jso_schema_keyword_get_union_of_2_types(schema, data,
			key, value, keyword_union_type_1, keyword_union_type_2, keyword_flags, schema_keyword);

	return jso_schema_keyword_check(schema, schema_keyword_ptr);
}

/* Set keyword. */
jso_rc jso_schema_keyword_set(jso_schema *schema, jso_value *data, const char *key,
		jso_schema_value *value, jso_schema_keyword *schema_keyword,
		jso_schema_keyword_type keyword_type, jso_uint32 keyword_flags)
{
	jso_schema_keyword *schema_keyword_ptr = jso_schema_keyword_get(
			schema, data, key, value, keyword_type, keyword_flags, true, schema_keyword);

	return jso_schema_keyword_check(schema, schema_keyword_ptr);
}

typedef void (*jso_schema_keyword_free_callback)(jso_schema_keyword *schema_keyword);

static const jso_schema_keyword_free_callback schema_keyword_free_callbacks[] = {
	[JSO_SCHEMA_KEYWORD_TYPE_ANY] = jso_schema_keyword_free_any,
	[JSO_SCHEMA_KEYWORD_TYPE_STRING] = jso_schema_keyword_free_string,
	[JSO_SCHEMA_KEYWORD_TYPE_REGEXP] = jso_schema_keyword_free_regexp,
	[JSO_SCHEMA_KEYWORD_TYPE_ARRAY] = jso_schema_keyword_free_array,
	[JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_STRINGS] = jso_schema_keyword_free_array_of_strings,
	[JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS]
	= jso_schema_keyword_free_array_of_schema_objects,
	[JSO_SCHEMA_KEYWORD_TYPE_OBJECT] = jso_schema_keyword_free_object,
	[JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT] = jso_schema_keyword_free_schema_object,
	[JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS]
	= jso_schema_keyword_free_object_of_schema_objects,
	[JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS_OR_ARRAY_OF_STRINGS]
	= jso_schema_keyword_free_object_of_schema_objects,
	[JSO_SCHEMA_KEYWORD_TYPE_REGEXP_OBJECT_OF_SCHEMA_OBJECTS]
	= jso_schema_keyword_free_object_of_schema_objects,
};

void jso_schema_keyword_free(jso_schema_keyword *keyword)
{
	if (!JSO_SCHEMA_KW_IS_SET_P(keyword)) {
		return;
	}

	jso_schema_keyword_free_callback callback
			= schema_keyword_free_callbacks[JSO_SCHEMA_KEYWORD_TYPE_P(keyword)];
	if (callback) {
		callback(keyword);
	}
	JSO_SCHEMA_KEYWORD_FLAGS_P(keyword) = 0;
}
