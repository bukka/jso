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
#include "jso_schema_keyword.h"
#include "jso_schema_value.h"

#include "jso.h"
#include "jso_number.h"

#include <math.h>

static jso_rc jso_schema_validation_value_type_error_ex(jso_schema *schema, jso_value_type expected,
		jso_value_type expected_alternative, jso_value_type actual)
{
	jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_TYPE,
			"Invalid validation type, expected %s or %s but received %s",
			jso_value_type_to_string(expected), jso_value_type_to_string(expected_alternative),
			jso_value_type_to_string(actual));
	return JSO_FAILURE;
}

static jso_rc jso_schema_validation_value_type_error(
		jso_schema *schema, jso_value_type expected, jso_value_type actual)
{
	jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_TYPE,
			"Invalid validation type, expected %s but received %s",
			jso_value_type_to_string(expected), jso_value_type_to_string(actual));
	return JSO_FAILURE;
}

typedef jso_rc (*jso_schema_validation_value_callback)(
		jso_schema *schema, jso_schema_value *value, jso_value *instance);

jso_rc jso_schema_validation_value_null(
		jso_schema *schema, jso_schema_value *value, jso_value *instance)
{
	if (JSO_TYPE_P(instance) != JSO_TYPE_NULL) {
		return jso_schema_validation_value_type_error(schema, JSO_TYPE_NULL, JSO_TYPE_P(instance));
	}

	return JSO_SUCCESS;
}

jso_rc jso_schema_validation_value_boolean(
		jso_schema *schema, jso_schema_value *value, jso_value *instance)
{
	if (JSO_TYPE_P(instance) != JSO_TYPE_BOOL) {
		return jso_schema_validation_value_type_error(schema, JSO_TYPE_BOOL, JSO_TYPE_P(instance));
	}

	return JSO_SUCCESS;
}

jso_rc jso_schema_validation_value_integer(
		jso_schema *schema, jso_schema_value *value, jso_value *instance)
{
	jso_int inst_ival;

	if (JSO_TYPE_P(instance) == JSO_TYPE_INT) {
		inst_ival = JSO_IVAL_P(instance);
	}
	if (JSO_TYPE_P(instance) == JSO_TYPE_DOUBLE) {
		if (nearbyint(JSO_DVAL_P(instance)) != JSO_DVAL_P(instance)) {
			jso_schema_error_set(schema, JSO_SCHEMA_ERROR_VALIDATION_TYPE,
					"Double integer type cannot have decimal point");
			return JSO_FAILURE;
		}
		inst_ival = (jso_int) JSO_DVAL_P(instance);
	} else {
		return jso_schema_validation_value_type_error_ex(
				schema, JSO_TYPE_INT, JSO_TYPE_DOUBLE, JSO_TYPE_P(instance));
	}

	jso_schema_value_integer *intval = JSO_SCHEMA_VALUE_DATA_INT_P(value);

	if (JSO_SCHEMA_KW_IS_SET(intval->minimum)) {
		jso_int kw_ival = JSO_SCHEMA_KEYWORD_DATA_INT(intval->minimum);
		if (inst_ival < kw_ival) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Value %ld is lower than minimum value %ld", inst_ival, kw_ival);
			return JSO_FAILURE;
		}
		if (JSO_SCHEMA_KW_IS_SET(intval->exclusive_minimum)
				&& JSO_SCHEMA_KEYWORD_DATA_BOOL(intval->exclusive_minimum)
				&& inst_ival == kw_ival) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Value %ld is equal to exclusive minimum", inst_ival);
			return JSO_FAILURE;
		}
	}

	if (JSO_SCHEMA_KW_IS_SET(intval->maximum)) {
		jso_int kw_ival = JSO_SCHEMA_KEYWORD_DATA_INT(intval->maximum);
		if (inst_ival > kw_ival) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Value %ld is greater than maximum value %ld", inst_ival, kw_ival);
			return JSO_FAILURE;
		}
		if (JSO_SCHEMA_KW_IS_SET(intval->exclusive_maximum)
				&& JSO_SCHEMA_KEYWORD_DATA_BOOL(intval->exclusive_maximum)
				&& inst_ival == kw_ival) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Value %ld is equal to exclusive maximum", inst_ival);
			return JSO_FAILURE;
		}
	}

	if (JSO_SCHEMA_KW_IS_SET(intval->multiple_of)) {
		jso_int kw_ival = JSO_SCHEMA_KEYWORD_DATA_INT(intval->multiple_of);
		if (inst_ival % kw_ival != 0) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Value %d is is not multiple of %d", inst_ival, kw_ival);
			return JSO_FAILURE;
		}
	}

	return JSO_SUCCESS;
}

jso_rc jso_schema_validation_value_number(
		jso_schema *schema, jso_schema_value *value, jso_value *instance)
{
	jso_number inst_num;

	if (JSO_TYPE_P(instance) == JSO_TYPE_INT) {
		inst_num.ival = JSO_IVAL_P(instance);
		inst_num.is_int = true;
	}
	if (JSO_TYPE_P(instance) == JSO_TYPE_DOUBLE) {
		inst_num.dval = JSO_DVAL_P(instance);
		inst_num.is_int = false;
	} else {
		return jso_schema_validation_value_type_error_ex(
				schema, JSO_TYPE_INT, JSO_TYPE_DOUBLE, JSO_TYPE_P(instance));
	}

	jso_schema_value_number *numval = JSO_SCHEMA_VALUE_DATA_NUM_P(value);

	if (JSO_SCHEMA_KW_IS_SET(numval->minimum)) {
		jso_number kw_num;
		JSO_ASSERT_EQ(jso_schema_keyword_convert_to_number(&numval->minimum, &kw_num), JSO_SUCCESS);
		if (jso_number_lt(&inst_num, &kw_num)) {
			// TODO: print number values in message like for ints
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Value is lower than minimum value");
			return JSO_FAILURE;
		}
		if (JSO_SCHEMA_KW_IS_SET(numval->exclusive_minimum)
				&& JSO_SCHEMA_KEYWORD_DATA_BOOL(numval->exclusive_minimum)
				&& jso_number_eq(&inst_num, &kw_num)) {
			// TODO: print number values in message like for ints
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Value is equal to exclusive minimum");
			return JSO_FAILURE;
		}
	}

	if (JSO_SCHEMA_KW_IS_SET(numval->maximum)) {
		jso_number kw_num;
		JSO_ASSERT_EQ(jso_schema_keyword_convert_to_number(&numval->maximum, &kw_num), JSO_SUCCESS);
		if (jso_number_gt(&inst_num, &kw_num)) {
			// TODO: print number values in message like for ints
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Value is greater than maximum value");
			return JSO_FAILURE;
		}
		if (JSO_SCHEMA_KW_IS_SET(numval->exclusive_maximum)
				&& JSO_SCHEMA_KEYWORD_DATA_BOOL(numval->exclusive_maximum)
				&& jso_number_eq(&inst_num, &kw_num)) {
			// TODO: print number values in message like for ints
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Value is equal to exclusive maximum");
			return JSO_FAILURE;
		}
	}

	if (JSO_SCHEMA_KW_IS_SET(numval->multiple_of)) {
		jso_number kw_num;
		JSO_ASSERT_EQ(
				jso_schema_keyword_convert_to_number(&numval->multiple_of, &kw_num), JSO_SUCCESS);
		if (jso_number_is_multiple_of(&inst_num, &kw_num)) {
			// TODO: print number values in message like for ints
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Value is is not multiple of keyword value");
			return JSO_FAILURE;
		}
	}

	return JSO_SUCCESS;
}

jso_rc jso_schema_validation_value_string(
		jso_schema *schema, jso_schema_value *value, jso_value *instance)
{
	if (JSO_TYPE_P(instance) != JSO_TYPE_STRING) {
		return jso_schema_validation_value_type_error(
				schema, JSO_TYPE_STRING, JSO_TYPE_P(instance));
	}

	jso_schema_value_string *strval = JSO_SCHEMA_VALUE_DATA_STR_P(value);

	if (JSO_SCHEMA_KW_IS_SET(strval->min_length)) {
		jso_uint kw_uval = JSO_SCHEMA_KEYWORD_DATA_UINT(strval->min_length);
		if (JSO_SLEN_P(instance) < kw_uval) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"String length %zu is lower than minimum length %lu", JSO_SLEN_P(instance),
					kw_uval);
			return JSO_FAILURE;
		}
	}

	if (JSO_SCHEMA_KW_IS_SET(strval->max_length)) {
		jso_uint kw_uval = JSO_SCHEMA_KEYWORD_DATA_UINT(strval->max_length);
		if (JSO_SLEN_P(instance) > kw_uval) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"String length %zu is greater than maximum length %lu", JSO_SLEN_P(instance),
					kw_uval);
			return JSO_FAILURE;
		}
	}

	// TODO: pattern validation

	return JSO_SUCCESS;
}

jso_rc jso_schema_validation_value_array(
		jso_schema *schema, jso_schema_value *value, jso_value *instance)
{
	if (JSO_TYPE_P(instance) != JSO_TYPE_ARRAY) {
		return jso_schema_validation_value_type_error(schema, JSO_TYPE_ARRAY, JSO_TYPE_P(instance));
	}

	jso_schema_value_array *arrval = JSO_SCHEMA_VALUE_DATA_ARR_P(value);

	if (JSO_SCHEMA_KW_IS_SET(arrval->min_items)) {
		jso_uint kw_uval = JSO_SCHEMA_KEYWORD_DATA_UINT(arrval->min_items);
		size_t arrlen = JSO_ARRAY_LEN(JSO_ARRVAL_P(instance));
		if (arrlen < kw_uval) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Array number of items is %zu which is lower than minimum number of items %lu",
					arrlen, kw_uval);
			return JSO_FAILURE;
		}
	}

	if (JSO_SCHEMA_KW_IS_SET(arrval->max_items)) {
		jso_uint kw_uval = JSO_SCHEMA_KEYWORD_DATA_UINT(arrval->max_items);
		size_t arrlen = JSO_ARRAY_LEN(JSO_ARRVAL_P(instance));
		if (arrlen > kw_uval) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Array number of items is %zu which is greater than maximum number of items "
					"%lu",
					arrlen, kw_uval);
			return JSO_FAILURE;
		}
	}

	// TODO: possibly other keywords validation

	return JSO_SUCCESS;
}

jso_rc jso_schema_validation_value_object(
		jso_schema *schema, jso_schema_value *value, jso_value *instance)
{
	if (JSO_TYPE_P(instance) != JSO_TYPE_OBJECT) {
		return jso_schema_validation_value_type_error(
				schema, JSO_TYPE_OBJECT, JSO_TYPE_P(instance));
	}

	jso_schema_value_object *objval = JSO_SCHEMA_VALUE_DATA_OBJ_P(value);

	if (JSO_SCHEMA_KW_IS_SET(objval->min_properties)) {
		jso_uint kw_uval = JSO_SCHEMA_KEYWORD_DATA_UINT(objval->min_properties);
		size_t objlen = JSO_OBJECT_COUNT(JSO_OBJVAL_P(instance));
		if (objlen < kw_uval) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Object number of properties is %zu which is lower than minimum number of "
					"properties %lu",
					objlen, kw_uval);
			return JSO_FAILURE;
		}
	}

	// TODO: move to jso_schema_validation_object_update
	if (JSO_SCHEMA_KW_IS_SET(objval->max_properties)) {
		jso_uint kw_uval = JSO_SCHEMA_KEYWORD_DATA_UINT(objval->max_properties);
		size_t objlen = JSO_OBJECT_COUNT(JSO_OBJVAL_P(instance));
		if (objlen > kw_uval) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Object number of properties is %zu which is greater than maximum number of "
					"properties %lu",
					objlen, kw_uval);
			return JSO_FAILURE;
		}
	}

	// TODO: possibly other keywords validation

	return JSO_SUCCESS;
}

static const jso_schema_validation_value_callback schema_validation_value_callbacks[] = {
	[JSO_SCHEMA_VALUE_TYPE_NULL] = jso_schema_validation_value_null,
	[JSO_SCHEMA_VALUE_TYPE_BOOLEAN] = jso_schema_validation_value_boolean,
	[JSO_SCHEMA_VALUE_TYPE_INTEGER] = jso_schema_validation_value_integer,
	[JSO_SCHEMA_VALUE_TYPE_NUMBER] = jso_schema_validation_value_number,
	[JSO_SCHEMA_VALUE_TYPE_STRING] = jso_schema_validation_value_string,
	[JSO_SCHEMA_VALUE_TYPE_ARRAY] = jso_schema_validation_value_array,
	[JSO_SCHEMA_VALUE_TYPE_OBJECT] = jso_schema_validation_value_object,
};

jso_rc jso_schema_validation_value(jso_schema *schema, jso_schema_value *value, jso_value *instance)
{
	return schema_validation_value_callbacks[JSO_SCHEMA_VALUE_TYPE_P(value)](
			schema, value, instance);
}
