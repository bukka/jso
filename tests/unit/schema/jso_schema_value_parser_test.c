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

#include "../../../src/schema/jso_schema_value.h"
#include "../../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* Wrapping. */

/* Wrapper for jso_schema_array_alloc. */
jso_schema_array *__wrap_jso_schema_array_alloc(size_t size)
{
	function_called();
	check_expected(size);

	return mock_ptr_type(jso_schema_array *);
}

/* Wrapper for jso_schema_array_append. */
jso_rc __wrap_jso_schema_array_append(jso_schema_array *arr, jso_schema_value *val)
{
	function_called();
	check_expected_ptr(arr);
	check_expected_ptr(val);

	return mock_type(jso_rc);
}

/* Wrapper for jso_schema_array_free. */
void __wrap_jso_schema_array_free(jso_schema_array *arr)
{
	function_called();
	check_expected_ptr(arr);
}

/* Wrapper for jso_schema_data_check_type. */
jso_rc __wrap_jso_schema_data_check_type(jso_schema *schema, const char *key, jso_value *val,
		jso_value_type type, jso_value_type primary_type, jso_bool error_on_invalid_type)
{
	function_called();
	check_expected_ptr(schema);
	check_expected(key);
	check_expected_ptr(val);
	check_expected(type);
	check_expected(primary_type);
	check_expected(error_on_invalid_type);

	return mock_type(jso_rc);
}

/* Wrapper for jso_schema_data_get_value. */
jso_value *__wrap_jso_schema_data_get_value_fast(
		jso_schema *schema, jso_value *data, const char *key, jso_uint32 keyword_flags)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected(keyword_flags);

	return mock_ptr_type(jso_value *);
}

/* Wrapper for jso_schema_keyword_set. */
jso_rc __wrap_jso_schema_keyword_set(jso_schema *schema, jso_value *data, const char *key,
		jso_schema_value *value, jso_schema_keyword *schema_keyword,
		jso_schema_keyword_type keyword_type, jso_uint32 keyword_flags)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected_ptr(value);
	check_expected_ptr(schema_keyword);
	check_expected(keyword_type);
	check_expected(keyword_flags);

	return mock_type(jso_rc);
}

/* Wrapper for jso_schema_keyword_set_union_of_2_types. */
jso_rc __wrap_jso_schema_keyword_set_union_of_2_types(jso_schema *schema, jso_value *data,
		const char *key, jso_schema_value *value, jso_schema_keyword *schema_keyword,
		jso_schema_keyword_type keyword_union_type_1, jso_schema_keyword_type keyword_union_type_2,
		jso_uint32 keyword_flags)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected_ptr(value);
	check_expected_ptr(schema_keyword);
	check_expected(keyword_union_type_1);
	check_expected(keyword_union_type_2);
	check_expected(keyword_flags);

	return mock_type(jso_rc);
}

/* Wrapper for jso_schema_keyword_validate_array_of_strings. */
jso_rc __wrap_jso_schema_keyword_validate_array_of_strings(
		jso_schema *schema, const char *key, jso_array *arr, jso_uint32 keyword_flags)
{
	function_called();
	check_expected_ptr(schema);
	check_expected(key);
	check_expected_ptr(arr);
	check_expected(keyword_flags);

	return mock_type(jso_rc);
}

/* Wrapper for jso_schema_value_free. */
void __wrap_jso_schema_value_free(jso_schema_value *value)
{
	function_called();
	check_expected_ptr(value);
}

/* Wrapper for jso_schema_value_init. */
jso_schema_value *__wrap_jso_schema_value_init(jso_schema *schema, jso_value *data,
		jso_schema_value *parent, const char *type_name, size_t value_size,
		jso_schema_value_type value_type)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected_ptr(parent);
	check_expected(type_name);
	check_expected(value_size);
	check_expected(value_type);

	return mock_ptr_type(jso_schema_value *);
}

/* Tests for jso_schema_value_parse. */

/* Test parsing value for string type null when all successful. */
static void test_jso_schema_value_parse_type_null_when_all_good(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_null nulval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("null");
	JSO_VALUE_SET_STRING(tval, type);

	JSO_SCHEMA_VALUE_DATA_NULL(value) = &nulval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "null");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_null));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_NULL);
	will_return(__wrap_jso_schema_value_init, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_ptr_equal(&value, returned_value);
}

/* Test parsing value for string type null when value init fails. */
static void test_jso_schema_value_parse_type_null_when_value_init_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("null");
	JSO_VALUE_SET_STRING(tval, type);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "null");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_null));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_NULL);
	will_return(__wrap_jso_schema_value_init, NULL);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type boolean when all successful. */
static void test_jso_schema_value_parse_type_boolean_when_all_good(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_boolean boolval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("boolean");
	JSO_VALUE_SET_STRING(tval, type);

	JSO_SCHEMA_VALUE_DATA_BOOL(value) = &boolval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "boolean");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_boolean));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_BOOLEAN);
	will_return(__wrap_jso_schema_value_init, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_ptr_equal(&value, returned_value);
}

/* Test parsing value for string type boolean when value init fails. */
static void test_jso_schema_value_parse_type_boolean_when_value_init_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("boolean");
	JSO_VALUE_SET_STRING(tval, type);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "boolean");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_boolean));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_BOOLEAN);
	will_return(__wrap_jso_schema_value_init, NULL);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type integer when all successful. */
static void test_jso_schema_value_parse_type_integer_when_all_good_without_exclusive(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_integer intval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("integer");
	JSO_VALUE_SET_STRING(tval, type);

	memset(&intval, 0, sizeof(jso_schema_value_integer));
	JSO_SCHEMA_VALUE_DATA_INT(value) = &intval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "integer");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_integer));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_INTEGER);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "multipleOf");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.multiple_of);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMinimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.exclusive_minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMaximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.exclusive_maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_ptr_equal(&value, returned_value);
}

/* Test parsing value for string type integer when all successful and min and max exclusive set. */
static void test_jso_schema_value_parse_type_integer_when_all_good_with_exclusive(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_integer intval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("integer");
	JSO_VALUE_SET_STRING(tval, type);

	memset(&intval, 0, sizeof(jso_schema_value_integer));
	JSO_SCHEMA_VALUE_DATA_INT(value) = &intval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "integer");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_integer));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_INTEGER);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "multipleOf");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.multiple_of);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMinimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.exclusive_minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMaximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.exclusive_maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	JSO_SCHEMA_KEYWORD_FLAGS(intval.exclusive_maximum) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_FLAGS(intval.maximum) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_FLAGS(intval.exclusive_minimum) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_FLAGS(intval.minimum) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_ptr_equal(&value, returned_value);
}

/* Test parsing value for string type integer when exclusive minimum is set but minimum is not. */
static void test_jso_schema_value_parse_type_integer_when_min_exclusive_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_integer intval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("integer");
	JSO_VALUE_SET_STRING(tval, type);

	memset(&intval, 0, sizeof(jso_schema_value_integer));
	JSO_SCHEMA_VALUE_DATA_INT(value) = &intval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "integer");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_integer));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_INTEGER);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "multipleOf");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.multiple_of);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMinimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.exclusive_minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMaximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.exclusive_maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	JSO_SCHEMA_KEYWORD_FLAGS(intval.exclusive_maximum) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_FLAGS(intval.maximum) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_FLAGS(intval.exclusive_minimum) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_FLAGS(intval.minimum) = 0;

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_DEPS, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("The minimum must be set when exclusiveMinimum is set",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));
}

/* Test parsing value for string type integer when exclusive maximum is set but maximum is not. */
static void test_jso_schema_value_parse_type_integer_when_max_exclusive_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_integer intval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("integer");
	JSO_VALUE_SET_STRING(tval, type);

	memset(&intval, 0, sizeof(jso_schema_value_integer));
	JSO_SCHEMA_VALUE_DATA_INT(value) = &intval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "integer");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_integer));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_INTEGER);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "multipleOf");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.multiple_of);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMinimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.exclusive_minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMaximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.exclusive_maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	JSO_SCHEMA_KEYWORD_FLAGS(intval.exclusive_maximum) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_FLAGS(intval.maximum) = 0;
	JSO_SCHEMA_KEYWORD_FLAGS(intval.exclusive_minimum) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_FLAGS(intval.minimum) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_DEPS, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("The maximum must be set when exclusiveMaximum is set",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));
}

/* Test parsing value for string type integer when setting of exclusive maximum fails. */
static void test_jso_schema_value_parse_type_integer_when_exclusive_max_setting_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_integer intval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("integer");
	JSO_VALUE_SET_STRING(tval, type);

	memset(&intval, 0, sizeof(jso_schema_value_integer));
	JSO_SCHEMA_VALUE_DATA_INT(value) = &intval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "integer");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_integer));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_INTEGER);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "multipleOf");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.multiple_of);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMinimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.exclusive_minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMaximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.exclusive_maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type integer when setting of maximum fails. */
static void test_jso_schema_value_parse_type_integer_when_maximum_setting_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_integer intval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("integer");
	JSO_VALUE_SET_STRING(tval, type);

	memset(&intval, 0, sizeof(jso_schema_value_integer));
	JSO_SCHEMA_VALUE_DATA_INT(value) = &intval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "integer");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_integer));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_INTEGER);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "multipleOf");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.multiple_of);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMinimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.exclusive_minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type integer when setting of exclusive minimum fails. */
static void test_jso_schema_value_parse_type_integer_when_exclusive_min_setting_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_integer intval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("integer");
	JSO_VALUE_SET_STRING(tval, type);

	memset(&intval, 0, sizeof(jso_schema_value_integer));
	JSO_SCHEMA_VALUE_DATA_INT(value) = &intval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "integer");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_integer));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_INTEGER);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "multipleOf");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.multiple_of);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMinimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.exclusive_minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type integer when setting of minimum fails. */
static void test_jso_schema_value_parse_type_integer_when_minimum_setting_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_integer intval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("integer");
	JSO_VALUE_SET_STRING(tval, type);

	memset(&intval, 0, sizeof(jso_schema_value_integer));
	JSO_SCHEMA_VALUE_DATA_INT(value) = &intval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "integer");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_integer));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_INTEGER);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "multipleOf");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.multiple_of);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type integer when setting of multiple of fails. */
static void test_jso_schema_value_parse_type_integer_when_multiple_of_setting_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_integer intval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("integer");
	JSO_VALUE_SET_STRING(tval, type);

	memset(&intval, 0, sizeof(jso_schema_value_integer));
	JSO_SCHEMA_VALUE_DATA_INT(value) = &intval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "integer");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_integer));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_INTEGER);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "multipleOf");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &intval.multiple_of);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO);
	will_return(__wrap_jso_schema_keyword_set, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type integer when value init fails. */
static void test_jso_schema_value_parse_type_integer_when_value_init_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("integer");
	JSO_VALUE_SET_STRING(tval, type);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "integer");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_integer));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_INTEGER);
	will_return(__wrap_jso_schema_value_init, NULL);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type number when all successful. */
static void test_jso_schema_value_parse_type_number_when_all_good_without_exclusive(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_number numval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("number");
	JSO_VALUE_SET_STRING(tval, type);

	memset(&numval, 0, sizeof(jso_schema_value_number));
	JSO_SCHEMA_VALUE_DATA_NUM(value) = &numval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "number");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_number));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_NUMBER);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "multipleOf");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.multiple_of);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_NUMBER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMinimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.exclusive_minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_NUMBER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMaximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.exclusive_maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_ptr_equal(&value, returned_value);
}

/* Test parsing value for string type number when all successful and min and max exclusive set. */
static void test_jso_schema_value_parse_type_number_when_all_good_with_exclusive(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_number numval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("number");
	JSO_VALUE_SET_STRING(tval, type);

	memset(&numval, 0, sizeof(jso_schema_value_number));
	JSO_SCHEMA_VALUE_DATA_NUM(value) = &numval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "number");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_number));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_NUMBER);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "multipleOf");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.multiple_of);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_NUMBER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMinimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.exclusive_minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_NUMBER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMaximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.exclusive_maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	JSO_SCHEMA_KEYWORD_FLAGS(numval.exclusive_maximum) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_FLAGS(numval.maximum) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_FLAGS(numval.exclusive_minimum) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_FLAGS(numval.minimum) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_ptr_equal(&value, returned_value);
}

/* Test parsing value for string type number when exclusive minimum is set but minimum is not. */
static void test_jso_schema_value_parse_type_number_when_min_exclusive_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_number numval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("number");
	JSO_VALUE_SET_STRING(tval, type);

	memset(&numval, 0, sizeof(jso_schema_value_number));
	JSO_SCHEMA_VALUE_DATA_NUM(value) = &numval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "number");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_number));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_NUMBER);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "multipleOf");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.multiple_of);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_NUMBER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMinimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.exclusive_minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_NUMBER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMaximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.exclusive_maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	JSO_SCHEMA_KEYWORD_FLAGS(numval.exclusive_maximum) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_FLAGS(numval.maximum) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_FLAGS(numval.exclusive_minimum) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_FLAGS(numval.minimum) = 0;

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_DEPS, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("The minimum must be set when exclusiveMinimum is set",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));
}

/* Test parsing value for string type number when exclusive maximum is set but maximum is not. */
static void test_jso_schema_value_parse_type_number_when_max_exclusive_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_number numval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("number");
	JSO_VALUE_SET_STRING(tval, type);

	memset(&numval, 0, sizeof(jso_schema_value_number));
	JSO_SCHEMA_VALUE_DATA_NUM(value) = &numval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "number");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_number));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_NUMBER);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "multipleOf");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.multiple_of);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_NUMBER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMinimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.exclusive_minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_NUMBER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMaximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.exclusive_maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	JSO_SCHEMA_KEYWORD_FLAGS(numval.exclusive_maximum) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_FLAGS(numval.maximum) = 0;
	JSO_SCHEMA_KEYWORD_FLAGS(numval.exclusive_minimum) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_FLAGS(numval.minimum) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_DEPS, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("The maximum must be set when exclusiveMaximum is set",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));
}

/* Test parsing value for string type number when setting of exclusive maximum fails. */
static void test_jso_schema_value_parse_type_number_when_exclusive_max_setting_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_number numval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("number");
	JSO_VALUE_SET_STRING(tval, type);

	memset(&numval, 0, sizeof(jso_schema_value_number));
	JSO_SCHEMA_VALUE_DATA_NUM(value) = &numval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "number");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_number));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_NUMBER);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "multipleOf");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.multiple_of);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_NUMBER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMinimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.exclusive_minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_NUMBER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMaximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.exclusive_maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type number when setting of maximum fails. */
static void test_jso_schema_value_parse_type_number_when_maximum_setting_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_number numval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("number");
	JSO_VALUE_SET_STRING(tval, type);

	memset(&numval, 0, sizeof(jso_schema_value_number));
	JSO_SCHEMA_VALUE_DATA_NUM(value) = &numval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "number");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_number));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_NUMBER);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "multipleOf");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.multiple_of);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_NUMBER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMinimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.exclusive_minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maximum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.maximum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_NUMBER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type number when setting of exclusive minimum fails. */
static void test_jso_schema_value_parse_type_number_when_exclusive_min_setting_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_number numval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("number");
	JSO_VALUE_SET_STRING(tval, type);

	memset(&numval, 0, sizeof(jso_schema_value_number));
	JSO_SCHEMA_VALUE_DATA_NUM(value) = &numval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "number");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_number));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_NUMBER);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "multipleOf");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.multiple_of);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_NUMBER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "exclusiveMinimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.exclusive_minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type number when setting of minimum fails. */
static void test_jso_schema_value_parse_type_number_when_minimum_setting_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_number numval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("number");
	JSO_VALUE_SET_STRING(tval, type);

	memset(&numval, 0, sizeof(jso_schema_value_number));
	JSO_SCHEMA_VALUE_DATA_NUM(value) = &numval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "number");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_number));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_NUMBER);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "multipleOf");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.multiple_of);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minimum");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.minimum);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_NUMBER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type number when setting of multiple of fails. */
static void test_jso_schema_value_parse_type_number_when_multiple_of_setting_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_number numval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("number");
	JSO_VALUE_SET_STRING(tval, type);

	memset(&numval, 0, sizeof(jso_schema_value_number));
	JSO_SCHEMA_VALUE_DATA_NUM(value) = &numval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "number");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_number));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_NUMBER);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "multipleOf");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &numval.multiple_of);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO);
	will_return(__wrap_jso_schema_keyword_set, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type number when value init fails. */
static void test_jso_schema_value_parse_type_number_when_value_init_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("number");
	JSO_VALUE_SET_STRING(tval, type);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "number");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_number));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_NUMBER);
	will_return(__wrap_jso_schema_value_init, NULL);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type string when all successful. */
static void test_jso_schema_value_parse_type_string_when_all_good(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_string strval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("string");
	JSO_VALUE_SET_STRING(tval, type);

	JSO_SCHEMA_VALUE_DATA_STR(value) = &strval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "string");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_string));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_STRING);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maxLength");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &strval.max_length);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minLength");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &strval.min_length);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "pattern");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &strval.pattern);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_REGEXP);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_ptr_equal(&value, returned_value);
}

/* Test parsing value for string type string when setting pattern fails. */
static void test_jso_schema_value_parse_type_string_when_pattern_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_string strval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("string");
	JSO_VALUE_SET_STRING(tval, type);

	JSO_SCHEMA_VALUE_DATA_STR(value) = &strval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "string");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_string));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_STRING);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maxLength");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &strval.max_length);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minLength");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &strval.min_length);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "pattern");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &strval.pattern);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_REGEXP);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type string when setting minLength fails. */
static void test_jso_schema_value_parse_type_string_when_min_length_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_string strval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("string");
	JSO_VALUE_SET_STRING(tval, type);

	JSO_SCHEMA_VALUE_DATA_STR(value) = &strval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "string");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_string));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_STRING);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maxLength");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &strval.max_length);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minLength");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &strval.min_length);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type string when setting maxLength fails. */
static void test_jso_schema_value_parse_type_string_when_max_length_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_string strval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("string");
	JSO_VALUE_SET_STRING(tval, type);

	JSO_SCHEMA_VALUE_DATA_STR(value) = &strval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "string");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_string));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_STRING);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maxLength");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &strval.max_length);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type string when value init fails. */
static void test_jso_schema_value_parse_type_string_when_value_init_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("string");
	JSO_VALUE_SET_STRING(tval, type);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "string");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_string));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_STRING);
	will_return(__wrap_jso_schema_value_init, NULL);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type array when all successful. */
static void test_jso_schema_value_parse_type_array_when_all_good(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_array arrval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("array");
	JSO_VALUE_SET_STRING(tval, type);

	JSO_SCHEMA_VALUE_DATA_ARR(value) = &arrval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "array");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_array));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_ARRAY);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set_union_of_2_types);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, data, &data);
	expect_string(__wrap_jso_schema_keyword_set_union_of_2_types, key, "additionalItems");
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, value, &value);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema_keyword,
			&arrval.additional_items);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_1,
			JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_2,
			JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set_union_of_2_types, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set_union_of_2_types);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, data, &data);
	expect_string(__wrap_jso_schema_keyword_set_union_of_2_types, key, "items");
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, value, &value);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema_keyword, &arrval.items);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_1,
			JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_2,
			JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set_union_of_2_types, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "uniqueItems");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &arrval.unique_items);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maxItems");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &arrval.max_items);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minItems");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &arrval.min_items);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_ptr_equal(&value, returned_value);
}

/* Test parsing value for string type array when min items setting fails. */
static void test_jso_schema_value_parse_type_array_when_min_items_setting_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_array arrval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("array");
	JSO_VALUE_SET_STRING(tval, type);

	JSO_SCHEMA_VALUE_DATA_ARR(value) = &arrval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "array");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_array));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_ARRAY);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set_union_of_2_types);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, data, &data);
	expect_string(__wrap_jso_schema_keyword_set_union_of_2_types, key, "additionalItems");
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, value, &value);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema_keyword,
			&arrval.additional_items);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_1,
			JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_2,
			JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set_union_of_2_types, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set_union_of_2_types);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, data, &data);
	expect_string(__wrap_jso_schema_keyword_set_union_of_2_types, key, "items");
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, value, &value);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema_keyword, &arrval.items);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_1,
			JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_2,
			JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set_union_of_2_types, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "uniqueItems");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &arrval.unique_items);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maxItems");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &arrval.max_items);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minItems");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &arrval.min_items);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type array when max items setting fails. */
static void test_jso_schema_value_parse_type_array_when_max_items_setting_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_array arrval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("array");
	JSO_VALUE_SET_STRING(tval, type);

	JSO_SCHEMA_VALUE_DATA_ARR(value) = &arrval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "array");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_array));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_ARRAY);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set_union_of_2_types);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, data, &data);
	expect_string(__wrap_jso_schema_keyword_set_union_of_2_types, key, "additionalItems");
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, value, &value);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema_keyword,
			&arrval.additional_items);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_1,
			JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_2,
			JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set_union_of_2_types, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set_union_of_2_types);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, data, &data);
	expect_string(__wrap_jso_schema_keyword_set_union_of_2_types, key, "items");
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, value, &value);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema_keyword, &arrval.items);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_1,
			JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_2,
			JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set_union_of_2_types, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "uniqueItems");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &arrval.unique_items);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maxItems");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &arrval.max_items);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type array when unique items setting fails. */
static void test_jso_schema_value_parse_type_array_when_unique_items_setting_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_array arrval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("array");
	JSO_VALUE_SET_STRING(tval, type);

	JSO_SCHEMA_VALUE_DATA_ARR(value) = &arrval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "array");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_array));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_ARRAY);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set_union_of_2_types);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, data, &data);
	expect_string(__wrap_jso_schema_keyword_set_union_of_2_types, key, "additionalItems");
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, value, &value);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema_keyword,
			&arrval.additional_items);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_1,
			JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_2,
			JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set_union_of_2_types, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set_union_of_2_types);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, data, &data);
	expect_string(__wrap_jso_schema_keyword_set_union_of_2_types, key, "items");
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, value, &value);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema_keyword, &arrval.items);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_1,
			JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_2,
			JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set_union_of_2_types, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "uniqueItems");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &arrval.unique_items);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type array when items setting fails. */
static void test_jso_schema_value_parse_type_array_when_items_setting_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_array arrval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("array");
	JSO_VALUE_SET_STRING(tval, type);

	JSO_SCHEMA_VALUE_DATA_ARR(value) = &arrval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "array");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_array));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_ARRAY);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set_union_of_2_types);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, data, &data);
	expect_string(__wrap_jso_schema_keyword_set_union_of_2_types, key, "additionalItems");
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, value, &value);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema_keyword,
			&arrval.additional_items);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_1,
			JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_2,
			JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set_union_of_2_types, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set_union_of_2_types);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, data, &data);
	expect_string(__wrap_jso_schema_keyword_set_union_of_2_types, key, "items");
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, value, &value);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema_keyword, &arrval.items);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_1,
			JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_2,
			JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set_union_of_2_types, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type array when additional items setting fails. */
static void test_jso_schema_value_parse_type_array_when_addition_items_setting_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_array arrval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("array");
	JSO_VALUE_SET_STRING(tval, type);

	JSO_SCHEMA_VALUE_DATA_ARR(value) = &arrval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "array");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_array));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_ARRAY);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set_union_of_2_types);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, data, &data);
	expect_string(__wrap_jso_schema_keyword_set_union_of_2_types, key, "additionalItems");
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, value, &value);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema_keyword,
			&arrval.additional_items);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_1,
			JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_2,
			JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set_union_of_2_types, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type array when value init fails. */
static void test_jso_schema_value_parse_type_array_when_value_init_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("array");
	JSO_VALUE_SET_STRING(tval, type);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "array");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_array));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_ARRAY);
	will_return(__wrap_jso_schema_value_init, NULL);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type object when all successful. */
static void test_jso_schema_value_parse_type_object_when_all_good(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_object objval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("object");
	JSO_VALUE_SET_STRING(tval, type);

	JSO_SCHEMA_VALUE_DATA_OBJ(value) = &objval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "object");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_object));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_OBJECT);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minProperties");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.min_properties);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maxProperties");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.max_properties);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set_union_of_2_types);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, data, &data);
	expect_string(__wrap_jso_schema_keyword_set_union_of_2_types, key, "additionalProperties");
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, value, &value);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema_keyword,
			&objval.additional_properties);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_1,
			JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_2,
			JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set_union_of_2_types, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "properties");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.properties);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type,
			JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "patternProperties");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.pattern_properties);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type,
			JSO_SCHEMA_KEYWORD_TYPE_REGEXP_OBJECT_OF_SCHEMA_OBJECTS);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "dependencies");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.dependencies);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type,
			JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS_OR_ARRAY_OF_STRINGS);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_EMPTY);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_ptr_equal(&value, returned_value);
}

/* Test parsing value for string type object when dependencies setting fails. */
static void test_jso_schema_value_parse_type_object_when_deps_setting_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_object objval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("object");
	JSO_VALUE_SET_STRING(tval, type);

	JSO_SCHEMA_VALUE_DATA_OBJ(value) = &objval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "object");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_object));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_OBJECT);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minProperties");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.min_properties);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maxProperties");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.max_properties);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set_union_of_2_types);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, data, &data);
	expect_string(__wrap_jso_schema_keyword_set_union_of_2_types, key, "additionalProperties");
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, value, &value);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema_keyword,
			&objval.additional_properties);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_1,
			JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_2,
			JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set_union_of_2_types, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "properties");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.properties);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type,
			JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "patternProperties");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.pattern_properties);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type,
			JSO_SCHEMA_KEYWORD_TYPE_REGEXP_OBJECT_OF_SCHEMA_OBJECTS);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "dependencies");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.dependencies);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type,
			JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS_OR_ARRAY_OF_STRINGS);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_EMPTY);
	will_return(__wrap_jso_schema_keyword_set, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type object when pattern properties setting fails. */
static void test_jso_schema_value_parse_type_object_when_pattern_props_setting_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_object objval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("object");
	JSO_VALUE_SET_STRING(tval, type);

	JSO_SCHEMA_VALUE_DATA_OBJ(value) = &objval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "object");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_object));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_OBJECT);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minProperties");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.min_properties);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maxProperties");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.max_properties);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set_union_of_2_types);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, data, &data);
	expect_string(__wrap_jso_schema_keyword_set_union_of_2_types, key, "additionalProperties");
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, value, &value);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema_keyword,
			&objval.additional_properties);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_1,
			JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_2,
			JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set_union_of_2_types, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "properties");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.properties);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type,
			JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "patternProperties");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.pattern_properties);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type,
			JSO_SCHEMA_KEYWORD_TYPE_REGEXP_OBJECT_OF_SCHEMA_OBJECTS);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type object when properties setting fails. */
static void test_jso_schema_value_parse_type_object_when_props_setting_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_object objval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("object");
	JSO_VALUE_SET_STRING(tval, type);

	JSO_SCHEMA_VALUE_DATA_OBJ(value) = &objval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "object");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_object));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_OBJECT);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minProperties");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.min_properties);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maxProperties");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.max_properties);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set_union_of_2_types);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, data, &data);
	expect_string(__wrap_jso_schema_keyword_set_union_of_2_types, key, "additionalProperties");
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, value, &value);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema_keyword,
			&objval.additional_properties);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_1,
			JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_2,
			JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set_union_of_2_types, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "properties");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.properties);
	expect_value(__wrap_jso_schema_keyword_set, keyword_type,
			JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type object when additional properties setting fails. */
static void test_jso_schema_value_parse_type_object_when_addition_props_setting_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_object objval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("object");
	JSO_VALUE_SET_STRING(tval, type);

	JSO_SCHEMA_VALUE_DATA_OBJ(value) = &objval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "object");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_object));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_OBJECT);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minProperties");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.min_properties);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maxProperties");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.max_properties);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set_union_of_2_types);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, data, &data);
	expect_string(__wrap_jso_schema_keyword_set_union_of_2_types, key, "additionalProperties");
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, value, &value);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, schema_keyword,
			&objval.additional_properties);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_1,
			JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_union_type_2,
			JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT);
	expect_value(__wrap_jso_schema_keyword_set_union_of_2_types, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set_union_of_2_types, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type object when max preperties setting fails. */
static void test_jso_schema_value_parse_type_object_when_max_props_setting_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_object objval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("object");
	JSO_VALUE_SET_STRING(tval, type);

	JSO_SCHEMA_VALUE_DATA_OBJ(value) = &objval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "object");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_object));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_OBJECT);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minProperties");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.min_properties);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_SUCCESS);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "maxProperties");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.max_properties);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type object when min preperties setting fails. */
static void test_jso_schema_value_parse_type_object_when_min_props_setting_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_object objval;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("object");
	JSO_VALUE_SET_STRING(tval, type);

	JSO_SCHEMA_VALUE_DATA_OBJ(value) = &objval;

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "object");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_object));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_OBJECT);
	will_return(__wrap_jso_schema_value_init, &value);

	expect_function_call(__wrap_jso_schema_keyword_set);
	expect_value(__wrap_jso_schema_keyword_set, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_set, data, &data);
	expect_string(__wrap_jso_schema_keyword_set, key, "minProperties");
	expect_value(__wrap_jso_schema_keyword_set, value, &value);
	expect_value(__wrap_jso_schema_keyword_set, schema_keyword, &objval.min_properties);
	expect_value(
			__wrap_jso_schema_keyword_set, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER);
	expect_value(__wrap_jso_schema_keyword_set, keyword_flags, 0);
	will_return(__wrap_jso_schema_keyword_set, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, value, &value);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

/* Test parsing value for string type object when value init fails. */
static void test_jso_schema_value_parse_type_object_when_value_init_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent;

	jso_schema_init(&schema);

	jso_string *type = jso_string_create_from_cstr("object");
	JSO_VALUE_SET_STRING(tval, type);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "type");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &tval);

	expect_function_call(__wrap_jso_schema_value_init);
	expect_value(__wrap_jso_schema_value_init, schema, &schema);
	expect_value(__wrap_jso_schema_value_init, data, &data);
	expect_value(__wrap_jso_schema_value_init, parent, &parent);
	expect_string(__wrap_jso_schema_value_init, type_name, "object");
	expect_value(__wrap_jso_schema_value_init, value_size, sizeof(jso_schema_value_object));
	expect_value(__wrap_jso_schema_value_init, value_type, JSO_SCHEMA_VALUE_TYPE_OBJECT);
	will_return(__wrap_jso_schema_value_init, NULL);

	jso_schema_value *returned_value = jso_schema_value_parse(&schema, &data, &parent);

	assert_null(returned_value);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_schema_value_parse_type_string_when_all_good),
		cmocka_unit_test(test_jso_schema_value_parse_type_string_when_pattern_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_string_when_min_length_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_string_when_max_length_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_string_when_value_init_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_null_when_all_good),
		cmocka_unit_test(test_jso_schema_value_parse_type_null_when_value_init_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_boolean_when_all_good),
		cmocka_unit_test(test_jso_schema_value_parse_type_boolean_when_value_init_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_integer_when_all_good_without_exclusive),
		cmocka_unit_test(test_jso_schema_value_parse_type_integer_when_all_good_with_exclusive),
		cmocka_unit_test(test_jso_schema_value_parse_type_integer_when_min_exclusive_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_integer_when_max_exclusive_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_integer_when_exclusive_max_setting_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_integer_when_maximum_setting_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_integer_when_exclusive_min_setting_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_integer_when_minimum_setting_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_integer_when_multiple_of_setting_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_integer_when_value_init_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_number_when_all_good_without_exclusive),
		cmocka_unit_test(test_jso_schema_value_parse_type_number_when_all_good_with_exclusive),
		cmocka_unit_test(test_jso_schema_value_parse_type_number_when_min_exclusive_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_number_when_max_exclusive_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_number_when_exclusive_max_setting_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_number_when_maximum_setting_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_number_when_exclusive_min_setting_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_number_when_minimum_setting_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_number_when_multiple_of_setting_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_number_when_value_init_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_array_when_all_good),
		cmocka_unit_test(test_jso_schema_value_parse_type_array_when_min_items_setting_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_array_when_max_items_setting_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_array_when_unique_items_setting_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_array_when_items_setting_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_array_when_addition_items_setting_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_array_when_value_init_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_object_when_all_good),
		cmocka_unit_test(test_jso_schema_value_parse_type_object_when_deps_setting_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_object_when_pattern_props_setting_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_object_when_props_setting_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_object_when_addition_props_setting_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_object_when_max_props_setting_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_object_when_min_props_setting_fails),
		cmocka_unit_test(test_jso_schema_value_parse_type_object_when_value_init_fails),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}