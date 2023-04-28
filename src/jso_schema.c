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
#include <math.h>

void jso_schema_value_free(jso_schema_value *val);
static jso_schema_value *jso_schema_parse_value(
		jso_schema *schema, jso_value *data, jso_schema_value *parent);

static jso_schema_array_of_values *jso_schema_array_of_values_alloc(size_t size)
{
	jso_schema_array_of_values *arr
			= jso_calloc(1, sizeof(jso_schema_array_of_values) + size * sizeof(jso_schema_value *));
	arr->capacity = size;
	return arr;
}

static jso_rc jso_schema_array_of_values_append(
		jso_schema_array_of_values *arr, jso_schema_value *val)
{
	if (arr->len < arr->capacity) {
		arr->values[arr->len++] = val;
		return JSO_SUCCESS;
	}
	return JSO_FAILURE;
}

static void jso_schema_array_of_values_free(jso_schema_array_of_values *arr)
{
	for (size_t i = 0; i < arr->len; i++) {
		jso_schema_value_free(arr->values[i]);
	}
	jso_free(arr);
}

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

static inline jso_value *jso_schema_data_get_value_fast(
		jso_schema *schema, jso_value *data, const char *key, jso_uint32 keyword_flags)
{
	jso_value *val;

	if (jso_ht_get_by_cstr_key(JSO_OBJHVAL_P(data), key, &val) == JSO_FAILURE) {
		if (keyword_flags & JSO_SCHEMA_KEYWORD_FLAG_REQUIRED) {
			jso_schema_error_format(
					schema, JSO_SCHEMA_ERROR_KEYWORD_REQUIRED, "Keyword %s is required", key);
		}
		return NULL;
	}

	return val;
}

static inline jso_value *jso_schema_data_get_value(jso_schema *schema, jso_value *data,
		const char *key, jso_uint32 keyword_flags, jso_value *val)
{
	if (val != NULL) {
		return val;
	}

	return jso_schema_data_get_value_fast(schema, data, key, keyword_flags);
}

static jso_rc jso_schema_data_check_type(jso_schema *schema, jso_value *data, const char *key,
		jso_value *val, jso_value_type type, jso_value_type primary_type,
		jso_bool error_on_invalid_type)
{
	if ((JSO_TYPE_P(val) & type) == 0) {
		if (error_on_invalid_type) {
			if (type != primary_type) {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_DATA_TYPE,
						"Invalid type for %s - expected either %s or %s but given %s", key,
						jso_value_type_to_string(primary_type), jso_value_type_to_string(type),
						jso_value_type_to_string(JSO_TYPE_P(val)));
			} else {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_DATA_TYPE,
						"Invalid type for %s - expected %s but given %s", key,
						jso_value_type_to_string(type), jso_value_type_to_string(JSO_TYPE_P(val)));
			}
		}
		return JSO_FAILURE;
	}

	return JSO_SUCCESS;
}

static jso_value *jso_schema_data_get(jso_schema *schema, jso_value *data, const char *key,
		jso_value_type type, jso_uint32 keyword_flags, jso_bool error_on_invalid_type,
		jso_value *val)
{
	val = jso_schema_data_get_value(schema, data, key, keyword_flags, val);
	if (val == NULL) {
		return NULL;
	}

	return jso_schema_data_check_type(schema, data, key, val, type, type, error_on_invalid_type)
					== JSO_SUCCESS
			? val
			: NULL;
}

static inline jso_string *jso_schema_data_get_str(
		jso_schema *schema, jso_value *data, const char *key)
{
	jso_value *value = jso_schema_data_get(schema, data, key, JSO_TYPE_STRING, 0, true, NULL);
	return value == NULL ? NULL : value->data.str;
}

static jso_schema_keyword *jso_schema_data_get_keyword_null(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val)
{
	if (jso_schema_data_get(
				schema, data, key, JSO_TYPE_NULL, keyword_flags, error_on_invalid_type, val)
			!= NULL) {
		schema_keyword->flags = keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
		return schema_keyword;
	}

	return NULL;
}

static inline jso_schema_keyword *jso_schema_data_get_keyword_bool(jso_schema *schema,
		jso_value *data, const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val)
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

static inline jso_schema_keyword *jso_schema_data_get_keyword_int(jso_schema *schema,
		jso_value *data, const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val)
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

static inline jso_schema_keyword *jso_schema_data_get_keyword_uint(jso_schema *schema,
		jso_value *data, const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val)
{
	jso_schema_keyword *schema_keyword_int = jso_schema_data_get_keyword_int(
			schema, data, key, keyword_flags, error_on_invalid_type, schema_keyword, val);
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

static inline jso_schema_keyword *jso_schema_data_get_keyword_number(jso_schema *schema,
		jso_value *data, const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val)
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

static inline jso_schema_keyword *jso_schema_data_get_keyword_string(jso_schema *schema,
		jso_value *data, const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val)
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

static inline jso_schema_keyword *jso_schema_data_get_keyword_array(jso_schema *schema,
		jso_value *data, const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val)
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

static inline jso_schema_keyword *jso_schema_data_get_keyword_array_of_strings(jso_schema *schema,
		jso_value *data, const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val)
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

static inline jso_schema_keyword *jso_schema_data_get_keyword_schema_object(jso_schema *schema,
		jso_value *data, const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get(
			schema, data, key, JSO_TYPE_OBJECT, keyword_flags, error_on_invalid_type, val);
	if (val != NULL) {
		jso_schema_value *schema_value = jso_schema_parse_value(schema, val, parent);
		if (schema_value != NULL) {
			schema_keyword->data.soval = schema_value;
			return schema_keyword;
		}
	}

	return NULL;
}

static inline jso_schema_keyword *jso_schema_data_get_keyword_array_of_schema_objects(
		jso_schema *schema, jso_value *data, const char *key, jso_bool error_on_invalid_type,
		jso_uint32 keyword_flags, jso_schema_keyword *schema_keyword, jso_value *val,
		jso_schema_value *parent)
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
	jso_schema_array_of_values *schema_arr = jso_schema_array_of_values_alloc(JSO_ARRAY_LEN(arr));
	if (schema_arr == NULL) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_ALLOC,
				"Allocating schema array of values for keyword %s failed", key);
		return NULL;
	}
	JSO_ARRAY_FOREACH(arr, item)
	{
		if (JSO_TYPE_P(item) != JSO_TYPE_OBJECT) {
			jso_schema_array_of_values_free(schema_arr);
			return NULL;
		}
		jso_schema_value *schema_value = jso_schema_parse_value(schema, item, parent);
		if (schema_value == NULL) {
			jso_schema_array_of_values_free(schema_arr);
			return NULL;
		}
		jso_schema_array_of_values_append(schema_arr, schema_value);
	}
	JSO_ARRAY_FOREACH_END;
	schema_keyword->data.asoval = schema_arr;
	return schema_keyword;
}

static inline jso_schema_keyword *jso_schema_data_get_keyword_object_of_schema_objects(
		jso_schema *schema, jso_value *data, const char *key, jso_bool error_on_invalid_type,
		jso_uint32 keyword_flags, jso_schema_keyword *schema_keyword, jso_value *val,
		jso_schema_value *parent)
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
		jso_schema_value *schema_value = jso_schema_parse_value(schema, item, parent);
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

static inline jso_schema_keyword *jso_schema_data_get_keyword_object(jso_schema *schema,
		jso_value *data, const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val)
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

static jso_schema_keyword *jso_schema_data_get_keyword_ex(jso_schema *schema, jso_value *data,
		const char *key, jso_schema_value *parent, jso_schema_keyword_type keyword_type,
		jso_uint32 keyword_flags, jso_bool error_on_invalid_type,
		jso_schema_keyword *schema_keyword, jso_value *val)
{
	schema_keyword->type = keyword_type;
	switch (keyword_type) {
		case JSO_SCHEMA_KEYWORD_TYPE_NULL:
			return jso_schema_data_get_keyword_null(
					schema, data, key, error_on_invalid_type, keyword_flags, schema_keyword, val);
		case JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN:
			return jso_schema_data_get_keyword_bool(
					schema, data, key, error_on_invalid_type, keyword_flags, schema_keyword, val);
		case JSO_SCHEMA_KEYWORD_TYPE_INTEGER:
			return jso_schema_data_get_keyword_int(
					schema, data, key, error_on_invalid_type, keyword_flags, schema_keyword, val);
		case JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER:
			return jso_schema_data_get_keyword_uint(
					schema, data, key, error_on_invalid_type, keyword_flags, schema_keyword, val);
		case JSO_SCHEMA_KEYWORD_TYPE_NUMBER:
			return jso_schema_data_get_keyword_number(
					schema, data, key, error_on_invalid_type, keyword_flags, schema_keyword, val);
		case JSO_SCHEMA_KEYWORD_TYPE_STRING:
			return jso_schema_data_get_keyword_string(
					schema, data, key, error_on_invalid_type, keyword_flags, schema_keyword, val);
		case JSO_SCHEMA_KEYWORD_TYPE_ARRAY:
			return jso_schema_data_get_keyword_array(
					schema, data, key, error_on_invalid_type, keyword_flags, schema_keyword, val);
		case JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_STRINGS:
			return jso_schema_data_get_keyword_array_of_schema_objects(schema, data, key,
					error_on_invalid_type, keyword_flags, schema_keyword, val, parent);
		case JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT:
			return jso_schema_data_get_keyword_schema_object(schema, data, key,
					error_on_invalid_type, keyword_flags, schema_keyword, val, parent);
		case JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS:
			return jso_schema_data_get_keyword_array_of_schema_objects(schema, data, key,
					error_on_invalid_type, keyword_flags, schema_keyword, val, parent);
		case JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS:
			return jso_schema_data_get_keyword_object_of_schema_objects(schema, data, key,
					error_on_invalid_type, keyword_flags, schema_keyword, val, parent);
		case JSO_SCHEMA_KEYWORD_TYPE_OBJECT:
			return jso_schema_data_get_keyword_object(
					schema, data, key, error_on_invalid_type, keyword_flags, schema_keyword, val);
	}

	return NULL;
}

static inline jso_schema_keyword *jso_schema_data_get_keyword(jso_schema *schema, jso_value *data,
		const char *key, jso_schema_value *parent, jso_schema_keyword_type keyword_type,
		jso_uint32 keyword_flags, jso_bool error_on_invalid_type,
		jso_schema_keyword *schema_keyword)
{
	return jso_schema_data_get_keyword_ex(schema, data, key, parent, keyword_type, keyword_flags,
			error_on_invalid_type, schema_keyword, NULL);
}

static jso_schema_keyword *jso_schema_data_get_keyword_union(jso_schema *schema, jso_value *data,
		const char *key, jso_schema_value *parent, jso_uint32 union_types, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword)
{
	jso_value *val = jso_schema_data_get_value_fast(schema, data, key, keyword_flags);
	if (val == NULL) {
		return NULL;
	}

	for (int union_type = 1; union_type <= JSO_SCHEMA_KEYWORD_TYPE_LAST; union_type <<= 1) {
		if (union_type & union_types) {
			jso_schema_keyword *schema_keyword_result = jso_schema_data_get_keyword_ex(
					schema, data, key, parent, union_type, 0, false, schema_keyword, val);
			if (schema_keyword_result != NULL || jso_schema_error_is_set(schema)) {
				return schema_keyword_result;
			}
		}
	}

	return NULL;
}

JSO_API jso_schema *jso_schema_alloc()
{
	return jso_calloc(1, sizeof(jso_schema));
}

JSO_API void jso_schema_init(jso_schema *schema)
{
	memset(schema, 0, sizeof(jso_schema));
}

static inline jso_rc jso_schema_keyword_check(jso_schema *schema, jso_schema_keyword *keyword)
{
	return keyword == NULL || jso_schema_error_is_set(schema) ? JSO_FAILURE : JSO_SUCCESS;
}

/* Set union keyword. */
static inline jso_rc jso_schema_keyword_set_union(jso_schema *schema, jso_value *data,
		const char *key, jso_schema_value *value, jso_schema_keyword *schema_keyword,
		jso_uint32 union_types, jso_uint32 keyword_flags)
{
	jso_schema_keyword *schema_keyword_ptr = jso_schema_data_get_keyword_union(
			schema, data, key, value, union_types, keyword_flags, schema_keyword);

	return jso_schema_keyword_check(schema, schema_keyword_ptr);
}

/* Set keyword. */
static inline jso_rc jso_schema_keyword_set(jso_schema *schema, jso_value *data, const char *key,
		jso_schema_value *value, jso_schema_keyword *schema_keyword,
		jso_schema_keyword_type keyword_type, jso_uint32 keyword_flags)
{
	jso_schema_keyword *schema_keyword_ptr = jso_schema_data_get_keyword(
			schema, data, key, value, keyword_type, keyword_flags, true, schema_keyword);

	return jso_schema_keyword_check(schema, schema_keyword_ptr);
}

#define JSO_SCHEMA_KW_TYPE(_kw_type) JSO_SCHEMA_KEYWORD_##_kw_type

#define JSO_SCHEMA_KW_SET_WRAP(_kw_set_call, _value, _value_data) \
	do { \
		if (_kw_set_call == JSO_FAILURE) { \
			jso_free(_value); \
			jso_free(_value_data); \
			return NULL; \
		} \
	} while (0)

#define JSO_SCHEMA_KW_SET_WITH_FLAGS( \
		_schema, _data, _key, _value, _value_data, _kw_name, _kw_type, _kw_flags) \
	JSO_SCHEMA_KW_SET_WRAP( \
			jso_schema_keyword_set(_schema, _data, #_key, _value, &_value_data->_kw_name, \
					JSO_SCHEMA_KW_TYPE(_kw_type), _kw_flags), \
			_value, _value_data)

#define JSO_SCHEMA_KW_SET_EX(_schema, _data, _key, _value, _value_data, _kw_name, _kw_type) \
	JSO_SCHEMA_KW_SET_WITH_FLAGS(_schema, _data, _key, _value, _value_data, _kw_name, _kw_type, 0)

#define JSO_SCHEMA_KW_SET(_schema, _data, _key, _value, _value_data, _kw_type) \
	JSO_SCHEMA_KW_SET_EX(schema, _data, _key, _value, _value_data, _key, _kw_type)

#define JSO_SCHEMA_KW_SET_UNION_2_WITH_FLAGS( \
		_schema, _data, _key, _value, _value_data, _kw_name, _utype1, _utype2, _kw_flags) \
	JSO_SCHEMA_KW_SET_WRAP( \
			jso_schema_keyword_set_union(_schema, _data, #_key, _value, &_value_data->_kw_name, \
					JSO_SCHEMA_KW_TYPE(_utype1) | JSO_SCHEMA_KW_TYPE(_utype2), _kw_flags), \
			_value, _value_data)

#define JSO_SCHEMA_KW_SET_UNION_2_EX( \
		_schema, _data, _key, _value, _value_data, _kw_name, _utype1, _utype2) \
	JSO_SCHEMA_KW_SET_UNION_2_WITH_FLAGS( \
			_schema, _data, _key, _value, _value_data, _kw_name, _utype1, _utype2, 0)

#define JSO_SCHEMA_KW_SET_UNION_2(_schema, _data, _key, _value, _value_data, _utype1, _utype2) \
	JSO_SCHEMA_KW_SET_UNION_2_EX(_schema, _data, _key, _value, _value_data, _key, _utype1, _utype2)

#define JSO_SCHEMA_KW_SET_UNION JSO_SCHEMA_KW_SET_UNION_2
#define JSO_SCHEMA_KW_SET_UNION_EX JSO_SCHEMA_KW_SET_UNION_2_EX
#define JSO_SCHEMA_KW_SET_UNION_WITH_FLAGS JSO_SCHEMA_KW_SET_UNION_2_WITH_FLAGS

#define JSO_SCHEMA_KW_SET_BOOL_EX(_schema, _data, _key, _value, _value_data, _kw_name) \
	JSO_SCHEMA_KW_SET_EX(_schema, _data, _key, _value, _value_data, _kw_name, TYPE_BOOLEAN)

#define JSO_SCHEMA_KW_SET_BOOL(_schema, _data, _key, _value, _value_data) \
	JSO_SCHEMA_KW_SET_BOOL_EX(_schema, _data, _key, _value, _value_data, _key)

#define JSO_SCHEMA_KW_SET_INT_EX(_schema, _data, _key, _value, _value_data, _kw_name) \
	JSO_SCHEMA_KW_SET_EX(_schema, _data, _key, _value, _value_data, _kw_name, TYPE_INTEGER)

#define JSO_SCHEMA_KW_SET_INT(_schema, _data, _key, _value, _value_data) \
	JSO_SCHEMA_KW_SET_INT_EX(_schema, _data, _key, _value, _value_data, _key)

#define JSO_SCHEMA_KW_SET_NUM_EX(_schema, _data, _key, _value, _value_data, _kw_name) \
	JSO_SCHEMA_KW_SET_EX(_schema, _data, _key, _value, _value_data, _kw_name, TYPE_NUMBER)

#define JSO_SCHEMA_KW_SET_NUM(_schema, _data, _key, _value, _value_data) \
	JSO_SCHEMA_KW_SET_NUM_EX(_schema, _data, _key, _value, _value_data, _key)

#define JSO_SCHEMA_KW_SET_UINT_EX(_schema, _data, _key, _value, _value_data, _kw_name) \
	JSO_SCHEMA_KW_SET_EX(_schema, _data, _key, _value, _value_data, _kw_name, TYPE_UNSIGNED_INTEGER)

#define JSO_SCHEMA_KW_SET_UINT(_schema, _data, _key, _value, _value_data) \
	JSO_SCHEMA_KW_SET_UINT_EX(_schema, _data, _key, _value, _value_data, _key)

#define JSO_SCHEMA_KW_SET_UINT_NZ_EX(_schema, _data, _key, _value, _value_data, _kw_name) \
	JSO_SCHEMA_KW_SET_WITH_FLAGS(_schema, _data, _key, _value, _value_data, _kw_name, \
			TYPE_UNSIGNED_INTEGER, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO)

#define JSO_SCHEMA_KW_SET_UINT_NZ(_schema, _data, _key, _value, _value_data) \
	JSO_SCHEMA_KW_SET_UINT_NZ_EX(_schema, _data, _key, _value, _value_data, _key)

#define JSO_SCHEMA_KW_SET_STR_EX(_schema, _data, _key, _value, _value_data, _kw_name) \
	JSO_SCHEMA_KW_SET_EX(_schema, _data, _key, _value, _value_data, _kw_name, TYPE_STRING)

#define JSO_SCHEMA_KW_SET_STR(_schema, _data, _key, _value, _value_data) \
	JSO_SCHEMA_KW_SET_STR_EX(_schema, _data, _key, _value, _value_data, _key)

#define JSO_SCHEMA_KW_IS_SET(_value_data) _value_data.flags &JSO_SCHEMA_KEYWORD_FLAG_PRESENT

#define JSO_SCHEMA_VALUE_INIT(_schema, _data, _parent, _type_name, _kw_type) \
	jso_schema_value_init(_schema, _data, _parent, #_type_name, \
			sizeof(jso_schema_value_##_type_name), JSO_SCHEMA_KW_TYPE(_kw_type))

static jso_schema_value *jso_schema_value_init(jso_schema *schema, jso_value *data,
		jso_schema_value *parent, const char *type_name, size_t value_size,
		jso_schema_keyword_type keyward_type)
{
	jso_schema_value *value = jso_calloc(1, sizeof(jso_schema_value));
	if (value == NULL) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_ALLOC,
				"Allocating value for type %s failed", type_name);
		return NULL;
	}

	jso_schema_value_common *value_data = jso_calloc(1, value_size);
	if (value_data == NULL) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_ALLOC,
				"Allocating value data for type %s failed", type_name);
		return NULL;
	}
	value_data->parent = parent;

	JSO_SCHEMA_KW_SET_STR(schema, data, description, value, value_data);
	JSO_SCHEMA_KW_SET_STR(schema, data, title, value, value_data);

	value->data.comval = value_data;

	return value;
}

static jso_schema_value *jso_schema_parse_null(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	return JSO_SCHEMA_VALUE_INIT(schema, data, parent, null, TYPE_NULL);
}

static jso_schema_value *jso_schema_parse_boolean(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	jso_schema_value *value = JSO_SCHEMA_VALUE_INIT(schema, data, parent, boolean, TYPE_BOOLEAN);
	jso_schema_value_boolean *boolval = value->data.boolval;

	JSO_SCHEMA_KW_SET_BOOL_EX(schema, data, default, value, boolval, default_value);

	return value;
}

static jso_schema_value *jso_schema_parse_integer(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	jso_schema_value *value = JSO_SCHEMA_VALUE_INIT(schema, data, parent, integer, TYPE_INTEGER);
	jso_schema_value_integer *intval = value->data.intval;

	JSO_SCHEMA_KW_SET_INT_EX(schema, data, default, value, intval, default_value);
	JSO_SCHEMA_KW_SET_UINT_NZ_EX(schema, data, multipleOf, value, intval, multiple_of);
	JSO_SCHEMA_KW_SET_INT(schema, data, minimum, value, intval);
	JSO_SCHEMA_KW_SET_BOOL_EX(schema, data, exclusiveMinimum, value, intval, exclusive_minimum);
	JSO_SCHEMA_KW_SET_INT(schema, data, maximum, value, intval);
	JSO_SCHEMA_KW_SET_BOOL_EX(schema, data, exclusiveMaximum, value, intval, exclusive_maximum);

	if (JSO_SCHEMA_KW_IS_SET(intval->exclusive_minimum) && !JSO_SCHEMA_KW_IS_SET(intval->minimum)) {
		jso_schema_error_set(schema, JSO_SCHEMA_ERROR_VALUE_DATA_DEPS,
				"The minimum must be set when exclusiveMinimum is set");
	}
	if (JSO_SCHEMA_KW_IS_SET(intval->exclusive_maximum) && !JSO_SCHEMA_KW_IS_SET(intval->maximum)) {
		jso_schema_error_set(schema, JSO_SCHEMA_ERROR_VALUE_DATA_DEPS,
				"The maximum must be set when exclusiveMaximum is set");
	}

	return value;
}

static jso_schema_value *jso_schema_parse_number(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	jso_schema_value *value = JSO_SCHEMA_VALUE_INIT(schema, data, parent, number, TYPE_NUMBER);
	jso_schema_value_number *numval = value->data.numval;

	JSO_SCHEMA_KW_SET_NUM_EX(schema, data, default, value, numval, default_value);
	JSO_SCHEMA_KW_SET_UINT_NZ_EX(schema, data, multipleOf, value, numval, multiple_of);
	JSO_SCHEMA_KW_SET_NUM(schema, data, minimum, value, numval);
	JSO_SCHEMA_KW_SET_BOOL_EX(schema, data, exclusiveMinimum, value, numval, exclusive_minimum);
	JSO_SCHEMA_KW_SET_NUM(schema, data, maximum, value, numval);
	JSO_SCHEMA_KW_SET_BOOL_EX(schema, data, exclusiveMaximum, value, numval, exclusive_maximum);

	if (JSO_SCHEMA_KW_IS_SET(numval->exclusive_minimum) && !JSO_SCHEMA_KW_IS_SET(numval->minimum)) {
		jso_schema_error_set(schema, JSO_SCHEMA_ERROR_VALUE_DATA_DEPS,
				"The minimum must be set when exclusiveMinimum is set");
	}
	if (JSO_SCHEMA_KW_IS_SET(numval->exclusive_maximum) && !JSO_SCHEMA_KW_IS_SET(numval->maximum)) {
		jso_schema_error_set(schema, JSO_SCHEMA_ERROR_VALUE_DATA_DEPS,
				"The maximum must be set when exclusiveMaximum is set");
	}

	return value;
}

static jso_schema_value *jso_schema_parse_string(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	jso_schema_value *value = JSO_SCHEMA_VALUE_INIT(schema, data, parent, string, TYPE_STRING);
	jso_schema_value_string *strval = value->data.strval;

	JSO_SCHEMA_KW_SET_STR_EX(schema, data, default, value, strval, default_value);
	JSO_SCHEMA_KW_SET_UINT_EX(schema, data, maxLength, value, strval, max_length);
	JSO_SCHEMA_KW_SET_UINT_EX(schema, data, minLength, value, strval, min_length);

	return value;
}

static jso_schema_value *jso_schema_parse_array(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	jso_schema_value *value = JSO_SCHEMA_VALUE_INIT(schema, data, parent, array, TYPE_ARRAY);
	jso_schema_value_array *arrval = value->data.arrval;

	JSO_SCHEMA_KW_SET_EX(schema, data, default, value, arrval, default_value, TYPE_ARRAY);
	JSO_SCHEMA_KW_SET_UNION_EX(schema, data, additionalItems, value, arrval, additional_items,
			TYPE_BOOLEAN, TYPE_ARRAY_OF_SCHEMA_OBJECTS);
	JSO_SCHEMA_KW_SET_UNION(
			schema, data, items, value, arrval, TYPE_SCHEMA_OBJECT, TYPE_ARRAY_OF_SCHEMA_OBJECTS);
	JSO_SCHEMA_KW_SET_BOOL_EX(schema, data, uniqueItems, value, arrval, unique_items);
	JSO_SCHEMA_KW_SET_UINT_EX(schema, data, maxItems, value, arrval, max_items);
	JSO_SCHEMA_KW_SET_UINT_EX(schema, data, minItems, value, arrval, min_items);

	return value;
}

static jso_schema_value *jso_schema_parse_object(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	jso_schema_value *value = JSO_SCHEMA_VALUE_INIT(schema, data, parent, array, TYPE_OBJECT);
	jso_schema_value_object *objval = value->data.objval;

	JSO_SCHEMA_KW_SET_EX(schema, data, default, value, objval, default_value, TYPE_OBJECT);
	JSO_SCHEMA_KW_SET_UINT_EX(schema, data, minProperties, value, objval, max_properties);
	JSO_SCHEMA_KW_SET_UINT_EX(schema, data, maxProperties, value, objval, max_properties);
	JSO_SCHEMA_KW_SET_UNION_EX(schema, data, additionalProperties, value, objval,
			additional_properties, TYPE_BOOLEAN, TYPE_SCHEMA_OBJECT);
	JSO_SCHEMA_KW_SET(schema, data, properties, value, objval, TYPE_OBJECT_OF_SCHEMA_OBJECTS);

	return value;
}

static jso_schema_value *jso_schema_parse_value(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	jso_string *type = jso_schema_data_get_str(schema, data, "type");

	if (type == NULL) {
		// TODO: support empty schema that always pass
		return NULL;
	}

	if (jso_string_equals_to_cstr(type, "string")) {
		return jso_schema_parse_string(schema, data, parent);
	}
	if (jso_string_equals_to_cstr(type, "number")) {
		return jso_schema_parse_number(schema, data, parent);
	}
	if (jso_string_equals_to_cstr(type, "integer")) {
		return jso_schema_parse_integer(schema, data, parent);
	}
	if (jso_string_equals_to_cstr(type, "boolean")) {
		return jso_schema_parse_boolean(schema, data, parent);
	}
	if (jso_string_equals_to_cstr(type, "array")) {
		return jso_schema_parse_array(schema, data, parent);
	}
	if (jso_string_equals_to_cstr(type, "object")) {
		return jso_schema_parse_object(schema, data, parent);
	}
	if (jso_string_equals_to_cstr(type, "null")) {
		return jso_schema_parse_null(schema, data, parent);
	}

	jso_schema_error_format(
			schema, JSO_SCHEMA_ERROR_TYPE_UNKNOWN, "Unknown type %s", JSO_STRING_VAL(type));

	return NULL;
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

	// parse id
	jso_string *id = jso_schema_data_get_str(schema, data, "id");
	if (jso_schema_error_is_set(schema)) {
		return JSO_FAILURE;
	}
	if (id) {
		schema->id = jso_string_copy(id);
	}

	// parse root value
	jso_schema_value *root = jso_schema_parse_value(schema, data, NULL);
	if (root == NULL) {
		return JSO_FAILURE;
	}
	schema->root = root;

	return JSO_SUCCESS;
}

JSO_API void jso_schema_clear(jso_schema *schema)
{
}

JSO_API void jso_schema_free(jso_schema *schema)
{
	if (schema->is_initialized) {
		jso_schema_clear(schema);
	}
	jso_free(schema);
}

void jso_schema_value_free(jso_schema_value *val)
{
}
