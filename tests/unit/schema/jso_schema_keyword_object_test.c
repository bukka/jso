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

#include "../../../src/schema/jso_schema_keyword_object.h"
#include "../../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* Wrapping. */

/* Wrapper for jso_object_add. */
jso_rc __wrap_jso_object_add(jso_object *obj, jso_string *key, jso_value *val)
{
	function_called();
	check_expected_ptr(obj);
	check_expected_ptr(key);
	check_expected_ptr(val);

	return mock_type(jso_rc);
}

/* Wrapper for jso_object_alloc. */
jso_object *__wrap_jso_object_alloc()
{
	function_called();

	return mock_ptr_type(jso_object *);
}

/* Wrapper for jso_object_free. */
void __wrap_jso_object_free(jso_object *obj)
{
	function_called();
	check_expected_ptr(obj);
}

/* Wrapper for jso_object_add. */
jso_rc __wrap_jso_object_resize(jso_object *obj, size_t size)
{
	function_called();
	check_expected_ptr(obj);
	check_expected(size);

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

/* Wrapper for jso_schema_keyword_get_regexp_code. */
jso_re_code *__wrap_jso_schema_keyword_get_regexp_code(
		jso_schema *schema, const char *keyword_key, jso_string *object_key, jso_value *val)
{
	function_called();
	check_expected_ptr(schema);
	check_expected(keyword_key);
	check_expected_ptr(object_key);
	check_expected_ptr(val);

	return mock_ptr_type(jso_re_code *);
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
extern void __real_jso_schema_value_free(jso_schema_value *val);
void __wrap_jso_schema_value_free(jso_schema_value *val)
{
	function_called();
	check_expected_ptr(val);
	__real_jso_schema_value_free(val);
}

/* Wrapper for jso_schema_value_parse. */
jso_schema_value *__wrap_jso_schema_value_parse(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected_ptr(parent);

	return mock_ptr_type(jso_schema_value *);
}

/* Helper funcitons */
static void jso_test_clear_schema(jso_schema *schema)
{
	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, val, schema->root);
	jso_schema_clear(schema);
}

/* Tests for jso_schema_keyword_get_object. */

/* Test getting of object if all good. */
static void test_jso_schema_keyword_get_object_when_value_ok(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_object object;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);
	jso_object_init(&object);

	JSO_VALUE_SET_OBJECT(val, &object);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "okey");
	expect_value(__wrap_jso_schema_data_get, type, JSO_TYPE_OBJECT);
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_TRUE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, &val);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_object(
			&schema, &data, "okey", JSO_TRUE, 0, &keyword, &val, &parent);

	assert_non_null(schema_keyword);
	assert_true(JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) & JSO_SCHEMA_KEYWORD_FLAG_PRESENT);
	assert_ptr_equal(&object, JSO_SCHEMA_KEYWORD_DATA_OBJ_P(schema_keyword));
	assert_int_equal(JSO_OBJECT_REFCOUNT(&object), 1);

	jso_test_clear_schema(&schema);
}

/* Test getting of object if no data. */
static void test_jso_schema_keyword_get_object_when_value_not_found(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_object object;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);
	jso_object_init(&object);

	JSO_VALUE_SET_OBJECT(val, &object);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "okey");
	expect_value(__wrap_jso_schema_data_get, type, JSO_TYPE_OBJECT);
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_FALSE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, NULL);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_object(
			&schema, &data, "okey", JSO_FALSE, 0, &keyword, &val, &parent);

	assert_null(schema_keyword);

	jso_test_clear_schema(&schema);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_schema_keyword_get_object_when_value_ok),
		cmocka_unit_test(test_jso_schema_keyword_get_object_when_value_not_found),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
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