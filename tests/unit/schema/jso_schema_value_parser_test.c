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

/* Test parsing value. */
static void test_jso_schema_value_parse_type_string_when_all_good(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, tval;
	jso_schema_value parent, value;
	jso_schema_value_string strval;
	jso_schema_keyword keyword;

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

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_schema_value_parse_type_string_when_all_good),
		// cmocka_unit_test(),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
