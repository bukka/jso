/*
 * Copyright (c) 2025 Jakub Zelenka. All rights reserved.
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

#include "../../src/jso_parser.h"
#include "../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* A test for parsing a simple C string object. */
static void test_jso_parser_parse_cstr_simple_object(void **state)
{
	(void) state; /* unused */

	jso_value result, *value;
	jso_parser_options options = { .max_depth = 1000 };
	const char *json = "{ \"foo\": \"bar\" }";

	assert_int_equal(JSO_SUCCESS, jso_parse_cstr(json, strlen(json), &options, &result));
	assert_int_equal(JSO_TYPE_OBJECT, JSO_TYPE(result));

	jso_object *obj = JSO_OBJVAL(result);
	assert_int_equal(1, JSO_OBJECT_COUNT(obj));
	assert_int_equal(JSO_SUCCESS, jso_object_get_by_cstr_key(obj, "foo", strlen("foo"), &value));
	assert_int_equal(JSO_TYPE_STRING, JSO_TYPE_P(value));
	assert_string_equal("bar", (const char *) JSO_SVAL_P(value));

	jso_value_clear(&result);
}

/* A test for parsing a simple C string object. */
static void test_jso_parser_parse_cstr_nested_object(void **state)
{
	(void) state; /* unused */

	jso_value result, *value;
	jso_parser_options options = { .max_depth = 1000 };
	const char *json = "{ \"av\": [1, 2, 3.1], \"bv\": true, \"ov\": { \"nv\": null } }";

	assert_int_equal(JSO_SUCCESS, jso_parse_cstr(json, strlen(json), &options, &result));
	assert_int_equal(JSO_TYPE_OBJECT, JSO_TYPE(result));

	jso_object *obj = JSO_OBJVAL(result);
	assert_int_equal(3, JSO_OBJECT_COUNT(obj));

	assert_int_equal(JSO_SUCCESS, jso_object_get_by_cstr_key(obj, "av", 2, &value));
	assert_int_equal(JSO_TYPE_ARRAY, JSO_TYPE_P(value));
	jso_array *arr = JSO_ARRVAL_P(value);
	assert_int_equal(3, JSO_ARRAY_LEN(arr));

	assert_int_equal(JSO_SUCCESS, jso_array_index(arr, 0, &value));
	assert_int_equal(1, JSO_IVAL_P(value));

	assert_int_equal(JSO_SUCCESS, jso_array_index(arr, 1, &value));
	assert_int_equal(2, JSO_IVAL_P(value));

	assert_int_equal(JSO_SUCCESS, jso_array_index(arr, 2, &value));
	assert_double_equal(3.1, JSO_DVAL_P(value), 0.000001);

	assert_int_equal(JSO_SUCCESS, jso_object_get_by_cstr_key(obj, "bv", 2, &value));
	assert_int_equal(JSO_TYPE_BOOL, JSO_TYPE_P(value));
	assert_true(JSO_IVAL_P(value));

	assert_int_equal(JSO_SUCCESS, jso_object_get_by_cstr_key(obj, "ov", 2, &value));
	assert_int_equal(JSO_TYPE_OBJECT, JSO_TYPE_P(value));
	obj = JSO_OBJVAL_P(value);
	assert_int_equal(1, JSO_OBJECT_COUNT(obj));

	assert_int_equal(JSO_SUCCESS, jso_object_get_by_cstr_key(obj, "nv", 2, &value));
	assert_int_equal(JSO_TYPE_NULL, JSO_TYPE_P(value));

	jso_value_clear(&result);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_parser_parse_cstr_simple_object),
		cmocka_unit_test(test_jso_parser_parse_cstr_nested_object),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}