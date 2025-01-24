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

#include "../../src/jso_pointer.h"
#include "../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* A test for creating a simple pointer. */
static void test_jso_pointer_create_simple(void **state)
{
	(void) state; /* unused */

	jso_string *str = jso_string_create_from_cstr("/path/to/string");
	jso_pointer *pointer = jso_pointer_create(str);

	assert_int_equal(JSO_POINTER_ERROR_TYPE(pointer), JSO_POINTER_ERROR_NONE);
	assert_string_equal(JSO_STRING_VAL(pointer->pointer_value), "/path/to/string");
	assert_int_equal(pointer->tokens_count, 3);
	assert_string_equal(JSO_STRING_VAL(pointer->tokens[0]), "path");
	assert_string_equal(JSO_STRING_VAL(pointer->tokens[1]), "to");
	assert_string_equal(JSO_STRING_VAL(pointer->tokens[2]), "string");
	assert_int_equal(pointer->refcount, 0);

	jso_string_free(str);
	jso_pointer_free(pointer);
}

/* A test for creating a pointer with single escape. */
static void test_jso_pointer_create_with_valid_simple_escapes(void **state)
{
	(void) state; /* unused */

	jso_string *str = jso_string_create_from_cstr("/e1~0+/e2~1+/e3+");
	jso_pointer *pointer = jso_pointer_create(str);

	assert_non_null(pointer);
	assert_int_equal(JSO_POINTER_ERROR_TYPE(pointer), JSO_POINTER_ERROR_NONE);
	assert_string_equal(JSO_STRING_VAL(pointer->pointer_value), "/e1~0+/e2~1+/e3+");
	assert_int_equal(pointer->tokens_count, 3);
	assert_string_equal(JSO_STRING_VAL(pointer->tokens[0]), "e1~+");
	assert_string_equal(JSO_STRING_VAL(pointer->tokens[1]), "e2/+");
	assert_string_equal(JSO_STRING_VAL(pointer->tokens[2]), "e3+");
	assert_int_equal(pointer->refcount, 0);

	jso_string_free(str);
	jso_pointer_free(pointer);
}

/* A test for creating a pointer with mixed escapes. */
static void test_jso_pointer_create_with_valid_mixed_escapes(void **state)
{
	(void) state; /* unused */

	jso_string *str = jso_string_create_from_cstr("/esc~0~1-~1-~0/end");
	jso_pointer *pointer = jso_pointer_create(str);

	assert_non_null(pointer);
	assert_int_equal(JSO_POINTER_ERROR_TYPE(pointer), JSO_POINTER_ERROR_NONE);
	assert_string_equal(JSO_STRING_VAL(pointer->pointer_value), "/esc~0~1-~1-~0/end");
	assert_int_equal(pointer->tokens_count, 2);
	assert_string_equal(JSO_STRING_VAL(pointer->tokens[0]), "esc~/-/-~");
	assert_string_equal(JSO_STRING_VAL(pointer->tokens[1]), "end");
	assert_int_equal(pointer->refcount, 0);

	jso_string_free(str);
	jso_pointer_free(pointer);
}

/* A test for creating a pointer with invalid escape number. */
static void test_jso_pointer_create_with_invalid_escape_number(void **state)
{
	(void) state; /* unused */

	jso_string *str = jso_string_create_from_cstr("/invalid~4/end");
	jso_pointer *pointer = jso_pointer_create(str);

	assert_non_null(pointer);
	assert_int_equal(JSO_POINTER_ERROR_TYPE(pointer), JSO_POINTER_ERROR_INVALID_FORMAT);

	jso_string_free(str);
	jso_pointer_free(pointer);
}

/* A test for creating a pointer with invalid escape number. */
static void test_jso_pointer_create_with_invalid_escape_end(void **state)
{
	(void) state; /* unused */

	jso_string *str = jso_string_create_from_cstr("/invalid~/end");
	jso_pointer *pointer = jso_pointer_create(str);

	assert_non_null(pointer);
	assert_int_equal(JSO_POINTER_ERROR_TYPE(pointer), JSO_POINTER_ERROR_INVALID_FORMAT);

	jso_string_free(str);
	jso_pointer_free(pointer);
}

/* A test evaluating object. */
static void test_jso_pointer_evaluate_object(void **state)
{
	(void) state; /* unused */

	jso_string *str = jso_string_create_from_cstr("/invalid~/end");
	jso_pointer *pointer = jso_pointer_create(str);

	assert_non_null(pointer);
	assert_int_equal(JSO_POINTER_ERROR_TYPE(pointer), JSO_POINTER_ERROR_INVALID_FORMAT);

	jso_string_free(str);
	jso_pointer_free(pointer);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_pointer_create_simple),
		cmocka_unit_test(test_jso_pointer_create_with_valid_simple_escapes),
		cmocka_unit_test(test_jso_pointer_create_with_valid_mixed_escapes),
		cmocka_unit_test(test_jso_pointer_create_with_invalid_escape_number),
		cmocka_unit_test(test_jso_pointer_create_with_invalid_escape_end),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
