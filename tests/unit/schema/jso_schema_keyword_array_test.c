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

#include "../../../src/schema/jso_schema_keyword_array.h"
#include "../../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* Wrapping. */

/* Wrapper for jso_schema_data_get. */
jso_bool __wrap_jso_array_are_all_items_of_type(jso_array *arr, jso_value_type type)
{
	function_called();
	check_expected_ptr(arr);
	check_expected(type);

	return mock_type(jso_bool);
}

/* Wrapper for jso_schema_data_get. */
void __wrap_jso_array_free(jso_array *arr)
{
	function_called();
	check_expected_ptr(arr);
}

jso_bool __wrap_jso_array_is_unique(jso_array *arr)
{
	function_called();
	check_expected_ptr(arr);

	return mock_type(jso_bool);
}

void __wrap_jso_schema_array_free(jso_schema_array *arr)
{
	function_called();
	check_expected_ptr(arr);
}

/* Wrapper for jso_schema_data_get. */
jso_value *__wrap_jso_schema_data_get(jso_schema *schema, jso_value *data, const char *key,
		jso_value_type type, jso_uint32 keyword_flags, jso_bool error_on_invalid_type,
		jso_value *val)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected(keyword_flags);
	check_expected(error_on_invalid_type);
	check_expected_ptr(val);

	jso_value *value = mock_ptr_type(jso_value *);

	return value;
}

jso_schema_value *__wrap_jso_schema_value_parse(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected_ptr(parent);

	return mock_ptr_type(jso_schema_value *);
}

/* Tests for jso_schema_keyword_validate_array_of_strings. */

/* Test validation of string array when all items are strings. */
static void test_jso_schema_keyword_validate_array_of_strings_when_all_strings(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_schema_init(&schema);

	jso_array array;
	jso_array_init(&array);

	expect_function_call(__wrap_jso_array_are_all_items_of_type);
	expect_value(__wrap_jso_array_are_all_items_of_type, arr, &array);
	expect_value(__wrap_jso_array_are_all_items_of_type, type, JSO_TYPE_STRING);
	will_return(__wrap_jso_array_are_all_items_of_type, JSO_TRUE);

	jso_rc rc = jso_schema_keyword_validate_array_of_strings(&schema, "tkey", &array, 0);

	assert_int_equal(rc, JSO_SUCCESS);

	jso_schema_clear(&schema);
}

/* Test validation of string array when not all items are strings. */
static void test_jso_schema_keyword_validate_array_of_strings_when_not_all_strings(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_schema_init(&schema);

	jso_array array;
	jso_array_init(&array);

	expect_function_call(__wrap_jso_array_are_all_items_of_type);
	expect_value(__wrap_jso_array_are_all_items_of_type, arr, &array);
	expect_value(__wrap_jso_array_are_all_items_of_type, type, JSO_TYPE_STRING);
	will_return(__wrap_jso_array_are_all_items_of_type, JSO_FALSE);

	jso_rc rc = jso_schema_keyword_validate_array_of_strings(&schema, "tkey", &array, 0);

	assert_int_equal(rc, JSO_FAILURE);
	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_TYPE, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal(
			"Array values for keyword tkey must be a string", JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
}

/* Test validation of string array when all items are strings and unique check is done. */
static void test_jso_schema_keyword_validate_array_of_strings_when_unique(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_schema_init(&schema);

	jso_array array;
	jso_array_init(&array);

	expect_function_call(__wrap_jso_array_is_unique);
	expect_value(__wrap_jso_array_is_unique, arr, &array);
	will_return(__wrap_jso_array_is_unique, JSO_TRUE);

	expect_function_call(__wrap_jso_array_are_all_items_of_type);
	expect_value(__wrap_jso_array_are_all_items_of_type, arr, &array);
	expect_value(__wrap_jso_array_are_all_items_of_type, type, JSO_TYPE_STRING);
	will_return(__wrap_jso_array_are_all_items_of_type, JSO_TRUE);

	jso_rc rc = jso_schema_keyword_validate_array_of_strings(
			&schema, "tkey", &array, JSO_SCHEMA_KEYWORD_FLAG_UNIQUE);

	assert_int_equal(rc, JSO_SUCCESS);

	jso_schema_clear(&schema);
}

/* Test validation of string array when items are not unique. */
static void test_jso_schema_keyword_validate_array_of_strings_when_not_unique(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_schema_init(&schema);

	jso_array array;
	jso_array_init(&array);

	expect_function_call(__wrap_jso_array_is_unique);
	expect_value(__wrap_jso_array_is_unique, arr, &array);
	will_return(__wrap_jso_array_is_unique, JSO_FALSE);

	jso_rc rc = jso_schema_keyword_validate_array_of_strings(
			&schema, "tkey", &array, JSO_SCHEMA_KEYWORD_FLAG_UNIQUE);

	assert_int_equal(rc, JSO_FAILURE);
	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_DEPS, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal(
			"Array values for keyword tkey must be unique", JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
}

/* Test validation of string array when array should be empty but it is not. */
static void test_jso_schema_keyword_validate_array_of_strings_when_empty(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_schema_init(&schema);

	jso_array array;
	jso_array_init(&array);

	jso_rc rc = jso_schema_keyword_validate_array_of_strings(
			&schema, "tkey", &array, JSO_SCHEMA_KEYWORD_FLAG_NOT_EMPTY);

	assert_int_equal(rc, JSO_FAILURE);
	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_DEPS, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal(
			"Array for keyword tkey must not be empty", JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
}

/* Tests for jso_schema_keyword_get_array. */

/* Test getting of array if all good. */
static void test_jso_schema_keyword_get_array_when_value_ok(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_array array;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);
	jso_array_init(&array);

	JSO_VALUE_SET_ARRAY(val, &array);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "akey");
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_TRUE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, &val);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_array(
			&schema, &data, "akey", JSO_TRUE, 0, &keyword, &val, &parent);

	assert_non_null(schema_keyword);
	assert_true(JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) & JSO_SCHEMA_KEYWORD_FLAG_PRESENT);
	assert_ptr_equal(&array, JSO_SCHEMA_KEYWORD_DATA_ARR_STR_P(schema_keyword));
	assert_int_equal(JSO_ARRAY_REFCOUNT(&array), 1);

	jso_schema_clear(&schema);
}

/* Test getting of array if no data. */
static void test_jso_schema_keyword_get_array_when_value_not_found(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_array array;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);
	jso_array_init(&array);

	JSO_VALUE_SET_ARRAY(val, &array);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "akey");
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_TRUE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, NULL);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_array(
			&schema, &data, "akey", JSO_TRUE, 0, &keyword, &val, &parent);

	assert_null(schema_keyword);

	jso_schema_clear(&schema);
}

/* Test getting of array if the data array is empty when it should not be empty. */
static void test_jso_schema_keyword_get_array_when_empty(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_array array;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);
	jso_array_init(&array);

	JSO_VALUE_SET_ARRAY(val, &array);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "akey");
	expect_value(__wrap_jso_schema_data_get, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_EMPTY);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_TRUE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, &val);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_array(&schema, &data, "akey",
			JSO_TRUE, JSO_SCHEMA_KEYWORD_FLAG_NOT_EMPTY, &keyword, &val, &parent);

	assert_null(schema_keyword);
	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_DEPS, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal(
			"Array for keyword akey must not be empty", JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
}

/* Test getting of array if the data array is not unique when it should be. */
static void test_jso_schema_keyword_get_array_when_not_unique(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_array array;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);
	jso_array_init(&array);
	array.len = 1;

	JSO_VALUE_SET_ARRAY(val, &array);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "akey");
	expect_value(__wrap_jso_schema_data_get, keyword_flags,
			JSO_SCHEMA_KEYWORD_FLAG_UNIQUE | JSO_SCHEMA_KEYWORD_FLAG_NOT_EMPTY);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_TRUE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, &val);

	expect_function_call(__wrap_jso_array_is_unique);
	expect_value(__wrap_jso_array_is_unique, arr, &array);
	will_return(__wrap_jso_array_is_unique, JSO_FALSE);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_array(&schema, &data, "akey",
			JSO_TRUE, JSO_SCHEMA_KEYWORD_FLAG_UNIQUE | JSO_SCHEMA_KEYWORD_FLAG_NOT_EMPTY, &keyword,
			&val, &parent);

	assert_null(schema_keyword);
	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_DEPS, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal(
			"Array values for keyword akey must be unique", JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
}

/* Tests for jso_schema_keyword_get_array_of_strings. */

/* Test getting of array of strings if only string in the array and everything else ok. */
static void test_jso_schema_keyword_get_array_of_strings_when_all_strings(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_array array;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);
	jso_array_init(&array);

	JSO_VALUE_SET_ARRAY(val, &array);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "akey");
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_TRUE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, &val);

	expect_function_call(__wrap_jso_array_are_all_items_of_type);
	expect_value(__wrap_jso_array_are_all_items_of_type, arr, &array);
	expect_value(__wrap_jso_array_are_all_items_of_type, type, JSO_TYPE_STRING);
	will_return(__wrap_jso_array_are_all_items_of_type, JSO_TRUE);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_array_of_strings(
			&schema, &data, "akey", JSO_TRUE, 0, &keyword, &val, &parent);

	assert_non_null(schema_keyword);
	assert_true(JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) & JSO_SCHEMA_KEYWORD_FLAG_PRESENT);
	assert_ptr_equal(&array, JSO_SCHEMA_KEYWORD_DATA_ARR_STR_P(schema_keyword));
	assert_int_equal(JSO_ARRAY_REFCOUNT(&array), 1);

	jso_schema_clear(&schema);
}

/* Test getting of array of strings if not only string in the array. */
static void test_jso_schema_keyword_get_array_of_strings_when_not_all_strings(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_array array;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);
	jso_array_init(&array);

	JSO_VALUE_SET_ARRAY(val, &array);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "akey");
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_TRUE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, &val);

	expect_function_call(__wrap_jso_array_are_all_items_of_type);
	expect_value(__wrap_jso_array_are_all_items_of_type, arr, &array);
	expect_value(__wrap_jso_array_are_all_items_of_type, type, JSO_TYPE_STRING);
	will_return(__wrap_jso_array_are_all_items_of_type, JSO_FALSE);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_array_of_strings(
			&schema, &data, "akey", JSO_TRUE, 0, &keyword, &val, &parent);

	assert_null(schema_keyword);
	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_TYPE, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal(
			"Array values for keyword akey must be a string", JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
}

/* Test getting of array of strings if no data. */
static void test_jso_schema_keyword_get_array_of_strings_when_value_not_found(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_array array;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);
	jso_array_init(&array);

	JSO_VALUE_SET_ARRAY(val, &array);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "akey");
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_TRUE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, NULL);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_array_of_strings(
			&schema, &data, "akey", JSO_TRUE, 0, &keyword, &val, &parent);

	assert_null(schema_keyword);

	jso_schema_clear(&schema);
}

/* Test getting of array if the data array of strings is empty when it should not be empty. */
static void test_jso_schema_keyword_get_array_of_strings_when_empty(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_array array;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);
	jso_array_init(&array);

	JSO_VALUE_SET_ARRAY(val, &array);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "akey");
	expect_value(__wrap_jso_schema_data_get, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_EMPTY);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_TRUE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, &val);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_array_of_strings(&schema, &data,
			"akey", JSO_TRUE, JSO_SCHEMA_KEYWORD_FLAG_NOT_EMPTY, &keyword, &val, &parent);

	assert_null(schema_keyword);
	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_DEPS, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal(
			"Array for keyword akey must not be empty", JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
}

/* Test getting of array if the data array of strings is not unique when it should be. */
static void test_jso_schema_keyword_get_array_of_strings_when_not_unique(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_array array;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);
	jso_array_init(&array);
	array.len = 1;

	JSO_VALUE_SET_ARRAY(val, &array);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "akey");
	expect_value(__wrap_jso_schema_data_get, keyword_flags,
			JSO_SCHEMA_KEYWORD_FLAG_UNIQUE | JSO_SCHEMA_KEYWORD_FLAG_NOT_EMPTY);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_TRUE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, &val);

	expect_function_call(__wrap_jso_array_is_unique);
	expect_value(__wrap_jso_array_is_unique, arr, &array);
	will_return(__wrap_jso_array_is_unique, JSO_FALSE);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_array_of_strings(&schema, &data,
			"akey", JSO_TRUE, JSO_SCHEMA_KEYWORD_FLAG_UNIQUE | JSO_SCHEMA_KEYWORD_FLAG_NOT_EMPTY,
			&keyword, &val, &parent);

	assert_null(schema_keyword);
	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_DEPS, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal(
			"Array values for keyword akey must be unique", JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_schema_keyword_validate_array_of_strings_when_all_strings),
		cmocka_unit_test(test_jso_schema_keyword_validate_array_of_strings_when_not_all_strings),
		cmocka_unit_test(test_jso_schema_keyword_validate_array_of_strings_when_unique),
		cmocka_unit_test(test_jso_schema_keyword_validate_array_of_strings_when_not_unique),
		cmocka_unit_test(test_jso_schema_keyword_validate_array_of_strings_when_empty),
		cmocka_unit_test(test_jso_schema_keyword_get_array_when_value_ok),
		cmocka_unit_test(test_jso_schema_keyword_get_array_when_value_not_found),
		cmocka_unit_test(test_jso_schema_keyword_get_array_when_empty),
		cmocka_unit_test(test_jso_schema_keyword_get_array_when_not_unique),
		cmocka_unit_test(test_jso_schema_keyword_get_array_of_strings_when_all_strings),
		cmocka_unit_test(test_jso_schema_keyword_get_array_of_strings_when_not_all_strings),
		cmocka_unit_test(test_jso_schema_keyword_get_array_of_strings_when_value_not_found),
		cmocka_unit_test(test_jso_schema_keyword_get_array_of_strings_when_empty),
		cmocka_unit_test(test_jso_schema_keyword_get_array_of_strings_when_not_unique),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
