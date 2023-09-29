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

#include "../../../src/schema/jso_schema_keyword_scalar.h"
#include "../../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* Wrapping. */

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

/* Wrapper for jso_schema_data_get. */
jso_value *__wrap_jso_schema_data_get(jso_schema *schema, jso_value *data, const char *key,
		jso_value_type type, jso_uint32 keyword_flags, jso_bool error_on_invalid_type,
		jso_value *val)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected(type);
	check_expected(keyword_flags);
	check_expected(error_on_invalid_type);
	check_expected_ptr(val);

	return mock_ptr_type(jso_value *);
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

/* Tests for jso_schema_keyword_get_any. */

/* Test getting any keyword when all successful. */
static void test_jso_schema_keyword_get_any_when_success(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "nkey");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &val);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_any(
			&schema, &data, "nkey", JSO_TRUE, 0, &keyword, NULL, &parent);

	assert_non_null(schema_keyword);
	assert_true(JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) & JSO_SCHEMA_KEYWORD_FLAG_PRESENT);
	assert_int_equal(JSO_SCHEMA_KEYWORD_TYPE_ANY, JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword));
	assert_ptr_equal(&val, JSO_SCHEMA_KEYWORD_DATA_ANY_P(schema_keyword));
}

/* Test getting any keyword when no schema data present. */
static void test_jso_schema_keyword_get_any_when_no_data(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "nkey");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, NULL);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_any(
			&schema, &data, "nkey", JSO_FALSE, 0, &keyword, NULL, &parent);

	assert_null(schema_keyword);

	jso_schema_clear(&schema);
}

/* Tests for jso_schema_keyword_get_null. */

/* Test getting null keyword when all successful. */
static void test_jso_schema_keyword_get_null_when_success(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	JSO_VALUE_SET_NULL(val);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "nkey");
	expect_value(__wrap_jso_schema_data_get, type, JSO_TYPE_NULL);
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_TRUE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, &val);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_null(
			&schema, &data, "nkey", JSO_TRUE, 0, &keyword, &val, &parent);

	assert_non_null(schema_keyword);
	assert_true(JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) & JSO_SCHEMA_KEYWORD_FLAG_PRESENT);
	assert_int_equal(JSO_SCHEMA_KEYWORD_TYPE_NULL, JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword));
}

/* Test getting null keyword when no schema data present. */
static void test_jso_schema_keyword_get_null_when_no_data(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "nkey");
	expect_value(__wrap_jso_schema_data_get, type, JSO_TYPE_NULL);
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_FALSE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, NULL);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_null(
			&schema, &data, "nkey", JSO_FALSE, 0, &keyword, &val, &parent);

	assert_null(schema_keyword);

	jso_schema_clear(&schema);
}

/* Tests for jso_schema_keyword_get_bool. */

/* Test getting boolean keyword when all successful. */
static void test_jso_schema_keyword_get_bool_when_success(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	JSO_VALUE_SET_BOOL(val, JSO_TRUE);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "bkey");
	expect_value(__wrap_jso_schema_data_get, type, JSO_TYPE_BOOL);
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_TRUE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, &val);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_bool(
			&schema, &data, "bkey", JSO_TRUE, 0, &keyword, &val, &parent);

	assert_non_null(schema_keyword);
	assert_true(JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) & JSO_SCHEMA_KEYWORD_FLAG_PRESENT);
	assert_int_equal(JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN, JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword));
	assert_ptr_equal(JSO_TRUE, JSO_SCHEMA_KEYWORD_DATA_BOOL_P(schema_keyword));
}

/* Test getting boolean keyword when no schema data present. */
static void test_jso_schema_keyword_get_bool_when_no_data(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "bkey");
	expect_value(__wrap_jso_schema_data_get, type, JSO_TYPE_BOOL);
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_FALSE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, NULL);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_bool(
			&schema, &data, "bkey", JSO_FALSE, 0, &keyword, &val, &parent);

	assert_null(schema_keyword);

	jso_schema_clear(&schema);
}

/* Tests for jso_schema_keyword_get_int. */

/* Test getting integer keyword when integer provided. */
static void test_jso_schema_keyword_get_int_when_int(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	JSO_VALUE_SET_INT(val, 3);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "ikey");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &val);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "ikey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_FALSE);
	will_return(__wrap_jso_schema_data_check_type, JSO_SUCCESS);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_int(
			&schema, &data, "ikey", JSO_TRUE, 0, &keyword, NULL, &parent);

	assert_non_null(schema_keyword);
	assert_true(JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) & JSO_SCHEMA_KEYWORD_FLAG_PRESENT);
	assert_int_equal(JSO_SCHEMA_KEYWORD_TYPE_INTEGER, JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword));
	assert_int_equal(3, JSO_SCHEMA_KEYWORD_DATA_INT_P(schema_keyword));
}

/* Test getting integer keyword when double that is near to integer value provided. */
static void test_jso_schema_keyword_get_int_when_near_double(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	JSO_VALUE_SET_DOUBLE(val, 3.0);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "ikey");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &val);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "ikey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_FALSE);
	will_return(__wrap_jso_schema_data_check_type, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "ikey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_DOUBLE);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_TRUE);
	will_return(__wrap_jso_schema_data_check_type, JSO_SUCCESS);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_int(
			&schema, &data, "ikey", JSO_TRUE, 0, &keyword, NULL, &parent);

	assert_non_null(schema_keyword);
	assert_true(JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) & JSO_SCHEMA_KEYWORD_FLAG_PRESENT);
	assert_int_equal(JSO_SCHEMA_KEYWORD_TYPE_INTEGER, JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword));
	assert_int_equal(3, JSO_SCHEMA_KEYWORD_DATA_INT_P(schema_keyword));
}

/* Test getting integer keyword when double that is not near to integer value provided. */
static void test_jso_schema_keyword_get_int_when_distant_double(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	JSO_VALUE_SET_DOUBLE(val, 3.1);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "ikey");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &val);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "ikey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_FALSE);
	will_return(__wrap_jso_schema_data_check_type, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "ikey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_DOUBLE);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_TRUE);
	will_return(__wrap_jso_schema_data_check_type, JSO_SUCCESS);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_int(
			&schema, &data, "ikey", JSO_TRUE, 0, &keyword, NULL, &parent);

	assert_null(schema_keyword);
	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_TYPE, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal(
			"Integer fraction is not zero for keyword ikey", JSO_SCHEMA_ERROR_MESSAGE(&schema));
}

/* Test getting integer keyword when not int or double provided. */
static void test_jso_schema_keyword_get_int_when_not_int_or_double(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;
	jso_string str;

	jso_schema_init(&schema);

	JSO_VALUE_SET_STRING(val, &str);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "ikey");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &val);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "ikey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_FALSE);
	will_return(__wrap_jso_schema_data_check_type, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "ikey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_DOUBLE);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_TRUE);
	will_return(__wrap_jso_schema_data_check_type, JSO_FAILURE);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_int(
			&schema, &data, "ikey", JSO_TRUE, 0, &keyword, NULL, &parent);

	assert_null(schema_keyword);
}

/* Test getting integer keyword when no schema data present. */
static void test_jso_schema_keyword_get_int_when_no_data(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "ikey");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, NULL);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_int(
			&schema, &data, "ikey", JSO_FALSE, 0, &keyword, NULL, &parent);

	assert_null(schema_keyword);

	jso_schema_clear(&schema);
}

/* Tests for jso_schema_keyword_get_uint. */

/* Test getting unsigned integer keyword when integer provided. */
static void test_jso_schema_keyword_get_uint_when_int(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	JSO_VALUE_SET_INT(val, 3);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "uikey");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &val);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "uikey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_FALSE);
	will_return(__wrap_jso_schema_data_check_type, JSO_SUCCESS);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_uint(
			&schema, &data, "uikey", JSO_TRUE, 0, &keyword, NULL, &parent);

	assert_non_null(schema_keyword);
	assert_true(JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) & JSO_SCHEMA_KEYWORD_FLAG_PRESENT);
	assert_int_equal(
			JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER, JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword));
	assert_int_equal(3, JSO_SCHEMA_KEYWORD_DATA_UINT_P(schema_keyword));
}

/* Test getting unsigned integer keyword when double that is near to integer value provided. */
static void test_jso_schema_keyword_get_uint_when_near_double(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	JSO_VALUE_SET_DOUBLE(val, 4.0);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "uikey");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &val);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "uikey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_FALSE);
	will_return(__wrap_jso_schema_data_check_type, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "uikey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_DOUBLE);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_TRUE);
	will_return(__wrap_jso_schema_data_check_type, JSO_SUCCESS);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_uint(
			&schema, &data, "uikey", JSO_TRUE, 0, &keyword, NULL, &parent);

	assert_non_null(schema_keyword);
	assert_true(JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) & JSO_SCHEMA_KEYWORD_FLAG_PRESENT);
	assert_int_equal(
			JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER, JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword));
	assert_int_equal(4, JSO_SCHEMA_KEYWORD_DATA_UINT_P(schema_keyword));
}

/* Test getting unsigned integer keyword when double that is not near to integer value provided. */
static void test_jso_schema_keyword_get_uint_when_distant_double(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	JSO_VALUE_SET_DOUBLE(val, 2.99999);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "uikey");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &val);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "uikey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_FALSE);
	will_return(__wrap_jso_schema_data_check_type, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "uikey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_DOUBLE);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_TRUE);
	will_return(__wrap_jso_schema_data_check_type, JSO_SUCCESS);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_uint(
			&schema, &data, "uikey", JSO_TRUE, 0, &keyword, NULL, &parent);

	assert_null(schema_keyword);
	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_TYPE, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal(
			"Integer fraction is not zero for keyword uikey", JSO_SCHEMA_ERROR_MESSAGE(&schema));
}

/* Test getting unsigned integer keyword when not int or double provided. */
static void test_jso_schema_keyword_get_uint_when_not_int_or_double(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;
	jso_string str;

	jso_schema_init(&schema);

	JSO_VALUE_SET_STRING(val, &str);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "uikey");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &val);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "uikey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_FALSE);
	will_return(__wrap_jso_schema_data_check_type, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "uikey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_DOUBLE);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_TRUE);
	will_return(__wrap_jso_schema_data_check_type, JSO_FAILURE);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_uint(
			&schema, &data, "uikey", JSO_TRUE, 0, &keyword, NULL, &parent);

	assert_null(schema_keyword);
}

/* Test getting unsigned integer keyword when integer value negative. */
static void test_jso_schema_keyword_get_uint_when_int_negative(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	JSO_VALUE_SET_INT(val, -5);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "uikey");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &val);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "uikey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_FALSE);
	will_return(__wrap_jso_schema_data_check_type, JSO_SUCCESS);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_uint(
			&schema, &data, "uikey", JSO_TRUE, 0, &keyword, NULL, &parent);

	assert_null(schema_keyword);
	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_TYPE, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("Value for for keyword uikey must be equal or greater than 0",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));
}

/* Test getting unsigned integer keyword when integer is zero and not zero flag supplied. */
static void test_jso_schema_keyword_get_uint_when_int_zero_not_allowed(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	JSO_VALUE_SET_INT(val, 0);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "uikey");
	expect_value(
			__wrap_jso_schema_data_get_value_fast, keyword_flags, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO);
	will_return(__wrap_jso_schema_data_get_value_fast, &val);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "uikey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_FALSE);
	will_return(__wrap_jso_schema_data_check_type, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "uikey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_DOUBLE);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_TRUE);
	will_return(__wrap_jso_schema_data_check_type, JSO_SUCCESS);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_uint(&schema, &data, "uikey",
			JSO_TRUE, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO, &keyword, NULL, &parent);

	assert_null(schema_keyword);
	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_TYPE, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("Value for for keyword uikey must be greater than 0",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));
}

/* Test getting unsigned integer keyword when no schema data present. */
static void test_jso_schema_keyword_get_uint_when_no_data(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "uikey");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, NULL);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_uint(
			&schema, &data, "uikey", JSO_FALSE, 0, &keyword, NULL, &parent);

	assert_null(schema_keyword);

	jso_schema_clear(&schema);
}

/* Tests for jso_schema_keyword_get_number. */

/* Test getting integer keyword when integer provided. */
static void test_jso_schema_keyword_get_number_when_int(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	JSO_VALUE_SET_INT(val, 3);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "numkey");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &val);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "numkey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_FALSE);
	will_return(__wrap_jso_schema_data_check_type, JSO_SUCCESS);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_number(
			&schema, &data, "numkey", JSO_TRUE, 0, &keyword, NULL, &parent);

	assert_non_null(schema_keyword);
	assert_true(JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) & JSO_SCHEMA_KEYWORD_FLAG_PRESENT);
	assert_int_equal(JSO_SCHEMA_KEYWORD_TYPE_INTEGER, JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword));
	assert_int_equal(3, JSO_SCHEMA_KEYWORD_DATA_INT_P(schema_keyword));
}

/* Test getting integer keyword when double value provided. */
static void test_jso_schema_keyword_get_number_when_double(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	JSO_VALUE_SET_DOUBLE(val, 3.0);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "numkey");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &val);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "numkey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_FALSE);
	will_return(__wrap_jso_schema_data_check_type, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "numkey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_DOUBLE);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_TRUE);
	will_return(__wrap_jso_schema_data_check_type, JSO_SUCCESS);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_number(
			&schema, &data, "numkey", JSO_TRUE, 0, &keyword, NULL, &parent);

	assert_non_null(schema_keyword);
	assert_true(JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) & JSO_SCHEMA_KEYWORD_FLAG_PRESENT);
	assert_int_equal(JSO_SCHEMA_KEYWORD_TYPE_DOUBLE, JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword));
	assert_int_equal(3, JSO_SCHEMA_KEYWORD_DATA_DOUBLE_P(schema_keyword));
}

/* Test getting integer keyword when not int or double provided. */
static void test_jso_schema_keyword_get_number_when_not_int_or_double(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;
	jso_string str;

	jso_schema_init(&schema);

	JSO_VALUE_SET_STRING(val, &str);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "numkey");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &val);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "numkey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_FALSE);
	will_return(__wrap_jso_schema_data_check_type, JSO_FAILURE);

	expect_function_call(__wrap_jso_schema_data_check_type);
	expect_value(__wrap_jso_schema_data_check_type, schema, &schema);
	expect_string(__wrap_jso_schema_data_check_type, key, "numkey");
	expect_value(__wrap_jso_schema_data_check_type, val, &val);
	expect_value(__wrap_jso_schema_data_check_type, type, JSO_TYPE_DOUBLE);
	expect_value(__wrap_jso_schema_data_check_type, primary_type, JSO_TYPE_INT);
	expect_value(__wrap_jso_schema_data_check_type, error_on_invalid_type, JSO_TRUE);
	will_return(__wrap_jso_schema_data_check_type, JSO_FAILURE);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_number(
			&schema, &data, "numkey", JSO_TRUE, 0, &keyword, NULL, &parent);

	assert_null(schema_keyword);
}

/* Test getting integer keyword when no schema data present. */
static void test_jso_schema_keyword_get_number_when_no_data(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	expect_function_call(__wrap_jso_schema_data_get_value_fast);
	expect_value(__wrap_jso_schema_data_get_value_fast, schema, &schema);
	expect_value(__wrap_jso_schema_data_get_value_fast, data, &data);
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "numkey");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, NULL);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_number(
			&schema, &data, "numkey", JSO_FALSE, 0, &keyword, NULL, &parent);

	assert_null(schema_keyword);

	jso_schema_clear(&schema);
}

/* Tests for jso_schema_keyword_get_string. */

/* Test getting string keyword when all successful. */
static void test_jso_schema_keyword_get_string_when_success(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;
	jso_string str;

	jso_schema_init(&schema);

	JSO_VALUE_SET_STRING(val, &str);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "skey");
	expect_value(__wrap_jso_schema_data_get, type, JSO_TYPE_STRING);
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_TRUE);
	expect_value(__wrap_jso_schema_data_get, val, NULL);
	will_return(__wrap_jso_schema_data_get, &val);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_string(
			&schema, &data, "skey", JSO_TRUE, 0, &keyword, NULL, &parent);

	assert_non_null(schema_keyword);
	assert_true(JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) & JSO_SCHEMA_KEYWORD_FLAG_PRESENT);
	assert_int_equal(JSO_SCHEMA_KEYWORD_TYPE_STRING, JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword));
	assert_ptr_equal(&str, JSO_SCHEMA_KEYWORD_DATA_STR_P(schema_keyword));
}

/* Test getting string keyword when no schema data present. */
static void test_jso_schema_keyword_get_string_when_no_data(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "skey");
	expect_value(__wrap_jso_schema_data_get, type, JSO_TYPE_STRING);
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_FALSE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, NULL);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_string(
			&schema, &data, "skey", JSO_FALSE, 0, &keyword, &val, &parent);

	assert_null(schema_keyword);

	jso_schema_clear(&schema);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_schema_keyword_get_any_when_success),
		cmocka_unit_test(test_jso_schema_keyword_get_any_when_no_data),
		cmocka_unit_test(test_jso_schema_keyword_get_null_when_success),
		cmocka_unit_test(test_jso_schema_keyword_get_null_when_no_data),
		cmocka_unit_test(test_jso_schema_keyword_get_bool_when_success),
		cmocka_unit_test(test_jso_schema_keyword_get_bool_when_no_data),
		cmocka_unit_test(test_jso_schema_keyword_get_int_when_int),
		cmocka_unit_test(test_jso_schema_keyword_get_int_when_near_double),
		cmocka_unit_test(test_jso_schema_keyword_get_int_when_distant_double),
		cmocka_unit_test(test_jso_schema_keyword_get_int_when_no_data),
		cmocka_unit_test(test_jso_schema_keyword_get_int_when_not_int_or_double),
		cmocka_unit_test(test_jso_schema_keyword_get_uint_when_int),
		cmocka_unit_test(test_jso_schema_keyword_get_uint_when_near_double),
		cmocka_unit_test(test_jso_schema_keyword_get_uint_when_distant_double),
		cmocka_unit_test(test_jso_schema_keyword_get_uint_when_int_negative),
		cmocka_unit_test(test_jso_schema_keyword_get_uint_when_int_zero_not_allowed),
		cmocka_unit_test(test_jso_schema_keyword_get_uint_when_no_data),
		cmocka_unit_test(test_jso_schema_keyword_get_uint_when_not_int_or_double),
		cmocka_unit_test(test_jso_schema_keyword_get_number_when_int),
		cmocka_unit_test(test_jso_schema_keyword_get_number_when_double),
		cmocka_unit_test(test_jso_schema_keyword_get_number_when_no_data),
		cmocka_unit_test(test_jso_schema_keyword_get_number_when_not_int_or_double),
		cmocka_unit_test(test_jso_schema_keyword_get_string_when_success),
		cmocka_unit_test(test_jso_schema_keyword_get_string_when_no_data),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
