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

#include "../../../src/schema/jso_schema_error.h"
#include "../../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* A test for setting error. */
static void test_jso_schema_error_set(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_schema_init(&schema);

	assert_int_equal(JSO_SUCCESS,
			jso_schema_error_set(&schema, JSO_SCHEMA_ERROR_TYPE_INVALID, "test message"));

	assert_int_equal(JSO_SCHEMA_ERROR_TYPE_INVALID, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("test message", JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
}

/* A test for formatting error with short message. */
static void test_jso_schema_error_format_short(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_schema_init(&schema);

	jso_rc rc = jso_schema_error_format(
			&schema, JSO_SCHEMA_ERROR_TYPE_INVALID, "test message %s: %d", "containing", 10);
	assert_int_equal(JSO_SUCCESS, rc);

	assert_int_equal(JSO_SCHEMA_ERROR_TYPE_INVALID, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("test message containing: 10", JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
}

/* A test for formatting error with invalid message. */
static void test_jso_schema_error_format_long(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_schema_init(&schema);

	size_t long_msg_size = JSO_SCHEMA_ERROR_FORMAT_SIZE + 10;
	char *long_msg = jso_malloc(long_msg_size + 1);
	memset(long_msg, 'a', long_msg_size);
	long_msg[long_msg_size] = '\0';

	size_t expected_msg_size = JSO_SCHEMA_ERROR_FORMAT_SIZE - 1;
	char *expected_msg = jso_malloc(expected_msg_size + 1);
	memset(expected_msg, 'a', expected_msg_size);
	expected_msg[expected_msg_size] = '\0';

	jso_rc rc = jso_schema_error_format(&schema, JSO_SCHEMA_ERROR_TYPE_INVALID, "%s", long_msg);
	assert_int_equal(JSO_SUCCESS, rc);

	assert_int_equal(JSO_SCHEMA_ERROR_TYPE_INVALID, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal(expected_msg, JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
	jso_free(long_msg);
	jso_free(expected_msg);
}

/* A test for freeing error. */
static void test_jso_schema_error_free(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_schema_init(&schema);

	assert_int_equal(JSO_SUCCESS,
			jso_schema_error_set(&schema, JSO_SCHEMA_ERROR_TYPE_INVALID, "test message"));

	assert_int_equal(JSO_SCHEMA_ERROR_TYPE_INVALID, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("test message", JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_error_free(&schema);
	assert_int_equal(JSO_SCHEMA_ERROR_NONE, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_null(JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_schema_error_set),
		cmocka_unit_test(test_jso_schema_error_format_short),
		cmocka_unit_test(test_jso_schema_error_format_long),
		cmocka_unit_test(test_jso_schema_error_free),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
