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

/* Wrapper for jso_schema_keyword_free. */
void __wrap_jso_schema_keyword_free(jso_schema_keyword *keyword)
{
	function_called();
	check_expected_ptr(keyword);
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

/* Tests for jso_schema_value_parse. */

/* Test parsing value. */
static void test_jso_schema_value_parse_string_when_all_good(void **state)
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
	expect_string(__wrap_jso_schema_data_get_value_fast, key, "ukey");
	expect_value(__wrap_jso_schema_data_get_value_fast, keyword_flags, 0);
	will_return(__wrap_jso_schema_data_get_value_fast, &val);

	expect_function_call(__wrap_jso_schema_keyword_get_ex);
	expect_value(__wrap_jso_schema_keyword_get_ex, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_ex, data, &data);
	expect_string(__wrap_jso_schema_keyword_get_ex, key, "ukey");
	expect_value(__wrap_jso_schema_keyword_get_ex, parent, &parent);
	expect_value(__wrap_jso_schema_keyword_get_ex, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_get_ex, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_ex, error_on_invalid_type, false);
	expect_value(__wrap_jso_schema_keyword_get_ex, schema_keyword, &keyword);
	expect_value(__wrap_jso_schema_keyword_get_ex, val, &val);
	will_return(__wrap_jso_schema_keyword_get_ex, JSO_SCHEMA_ERROR_NONE);
	will_return(__wrap_jso_schema_keyword_get_ex, NULL);

	expect_function_call(__wrap_jso_schema_keyword_get_ex);
	expect_value(__wrap_jso_schema_keyword_get_ex, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_ex, data, &data);
	expect_string(__wrap_jso_schema_keyword_get_ex, key, "ukey");
	expect_value(__wrap_jso_schema_keyword_get_ex, parent, &parent);
	expect_value(__wrap_jso_schema_keyword_get_ex, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_STRING);
	expect_value(__wrap_jso_schema_keyword_get_ex, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_ex, error_on_invalid_type, false);
	expect_value(__wrap_jso_schema_keyword_get_ex, schema_keyword, &keyword);
	expect_value(__wrap_jso_schema_keyword_get_ex, val, &val);
	will_return(__wrap_jso_schema_keyword_get_ex, JSO_SCHEMA_ERROR_NONE);
	will_return(__wrap_jso_schema_keyword_get_ex, NULL);

	expect_function_call(__wrap_jso_schema_keyword_get_types);
	expect_value(__wrap_jso_schema_keyword_get_types, schema, &schema);
	expect_string(__wrap_jso_schema_keyword_get_types, key, "ukey");
	expect_value(
			__wrap_jso_schema_keyword_get_types, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_INTEGER);
	expect_value(__wrap_jso_schema_keyword_get_types, types_size, JSO_SCHEMA_KW_VALUE_TYPES_SIZE);
	will_return(__wrap_jso_schema_keyword_get_types, 2);

	expect_function_call(__wrap_jso_schema_keyword_get_types);
	expect_value(__wrap_jso_schema_keyword_get_types, schema, &schema);
	expect_string(__wrap_jso_schema_keyword_get_types, key, "ukey");
	expect_value(__wrap_jso_schema_keyword_get_types, keyword_type, JSO_SCHEMA_KEYWORD_TYPE_STRING);
	expect_value(
			__wrap_jso_schema_keyword_get_types, types_size, JSO_SCHEMA_KW_VALUE_TYPES_SIZE - 2);
	will_return(__wrap_jso_schema_keyword_get_types, 1);

	expect_function_call(__wrap_jso_schema_data_type_error);
	expect_value(__wrap_jso_schema_data_type_error, schema, &schema);
	expect_string(__wrap_jso_schema_data_type_error, key, "ukey");
	expect_value(__wrap_jso_schema_data_type_error, val, &val);
	expect_value(__wrap_jso_schema_data_type_error, expected_types_count, 3);

	jso_schema_keyword *schema_keyword
			= jso_schema_keyword_get_union_of_2_types(&schema, &data, "ukey", &parent,
					JSO_SCHEMA_KEYWORD_TYPE_INTEGER, JSO_SCHEMA_KEYWORD_TYPE_STRING, 0, &keyword);

	assert_null(schema_keyword);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		// cmocka_unit_test(),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
