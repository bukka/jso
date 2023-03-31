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

static jso_value *jso_schema_data_get(jso_schema *schema, jso_value *data, const char *key,
		jso_value_type type, jso_bool required)
{
	jso_value *val;

	if (jso_ht_get_by_cstr_key(JSO_OBJHVAL_P(data), key, &val) == JSO_FAILURE) {
		if (required) {
			jso_schema_error_format(
					schema, JSO_SCHEMA_ERROR_KEYWORD_REQUIRED, "Keyword %s is required", key);
		}
		return NULL;
	}

	if ((JSO_TYPE_P(val) & type) == 0) {
		jso_schema_error_format(
				schema, JSO_SCHEMA_ERROR_VALUE_DATA_TYPE, "Invalid type for %s", key);
		return NULL;
	}

	return val;
}

static inline jso_string *jso_schema_data_get_str_ex(
		jso_schema *schema, jso_value *data, const char *key, jso_bool required)
{
	jso_value *val = jso_schema_data_get(schema, data, key, JSO_TYPE_STRING, required);

	return val == NULL ? NULL : JSO_STR_P(val);
}

static inline jso_string *jso_schema_data_get_str_required(
		jso_schema *schema, jso_value *data, const char *key)
{
	return jso_schema_data_get_str_ex(schema, data, key, true);
}

static inline jso_string *jso_schema_data_get_str(
		jso_schema *schema, jso_value *data, const char *key)
{
	return jso_schema_data_get_str_ex(schema, data, key, false);
}

static inline jso_bool jso_schema_data_get_bool(
		jso_schema *schema, jso_value *data, const char *key, bool *present)
{
	jso_value *val = jso_schema_data_get(schema, data, key, JSO_TYPE_BOOL, false);
	if (val == NULL) {
		*present = false;
	}
	*present = true;

	return val == NULL ? false : (jso_bool) JSO_IVAL_P(val);
}

static inline jso_number *jso_schema_data_get_number(
		jso_schema *schema, jso_value *data, const char *key, jso_number *num)
{
	jso_value *val = jso_schema_data_get(schema, data, key, JSO_TYPE_INT | JSO_TYPE_DOUBLE, false);

	if (val == NULL) {
		return NULL;
	}

	if (JSO_TYPE_P(val) == JSO_TYPE_INT) {
		num->is_int = true;
		num->ival = JSO_IVAL_P(val);
	} else {
		num->is_int = false;
		num->dval = JSO_DVAL_P(val);
	}

	return num;
}

static inline jso_int jso_schema_data_get_int(
		jso_schema *schema, jso_value *data, const char *key, bool *present)
{
	jso_value *val = jso_schema_data_get(schema, data, key, JSO_TYPE_INT | JSO_TYPE_DOUBLE, false);

	if (val == NULL) {
		*present = false;
		return 0;
	}
	*present = true;

	if (JSO_TYPE_P(val) == JSO_TYPE_INT) {
		return JSO_IVAL_P(val);
	}

	return (jso_int) JSO_DVAL_P(val);
}

static inline jso_uint jso_schema_data_get_uint(
		jso_schema *schema, jso_value *data, const char *key, bool *present, bool no_zero)
{
	jso_int val = jso_schema_data_get_int(schema, data, key, present);

	if (!*present || jso_schema_error_is_set(schema)) {
		return 0;
	}

	if (no_zero) {
		if (val <= 0) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_DATA_TYPE,
					"Value for for type %s must be greater than 0", key);
			return 0;
		}
	} else if (val < 0) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_DATA_TYPE,
				"Value for for type %s must be equal or greater than 0", key);
		return 0;
	}

	return (jso_uint) val;
}

static jso_schema_internal *jso_schema_data_get_internal(jso_schema *schema, jso_value *data,
		const char *key, jso_schema_value *parent, jso_schema_internal_type internal_type,
		jso_schema_internal *schema_internal)
{
	schema_internal->type = internal_type;
	switch (internal_type) {
		case JSO_SCHEMA_INTERNAL_TYPE_NULL:
			if (jso_schema_data_get(schema, data, key, JSO_TYPE_NULL, false) != NULL) {
				return schema_internal;
			}
			break;
		case JSO_SCHEMA_INTERNAL_TYPE_BOOLEAN: {
			jso_value *val = jso_schema_data_get(schema, data, key, JSO_TYPE_BOOL, false);
			if (val != NULL) {
				schema_internal->data.bval = JSO_IVAL_P(val) ? true : false;
				return schema_internal;
			}
		} break;
		case JSO_SCHEMA_INTERNAL_TYPE_INTEGER: {
			jso_value *val = jso_schema_data_get(schema, data, key, JSO_TYPE_INT, false);
			if (val != NULL) {
				schema_internal->data.ival = JSO_IVAL_P(val);
				return schema_internal;
			}
		} break;
		case JSO_SCHEMA_INTERNAL_TYPE_UNSIGNED_INTEGER: {
			jso_value *val = jso_schema_data_get(schema, data, key, JSO_TYPE_INT, false);
			if (val != NULL && JSO_IVAL_P(val) > 0) {
				schema_internal->data.uval = JSO_IVAL_P(val);
				return schema_internal;
			}
		} break;
		case JSO_SCHEMA_INTERNAL_TYPE_STRING: {
			jso_value *val = jso_schema_data_get(schema, data, key, JSO_TYPE_STRING, false);
			if (val != NULL) {
				schema_internal->data.sval = jso_string_copy(JSO_STR_P(val));
				return schema_internal;
			}
		} break;
		case JSO_SCHEMA_INTERNAL_TYPE_ARRAY_OF_STRINGS: {
			jso_value *val = jso_schema_data_get(schema, data, key, JSO_TYPE_ARRAY, false);
			if (val != NULL) {
				jso_value *item;
				JSO_ARRAY_FOREACH(JSO_ARRVAL_P(val), item)
				{
					if (JSO_TYPE_P(item) != JSO_TYPE_STRING) {
						return NULL;
					}
				}
				JSO_ARRAY_FOREACH_END;
				schema_internal->data.asval = jso_array_copy(JSO_ARRVAL_P(val));
				return schema_internal;
			}
		} break;
		case JSO_SCHEMA_INTERNAL_TYPE_ARRAY_OF_SCHEMA_OBJECTS: {
			jso_value *val = jso_schema_data_get(schema, data, key, JSO_TYPE_ARRAY, false);
			if (val != NULL) {
				jso_value *item;
				jso_array *arr = JSO_ARRVAL_P(val);
				jso_schema_array_of_values *schema_arr
						= jso_schema_array_of_values_alloc(JSO_ARRAY_LEN(arr));
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
				schema_internal->data.asoval = schema_arr;
				return schema_internal;
			}
		} break;
		case JSO_SCHEMA_INTERNAL_TYPE_SCHEMA_OBJECT: {
			jso_value *val = jso_schema_data_get(schema, data, key, JSO_TYPE_OBJECT, false);
			if (val != NULL) {
				jso_schema_value *schema_value = jso_schema_parse_value(schema, val, parent);
				if (schema_value != NULL) {
					schema_internal->data.soval = schema_value;
					return schema_internal;
				}
			}
		} break;
	}

	return NULL;
}

static jso_schema_internal *jso_schema_data_get_internal_union(jso_schema *schema, jso_value *data,
		const char *key, jso_schema_value *parent, jso_uint32 union_types,
		jso_schema_internal *schema_internal)
{
	for (int union_type = 1; union_type <= JSO_SCHEMA_INTERNAL_TYPE_SCHEMA_OBJECT;
			union_type <<= 1) {
		if (union_type & union_types) {
			jso_schema_internal *schema_internal_result = jso_schema_data_get_internal(
					schema, data, key, parent, union_type, schema_internal);
			if (schema_internal_result != NULL || jso_schema_error_is_set(schema)) {
				return schema_internal_result;
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

/* Set boolean keyword. */
static jso_rc jso_schema_keyword_set_boolean(jso_schema *schema, jso_value *data, const char *key,
		jso_bool *keyword, jso_bitset *keywords, jso_uint64 keyword_type)
{
	jso_bool present;
	jso_bool keyword_bool = jso_schema_data_get_bool(schema, data, key, &present);
	if (jso_schema_error_is_set(schema)) {
		return JSO_FAILURE;
	}
	if (present) {
		*keyword = keyword_bool;
		JSO_BITSET_SET(*keywords, keyword_type);
	}

	return JSO_SUCCESS;
}

/* Set number keyword. */
static jso_rc jso_schema_keyword_set_number(jso_schema *schema, jso_value *data, const char *key,
		jso_number *keyword, jso_bitset *keywords, jso_uint64 keyword_type)
{
	jso_number num;
	jso_number *pnum = jso_schema_data_get_number(schema, data, key, &num);
	if (jso_schema_error_is_set(schema)) {
		return JSO_FAILURE;
	}
	if (pnum != NULL) {
		memcpy(keyword, pnum, sizeof(jso_number));
		JSO_BITSET_SET(*keywords, keyword_type);
	}

	return JSO_SUCCESS;
}

/* Set integer keyword. */
static jso_rc jso_schema_keyword_set_int(jso_schema *schema, jso_value *data, const char *key,
		jso_int *keyword, jso_bitset *keywords, jso_uint64 keyword_type)
{
	jso_bool present;
	jso_int num = jso_schema_data_get_int(schema, data, key, &present);
	if (jso_schema_error_is_set(schema)) {
		return JSO_FAILURE;
	}
	if (present) {
		*keyword = num;
		JSO_BITSET_SET(*keywords, keyword_type);
	}

	return JSO_SUCCESS;
}

/* Set unsigned integer keyword either with zero or without zero. */
static jso_rc jso_schema_keyword_set_uint_ex(jso_schema *schema, jso_value *data, const char *key,
		jso_uint *keyword, jso_bitset *keywords, jso_uint64 keyword_type, bool no_zero)
{
	jso_bool present;
	jso_uint num = jso_schema_data_get_uint(schema, data, key, &present, no_zero);
	if (jso_schema_error_is_set(schema)) {
		return JSO_FAILURE;
	}
	if (present) {
		*keyword = num;
		JSO_BITSET_SET(*keywords, keyword_type);
	}

	return JSO_SUCCESS;
}

/* Set unsigned integer keyword. */
static inline jso_rc jso_schema_keyword_set_uint(jso_schema *schema, jso_value *data,
		const char *key, jso_uint *keyword, jso_bitset *keywords, jso_uint64 keyword_type)
{
	return jso_schema_keyword_set_uint_ex(
			schema, data, key, keyword, keywords, keyword_type, false);
}

/* Set non zero unsigned integer keyword. */
static inline jso_rc jso_schema_keyword_set_uint_nz(jso_schema *schema, jso_value *data,
		const char *key, jso_uint *keyword, jso_bitset *keywords, jso_uint64 keyword_type)
{
	return jso_schema_keyword_set_uint_ex(schema, data, key, keyword, keywords, keyword_type, true);
}

/* Set string keyword. */
static jso_rc jso_schema_keyword_set_str(jso_schema *schema, jso_value *data, const char *key,
		jso_string **keyword, jso_bitset *keywords, jso_uint64 keyword_type)
{
	jso_string *keyword_str = jso_schema_data_get_str(schema, data, key);
	if (jso_schema_error_is_set(schema)) {
		return JSO_FAILURE;
	}
	if (keyword_str != NULL) {
		*keyword = jso_string_copy(keyword_str);
		JSO_BITSET_SET(*keywords, keyword_type);
	}
	return JSO_SUCCESS;
}

/* Set union keyword. */
static jso_rc jso_schema_keyword_set_internal_union(jso_schema *schema, jso_value *data,
		const char *key, jso_schema_value *value, jso_schema_internal **keyword,
		jso_bitset *keywords, jso_uint64 keyword_type, jso_uint32 union_types)
{
	jso_schema_internal schema_internal;
	jso_schema_internal *schema_internal_ptr = jso_schema_data_get_internal_union(
			schema, data, key, value, union_types, &schema_internal);
	if (jso_schema_error_is_set(schema)) {
		return JSO_FAILURE;
	}
	if (schema_internal_ptr != NULL) {
		jso_schema_internal *schema_internal_new = jso_calloc(1, sizeof(jso_schema_internal));
		memcpy(*keyword, schema_internal_ptr, sizeof(jso_schema_internal));
		*keyword = schema_internal_new;
		JSO_BITSET_SET(*keywords, keyword_type);
	}
	return JSO_SUCCESS;
}

#define JSO_SCHEMA_KW_SET_WRAP(_kw_set_call, _value, _value_data) \
	do { \
		if (_kw_set_call == JSO_FAILURE) { \
			jso_free(_value); \
			jso_free(_value_data); \
			return NULL; \
		} \
	} while (0)

#define JSO_SCHEMA_KW_SET( \
		_type_name, _schema, _data, _key, _value, _value_data, _keyword_name, _keyword_type) \
	JSO_SCHEMA_KW_SET_WRAP(jso_schema_keyword_set_##_type_name(_schema, _data, #_key, \
								   &_value_data->_keyword_name, &_value_data->keywords, \
								   JSO_SCHEMA_KEYWORD_##_keyword_type), \
			_value, _value_data)

#define JSO_SCHEMA_KW_SET_UNION_2_EX( \
		_schema, _data, _key, _value, _value_data, _keyword_name, _keyword_type, _utype1, _utype2) \
	JSO_SCHEMA_KW_SET_WRAP(jso_schema_keyword_set_internal_union(_schema, _data, #_key, _value, \
								   &_value_data->_keyword_name, &_value_data->keywords, \
								   JSO_SCHEMA_KEYWORD_##_keyword_type, \
								   JSO_SCHEMA_INTERNAL_##_utype1 | JSO_SCHEMA_INTERNAL_##_utype2), \
			_value, _value_data)

#define JSO_SCHEMA_KW_SET_UNION_2( \
		_schema, _data, _key, _value, _value_data, _keyword_type, _utype1, _utype2) \
	JSO_SCHEMA_KW_SET_UNION_2_EX( \
			_schema, _data, _key, _value, _value_data, _key, _keyword_type, _utype1, _utype2)

#define JSO_SCHEMA_KW_SET_UNION JSO_SCHEMA_KW_SET_UNION_2
#define JSO_SCHEMA_KW_SET_UNION_EX JSO_SCHEMA_KW_SET_UNION_2_EX

#define JSO_SCHEMA_KW_SET_BOOL_EX( \
		_schema, _data, _key, _value, _value_data, _keyword_name, _keyword_type) \
	JSO_SCHEMA_KW_SET( \
			boolean, _schema, _data, _key, _value, _value_data, _keyword_name, _keyword_type)

#define JSO_SCHEMA_KW_SET_BOOL(_schema, _data, _key, _value, _value_data, _keyword_type) \
	JSO_SCHEMA_KW_SET_BOOL_EX(_schema, _data, _key, _value, _value_data, _key, _keyword_type)

#define JSO_SCHEMA_KW_SET_INT_EX( \
		_schema, _data, _key, _value, _value_data, _keyword_name, _keyword_type) \
	JSO_SCHEMA_KW_SET(int, _schema, _data, _key, _value, _value_data, _keyword_name, _keyword_type)

#define JSO_SCHEMA_KW_SET_INT(_schema, _data, _key, _value, _value_data, _keyword_type) \
	JSO_SCHEMA_KW_SET_INT_EX(_schema, _data, _key, _value, _value_data, _key, _keyword_type)

#define JSO_SCHEMA_KW_SET_NUM_EX( \
		_schema, _data, _key, _value, _value_data, _keyword_name, _keyword_type) \
	JSO_SCHEMA_KW_SET( \
			number, _schema, _data, _key, _value, _value_data, _keyword_name, _keyword_type)

#define JSO_SCHEMA_KW_SET_NUM(_schema, _data, _key, _value, _value_data, _keyword_type) \
	JSO_SCHEMA_KW_SET_NUM_EX(_schema, _data, _key, _value, _value_data, _key, _keyword_type)

#define JSO_SCHEMA_KW_SET_UINT_EX( \
		_schema, _data, _key, _value, _value_data, _keyword_name, _keyword_type) \
	JSO_SCHEMA_KW_SET(uint, _schema, _data, _key, _value, _value_data, _keyword_name, _keyword_type)

#define JSO_SCHEMA_KW_SET_UINT(_schema, _data, _key, _value, _value_data, _keyword_type) \
	JSO_SCHEMA_KW_SET_UINT_EX(_schema, _data, _key, _value, _value_data, _key, _keyword_type)

#define JSO_SCHEMA_KW_SET_UINT_NZ_EX( \
		_schema, _data, _key, _value, _value_data, _keyword_name, _keyword_type) \
	JSO_SCHEMA_KW_SET( \
			uint_nz, _schema, _data, _key, _value, _value_data, _keyword_name, _keyword_type)

#define JSO_SCHEMA_KW_SET_UINT_NZ(_schema, _data, _key, _value, _value_data, _keyword_type) \
	JSO_SCHEMA_KW_SET_UINT_NZ_EX(_schema, _data, _key, _value, _value_data, _key, _keyword_type)

#define JSO_SCHEMA_KW_SET_STR_EX( \
		_schema, _data, _key, _value, _value_data, _keyword_name, _keyword_type) \
	JSO_SCHEMA_KW_SET(str, _schema, _data, _key, _value, _value_data, _keyword_name, _keyword_type)

#define JSO_SCHEMA_KW_SET_STR(_schema, _data, _key, _value, _value_data, _keyword_type) \
	JSO_SCHEMA_KW_SET_STR_EX(_schema, _data, _key, _value, _value_data, _key, _keyword_type)

#define JSO_SCHEMA_KW_IS_SET(_value, _keyword_type) \
	JSO_BITSET_IS_SET(_value->keywords, JSO_SCHEMA_KEYWORD_##_keyword_type)

#define JSO_SCHEMA_VALUE_DATA_INIT(_schema, _data, _parent, _value_var, _value_type_var, \
		_value_type_name, _value_type_id, _value_data_var, _value_data_name) \
	jso_schema_value_data _value_data_var; \
	jso_schema_value_type _value_type_var = JSO_SCHEMA_VALUE_##_value_type_id; \
	_value_data_var._value_data_name = jso_calloc(1, sizeof(jso_schema_value_##_value_type_name)); \
	jso_schema_value_##_value_type_name *_value_data_name = _value_data_var._value_data_name; \
	jso_schema_value *_value_var = jso_schema_value_create( \
			_schema, #_value_type_name, _value_type_var, &_value_data_var); \
	do { \
		if (jso_schema_value_common_init(_schema, _data, _parent, _value_var, \
					(jso_schema_value_common *) _value_data_name, #_value_type_name) \
				== NULL) { \
			if (_value_var) \
				jso_free(_value_var); \
			jso_free(_value_data_name); \
			return NULL; \
		} \
	} while (0)

static inline jso_schema_value *jso_schema_value_create(jso_schema *schema, const char *type_name,
		jso_schema_value_type type, jso_schema_value_data *data)
{
	jso_schema_value *value = jso_malloc(sizeof(jso_schema_value));
	if (value == NULL) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_ALLOC,
				"Allocating value for type %s failed", type_name);
		return NULL;
	}
	value->type = type;
	value->data = *data;

	return value;
}

static jso_schema_value_common *jso_schema_value_common_init(jso_schema *schema, jso_value *data,
		jso_schema_value *parent, jso_schema_value *value, jso_schema_value_common *value_data,
		const char *type)
{
	if (value == NULL) {
		return NULL;
	}
	if (value_data == NULL) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_ALLOC,
				"Allocating value data for type %s failed", type);
		return NULL;
	}
	value_data->parent = parent;
	JSO_SCHEMA_KW_SET_STR(schema, data, description, value, value_data, COMMON_DESCRIPTION);
	JSO_SCHEMA_KW_SET_STR(schema, data, title, value, value_data, COMMON_TITLE);

	return value_data;
}

static jso_schema_value *jso_schema_parse_null(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	JSO_SCHEMA_VALUE_DATA_INIT(
			schema, data, parent, value, value_type, null, TYPE_NULL, value_data, nulval);

	return value;
}

static jso_schema_value *jso_schema_parse_boolean(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	JSO_SCHEMA_VALUE_DATA_INIT(
			schema, data, parent, value, value_type, boolean, TYPE_BOOLEAN, value_data, boolval);

	JSO_SCHEMA_KW_SET_BOOL_EX(schema, data, default, value, boolval, default_value, COMMON_DEFAULT);

	return value;
}

static jso_schema_value *jso_schema_parse_integer(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	JSO_SCHEMA_VALUE_DATA_INIT(
			schema, data, parent, value, value_type, integer, TYPE_INTEGER, value_data, intval);

	JSO_SCHEMA_KW_SET_INT_EX(schema, data, default, value, intval, default_value, COMMON_DEFAULT);
	JSO_SCHEMA_KW_SET_UINT_NZ_EX(
			schema, data, multipleOf, value, intval, multiple_of, INTEGER_MULTIPLE_OF);
	JSO_SCHEMA_KW_SET_INT(schema, data, minimum, value, intval, INTEGER_MINIMUM);
	JSO_SCHEMA_KW_SET_BOOL_EX(schema, data, exclusiveMinimum, value, intval, exclusive_minimum,
			INTEGER_EXCLUSIVE_MINIMUM);
	JSO_SCHEMA_KW_SET_INT(schema, data, maximum, value, intval, INTEGER_MAXIMUM);
	JSO_SCHEMA_KW_SET_BOOL_EX(schema, data, exclusiveMaximum, value, intval, exclusive_maximum,
			INTEGER_EXCLUSIVE_MAXIMUM);

	if (JSO_SCHEMA_KW_IS_SET(intval, INTEGER_EXCLUSIVE_MINIMUM)
			&& !JSO_SCHEMA_KW_IS_SET(intval, INTEGER_MINIMUM)) {
		jso_schema_error_set(schema, JSO_SCHEMA_ERROR_VALUE_DATA_DEPS,
				"The minimum must be set when exclusiveMinimum is set");
	}
	if (JSO_SCHEMA_KW_IS_SET(intval, INTEGER_EXCLUSIVE_MAXIMUM)
			&& !JSO_SCHEMA_KW_IS_SET(intval, INTEGER_MAXIMUM)) {
		jso_schema_error_set(schema, JSO_SCHEMA_ERROR_VALUE_DATA_DEPS,
				"The maximum must be set when exclusiveMaximum is set");
	}

	return value;
}

static jso_schema_value *jso_schema_parse_number(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	JSO_SCHEMA_VALUE_DATA_INIT(
			schema, data, parent, value, value_type, number, TYPE_NUMBER, value_data, numval);

	JSO_SCHEMA_KW_SET_NUM_EX(schema, data, default, value, numval, default_value, COMMON_DEFAULT);
	JSO_SCHEMA_KW_SET_UINT_NZ_EX(
			schema, data, multipleOf, value, numval, multiple_of, NUMBER_MULTIPLE_OF);
	JSO_SCHEMA_KW_SET_NUM(schema, data, minimum, value, numval, NUMBER_MINIMUM);
	JSO_SCHEMA_KW_SET_BOOL_EX(schema, data, exclusiveMinimum, value, numval, exclusive_minimum,
			NUMBER_EXCLUSIVE_MINIMUM);
	JSO_SCHEMA_KW_SET_NUM(schema, data, maximum, value, numval, NUMBER_MAXIMUM);
	JSO_SCHEMA_KW_SET_BOOL_EX(schema, data, exclusiveMaximum, value, numval, exclusive_maximum,
			NUMBER_EXCLUSIVE_MAXIMUM);

	if (JSO_SCHEMA_KW_IS_SET(numval, NUMBER_EXCLUSIVE_MINIMUM)
			&& !JSO_SCHEMA_KW_IS_SET(numval, NUMBER_MINIMUM)) {
		jso_schema_error_set(schema, JSO_SCHEMA_ERROR_VALUE_DATA_DEPS,
				"The minimum must be set when exclusiveMinimum is set");
	}
	if (JSO_SCHEMA_KW_IS_SET(numval, NUMBER_EXCLUSIVE_MAXIMUM)
			&& !JSO_SCHEMA_KW_IS_SET(numval, NUMBER_MAXIMUM)) {
		jso_schema_error_set(schema, JSO_SCHEMA_ERROR_VALUE_DATA_DEPS,
				"The maximum must be set when exclusiveMaximum is set");
	}

	return value;
}

static jso_schema_value *jso_schema_parse_string(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	JSO_SCHEMA_VALUE_DATA_INIT(
			schema, data, parent, value, value_type, string, TYPE_STRING, value_data, strval);

	JSO_SCHEMA_KW_SET_STR_EX(schema, data, default, value, strval, default_value, COMMON_DEFAULT);
	JSO_SCHEMA_KW_SET_UINT_EX(
			schema, data, maxLength, value, strval, max_length, STRING_MAX_LENGTH);
	JSO_SCHEMA_KW_SET_UINT_EX(
			schema, data, minLength, value, strval, min_length, STRING_MIN_LENGTH);

	return value;
}

static jso_schema_value *jso_schema_parse_array(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	JSO_SCHEMA_VALUE_DATA_INIT(
			schema, data, parent, value, value_type, array, TYPE_ARRAY, value_data, arrval);

	JSO_SCHEMA_KW_SET_UNION_EX(schema, data, additionalItems, value, arrval, additional_items,
			ARRAY_ADDITIONAL_ITEMS, TYPE_BOOLEAN, TYPE_ARRAY_OF_SCHEMA_OBJECTS);
	JSO_SCHEMA_KW_SET_UNION(schema, data, items, value, arrval, ARRAY_ITEMS, TYPE_SCHEMA_OBJECT,
			TYPE_ARRAY_OF_SCHEMA_OBJECTS);
	JSO_SCHEMA_KW_SET_BOOL_EX(
			schema, data, uniqueItems, value, arrval, unique_items, ARRAY_UNIQUE_ITEMS);
	JSO_SCHEMA_KW_SET_UINT_EX(schema, data, maxItems, value, arrval, max_items, ARRAY_MAX_ITEMS);
	JSO_SCHEMA_KW_SET_UINT_EX(schema, data, minItems, value, arrval, min_items, ARRAY_MIN_ITEMS);

	return jso_schema_value_create(schema, "array", value_type, &value_data);
}

static jso_schema_value *jso_schema_parse_object(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	JSO_SCHEMA_VALUE_DATA_INIT(
			schema, data, parent, value, value_type, object, TYPE_OBJECT, value_data, objval);

	JSO_SCHEMA_KW_SET_UINT_EX(
			schema, data, minProperties, value, objval, max_properties, OBJECT_MIN_PROPERTIES);
	JSO_SCHEMA_KW_SET_UINT_EX(
			schema, data, maxProperties, value, objval, max_properties, OBJECT_MAX_PROPERTIES);
	JSO_SCHEMA_KW_SET_UNION_EX(schema, data, additionalProperties, value, objval,
			additional_properties, OBJECT_ADDITIONAL_PROPERTIES, TYPE_BOOLEAN, TYPE_SCHEMA_OBJECT);

	return value;
}

static jso_schema_value *jso_schema_parse_value(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	jso_string *type = jso_schema_data_get_str_required(schema, data, "type");

	if (type == NULL) {
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
