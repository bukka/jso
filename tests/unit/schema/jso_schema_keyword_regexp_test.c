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

#include "../../../src/schema/jso_schema_keyword_regexp.h"
#include "../../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* Wrapping. */

/* Wrapper for jso_re_code_alloc. */
jso_bool __wrap_jso_re_code_alloc()
{
	function_called();

	return mock_ptr_type(jso_re_code *);
}

/* Wrapper for jso_re_code_compile. */
jso_rc __wrap_jso_re_code_compile(jso_string *pattern, jso_re_code *code)
{
	check_expected_ptr(pattern);
	check_expected_ptr(code);
	function_called();
	
	return mock_type(jso_rc);
}

/* Wrapper for jso_re_code_free. */
void __wrap_jso_re_code_free(jso_re_code *code)
{
	function_called();
	check_expected_ptr(code);
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

/* Tests for jso_schema_keyword_get_regexp_code. */

/* Test validation of regexp code if successfully compiled. */
static void test_jso_schema_keyword_get_regexp_code_when_success(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_string pattern;
	jso_re_code code;

	jso_schema_init(&schema);

	expect_function_call(__wrap_jso_re_code_alloc);
	will_return(__wrap_jso_re_code_alloc, &code);

	expect_function_call(__wrap_jso_re_code_compile);
	expect_value(__wrap_jso_re_code_compile, pattern, &pattern);
	expect_value(__wrap_jso_re_code_compile, code, &code);
	will_return(__wrap_jso_re_code_compile, JSO_SUCCESS);

	jso_re_code *ret_re_code = jso_schema_keyword_get_regexp_code(&schema, "kre", NULL, &pattern);

	assert_ptr_equal(&code, ret_re_code);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		// cmocka_unit_test(test_jso_schema_keyword_get_regexp_code_when_success),
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
