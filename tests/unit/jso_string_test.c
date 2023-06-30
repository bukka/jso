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

#include "../../src/jso_string.h"
#include "../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* A test case that tests whether string is correctly created from C string. */
static void jso_test_string_create_from_cstr(void **state)
{
	(void) state; /* unused */

	jso_string *str = jso_string_create_from_cstr("test");

	assert_non_null(str);
	assert_int_equal(4, JSO_STRING_LEN(str));
	assert_int_equal(0, memcmp(JSO_STRING_VAL(str), "test", 4));

	jso_string_free(str);
}

/* A test case that tests whether two strings are equal. */
static void jso_test_string_equals(void **state)
{
	(void) state; /* unused */
	jso_string *str1 = jso_string_create_from_cstr("test");
	jso_string *str2 = jso_string_create_from_cstr("test");
	jso_string *str3 = jso_string_create_from_cstr("test extra");

	assert_true(jso_string_equals(str1, str2));
	assert_false(jso_string_equals(str1, str3));
	assert_false(jso_string_equals(str2, str3));

	jso_string_free(str1);
	jso_string_free(str2);
	jso_string_free(str3);
}

/* A test case that tests whether string are equal to C string. */
static void jso_test_string_equals_to_cstr(void **state)
{
	(void) state; /* unused */
	jso_string *str = jso_string_create_from_cstr("test");

	assert_true(jso_string_equals_to_cstr(str, "test"));
	assert_false(jso_string_equals_to_cstr(str, "no test"));
	assert_false(jso_string_equals_to_cstr(str, "test1"));

	jso_string_free(str);
}

/* A test to check whether string copying works by increasing reference. */
static void jso_test_string_copy(void **state)
{
	(void) state; /* unused */

	jso_string *str = jso_string_alloc(10);

	assert_int_equal(0, JSO_STRING_REFCOUNT(str));
	assert_ptr_equal(str, jso_string_copy(str));
	assert_int_equal(1, JSO_STRING_REFCOUNT(str));
	assert_ptr_equal(str, jso_string_copy(str));
	assert_int_equal(2, JSO_STRING_REFCOUNT(str));
	jso_string_free(str);
	assert_int_equal(1, JSO_STRING_REFCOUNT(str));
	jso_string_free(str);
	assert_int_equal(0, JSO_STRING_REFCOUNT(str));
	jso_string_free(str);
}

/* A test to check whether getting hash works. */
static void jso_test_string_get_hash(void **state)
{
	(void) state; /* unused */

	jso_string *str = jso_string_alloc(10);

	assert_int_equal(0, jso_string_get_hash(str));
	jso_string_set_hash(str, 1234);
	assert_int_equal(1234, jso_string_get_hash(str));

	jso_string_free(str);
}

/* A test to check whether hash is set. */
static void jso_test_string_has_hash(void **state)
{
	(void) state; /* unused */

	jso_string *str = jso_string_alloc(10);

	assert_false(jso_string_has_hash(str));
	jso_string_set_hash(str, 1234);
	assert_true(jso_string_has_hash(str));

	jso_string_free(str);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(jso_test_string_create_from_cstr),
		cmocka_unit_test(jso_test_string_equals),
		cmocka_unit_test(jso_test_string_equals_to_cstr),
		cmocka_unit_test(jso_test_string_copy),
		cmocka_unit_test(jso_test_string_get_hash),
		cmocka_unit_test(jso_test_string_has_hash),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
