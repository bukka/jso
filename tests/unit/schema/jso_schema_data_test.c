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

#include "../../../src/schema/jso_schema_data.h"
#include "../../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* Wrapping. */

/* Wrapper for jso_ht_get_by_cstr_key. */
jso_rc __wrap_jso_ht_get_by_cstr_key(jso_ht *ht, const char *key, jso_value **value)
{
	check_expected_ptr(ht);
	check_expected(key);

	jso_rc rc = mock_type(jso_rc);
	if (rc == JSO_SUCCESS) {
		jso_value *val = mock_ptr_type(jso_value *);
		*value = val;
	}
	return rc;
}

/* Tests for jso_schema_data_type_error. */

/* Test type error emitting for no type. */
static void test_jso_schema_data_type_error_with_no_type(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_schema_init(&schema);

	jso_value val;
	JSO_VALUE_SET_INT(val, 1);

	const jso_value_type types[] = { JSO_TYPE_BOOL };

	jso_schema_data_type_error(&schema, "tkey", &val, types, 0);

	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_TYPE, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("Invalid type for tkey - expected no type but given int",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
}

/* Test type error emitting for 1 type. */
static void test_jso_schema_data_type_error_with_one_type(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_schema_init(&schema);

	jso_value val;
	JSO_VALUE_SET_INT(val, 1);

	const jso_value_type types[] = { JSO_TYPE_BOOL };

	jso_schema_data_type_error(&schema, "tkey", &val, types, 1);

	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_TYPE, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("Invalid type for tkey - expected bool but given int",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
}

/* Test type error emitting for 2 types. */
static void test_jso_schema_data_type_error_with_two_types(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_schema_init(&schema);

	jso_value val;
	JSO_VALUE_SET_INT(val, 1);

	const jso_value_type types[] = { JSO_TYPE_BOOL, JSO_TYPE_DOUBLE };

	jso_schema_data_type_error(&schema, "tkey", &val, types, 2);

	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_TYPE, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("Invalid type for tkey - expected either bool or double but given int",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
}

/* Test type error emitting for 3 types. */
static void test_jso_schema_data_type_error_with_three_types(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_schema_init(&schema);

	jso_value val;
	JSO_VALUE_SET_INT(val, 1);

	const jso_value_type types[] = { JSO_TYPE_BOOL, JSO_TYPE_ARRAY, JSO_TYPE_OBJECT };

	jso_schema_data_type_error(&schema, "tkey", &val, types, 3);

	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_TYPE, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal(
			"Invalid type for tkey - expected either bool, array or object but given int",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
}

/* tests for jso_schema_data_check_type */

/* Test checking type failure for successful check. */
static void test_jso_schema_data_check_type_success(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_schema_init(&schema);

	jso_value val;
	JSO_VALUE_SET_INT(val, 1);

	assert_true(
			jso_schema_data_check_type(&schema, "tkey", &val, JSO_TYPE_BOOL, JSO_TYPE_INT, false)
			== JSO_SUCCESS);
	assert_false(jso_schema_error_is_set(&schema));

	jso_schema_clear(&schema);
}

/* Test checking type failure for failed check without error reporting. */
static void test_jso_schema_data_check_type_failure_and_no_error(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_schema_init(&schema);

	jso_value val;
	JSO_VALUE_SET_INT(val, 1);

	assert_true(
			jso_schema_data_check_type(&schema, "tkey", &val, JSO_TYPE_ARRAY, JSO_TYPE_BOOL, false)
			== JSO_FAILURE);
	assert_false(jso_schema_error_is_set(&schema));

	jso_schema_clear(&schema);
}

/* Test checking type failure for failed check with error reporting and the same type. */
static void test_jso_schema_data_check_type_failure_and_single_type_error(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_schema_init(&schema);

	jso_value val;
	JSO_VALUE_SET_INT(val, 1);

	assert_true(
			jso_schema_data_check_type(&schema, "tkey", &val, JSO_TYPE_ARRAY, JSO_TYPE_ARRAY, true)
			== JSO_FAILURE);
	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_TYPE, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("Invalid type for tkey - expected array but given int",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
}

/* Test checking type failure for failed check with error reporting and two different types. */
static void test_jso_schema_data_check_type_failure_and_double_type_error(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_schema_init(&schema);

	jso_value val;
	JSO_VALUE_SET_INT(val, 1);

	assert_true(
			jso_schema_data_check_type(&schema, "tkey", &val, JSO_TYPE_ARRAY, JSO_TYPE_BOOL, true)
			== JSO_FAILURE);
	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_TYPE, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("Invalid type for tkey - expected either array or bool but given int",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
}

/* tests for jso_schema_data_get_value_fast */

/* Test getting value fast if it is found in the object. */
static void test_jso_schema_data_get_value_fast_if_found(void **state)
{
	jso_schema schema;
	jso_schema_init(&schema);

	jso_object *obj = jso_object_alloc();
	jso_value data;
	JSO_VALUE_SET_OBJECT(data, obj);

	jso_value val;
	JSO_VALUE_SET_INT(val, 1);

	expect_value(__wrap_jso_ht_get_by_cstr_key, ht, &obj->ht);
	expect_string(__wrap_jso_ht_get_by_cstr_key, key, "hkey");

	will_return(__wrap_jso_ht_get_by_cstr_key, JSO_SUCCESS);
	will_return(__wrap_jso_ht_get_by_cstr_key, &val);

	jso_value *rv = jso_schema_data_get_value_fast(&schema, &data, "hkey", 0);

	assert_true(jso_value_equals(rv, &val));

	jso_object_free(obj);
	jso_schema_clear(&schema);
}

/* Test getting value fast if it is not found in the object and value is not required. */
static void test_jso_schema_data_get_value_fast_if_not_found_and_not_required(void **state)
{
	jso_schema schema;
	jso_schema_init(&schema);

	jso_object *obj = jso_object_alloc();
	jso_value data;
	JSO_VALUE_SET_OBJECT(data, obj);

	jso_value val;
	JSO_VALUE_SET_NULL(val);

	expect_value(__wrap_jso_ht_get_by_cstr_key, ht, &obj->ht);
	expect_string(__wrap_jso_ht_get_by_cstr_key, key, "hkey");

	will_return(__wrap_jso_ht_get_by_cstr_key, JSO_FAILURE);

	jso_value *rv = jso_schema_data_get_value_fast(&schema, &data, "hkey", 0);

	assert_null(rv);
	assert_int_equal(JSO_TYPE_NULL, JSO_TYPE(val));
	assert_false(jso_schema_error_is_set(&schema));

	jso_object_free(obj);
	jso_schema_clear(&schema);
}

/* Test getting value fast if it is not found in the object and value is required. */
static void test_jso_schema_data_get_value_fast_if_not_found_and_required(void **state)
{
	jso_schema schema;
	jso_schema_init(&schema);

	jso_object *obj = jso_object_alloc();
	jso_value data;
	JSO_VALUE_SET_OBJECT(data, obj);

	jso_value val;
	JSO_VALUE_SET_NULL(val);

	expect_value(__wrap_jso_ht_get_by_cstr_key, ht, &obj->ht);
	expect_string(__wrap_jso_ht_get_by_cstr_key, key, "hkey");

	will_return(__wrap_jso_ht_get_by_cstr_key, JSO_FAILURE);

	jso_value *rv = jso_schema_data_get_value_fast(
			&schema, &data, "hkey", JSO_SCHEMA_KEYWORD_FLAG_REQUIRED);

	assert_null(rv);
	assert_int_equal(JSO_TYPE_NULL, JSO_TYPE(val));
	assert_int_equal(JSO_SCHEMA_ERROR_KEYWORD_REQUIRED, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("Keyword hkey is required", JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_object_free(obj);
	jso_schema_clear(&schema);
}

/* tests for jso_schema_data_get_value */

/* Test getting value if value set before. */
static void test_jso_schema_data_get_value_if_set(void **state)
{
	jso_schema schema;
	jso_schema_init(&schema);

	jso_object *obj = jso_object_alloc();
	jso_value data;
	JSO_VALUE_SET_OBJECT(data, obj);

	jso_value val;
	JSO_VALUE_SET_INT(val, 1);

	jso_value *rv = jso_schema_data_get_value(&schema, &data, "hkey", 0, &val);

	assert_true(jso_value_equals(rv, &val));

	jso_object_free(obj);
	jso_schema_clear(&schema);
}

/* Test getting value if no value set before. */
static void test_jso_schema_data_get_value_if_not_set(void **state)
{
	jso_schema schema;
	jso_schema_init(&schema);

	jso_object *obj = jso_object_alloc();
	jso_value data;
	JSO_VALUE_SET_OBJECT(data, obj);

	jso_value val;
	JSO_VALUE_SET_INT(val, 1);

	expect_value(__wrap_jso_ht_get_by_cstr_key, ht, &obj->ht);
	expect_string(__wrap_jso_ht_get_by_cstr_key, key, "hkey");

	will_return(__wrap_jso_ht_get_by_cstr_key, JSO_SUCCESS);
	will_return(__wrap_jso_ht_get_by_cstr_key, &val);

	jso_value *rv = jso_schema_data_get_value(&schema, &data, "hkey", 0, NULL);

	assert_true(jso_value_equals(rv, &val));

	jso_object_free(obj);
	jso_schema_clear(&schema);
}

/* Tests for jso_schema_data_get. */

/* Test getting value if no value set before and successful type check. */
static void test_jso_schema_data_get_if_set(void **state)
{
	jso_schema schema;
	jso_schema_init(&schema);

	jso_object *obj = jso_object_alloc();
	jso_value data;
	JSO_VALUE_SET_OBJECT(data, obj);

	jso_value val;
	JSO_VALUE_SET_INT(val, 1);

	jso_value *rv = jso_schema_data_get(&schema, &data, "hkey", JSO_TYPE_INT, 0, true, &val);

	assert_ptr_equal(rv, &val);

	jso_object_free(obj);
	jso_schema_clear(&schema);
}

/* Test getting value if no value set before and successful type check. */
static void test_jso_schema_data_get_if_not_set_and_success(void **state)
{
	jso_schema schema;
	jso_schema_init(&schema);

	jso_object *obj = jso_object_alloc();
	jso_value data;
	JSO_VALUE_SET_OBJECT(data, obj);

	jso_value val;
	JSO_VALUE_SET_INT(val, 1);

	expect_value(__wrap_jso_ht_get_by_cstr_key, ht, &obj->ht);
	expect_string(__wrap_jso_ht_get_by_cstr_key, key, "hkey");

	will_return(__wrap_jso_ht_get_by_cstr_key, JSO_SUCCESS);
	will_return(__wrap_jso_ht_get_by_cstr_key, &val);

	jso_value *rv = jso_schema_data_get(&schema, &data, "hkey", JSO_TYPE_INT, 0, true, NULL);

	assert_true(jso_value_equals(rv, &val));
	assert_false(jso_schema_error_is_set(&schema));

	jso_object_free(obj);
	jso_schema_clear(&schema);
}

/* Test getting value if no value set before and failed type check with no error. */
static void test_jso_schema_data_get_if_not_set_and_fail_with_no_error(void **state)
{
	jso_schema schema;
	jso_schema_init(&schema);

	jso_object *obj = jso_object_alloc();
	jso_value data;
	JSO_VALUE_SET_OBJECT(data, obj);

	jso_value val;
	JSO_VALUE_SET_INT(val, 1);

	expect_value(__wrap_jso_ht_get_by_cstr_key, ht, &obj->ht);
	expect_string(__wrap_jso_ht_get_by_cstr_key, key, "hkey");

	will_return(__wrap_jso_ht_get_by_cstr_key, JSO_SUCCESS);
	will_return(__wrap_jso_ht_get_by_cstr_key, &val);

	jso_value *rv = jso_schema_data_get(&schema, &data, "hkey", JSO_TYPE_BOOL, 0, false, NULL);

	assert_null(rv);
	assert_false(jso_schema_error_is_set(&schema));

	jso_object_free(obj);
	jso_schema_clear(&schema);
}

/* Test getting value if no value set before and failed type check with no error. */
static void test_jso_schema_data_get_if_not_set_and_fail_with_error(void **state)
{
	jso_schema schema;
	jso_schema_init(&schema);

	jso_object *obj = jso_object_alloc();
	jso_value data;
	JSO_VALUE_SET_OBJECT(data, obj);

	jso_value val;
	JSO_VALUE_SET_INT(val, 1);

	expect_value(__wrap_jso_ht_get_by_cstr_key, ht, &obj->ht);
	expect_string(__wrap_jso_ht_get_by_cstr_key, key, "hkey");

	will_return(__wrap_jso_ht_get_by_cstr_key, JSO_SUCCESS);
	will_return(__wrap_jso_ht_get_by_cstr_key, &val);

	jso_value *rv = jso_schema_data_get(&schema, &data, "hkey", JSO_TYPE_BOOL, 0, true, NULL);

	assert_null(rv);
	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_TYPE, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("Invalid type for hkey - expected bool but given int",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_object_free(obj);
	jso_schema_clear(&schema);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_schema_data_type_error_with_no_type),
		cmocka_unit_test(test_jso_schema_data_type_error_with_one_type),
		cmocka_unit_test(test_jso_schema_data_type_error_with_two_types),
		cmocka_unit_test(test_jso_schema_data_type_error_with_three_types),
		cmocka_unit_test(test_jso_schema_data_check_type_success),
		cmocka_unit_test(test_jso_schema_data_check_type_failure_and_no_error),
		cmocka_unit_test(test_jso_schema_data_check_type_failure_and_single_type_error),
		cmocka_unit_test(test_jso_schema_data_check_type_failure_and_double_type_error),
		cmocka_unit_test(test_jso_schema_data_get_value_fast_if_found),
		cmocka_unit_test(test_jso_schema_data_get_value_fast_if_not_found_and_not_required),
		cmocka_unit_test(test_jso_schema_data_get_value_fast_if_not_found_and_required),
		cmocka_unit_test(test_jso_schema_data_get_value_if_set),
		cmocka_unit_test(test_jso_schema_data_get_value_if_not_set),
		cmocka_unit_test(test_jso_schema_data_get_if_set),
		cmocka_unit_test(test_jso_schema_data_get_if_not_set_and_success),
		cmocka_unit_test(test_jso_schema_data_get_if_not_set_and_fail_with_no_error),
		cmocka_unit_test(test_jso_schema_data_get_if_not_set_and_fail_with_error),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
