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
#include "jso_schema_keyword.h"
#include "jso_schema_value.h"

#include "jso.h"

#define JSO_SCHEMA_VALUE_INIT(_schema, _data, _parent, _type_name, _kw_type) \
	jso_schema_value_init(_schema, _data, _parent, #_type_name, \
			sizeof(jso_schema_value_##_type_name), JSO_SCHEMA_KW_TYPE(_kw_type))

static jso_schema_value *jso_schema_value_init(jso_schema *schema, jso_value *data,
		jso_schema_value *parent, const char *type_name, size_t value_size,
		jso_schema_keyword_type keyword_type)
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

	JSO_SCHEMA_VALUE_DATA_COMMON_P(value) = value_data;
	JSO_SCHEMA_VALUE_TYPE_P(value) = keyword_type;

	return value;
}

static jso_schema_value *jso_schema_value_parse_null(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	return JSO_SCHEMA_VALUE_INIT(schema, data, parent, null, TYPE_NULL);
}

static jso_schema_value *jso_schema_value_parse_boolean(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	jso_schema_value *value = JSO_SCHEMA_VALUE_INIT(schema, data, parent, boolean, TYPE_BOOLEAN);
	jso_schema_value_boolean *boolval = JSO_SCHEMA_VALUE_DATA_BOOL_P(value);

	JSO_SCHEMA_KW_SET_BOOL_EX(schema, data, default, value, boolval, default_value);

	return value;
}

static jso_schema_value *jso_schema_value_parse_integer(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	jso_schema_value *value = JSO_SCHEMA_VALUE_INIT(schema, data, parent, integer, TYPE_INTEGER);
	jso_schema_value_integer *intval = JSO_SCHEMA_VALUE_DATA_INT_P(value);

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

static jso_schema_value *jso_schema_value_parse_number(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	jso_schema_value *value = JSO_SCHEMA_VALUE_INIT(schema, data, parent, number, TYPE_NUMBER);
	jso_schema_value_number *numval = JSO_SCHEMA_VALUE_DATA_NUM_P(value);

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

static jso_schema_value *jso_schema_value_parse_string(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	jso_schema_value *value = JSO_SCHEMA_VALUE_INIT(schema, data, parent, string, TYPE_STRING);
	jso_schema_value_string *strval = JSO_SCHEMA_VALUE_DATA_STR_P(value);

	JSO_SCHEMA_KW_SET_STR_EX(schema, data, default, value, strval, default_value);
	JSO_SCHEMA_KW_SET_UINT_EX(schema, data, maxLength, value, strval, max_length);
	JSO_SCHEMA_KW_SET_UINT_EX(schema, data, minLength, value, strval, min_length);
	JSO_SCHEMA_KW_SET_RE(schema, data, pattern, value, strval);

	return value;
}

static jso_schema_value *jso_schema_value_parse_array(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	jso_schema_value *value = JSO_SCHEMA_VALUE_INIT(schema, data, parent, array, TYPE_ARRAY);
	jso_schema_value_array *arrval = JSO_SCHEMA_VALUE_DATA_ARR_P(value);

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

static jso_schema_value *jso_schema_value_parse_object(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	jso_schema_value *value = JSO_SCHEMA_VALUE_INIT(schema, data, parent, array, TYPE_OBJECT);
	jso_schema_value_object *objval = JSO_SCHEMA_VALUE_DATA_OBJ_P(value);

	JSO_SCHEMA_KW_SET_EX(schema, data, default, value, objval, default_value, TYPE_OBJECT);
	JSO_SCHEMA_KW_SET_UINT_EX(schema, data, minProperties, value, objval, max_properties);
	JSO_SCHEMA_KW_SET_UINT_EX(schema, data, maxProperties, value, objval, max_properties);
	JSO_SCHEMA_KW_SET_UNION_EX(schema, data, additionalProperties, value, objval,
			additional_properties, TYPE_BOOLEAN, TYPE_SCHEMA_OBJECT);
	JSO_SCHEMA_KW_SET(schema, data, properties, value, objval, TYPE_OBJECT_OF_SCHEMA_OBJECTS);
	JSO_SCHEMA_KW_SET_EX(schema, data, patternProperties, value, objval, pattern_properties,
			TYPE_REGEXP_OBJECT_OF_SCHEMA_OBJECTS);

	return value;
}

jso_schema_value *jso_schema_value_parse(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	jso_string *type = jso_schema_data_get_str(schema, data, "type");

	if (type == NULL) {
		// TODO: support empty schema that always pass
		return NULL;
	}

	if (jso_string_equals_to_cstr(type, "string")) {
		return jso_schema_value_parse_string(schema, data, parent);
	}
	if (jso_string_equals_to_cstr(type, "number")) {
		return jso_schema_value_parse_number(schema, data, parent);
	}
	if (jso_string_equals_to_cstr(type, "integer")) {
		return jso_schema_value_parse_integer(schema, data, parent);
	}
	if (jso_string_equals_to_cstr(type, "boolean")) {
		return jso_schema_value_parse_boolean(schema, data, parent);
	}
	if (jso_string_equals_to_cstr(type, "array")) {
		return jso_schema_value_parse_array(schema, data, parent);
	}
	if (jso_string_equals_to_cstr(type, "object")) {
		return jso_schema_value_parse_object(schema, data, parent);
	}
	if (jso_string_equals_to_cstr(type, "null")) {
		return jso_schema_value_parse_null(schema, data, parent);
	}

	jso_schema_error_format(
			schema, JSO_SCHEMA_ERROR_TYPE_UNKNOWN, "Unknown type %s", JSO_STRING_VAL(type));

	return NULL;
}

typedef void (*jso_schema_value_free_callback)(jso_schema_value *val);

static void jso_schema_value_free_common(jso_schema_value *val)
{
	jso_schema_value_common *intval = JSO_SCHEMA_VALUE_DATA_COMMON_P(val);
	if (intval->re != NULL) {
		jso_re_code_free(intval->re);
	}
	jso_schema_keyword_free(&intval->title);
	jso_schema_keyword_free(&intval->description);
}

static void jso_schema_value_free_null(jso_schema_value *val)
{
	jso_free(JSO_SCHEMA_VALUE_DATA_NULL_P(val));
	JSO_SCHEMA_VALUE_DATA_NULL_P(val) = NULL;
}

static void jso_schema_value_free_boolean(jso_schema_value *val)
{
	jso_schema_value_boolean *boolval = JSO_SCHEMA_VALUE_DATA_BOOL_P(val);
	jso_schema_keyword_free(&boolval->default_value);
	jso_free(boolval);
	JSO_SCHEMA_VALUE_DATA_BOOL_P(val) = NULL;
}

static void jso_schema_value_free_integer(jso_schema_value *val)
{
	jso_schema_value_integer *intval = JSO_SCHEMA_VALUE_DATA_INT_P(val);
	jso_schema_keyword_free(&intval->exclusive_maximum);
	jso_schema_keyword_free(&intval->exclusive_minimum);
	jso_schema_keyword_free(&intval->maximum);
	jso_schema_keyword_free(&intval->minimum);
	jso_schema_keyword_free(&intval->multiple_of);
	jso_schema_keyword_free(&intval->default_value);
	jso_free(intval);
	JSO_SCHEMA_VALUE_DATA_INT_P(val) = NULL;
}

static void jso_schema_value_free_number(jso_schema_value *val)
{
	jso_schema_value_number *numval = JSO_SCHEMA_VALUE_DATA_NUM_P(val);
	jso_schema_keyword_free(&numval->exclusive_maximum);
	jso_schema_keyword_free(&numval->exclusive_minimum);
	jso_schema_keyword_free(&numval->maximum);
	jso_schema_keyword_free(&numval->minimum);
	jso_schema_keyword_free(&numval->multiple_of);
	jso_schema_keyword_free(&numval->default_value);
	jso_free(numval);
	JSO_SCHEMA_VALUE_DATA_NUM_P(val) = NULL;
}

static void jso_schema_value_free_string(jso_schema_value *val)
{
	jso_schema_value_string *strval = JSO_SCHEMA_VALUE_DATA_STR_P(val);
	jso_schema_keyword_free(&strval->min_length);
	jso_schema_keyword_free(&strval->max_length);
	jso_free(strval);
	JSO_SCHEMA_VALUE_DATA_STR_P(val) = NULL;
}

static void jso_schema_value_free_array(jso_schema_value *val)
{
	jso_schema_value_array *arrval = JSO_SCHEMA_VALUE_DATA_ARR_P(val);
	jso_schema_keyword_free(&arrval->additional_items);
	jso_schema_keyword_free(&arrval->items);
	jso_schema_keyword_free(&arrval->max_items);
	jso_schema_keyword_free(&arrval->min_items);
	jso_schema_keyword_free(&arrval->unique_items);
	jso_free(arrval);
	JSO_SCHEMA_VALUE_DATA_ARR_P(val) = NULL;
}

static void jso_schema_value_free_object(jso_schema_value *val)
{
	jso_schema_value_object *objval = JSO_SCHEMA_VALUE_DATA_OBJ_P(val);
	jso_schema_keyword_free(&objval->additional_properties);
	jso_schema_keyword_free(&objval->max_properties);
	jso_schema_keyword_free(&objval->min_properties);
	jso_schema_keyword_free(&objval->properties);
	jso_schema_keyword_free(&objval->required);
	jso_free(objval);
	JSO_SCHEMA_VALUE_DATA_OBJ_P(val) = NULL;
}

static const jso_schema_value_free_callback schema_value_free_callbacks[] = {
	[JSO_SCHEMA_VALUE_TYPE_NULL] = jso_schema_value_free_null,
	[JSO_SCHEMA_VALUE_TYPE_BOOLEAN] = jso_schema_value_free_boolean,
	[JSO_SCHEMA_VALUE_TYPE_INTEGER] = jso_schema_value_free_integer,
	[JSO_SCHEMA_VALUE_TYPE_NUMBER] = jso_schema_value_free_number,
	[JSO_SCHEMA_VALUE_TYPE_STRING] = jso_schema_value_free_string,
	[JSO_SCHEMA_VALUE_TYPE_ARRAY] = jso_schema_value_free_array,
	[JSO_SCHEMA_VALUE_TYPE_OBJECT] = jso_schema_value_free_object,
};

void jso_schema_value_free(jso_schema_value *val)
{
	jso_schema_value_free_common(val);
	schema_value_free_callbacks[JSO_SCHEMA_VALUE_TYPE_P(val)](val);
	JSO_SCHEMA_VALUE_TYPE_P(val) = 0;
}
