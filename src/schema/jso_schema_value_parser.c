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
#include "jso_schema_keyword_freer.h"
#include "jso_schema_value.h"

#include "jso.h"

static jso_schema_value *jso_schema_value_parse_any(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	return JSO_SCHEMA_VALUE_INIT(schema, data, parent, null, TYPE_ANY, true);
}

static jso_schema_value *jso_schema_value_parse_null(
		jso_schema *schema, jso_value *data, jso_schema_value *parent, jso_bool init_keywords)
{
	return JSO_SCHEMA_VALUE_INIT(schema, data, parent, null, TYPE_NULL, init_keywords);
}

static jso_schema_value *jso_schema_value_parse_boolean(
		jso_schema *schema, jso_value *data, jso_schema_value *parent, jso_bool init_keywords)
{
	return JSO_SCHEMA_VALUE_INIT(schema, data, parent, boolean, TYPE_BOOLEAN, init_keywords);
}

static jso_schema_value *jso_schema_value_parse_integer(
		jso_schema *schema, jso_value *data, jso_schema_value *parent, jso_bool init_keywords)
{
	jso_schema_value *value
			= JSO_SCHEMA_VALUE_INIT(schema, data, parent, integer, TYPE_INTEGER, init_keywords);
	if (value == NULL) {
		return NULL;
	}
	jso_schema_value_integer *intval = JSO_SCHEMA_VALUE_DATA_INT_P(value);

	JSO_SCHEMA_KW_SET_UINT_NZ_EX(schema, data, multipleOf, value, intval, multiple_of);
	JSO_SCHEMA_KW_SET_INT(schema, data, minimum, value, intval);
	JSO_SCHEMA_KW_SET_BOOL_EX(schema, data, exclusiveMinimum, value, intval, exclusive_minimum);
	JSO_SCHEMA_KW_SET_INT(schema, data, maximum, value, intval);
	JSO_SCHEMA_KW_SET_BOOL_EX(schema, data, exclusiveMaximum, value, intval, exclusive_maximum);

	if (JSO_SCHEMA_KW_IS_SET(intval->exclusive_minimum) && !JSO_SCHEMA_KW_IS_SET(intval->minimum)) {
		jso_schema_error_set(schema, JSO_SCHEMA_ERROR_VALUE_DATA_DEPS,
				"The minimum must be set when exclusiveMinimum is set");
		jso_schema_value_free(value);
		return NULL;
	}
	if (JSO_SCHEMA_KW_IS_SET(intval->exclusive_maximum) && !JSO_SCHEMA_KW_IS_SET(intval->maximum)) {
		jso_schema_error_set(schema, JSO_SCHEMA_ERROR_VALUE_DATA_DEPS,
				"The maximum must be set when exclusiveMaximum is set");
		jso_schema_value_free(value);
		return NULL;
	}

	return value;
}

static jso_schema_value *jso_schema_value_parse_number(
		jso_schema *schema, jso_value *data, jso_schema_value *parent, jso_bool init_keywords)
{
	jso_schema_value *value
			= JSO_SCHEMA_VALUE_INIT(schema, data, parent, number, TYPE_NUMBER, init_keywords);
	if (value == NULL) {
		return NULL;
	}
	jso_schema_value_number *numval = JSO_SCHEMA_VALUE_DATA_NUM_P(value);

	JSO_SCHEMA_KW_SET_UINT_NZ_EX(schema, data, multipleOf, value, numval, multiple_of);
	JSO_SCHEMA_KW_SET_NUM(schema, data, minimum, value, numval);
	JSO_SCHEMA_KW_SET_BOOL_EX(schema, data, exclusiveMinimum, value, numval, exclusive_minimum);
	JSO_SCHEMA_KW_SET_NUM(schema, data, maximum, value, numval);
	JSO_SCHEMA_KW_SET_BOOL_EX(schema, data, exclusiveMaximum, value, numval, exclusive_maximum);

	if (JSO_SCHEMA_KW_IS_SET(numval->exclusive_minimum) && !JSO_SCHEMA_KW_IS_SET(numval->minimum)) {
		jso_schema_error_set(schema, JSO_SCHEMA_ERROR_VALUE_DATA_DEPS,
				"The minimum must be set when exclusiveMinimum is set");
		jso_schema_value_free(value);
		return NULL;
	}
	if (JSO_SCHEMA_KW_IS_SET(numval->exclusive_maximum) && !JSO_SCHEMA_KW_IS_SET(numval->maximum)) {
		jso_schema_error_set(schema, JSO_SCHEMA_ERROR_VALUE_DATA_DEPS,
				"The maximum must be set when exclusiveMaximum is set");
		jso_schema_value_free(value);
		return NULL;
	}

	return value;
}

static jso_schema_value *jso_schema_value_parse_string(
		jso_schema *schema, jso_value *data, jso_schema_value *parent, jso_bool init_keywords)
{
	jso_schema_value *value
			= JSO_SCHEMA_VALUE_INIT(schema, data, parent, string, TYPE_STRING, init_keywords);
	if (value == NULL) {
		return NULL;
	}
	jso_schema_value_string *strval = JSO_SCHEMA_VALUE_DATA_STR_P(value);

	JSO_SCHEMA_KW_SET_UINT_EX(schema, data, maxLength, value, strval, max_length);
	JSO_SCHEMA_KW_SET_UINT_EX(schema, data, minLength, value, strval, min_length);
	JSO_SCHEMA_KW_SET_RE(schema, data, pattern, value, strval);

	return value;
}

static jso_schema_value *jso_schema_value_parse_array(
		jso_schema *schema, jso_value *data, jso_schema_value *parent, jso_bool init_keywords)
{
	jso_schema_value *value
			= JSO_SCHEMA_VALUE_INIT(schema, data, parent, array, TYPE_ARRAY, init_keywords);
	if (value == NULL) {
		return NULL;
	}
	jso_schema_value_array *arrval = JSO_SCHEMA_VALUE_DATA_ARR_P(value);

	JSO_SCHEMA_KW_SET_UNION_EX(schema, data, additionalItems, value, arrval, additional_items,
			TYPE_BOOLEAN, TYPE_ARRAY_OF_SCHEMA_OBJECTS);
	JSO_SCHEMA_KW_SET_UNION(
			schema, data, items, value, arrval, TYPE_SCHEMA_OBJECT, TYPE_ARRAY_OF_SCHEMA_OBJECTS);
	JSO_SCHEMA_KW_SET_BOOL_EX(schema, data, uniqueItems, value, arrval, unique_items);
	JSO_SCHEMA_KW_SET_UINT_EX(schema, data, maxItems, value, arrval, max_items);
	JSO_SCHEMA_KW_SET_UINT_EX(schema, data, minItems, value, arrval, min_items);

	return value;
}

static jso_schema_value *jso_schema_value_parse_object(
		jso_schema *schema, jso_value *data, jso_schema_value *parent, jso_bool init_keywords)
{
	jso_schema_value *value
			= JSO_SCHEMA_VALUE_INIT(schema, data, parent, object, TYPE_OBJECT, init_keywords);
	if (value == NULL) {
		return NULL;
	}
	jso_schema_value_object *objval = JSO_SCHEMA_VALUE_DATA_OBJ_P(value);

	JSO_SCHEMA_KW_SET_UINT_EX(schema, data, minProperties, value, objval, min_properties);
	JSO_SCHEMA_KW_SET_UINT_EX(schema, data, maxProperties, value, objval, max_properties);
	JSO_SCHEMA_KW_SET_UNION_EX(schema, data, additionalProperties, value, objval,
			additional_properties, TYPE_BOOLEAN, TYPE_SCHEMA_OBJECT);
	JSO_SCHEMA_KW_SET_OBJ_OF_SCHEMA_OBJS(schema, data, properties, value, objval);
	JSO_SCHEMA_KW_SET_EX(schema, data, patternProperties, value, objval, pattern_properties,
			TYPE_REGEXP_OBJECT_OF_SCHEMA_OBJECTS);
	JSO_SCHEMA_KW_SET_WITH_FLAGS(schema, data, dependencies, value, objval,
			TYPE_OBJECT_OF_SCHEMA_OBJECTS_OR_ARRAY_OF_STRINGS, JSO_SCHEMA_KEYWORD_FLAG_NOT_EMPTY);

	return value;
}

typedef jso_schema_value *(*jso_schema_value_parse_type_func)(
		jso_schema *schema, jso_value *data, jso_schema_value *parent, jso_bool init_keyword);

const jso_schema_value_parse_type_func value_parse_funcs[] = {
	jso_schema_value_parse_null,
	jso_schema_value_parse_boolean,
	jso_schema_value_parse_integer,
	jso_schema_value_parse_number,
	jso_schema_value_parse_string,
	jso_schema_value_parse_array,
	jso_schema_value_parse_object,
};

static jso_schema_value *jso_schema_value_parse_by_string_type(jso_schema *schema, jso_value *data,
		jso_schema_value *parent, jso_string *type, jso_bool init_keywords)
{
	if (JSO_STRING_LEN(type) == 0) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_TYPE_INVALID,
				"Invalid schema type because it is an empty string");
		return NULL;
	}

	switch (JSO_STRING_VAL(type)[0]) {
		case 's':
			if (jso_string_equals_to_cstr(type, "string")) {
				return jso_schema_value_parse_string(schema, data, parent, init_keywords);
			}
			break;
		case 'n':
			if (jso_string_equals_to_cstr(type, "number")) {
				return jso_schema_value_parse_number(schema, data, parent, init_keywords);
			}
			if (jso_string_equals_to_cstr(type, "null")) {
				return jso_schema_value_parse_null(schema, data, parent, init_keywords);
			}
			break;
		case 'i':
			if (jso_string_equals_to_cstr(type, "integer")) {
				return jso_schema_value_parse_integer(schema, data, parent, init_keywords);
			}
			break;
		case 'b':
			if (jso_string_equals_to_cstr(type, "boolean")) {
				return jso_schema_value_parse_boolean(schema, data, parent, init_keywords);
			}
			break;
		case 'a':
			if (jso_string_equals_to_cstr(type, "array")) {
				return jso_schema_value_parse_array(schema, data, parent, init_keywords);
			}
			break;
		case 'o':
			if (jso_string_equals_to_cstr(type, "object")) {
				return jso_schema_value_parse_object(schema, data, parent, init_keywords);
			}
			break;
	}

	jso_schema_error_format(
			schema, JSO_SCHEMA_ERROR_TYPE_INVALID, "Invalid schema type %s", JSO_STRING_VAL(type));

	return NULL;
}

static jso_schema_value *jso_schema_value_parse_any_type(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	size_t func_count = sizeof(value_parse_funcs) / sizeof(jso_schema_value_parse_type_func);
	// create anyOf subschemas for each listed type
	jso_schema_array *any_of_arr = jso_schema_array_alloc(func_count);
	// create parent common value that will contain subschema for each type
	jso_schema_value *schema_value = jso_schema_value_parse_any(schema, data, parent);

	for (int i = 0; i < func_count; i++) {
		jso_schema_value *schema_type_value = value_parse_funcs[i](schema, data, parent, false);
		if (schema_type_value == NULL) {
			jso_schema_array_free(any_of_arr);
			jso_schema_value_free(schema_value);
			return NULL;
		}
		jso_schema_array_append(any_of_arr, schema_type_value);
	}

	// save subschema to the
	jso_schema_keyword *any_of_kw = &JSO_SCHEMA_VALUE_DATA_COMMON_P(schema_value)->any_of;
	JSO_SCHEMA_KEYWORD_FLAGS_P(any_of_kw) = JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_DATA_ARR_SCHEMA_OBJ_P(any_of_kw) = any_of_arr;

	return schema_value;
}

jso_schema_value *jso_schema_value_parse(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	jso_value *val = jso_schema_data_get_value_fast(schema, data, "type", 0);

	if (val == NULL) {
		return jso_schema_value_parse_any_type(schema, data, parent);
	} else if (JSO_TYPE_P(val) == JSO_TYPE_STRING) {
		return jso_schema_value_parse_by_string_type(schema, data, parent, JSO_STR_P(val), true);
	}
	if (jso_schema_data_check_type(schema, "type", val, JSO_TYPE_STRING, JSO_TYPE_ARRAY, true)
			== JSO_FAILURE) {
		return NULL;
	}
	// type is an array so we need to check if it is an array of unique strings
	jso_array *arr = JSO_ARRVAL_P(val);
	if (jso_schema_keyword_validate_array_of_strings(
				schema, "type", arr, JSO_SCHEMA_KEYWORD_FLAG_UNIQUE)
			== JSO_FAILURE) {
		return NULL;
	}

	// create parent common value that will contain subschema for each type
	jso_schema_value *schema_value = jso_schema_value_parse_any(schema, data, parent);

	// create anyOf subschemas for each listed type
	jso_schema_array *any_of_arr = jso_schema_array_alloc(JSO_ARRAY_LEN(arr));
	jso_value *item;
	JSO_ARRAY_FOREACH(arr, item)
	{
		JSO_ASSERT_EQ(JSO_TYPE_P(item), JSO_TYPE_STRING);
		jso_schema_value *schema_type_value = jso_schema_value_parse_by_string_type(
				schema, data, schema_value, JSO_STR_P(item), false);
		if (schema_type_value == NULL) {
			jso_schema_array_free(any_of_arr);
			jso_schema_value_free(schema_value);
			return NULL;
		}
		jso_schema_array_append(any_of_arr, schema_type_value);
	}
	JSO_ARRAY_FOREACH_END;

	// save subschema to the
	jso_schema_keyword *any_of_kw = &JSO_SCHEMA_VALUE_DATA_COMMON_P(schema_value)->any_of;
	JSO_SCHEMA_KEYWORD_FLAGS_P(any_of_kw) = JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_DATA_ARR_SCHEMA_OBJ_P(any_of_kw) = any_of_arr;

	return schema_value;
}
