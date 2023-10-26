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

#include "../../../src/schema/jso_schema_keyword_freer.h"
#include "../../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* Wrapping. */

/* Wrapper for jso_schema_keyword_free_any. */
void __wrap_jso_schema_keyword_free_any(jso_schema_keyword *schema_keyword)
{
	function_called();
	check_expected_ptr(schema_keyword);
}

/* Wrapper for jso_schema_keyword_free_string. */
void __wrap_jso_schema_keyword_free_string(jso_schema_keyword *schema_keyword)
{
	function_called();
	check_expected_ptr(schema_keyword);
}

/* Wrapper for jso_schema_keyword_free_regexp. */
void __wrap_jso_schema_keyword_free_regexp(jso_schema_keyword *schema_keyword)
{
	check_expected_ptr(schema_keyword);
	function_called();
}

/* Wrapper for jso_schema_keyword_free_array. */
void __wrap_jso_schema_keyword_free_array(jso_schema_keyword *schema_keyword)
{
	function_called();
	check_expected_ptr(schema_keyword);
}

/* Wrapper for jso_schema_keyword_free_array_of_strings. */
void __wrap_jso_schema_keyword_free_array_of_strings(jso_schema_keyword *schema_keyword)
{
	function_called();
	check_expected_ptr(schema_keyword);
}

/* Wrapper for jso_schema_keyword_free_array_of_schema_objects. */
void __wrap_jso_schema_keyword_free_array_of_schema_objects(jso_schema_keyword *schema_keyword)
{
	function_called();
	check_expected_ptr(schema_keyword);
}

/* Wrapper for jso_schema_keyword_free_object. */
void __wrap_jso_schema_keyword_free_object(jso_schema_keyword *schema_keyword)
{
	function_called();
	check_expected_ptr(schema_keyword);
}

/* Wrapper for jso_schema_keyword_free_schema_object. */
void __wrap_jso_schema_keyword_free_schema_object(jso_schema_keyword *schema_keyword)
{
	function_called();
	check_expected_ptr(schema_keyword);
}

/* Wrapper for jso_schema_keyword_free_object_of_schema_objects. */
void __wrap_jso_schema_keyword_free_object_of_schema_objects(jso_schema_keyword *schema_keyword)
{
	function_called();
	check_expected_ptr(schema_keyword);
}

/* Tests for jso_schema_keyword_free. */

/* Test freeing keyword if not present. */
static void test_jso_schema_keyword_free_if_not_present(void **state)
{
	(void) state; /* unused */

	jso_schema_keyword keyword;

	JSO_SCHEMA_KEYWORD_FLAGS(keyword) = 0;
	JSO_SCHEMA_KEYWORD_TYPE(keyword) = JSO_SCHEMA_KEYWORD_TYPE_ANY;

	jso_schema_keyword_free(&keyword);
}

/* Test freeing keyword with any type. */
static void test_jso_schema_keyword_free_for_any(void **state)
{
	(void) state; /* unused */

	jso_schema_keyword keyword;

	JSO_SCHEMA_KEYWORD_FLAGS(keyword) = JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_TYPE(keyword) = JSO_SCHEMA_KEYWORD_TYPE_ANY;

	expect_function_call(__wrap_jso_schema_keyword_free_any);
	expect_value(__wrap_jso_schema_keyword_free_any, schema_keyword, &keyword);

	jso_schema_keyword_free(&keyword);
}

/* Test freeing keyword with string type. */
static void test_jso_schema_keyword_free_for_string(void **state)
{
	(void) state; /* unused */

	jso_schema_keyword keyword;

	JSO_SCHEMA_KEYWORD_FLAGS(keyword) = JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_TYPE(keyword) = JSO_SCHEMA_KEYWORD_TYPE_STRING;

	expect_function_call(__wrap_jso_schema_keyword_free_string);
	expect_value(__wrap_jso_schema_keyword_free_string, schema_keyword, &keyword);

	jso_schema_keyword_free(&keyword);
}

/* Test freeing keyword with regexp type. */
static void test_jso_schema_keyword_free_for_regexp(void **state)
{
	(void) state; /* unused */

	jso_schema_keyword keyword;

	JSO_SCHEMA_KEYWORD_FLAGS(keyword) = JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_TYPE(keyword) = JSO_SCHEMA_KEYWORD_TYPE_REGEXP;

	expect_function_call(__wrap_jso_schema_keyword_free_regexp);
	expect_value(__wrap_jso_schema_keyword_free_regexp, schema_keyword, &keyword);

	jso_schema_keyword_free(&keyword);
}

/* Test freeing keyword with array type. */
static void test_jso_schema_keyword_free_for_array(void **state)
{
	(void) state; /* unused */

	jso_schema_keyword keyword;

	JSO_SCHEMA_KEYWORD_FLAGS(keyword) = JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_TYPE(keyword) = JSO_SCHEMA_KEYWORD_TYPE_ARRAY;

	expect_function_call(__wrap_jso_schema_keyword_free_array);
	expect_value(__wrap_jso_schema_keyword_free_array, schema_keyword, &keyword);

	jso_schema_keyword_free(&keyword);
}

/* Test freeing keyword with array of strings type. */
static void test_jso_schema_keyword_free_for_array_of_strings(void **state)
{
	(void) state; /* unused */

	jso_schema_keyword keyword;

	JSO_SCHEMA_KEYWORD_FLAGS(keyword) = JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_TYPE(keyword) = JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_STRINGS;

	expect_function_call(__wrap_jso_schema_keyword_free_array_of_strings);
	expect_value(__wrap_jso_schema_keyword_free_array_of_strings, schema_keyword, &keyword);

	jso_schema_keyword_free(&keyword);
}

/* Test freeing keyword with array of schema objects type. */
static void test_jso_schema_keyword_free_for_array_of_schema_objects(void **state)
{
	(void) state; /* unused */

	jso_schema_keyword keyword;

	JSO_SCHEMA_KEYWORD_FLAGS(keyword) = JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_TYPE(keyword) = JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS;

	expect_function_call(__wrap_jso_schema_keyword_free_array_of_schema_objects);
	expect_value(__wrap_jso_schema_keyword_free_array_of_schema_objects, schema_keyword, &keyword);

	jso_schema_keyword_free(&keyword);
}

/* Test freeing keyword with object type. */
static void test_jso_schema_keyword_free_for_object(void **state)
{
	(void) state; /* unused */

	jso_schema_keyword keyword;

	JSO_SCHEMA_KEYWORD_FLAGS(keyword) = JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_TYPE(keyword) = JSO_SCHEMA_KEYWORD_TYPE_OBJECT;

	expect_function_call(__wrap_jso_schema_keyword_free_object);
	expect_value(__wrap_jso_schema_keyword_free_object, schema_keyword, &keyword);

	jso_schema_keyword_free(&keyword);
}

/* Test freeing keyword with schema object type. */
static void test_jso_schema_keyword_free_for_schema_object(void **state)
{
	(void) state; /* unused */

	jso_schema_keyword keyword;

	JSO_SCHEMA_KEYWORD_FLAGS(keyword) = JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_TYPE(keyword) = JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT;

	expect_function_call(__wrap_jso_schema_keyword_free_schema_object);
	expect_value(__wrap_jso_schema_keyword_free_schema_object, schema_keyword, &keyword);

	jso_schema_keyword_free(&keyword);
}

/* Test freeing keyword with object of schema objects type. */
static void test_jso_schema_keyword_free_for_object_of_schema_objects(void **state)
{
	(void) state; /* unused */

	jso_schema_keyword keyword;

	JSO_SCHEMA_KEYWORD_FLAGS(keyword) = JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_TYPE(keyword) = JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS;

	expect_function_call(__wrap_jso_schema_keyword_free_object_of_schema_objects);
	expect_value(__wrap_jso_schema_keyword_free_object_of_schema_objects, schema_keyword, &keyword);

	jso_schema_keyword_free(&keyword);
}

/* Test freeing keyword with object of schema objects type. */
static void test_jso_schema_keyword_free_for_object_of_schema_objects_or_arrstr(void **state)
{
	(void) state; /* unused */

	jso_schema_keyword keyword;

	JSO_SCHEMA_KEYWORD_FLAGS(keyword) = JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_TYPE(keyword)
			= JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS_OR_ARRAY_OF_STRINGS;

	// TODO: verify this as it might not free the array of strings or fail on it
	expect_function_call(__wrap_jso_schema_keyword_free_object_of_schema_objects);
	expect_value(__wrap_jso_schema_keyword_free_object_of_schema_objects, schema_keyword, &keyword);

	jso_schema_keyword_free(&keyword);
}

/* Test freeing keyword with regexp object of schema objects type. */
static void test_jso_schema_keyword_free_for_regexp_object_of_schema_objects(void **state)
{
	(void) state; /* unused */

	jso_schema_keyword keyword;

	JSO_SCHEMA_KEYWORD_FLAGS(keyword) = JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_TYPE(keyword) = JSO_SCHEMA_KEYWORD_TYPE_REGEXP_OBJECT_OF_SCHEMA_OBJECTS;

	expect_function_call(__wrap_jso_schema_keyword_free_object_of_schema_objects);
	expect_value(__wrap_jso_schema_keyword_free_object_of_schema_objects, schema_keyword, &keyword);

	jso_schema_keyword_free(&keyword);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_schema_keyword_free_if_not_present),
		cmocka_unit_test(test_jso_schema_keyword_free_for_any),
		cmocka_unit_test(test_jso_schema_keyword_free_for_string),
		cmocka_unit_test(test_jso_schema_keyword_free_for_regexp),
		cmocka_unit_test(test_jso_schema_keyword_free_for_array),
		cmocka_unit_test(test_jso_schema_keyword_free_for_array_of_strings),
		cmocka_unit_test(test_jso_schema_keyword_free_for_array_of_schema_objects),
		cmocka_unit_test(test_jso_schema_keyword_free_for_object),
		cmocka_unit_test(test_jso_schema_keyword_free_for_schema_object),
		cmocka_unit_test(test_jso_schema_keyword_free_for_object_of_schema_objects),
		cmocka_unit_test(test_jso_schema_keyword_free_for_object_of_schema_objects_or_arrstr),
		cmocka_unit_test(test_jso_schema_keyword_free_for_regexp_object_of_schema_objects),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
