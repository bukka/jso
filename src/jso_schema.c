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

#if 0 /* disable as currently not used */
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
#endif

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
		*keyword = jso_string_dup(keyword_str);
		JSO_BITSET_SET(*keywords, keyword_type);
	}
	return JSO_SUCCESS;
}

#define JSO_SCHEMA_KW_SET(_type_name, _schema, _data, _key, _value, _keyword_name, _keyword_type) \
	do { \
		if (jso_schema_keyword_set_##_type_name(_schema, _data, #_key, &_value->_keyword_name, \
					&_value->keywords, JSO_SCHEMA_KEYWORD_##_keyword_type) \
				== JSO_FAILURE) { \
			return NULL; \
		} \
	} while (0)

#define JSO_SCHEMA_KW_SET_BOOL_EX(_schema, _data, _key, _value, _keyword_name, _keyword_type) \
	JSO_SCHEMA_KW_SET(boolean, _schema, _data, _key, _value, _keyword_name, _keyword_type)

#define JSO_SCHEMA_KW_SET_BOOL(_schema, _data, _key, _value, _keyword_type) \
	JSO_SCHEMA_KW_SET_BOOL_EX(_schema, _data, _key, _value, _key, _keyword_type)

#define JSO_SCHEMA_KW_SET_NUM_EX(_schema, _data, _key, _value, _keyword_name, _keyword_type) \
	JSO_SCHEMA_KW_SET(number, _schema, _data, _key, _value, _keyword_name, _keyword_type)

#define JSO_SCHEMA_KW_SET_NUM(_schema, _data, _key, _value, _keyword_type) \
	JSO_SCHEMA_KW_SET_NUM_EX(_schema, _data, _key, _value, _key, _keyword_type)

#if 0 /* disable as currently not used */
#define JSO_SCHEMA_KW_SET_INT_EX(_schema, _data, _key, _value, _keyword_name, _keyword_type) \
	JSO_SCHEMA_KW_SET(int, _schema, _data, _key, _value, _keyword_name, _keyword_type)

#define JSO_SCHEMA_KW_SET_INT(_schema, _data, _key, _value, _keyword_type) \
	JSO_SCHEMA_KW_SET_INT_EX(_schema, _data, _key, _value, _key, _keyword_type)
#endif

#define JSO_SCHEMA_KW_SET_UINT_EX(_schema, _data, _key, _value, _keyword_name, _keyword_type) \
	JSO_SCHEMA_KW_SET(uint, _schema, _data, _key, _value, _keyword_name, _keyword_type)

#define JSO_SCHEMA_KW_SET_UINT(_schema, _data, _key, _value, _keyword_type) \
	JSO_SCHEMA_KW_SET_UINT_EX(_schema, _data, _key, _value, _key, _keyword_type)

#define JSO_SCHEMA_KW_SET_UINT_NZ_EX(_schema, _data, _key, _value, _keyword_name, _keyword_type) \
	JSO_SCHEMA_KW_SET(uint_nz, _schema, _data, _key, _value, _keyword_name, _keyword_type)

#define JSO_SCHEMA_KW_SET_UINT_NZ(_schema, _data, _key, _value, _keyword_type) \
	JSO_SCHEMA_KW_SET_UINT_NZ_EX(_schema, _data, _key, _value, _key, _keyword_type)

#define JSO_SCHEMA_KW_SET_STR_EX(_schema, _data, _key, _value, _keyword_name, _keyword_type) \
	JSO_SCHEMA_KW_SET(str, _schema, _data, _key, _value, _keyword_name, _keyword_type)

#define JSO_SCHEMA_KW_SET_STR(_schema, _data, _key, _value, _keyword_type) \
	JSO_SCHEMA_KW_SET_STR_EX(_schema, _data, _key, _value, _key, _keyword_type)

#define JSO_SCHEMA_KW_IS_SET(_value, _keyword_type) \
	JSO_BITSET_IS_SET(_value->keywords, JSO_SCHEMA_KEYWORD_##_keyword_type)

#define JSO_SCHEMA_VALUE_DATA_INIT(_schema, _data, _parent, _value_type_var, _value_type_name, \
		_value_type_id, _value_data_var, _value_data_name) \
	jso_schema_value_data _value_data_var; \
	jso_schema_value_type _value_type_var; \
	_value_data_var._value_data_name = jso_calloc(1, sizeof(jso_schema_value_##_value_type_name)); \
	jso_schema_value_##_value_type_name *_value_data_name = _value_data_var._value_data_name; \
	(void) _value_data_name; \
	if (jso_schema_value_common_init(_schema, _data, _parent, \
				(jso_schema_value_common *) _value_data_var._value_data_name, #_value_type_name) \
			== NULL) { \
		return NULL; \
	} \
	_value_type_var = JSO_SCHEMA_##_value_type_id

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
		jso_schema_value *parent, jso_schema_value_common *value, const char *type)
{
	if (value == NULL) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_ALLOC,
				"Allocating value data for type %s failed", type);
		return NULL;
	}
	value->parent = parent;
	JSO_SCHEMA_KW_SET_STR(schema, data, description, value, COMMON_DESCRIPTION);
	JSO_SCHEMA_KW_SET_STR(schema, data, title, value, COMMON_TITLE);

	return value;
}

static jso_schema_value *jso_schema_parse_null(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	JSO_SCHEMA_VALUE_DATA_INIT(
			schema, data, parent, value_type, null, TYPE_NULL, value_data, nulval);

	return jso_schema_value_create(schema, "null", value_type, &value_data);
}

static jso_schema_value *jso_schema_parse_boolean(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	JSO_SCHEMA_VALUE_DATA_INIT(
			schema, data, parent, value_type, boolean, TYPE_BOOLEAN, value_data, boolval);

	JSO_SCHEMA_KW_SET_BOOL_EX(schema, data, default, boolval, default_value, COMMON_DEFAULT);

	return jso_schema_value_create(schema, "boolean", value_type, &value_data);
}

static jso_schema_value *jso_schema_parse_number(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	JSO_SCHEMA_VALUE_DATA_INIT(
			schema, data, parent, value_type, number, TYPE_NUMBER, value_data, numval);

	JSO_SCHEMA_KW_SET_NUM_EX(schema, data, default, numval, default_value, COMMON_DEFAULT);
	JSO_SCHEMA_KW_SET_UINT_NZ_EX(schema, data, multipleOf, numval, multiple_of, NUMBER_MULTIPLE_OF);
	JSO_SCHEMA_KW_SET_NUM(schema, data, minimum, numval, NUMBER_MINIMUM);
	JSO_SCHEMA_KW_SET_BOOL_EX(
			schema, data, exclusiveMinimum, numval, exclusive_minimum, NUMBER_EXCLUSIVE_MINIMUM);
	JSO_SCHEMA_KW_SET_NUM(schema, data, maximum, numval, NUMBER_MAXIMUM);
	JSO_SCHEMA_KW_SET_BOOL_EX(
			schema, data, exclusiveMaximum, numval, exclusive_maximum, NUMBER_EXCLUSIVE_MAXIMUM);

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

	return jso_schema_value_create(schema, "number", value_type, &value_data);
}

static jso_schema_value *jso_schema_parse_string(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	JSO_SCHEMA_VALUE_DATA_INIT(
			schema, data, parent, value_type, string, TYPE_STRING, value_data, strval);

	JSO_SCHEMA_KW_SET_STR_EX(schema, data, default, strval, default_value, COMMON_DEFAULT);
	JSO_SCHEMA_KW_SET_UINT_EX(schema, data, maxLength, strval, max_length, STRING_MAX_LENGTH);
	JSO_SCHEMA_KW_SET_UINT_EX(schema, data, minLength, strval, min_length, STRING_MIN_LENGTH);

	return jso_schema_value_create(schema, "string", value_type, &value_data);
}

static jso_schema_value *jso_schema_parse_array(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{

	return NULL;
}

static jso_schema_value *jso_schema_parse_object(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{

	return NULL;
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
			schema, JSO_SCHEMA_ERROR_TYPE_UNKNOWN, "Unknown type %s", JSO_STRING_VAL_P(type));

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
		schema->id = jso_string_dup(id);
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
	jso_schema_clear(schema);
	jso_free(schema);
}
