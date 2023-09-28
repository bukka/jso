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
jso_re_code *__wrap_jso_re_code_alloc()
{
	function_called();

	return mock_ptr_type(jso_re_code *);
}

/* Wrapper for jso_re_compile. */
jso_rc __wrap_jso_re_compile(jso_string *pattern, jso_re_code *code)
{
	function_called();
	check_expected_ptr(pattern);
	check_expected_ptr(code);

	return mock_type(jso_rc);
}

/* Wrapper for jso_re_get_error_message. */
char *__wrap_jso_re_get_error_message(jso_re_code *code, jso_ctype *buf, size_t buf_size)
{
	function_called();

	check_expected_ptr(code);
	check_expected_ptr(buf_size);
	const char *data = mock_ptr_type(const char *);

	strncpy(buf, data, buf_size);

	return (char *) buf;
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

	expect_function_call(__wrap_jso_re_compile);
	expect_value(__wrap_jso_re_compile, pattern, &pattern);
	expect_value(__wrap_jso_re_compile, code, &code);
	will_return(__wrap_jso_re_compile, JSO_SUCCESS);

	jso_re_code *ret_re_code = jso_schema_keyword_get_regexp_code(&schema, "kre", NULL, &pattern);

	assert_ptr_equal(&code, ret_re_code);
}

/* Test validation of regexp code if code allocation fails and there is no objkey. */
static void test_jso_schema_keyword_get_regexp_code_when_alloc_fails_without_objkey(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_string pattern;

	jso_schema_init(&schema);

	expect_function_call(__wrap_jso_re_code_alloc);
	will_return(__wrap_jso_re_code_alloc, NULL);

	jso_re_code *ret_re_code = jso_schema_keyword_get_regexp_code(&schema, "kre", NULL, &pattern);

	assert_null(ret_re_code);
	assert_int_equal(JSO_SCHEMA_ERROR_KEYWORD_ALLOC, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("Allocating regular expression code for keyword kre failed",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));
}

/* Test validation of regexp code if code allocation fails and there is objkey. */
static void test_jso_schema_keyword_get_regexp_code_when_alloc_fails_with_objkey(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_string pattern;
	jso_string *objkey = jso_string_create_from_cstr("okey");

	jso_schema_init(&schema);

	expect_function_call(__wrap_jso_re_code_alloc);
	will_return(__wrap_jso_re_code_alloc, NULL);

	jso_re_code *ret_re_code = jso_schema_keyword_get_regexp_code(&schema, "kre", objkey, &pattern);

	assert_null(ret_re_code);
	assert_int_equal(JSO_SCHEMA_ERROR_KEYWORD_ALLOC, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("Allocating regular expression code for key okey in keyword kre failed",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_string_free(objkey);
}

/* Test validation of regexp code if pattern compilation fails and there is no objkey. */
static void test_jso_schema_keyword_get_regexp_code_when_comp_fails_without_objkey(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_string pattern;
	jso_re_code code;

	code.error_offset = 10;

	jso_schema_init(&schema);

	expect_function_call(__wrap_jso_re_code_alloc);
	will_return(__wrap_jso_re_code_alloc, &code);

	expect_function_call(__wrap_jso_re_compile);
	expect_value(__wrap_jso_re_compile, pattern, &pattern);
	expect_value(__wrap_jso_re_compile, code, &code);
	will_return(__wrap_jso_re_compile, JSO_FAILURE);

	expect_function_call(__wrap_jso_re_get_error_message);
	expect_value(__wrap_jso_re_get_error_message, code, &code);
	expect_value(__wrap_jso_re_get_error_message, buf_size, 256);
	will_return(__wrap_jso_re_get_error_message, "rerr");

	expect_function_call(__wrap_jso_re_code_free);
	expect_value(__wrap_jso_re_code_free, code, &code);

	jso_re_code *ret_re_code = jso_schema_keyword_get_regexp_code(&schema, "kre", NULL, &pattern);

	assert_null(ret_re_code);
	assert_int_equal(JSO_SCHEMA_ERROR_KEYWORD_PREP, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("Compiling regular expression for keyword kre failed at position 10 with "
						"error: rerr",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));
}

/* Test validation of regexp code if pattern compilation fails and there is objkey. */
static void test_jso_schema_keyword_get_regexp_code_when_comp_fails_with_objkey(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_string pattern;
	jso_re_code code;
	jso_string *objkey = jso_string_create_from_cstr("okey");

	code.error_offset = 10;

	jso_schema_init(&schema);

	expect_function_call(__wrap_jso_re_code_alloc);
	will_return(__wrap_jso_re_code_alloc, &code);

	expect_function_call(__wrap_jso_re_compile);
	expect_value(__wrap_jso_re_compile, pattern, &pattern);
	expect_value(__wrap_jso_re_compile, code, &code);
	will_return(__wrap_jso_re_compile, JSO_FAILURE);

	expect_function_call(__wrap_jso_re_get_error_message);
	expect_value(__wrap_jso_re_get_error_message, code, &code);
	expect_value(__wrap_jso_re_get_error_message, buf_size, 256);
	will_return(__wrap_jso_re_get_error_message, "rerr");

	expect_function_call(__wrap_jso_re_code_free);
	expect_value(__wrap_jso_re_code_free, code, &code);

	jso_re_code *ret_re_code = jso_schema_keyword_get_regexp_code(&schema, "kre", objkey, &pattern);

	assert_null(ret_re_code);
	assert_int_equal(JSO_SCHEMA_ERROR_KEYWORD_PREP, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("Compiling regular expression for key okey in keyword kre failed at "
						"position 10 with error: rerr",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_string_free(objkey);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_schema_keyword_get_regexp_code_when_success),
		cmocka_unit_test(test_jso_schema_keyword_get_regexp_code_when_alloc_fails_without_objkey),
		cmocka_unit_test(test_jso_schema_keyword_get_regexp_code_when_alloc_fails_with_objkey),
		cmocka_unit_test(test_jso_schema_keyword_get_regexp_code_when_comp_fails_without_objkey),
		cmocka_unit_test(test_jso_schema_keyword_get_regexp_code_when_comp_fails_with_objkey),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
