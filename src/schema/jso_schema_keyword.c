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
#include "jso_schema_value.h"

#include "jso.h"

#include <math.h>

static jso_schema_keyword *jso_schema_keyword_get_null(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	if (jso_schema_data_get(
				schema, data, key, JSO_TYPE_NULL, keyword_flags, error_on_invalid_type, val)
			!= NULL) {
		schema_keyword->flags = keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
		return schema_keyword;
	}

	return NULL;
}

static jso_schema_keyword *jso_schema_keyword_get_bool(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get(
			schema, data, key, JSO_TYPE_BOOL, keyword_flags, error_on_invalid_type, val);
	if (val != NULL) {
		schema_keyword->flags = keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
		schema_keyword->data.bval = JSO_IVAL_P(val) ? true : false;
		return schema_keyword;
	}
	return NULL;
}

static jso_schema_keyword *jso_schema_keyword_get_int(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get_value(schema, data, key, keyword_flags, val);
	if (val == NULL) {
		return NULL;
	}

	if (jso_schema_data_check_type(schema, data, key, val, JSO_TYPE_INT, JSO_TYPE_INT, false)
			== JSO_SUCCESS) {
		schema_keyword->flags = keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
		schema_keyword->data.ival = JSO_IVAL_P(val);
		return schema_keyword;
	}
	if (jso_schema_data_check_type(
				schema, data, key, val, JSO_TYPE_DOUBLE, JSO_TYPE_INT, error_on_invalid_type)
			== JSO_FAILURE) {
		return NULL;
	}

	double dval = JSO_DVAL_P(val);
	if (nearbyint(dval) == dval) {
		schema_keyword->flags = keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
		schema_keyword->data.ival = (jso_int) dval;
		return schema_keyword;
	}

	jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_DATA_TYPE,
			"Integer fraction is not zero for key %s", key);

	return NULL;
}

static jso_schema_keyword *jso_schema_keyword_get_uint(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	jso_schema_keyword *schema_keyword_int = jso_schema_keyword_get_int(
			schema, data, key, keyword_flags, error_on_invalid_type, schema_keyword, val, parent);
	if (schema_keyword_int == NULL) {
		return NULL;
	}

	jso_int ival = schema_keyword_int->data.ival;

	if (keyword_flags & JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO) {
		if (ival <= 0) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_DATA_TYPE,
					"Value for for keyword %s must be greater than 0", key);
			return NULL;
		}
	} else if (ival < 0) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_DATA_TYPE,
				"Value for for keyword %s must be equal or greater than 0", key);
		return NULL;
	}
	schema_keyword->data.uval = (jso_uint) ival;

	return schema_keyword;
}

static jso_schema_keyword *jso_schema_keyword_get_number(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get_value(schema, data, key, keyword_flags, val);
	if (val == NULL) {
		return NULL;
	}

	if (jso_schema_data_check_type(schema, data, key, val, JSO_TYPE_INT, JSO_TYPE_INT, false)
			== JSO_SUCCESS) {
		schema_keyword->flags = keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
		schema_keyword->data.ival = JSO_IVAL_P(val);
		return schema_keyword;
	}
	if (jso_schema_data_check_type(
				schema, data, key, val, JSO_TYPE_DOUBLE, JSO_TYPE_INT, error_on_invalid_type)
			== JSO_SUCCESS) {
		schema_keyword->flags = keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT
				| JSO_SCHEMA_KEYWORD_FLAG_FLOATING;
		schema_keyword->data.dval = JSO_DVAL_P(val);
		return schema_keyword;
	}

	return NULL;
}

static jso_schema_keyword *jso_schema_keyword_get_string(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get(
			schema, data, key, JSO_TYPE_STRING, keyword_flags, error_on_invalid_type, val);
	if (val != NULL) {
		schema_keyword->flags = keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
		schema_keyword->data.sval = JSO_STR_P(val);
		return schema_keyword;
	}
	return NULL;
}

static inline jso_rc jso_schema_data_check_keyword_array(
		jso_schema *schema, const char *key, jso_array *arr, jso_uint32 keyword_flags)
{
	if (keyword_flags & JSO_SCHEMA_KEYWORD_FLAG_NOT_EMPTY && JSO_ARRAY_LEN(arr) == 0) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_DATA_DEPS,
				"Array value for keyword %s must not be empty", key);
		return JSO_FAILURE;
	}
	return JSO_SUCCESS;
}

static jso_schema_keyword *jso_schema_keyword_get_array(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get(
			schema, data, key, JSO_TYPE_ARRAY, keyword_flags, error_on_invalid_type, val);
	if (val == NULL) {
		return NULL;
	}
	jso_array *arr = JSO_ARRVAL_P(val);
	if (jso_schema_data_check_keyword_array(schema, key, arr, keyword_flags == JSO_FAILURE)) {
		return NULL;
	}
	schema_keyword->flags = keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	schema_keyword->data.aval = jso_array_copy(arr);
	return schema_keyword;
}

static jso_schema_keyword *jso_schema_keyword_get_array_of_strings(jso_schema *schema,
		jso_value *data, const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get(
			schema, data, key, JSO_TYPE_ARRAY, keyword_flags, error_on_invalid_type, val);
	if (val == NULL) {
		return NULL;
	}
	jso_array *arr = JSO_ARRVAL_P(val);
	if (jso_schema_data_check_keyword_array(schema, key, arr, keyword_flags == JSO_FAILURE)) {
		return NULL;
	}
	jso_value *item;
	JSO_ARRAY_FOREACH(arr, item)
	{
		if (JSO_TYPE_P(item) != JSO_TYPE_STRING) {
			return NULL;
		}
	}
	JSO_ARRAY_FOREACH_END;
	schema_keyword->flags = keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	schema_keyword->data.asval = jso_array_copy(arr);
	return schema_keyword;
}

static jso_schema_keyword *jso_schema_keyword_get_schema_object(jso_schema *schema,
		jso_value *data, const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get(
			schema, data, key, JSO_TYPE_OBJECT, keyword_flags, error_on_invalid_type, val);
	if (val != NULL) {
		jso_schema_value *schema_value = jso_schema_value_parse(schema, val, parent);
		if (schema_value != NULL) {
			schema_keyword->data.soval = schema_value;
			return schema_keyword;
		}
	}

	return NULL;
}

static jso_schema_keyword *jso_schema_keyword_get_array_of_schema_objects(jso_schema *schema,
		jso_value *data, const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get(
			schema, data, key, JSO_TYPE_ARRAY, keyword_flags, error_on_invalid_type, val);
	if (val == NULL) {
		return NULL;
	}
	jso_value *item;
	jso_array *arr = JSO_ARRVAL_P(val);
	if (jso_schema_data_check_keyword_array(schema, key, arr, keyword_flags == JSO_FAILURE)) {
		return NULL;
	}
	jso_schema_array *schema_arr = jso_schema_array_alloc(JSO_ARRAY_LEN(arr));
	if (schema_arr == NULL) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_ALLOC,
				"Allocating schema array of values for keyword %s failed", key);
		return NULL;
	}
	JSO_ARRAY_FOREACH(arr, item)
	{
		if (JSO_TYPE_P(item) != JSO_TYPE_OBJECT) {
			jso_schema_array_free(schema_arr);
			return NULL;
		}
		jso_schema_value *schema_value = jso_schema_value_parse(schema, item, parent);
		if (schema_value == NULL) {
			jso_schema_array_free(schema_arr);
			return NULL;
		}
		jso_schema_array_append(schema_arr, schema_value);
	}
	JSO_ARRAY_FOREACH_END;
	schema_keyword->data.asoval = schema_arr;
	return schema_keyword;
}

static jso_schema_keyword *jso_schema_keyword_get_object_of_schema_objects(jso_schema *schema,
		jso_value *data, const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get(
			schema, data, key, JSO_TYPE_OBJECT, keyword_flags, error_on_invalid_type, val);
	if (val == NULL) {
		return NULL;
	}
	jso_value objval;
	jso_string *objkey;
	jso_value *item;
	jso_object *obj = JSO_OBJVAL_P(val);
	jso_object *schema_obj = jso_object_alloc();
	jso_object_resize(schema_obj, JSO_OBJECT_COUNT(obj));
	JSO_OBJECT_FOREACH(obj, objkey, item)
	{
		if (JSO_TYPE_P(item) != JSO_TYPE_OBJECT) {
			jso_object_free(schema_obj);
			return NULL;
		}
		jso_schema_value *schema_value = jso_schema_value_parse(schema, item, parent);
		if (schema_value == NULL) {
			jso_object_free(schema_obj);
			return NULL;
		}
		JSO_VALUE_SET_SCHEMA_VALUE(objval, schema_value);
		jso_object_add(schema_obj, jso_string_copy(objkey), &objval);
	}
	JSO_OBJECT_FOREACH_END;
	schema_keyword->data.osoval = schema_obj;
	return schema_keyword;
}

static jso_schema_keyword *jso_schema_keyword_get_object(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get(
			schema, data, key, JSO_TYPE_OBJECT, keyword_flags, error_on_invalid_type, val);
	if (val == NULL) {
		return NULL;
	}
	schema_keyword->flags = keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	schema_keyword->data.oval = jso_object_copy(JSO_OBJVAL_P(val));
	return schema_keyword;
}

typedef jso_schema_keyword *(*jso_schema_keyword_callback)(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent);

static const jso_schema_keyword_callback schema_keyword_callbacks[] = {
	[JSO_SCHEMA_KEYWORD_TYPE_NULL] = jso_schema_keyword_get_null,
	[JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN] = jso_schema_keyword_get_bool,
	[JSO_SCHEMA_KEYWORD_TYPE_INTEGER] = jso_schema_keyword_get_int,
	[JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER] = jso_schema_keyword_get_uint,
	[JSO_SCHEMA_KEYWORD_TYPE_NUMBER] = jso_schema_keyword_get_number,
	[JSO_SCHEMA_KEYWORD_TYPE_STRING] = jso_schema_keyword_get_string,
	[JSO_SCHEMA_KEYWORD_TYPE_ARRAY] = jso_schema_keyword_get_array,
	[JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_STRINGS] = jso_schema_keyword_get_array_of_strings,
	[JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS]
	= jso_schema_keyword_get_array_of_schema_objects,
	[JSO_SCHEMA_KEYWORD_TYPE_OBJECT] = jso_schema_keyword_get_object,
	[JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT] = jso_schema_keyword_get_schema_object,
	[JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS]
	= jso_schema_keyword_get_object_of_schema_objects,
};

static inline jso_schema_keyword *jso_schema_keyword_get_ex(jso_schema *schema, jso_value *data,
		const char *key, jso_schema_value *parent, jso_schema_keyword_type keyword_type,
		jso_uint32 keyword_flags, jso_bool error_on_invalid_type,
		jso_schema_keyword *schema_keyword, jso_value *val)
{
	schema_keyword->type = keyword_type;
	return schema_keyword_callbacks[keyword_type](
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
	jso_schema_data_check_type(
			schema, data, key, val, keyword_union_type_1, keyword_union_type_2, true);

	return NULL;
}

static inline jso_rc jso_schema_keyword_check(jso_schema *schema, jso_schema_keyword *keyword)
{
	return keyword == NULL || jso_schema_error_is_set(schema) ? JSO_FAILURE : JSO_SUCCESS;
}

/* Set union keyword. */
jso_rc jso_schema_keyword_set_union_of_2_types(jso_schema *schema, jso_value *data,
		const char *key, jso_schema_value *value, jso_schema_keyword *schema_keyword,
		jso_schema_keyword_type keyword_union_type_1, jso_schema_keyword_type keyword_union_type_2,
		jso_uint32 keyword_flags)
{
	jso_schema_keyword *schema_keyword_ptr
			= jso_schema_keyword_get_union_of_2_types(schema, data, key, value,
					keyword_union_type_1, keyword_union_type_2, keyword_flags, schema_keyword);

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

void jso_schema_keyword_free(jso_schema_keyword *keyword)
{
	if (JSO_SCHEMA_KW_IS_SET_P(keyword)) {
		
	}
}
