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

#include "../../../src/schema/jso_schema_keyword.h"
#include "../../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* Wrapping. */

/* Wrapper for jso_schema_keyword_free. */
void __wrap_jso_schema_keyword_free(jso_schema_keyword *keyword)
{
	function_called();
	check_expected_ptr(keyword);
}

/* Wrapper for jso_schema_data_type_error. */
void __wrap_jso_schema_data_type_error(jso_schema *schema, const char *key, jso_value *val,
		const jso_value_type *expected_types, size_t expected_types_count)
{
	function_called();
	check_expected_ptr(schema);
	check_expected(key);
	check_expected_ptr(val);
	assert_non_null(expected_types);
	check_expected(expected_types_count);
}

/* Wrapper for jso_schema_keyword_get_ex. */
jso_schema_keyword *__wrap_jso_schema_keyword_get_ex(jso_schema *schema, jso_value *data,
		const char *key, jso_schema_value *parent, jso_schema_keyword_type keyword_type,
		jso_uint32 keyword_flags, jso_bool error_on_invalid_type,
		jso_schema_keyword *schema_keyword, jso_value *val)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected_ptr(parent);
	check_expected(keyword_type);
	check_expected(keyword_flags);
	check_expected(error_on_invalid_type);
	check_expected_ptr(schema_keyword);
	check_expected_ptr(val);

	JSO_SCHEMA_ERROR_TYPE(schema) = mock_type(jso_schema_error_type);

	return mock_ptr_type(jso_schema_keyword *);
}
/* Wrapper for jso_schema_keyword_get_union_of_2_types. */
jso_schema_keyword *__wrap_jso_schema_keyword_get_union_of_2_types(jso_schema *schema,
		jso_value *data, const char *key, jso_schema_value *parent,
		jso_schema_keyword_type keyword_union_type_1, jso_schema_keyword_type keyword_union_type_2,
		jso_uint32 keyword_flags, jso_schema_keyword *schema_keyword)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected_ptr(parent);
	check_expected(keyword_union_type_1);
	check_expected(keyword_union_type_2);
	check_expected(keyword_flags);
	check_expected_ptr(schema_keyword);

	JSO_SCHEMA_ERROR_TYPE(schema) = mock_type(jso_schema_error_type);

	return mock_ptr_type(jso_schema_keyword *);
}

/* Tests for jso_schema_keyword_set. */

/* Test setting of keyword when all successful and value is empty. */
static void test_jso_schema_keyword_set_when_success_and_empty(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data;
	jso_schema_value value;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	memset(&value, 0, sizeof(jso_schema_value));
	memset(&keyword, 0, sizeof(jso_schema_keyword));

	expect_function_call(__wrap_jso_schema_keyword_get_ex);
	expect_value(__wrap_jso_schema_keyword_get_ex, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_ex, data, &data);
	expect_string(__wrap_jso_schema_keyword_get_ex, key, "ukey");
	expect_value(__wrap_jso_schema_keyword_get_ex, parent, &value);
	expect_value(__wrap_jso_schema_keyword_get_ex, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_get_ex, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_ex, error_on_invalid_type, true);
	expect_value(__wrap_jso_schema_keyword_get_ex, schema_keyword, &keyword);
	expect_value(__wrap_jso_schema_keyword_get_ex, val, NULL);
	will_return(__wrap_jso_schema_keyword_get_ex, JSO_SCHEMA_ERROR_NONE);
	will_return(__wrap_jso_schema_keyword_get_ex, &keyword);

	jso_rc result = jso_schema_keyword_set(
			&schema, &data, "ukey", &value, &keyword, JSO_SCHEMA_KEYWORD_TYPE_INTEGER, 0);

	assert_int_equal(JSO_SUCCESS, result);
	assert_int_equal(0, JSO_SCHEMA_VALUE_FLAGS(value) & JSO_SCHEMA_VALUE_FLAG_NOT_EMPTY);
}

/* Test setting of keyword when all successful and value is not empty. */
static void test_jso_schema_keyword_set_when_success_and_not_empty(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data;
	jso_schema_value value;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	memset(&value, 0, sizeof(jso_schema_value));
	memset(&keyword, 0, sizeof(jso_schema_keyword));
	JSO_SCHEMA_KEYWORD_FLAGS(keyword) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;

	expect_function_call(__wrap_jso_schema_keyword_get_ex);
	expect_value(__wrap_jso_schema_keyword_get_ex, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_ex, data, &data);
	expect_string(__wrap_jso_schema_keyword_get_ex, key, "ukey");
	expect_value(__wrap_jso_schema_keyword_get_ex, parent, &value);
	expect_value(__wrap_jso_schema_keyword_get_ex, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_get_ex, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_ex, error_on_invalid_type, true);
	expect_value(__wrap_jso_schema_keyword_get_ex, schema_keyword, &keyword);
	expect_value(__wrap_jso_schema_keyword_get_ex, val, NULL);
	will_return(__wrap_jso_schema_keyword_get_ex, JSO_SCHEMA_ERROR_NONE);
	will_return(__wrap_jso_schema_keyword_get_ex, &keyword);

	jso_rc result = jso_schema_keyword_set(
			&schema, &data, "ukey", &value, &keyword, JSO_SCHEMA_KEYWORD_TYPE_INTEGER, 0);

	assert_int_equal(JSO_SUCCESS, result);
	assert_int_equal(JSO_SCHEMA_VALUE_FLAG_NOT_EMPTY,
			JSO_SCHEMA_VALUE_FLAGS(value) & JSO_SCHEMA_VALUE_FLAG_NOT_EMPTY);
}

/* Test setting of keyword when returned keyword is null. */
static void test_jso_schema_keyword_set_when_null(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data;
	jso_schema_value value;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	expect_function_call(__wrap_jso_schema_keyword_get_ex);
	expect_value(__wrap_jso_schema_keyword_get_ex, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_ex, data, &data);
	expect_string(__wrap_jso_schema_keyword_get_ex, key, "ukey");
	expect_value(__wrap_jso_schema_keyword_get_ex, parent, &value);
	expect_value(__wrap_jso_schema_keyword_get_ex, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_get_ex, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_ex, error_on_invalid_type, true);
	expect_value(__wrap_jso_schema_keyword_get_ex, schema_keyword, &keyword);
	expect_value(__wrap_jso_schema_keyword_get_ex, val, NULL);
	will_return(__wrap_jso_schema_keyword_get_ex, JSO_SCHEMA_ERROR_NONE);
	will_return(__wrap_jso_schema_keyword_get_ex, NULL);

	jso_rc result = jso_schema_keyword_set(
			&schema, &data, "ukey", &value, &keyword, JSO_SCHEMA_KEYWORD_TYPE_INTEGER, 0);

	assert_int_equal(JSO_FAILURE, result);
}

/* Test setting of keyword when schema error is set even if keyword returned. */
static void test_jso_schema_keyword_set_when_error(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data;
	jso_schema_value value;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	expect_function_call(__wrap_jso_schema_keyword_get_ex);
	expect_value(__wrap_jso_schema_keyword_get_ex, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_ex, data, &data);
	expect_string(__wrap_jso_schema_keyword_get_ex, key, "ukey");
	expect_value(__wrap_jso_schema_keyword_get_ex, parent, &value);
	expect_value(__wrap_jso_schema_keyword_get_ex, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_get_ex, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_ex, error_on_invalid_type, true);
	expect_value(__wrap_jso_schema_keyword_get_ex, schema_keyword, &keyword);
	expect_value(__wrap_jso_schema_keyword_get_ex, val, NULL);
	will_return(__wrap_jso_schema_keyword_get_ex, JSO_SCHEMA_ERROR_KEYWORD_TYPE);
	will_return(__wrap_jso_schema_keyword_get_ex, &keyword);

	jso_rc result = jso_schema_keyword_set(
			&schema, &data, "ukey", &value, &keyword, JSO_SCHEMA_KEYWORD_TYPE_INTEGER, 0);

	assert_int_equal(JSO_FAILURE, result);
}

/* Tests for jso_schema_keyword_set_union_of_2_types. */

/* Test setting of keyword with union of 2 types when all successful and value empty. */
static void test_jso_schema_keyword_set_union_of_2_types_when_success_and_empty(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data;
	jso_schema_value value;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	memset(&value, 0, sizeof(jso_schema_value));
	memset(&keyword, 0, sizeof(jso_schema_keyword));

	expect_function_call(__wrap_jso_schema_keyword_get_union_of_2_types);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, data, &data);
	expect_string(__wrap_jso_schema_keyword_get_union_of_2_types, key, "ukey");
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, parent, &value);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, keyword_union_type_1,
			JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, keyword_union_type_2,
			JSO_SCHEMA_KEYWORD_TYPE_STRING);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, schema_keyword, &keyword);
	will_return(__wrap_jso_schema_keyword_get_union_of_2_types, JSO_SCHEMA_ERROR_NONE);
	will_return(__wrap_jso_schema_keyword_get_union_of_2_types, &keyword);

	jso_rc result = jso_schema_keyword_set_union_of_2_types(&schema, &data, "ukey", &value,
			&keyword, JSO_SCHEMA_KEYWORD_TYPE_INTEGER, JSO_SCHEMA_KEYWORD_TYPE_STRING, 0);

	assert_int_equal(JSO_SUCCESS, result);
	assert_int_equal(0, JSO_SCHEMA_VALUE_FLAGS(value) & JSO_SCHEMA_VALUE_FLAG_NOT_EMPTY);
}

/* Test setting of keyword with union of 2 types when all successful and value is not empty. */
static void test_jso_schema_keyword_set_union_of_2_types_when_success_and_not_empty(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data;
	jso_schema_value value;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	memset(&value, 0, sizeof(jso_schema_value));
	memset(&keyword, 0, sizeof(jso_schema_keyword));
	JSO_SCHEMA_KEYWORD_FLAGS(keyword) |= JSO_SCHEMA_KEYWORD_FLAG_PRESENT;

	expect_function_call(__wrap_jso_schema_keyword_get_union_of_2_types);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, data, &data);
	expect_string(__wrap_jso_schema_keyword_get_union_of_2_types, key, "ukey");
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, parent, &value);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, keyword_union_type_1,
			JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, keyword_union_type_2,
			JSO_SCHEMA_KEYWORD_TYPE_STRING);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, schema_keyword, &keyword);
	will_return(__wrap_jso_schema_keyword_get_union_of_2_types, JSO_SCHEMA_ERROR_NONE);
	will_return(__wrap_jso_schema_keyword_get_union_of_2_types, &keyword);

	jso_rc result = jso_schema_keyword_set_union_of_2_types(&schema, &data, "ukey", &value,
			&keyword, JSO_SCHEMA_KEYWORD_TYPE_INTEGER, JSO_SCHEMA_KEYWORD_TYPE_STRING, 0);

	assert_int_equal(JSO_SUCCESS, result);
	assert_int_equal(JSO_SCHEMA_VALUE_FLAG_NOT_EMPTY,
			JSO_SCHEMA_VALUE_FLAGS(value) & JSO_SCHEMA_VALUE_FLAG_NOT_EMPTY);
}

/* Test setting of keyword with union of 2 types when returned keyword is null. */
static void test_jso_schema_keyword_set_union_of_2_types_when_null(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data;
	jso_schema_value value;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	expect_function_call(__wrap_jso_schema_keyword_get_union_of_2_types);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, data, &data);
	expect_string(__wrap_jso_schema_keyword_get_union_of_2_types, key, "ukey");
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, parent, &value);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, keyword_union_type_1,
			JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, keyword_union_type_2,
			JSO_SCHEMA_KEYWORD_TYPE_STRING);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, schema_keyword, &keyword);
	will_return(__wrap_jso_schema_keyword_get_union_of_2_types, JSO_SCHEMA_ERROR_NONE);
	will_return(__wrap_jso_schema_keyword_get_union_of_2_types, NULL);

	jso_rc result = jso_schema_keyword_set_union_of_2_types(&schema, &data, "ukey", &value,
			&keyword, JSO_SCHEMA_KEYWORD_TYPE_INTEGER, JSO_SCHEMA_KEYWORD_TYPE_STRING, 0);

	assert_int_equal(JSO_FAILURE, result);
}

/* Test setting of keyword with union of 2 types when schema error is set even if keyword returned.
 */
static void test_jso_schema_keyword_set_union_of_2_types_when_error(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data;
	jso_schema_value value;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	expect_function_call(__wrap_jso_schema_keyword_get_union_of_2_types);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, data, &data);
	expect_string(__wrap_jso_schema_keyword_get_union_of_2_types, key, "ukey");
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, parent, &value);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, keyword_union_type_1,
			JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, keyword_union_type_2,
			JSO_SCHEMA_KEYWORD_TYPE_STRING);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_union_of_2_types, schema_keyword, &keyword);
	will_return(__wrap_jso_schema_keyword_get_union_of_2_types, JSO_SCHEMA_ERROR_KEYWORD_PREP);
	will_return(__wrap_jso_schema_keyword_get_union_of_2_types, &keyword);

	jso_rc result = jso_schema_keyword_set_union_of_2_types(&schema, &data, "ukey", &value,
			&keyword, JSO_SCHEMA_KEYWORD_TYPE_INTEGER, JSO_SCHEMA_KEYWORD_TYPE_STRING, 0);

	assert_int_equal(JSO_FAILURE, result);
}

/* Tests for jso_schema_keyword_clear. */

/* Test schema clearing. */
static void test_jso_schema_keyword_clear(void **state)
{
	(void) state; /* unused */

	jso_schema_keyword keyword;

	JSO_SCHEMA_KEYWORD_FLAGS(keyword) = JSO_SCHEMA_KEYWORD_FLAG_PRESENT;

	expect_function_call(__wrap_jso_schema_keyword_free);
	expect_value(__wrap_jso_schema_keyword_free, keyword, &keyword);

	jso_schema_keyword_clear(&keyword);

	assert_int_equal(0, JSO_SCHEMA_KEYWORD_FLAGS(keyword));
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_schema_keyword_set_when_success_and_empty),
		cmocka_unit_test(test_jso_schema_keyword_set_when_success_and_not_empty),
		cmocka_unit_test(test_jso_schema_keyword_set_when_null),
		cmocka_unit_test(test_jso_schema_keyword_set_when_error),
		cmocka_unit_test(test_jso_schema_keyword_set_union_of_2_types_when_success_and_empty),
		cmocka_unit_test(test_jso_schema_keyword_set_union_of_2_types_when_success_and_not_empty),
		cmocka_unit_test(test_jso_schema_keyword_set_union_of_2_types_when_null),
		cmocka_unit_test(test_jso_schema_keyword_set_union_of_2_types_when_error),
		cmocka_unit_test(test_jso_schema_keyword_clear),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
