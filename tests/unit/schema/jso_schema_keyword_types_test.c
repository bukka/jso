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

#include "../../../src/schema/jso_schema_keyword_types.h"
#include "../../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* Tests for jso_schema_keyword_get_types. */

#define JSO_VALUE_TYPES_SIZE 8

/* Test getting types for keyword any type. */
static void test_jso_schema_keyword_get_types_for_any(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value_type types[JSO_VALUE_TYPES_SIZE];

	jso_schema_init(&schema);

	size_t num_types = jso_schema_keyword_get_types(
			&schema, "akey", JSO_SCHEMA_KEYWORD_TYPE_ANY, types, JSO_VALUE_TYPES_SIZE);

	assert_int_equal(7, num_types);
	assert_int_equal(JSO_TYPE_NULL, types[0]);
	assert_int_equal(JSO_TYPE_BOOL, types[1]);
	assert_int_equal(JSO_TYPE_INT, types[2]);
	assert_int_equal(JSO_TYPE_DOUBLE, types[3]);
	assert_int_equal(JSO_TYPE_STRING, types[4]);
	assert_int_equal(JSO_TYPE_ARRAY, types[5]);
	assert_int_equal(JSO_TYPE_OBJECT, types[6]);
}

/* Test getting types for keyword null type. */
static void test_jso_schema_keyword_get_types_for_null(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value_type types[JSO_VALUE_TYPES_SIZE];

	jso_schema_init(&schema);

	size_t num_types = jso_schema_keyword_get_types(
			&schema, "nkey", JSO_SCHEMA_KEYWORD_TYPE_NULL, types, JSO_VALUE_TYPES_SIZE);

	assert_int_equal(1, num_types);
	assert_int_equal(JSO_TYPE_NULL, types[0]);
}

/* Test getting types for keyword bool type. */
static void test_jso_schema_keyword_get_types_for_bool(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value_type types[JSO_VALUE_TYPES_SIZE];

	jso_schema_init(&schema);

	size_t num_types = jso_schema_keyword_get_types(
			&schema, "bkey", JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN, types, JSO_VALUE_TYPES_SIZE);

	assert_int_equal(1, num_types);
	assert_int_equal(JSO_TYPE_BOOL, types[0]);
}

/* Test getting types for keyword int type. */
static void test_jso_schema_keyword_get_types_for_int(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value_type types[JSO_VALUE_TYPES_SIZE];

	jso_schema_init(&schema);

	size_t num_types = jso_schema_keyword_get_types(
			&schema, "ikey", JSO_SCHEMA_KEYWORD_TYPE_INTEGER, types, JSO_VALUE_TYPES_SIZE);

	assert_int_equal(2, num_types);
	assert_int_equal(JSO_TYPE_INT, types[0]);
	assert_int_equal(JSO_TYPE_DOUBLE, types[1]);
}

/* Test getting types for keyword unsigned int type. */
static void test_jso_schema_keyword_get_types_for_uint(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value_type types[JSO_VALUE_TYPES_SIZE];

	jso_schema_init(&schema);

	size_t num_types = jso_schema_keyword_get_types(&schema, "uikey",
			JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER, types, JSO_VALUE_TYPES_SIZE);

	assert_int_equal(2, num_types);
	assert_int_equal(JSO_TYPE_INT, types[0]);
	assert_int_equal(JSO_TYPE_DOUBLE, types[1]);
}

/* Test getting types for keyword double type. */
static void test_jso_schema_keyword_get_types_for_double(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value_type types[JSO_VALUE_TYPES_SIZE];

	jso_schema_init(&schema);

	size_t num_types = jso_schema_keyword_get_types(
			&schema, "uikey", JSO_SCHEMA_KEYWORD_TYPE_DOUBLE, types, JSO_VALUE_TYPES_SIZE);

	assert_int_equal(2, num_types);
	assert_int_equal(JSO_TYPE_INT, types[0]);
	assert_int_equal(JSO_TYPE_DOUBLE, types[1]);
}

/* Test getting types for keyword number type. */
static void test_jso_schema_keyword_get_types_for_number(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value_type types[JSO_VALUE_TYPES_SIZE];

	jso_schema_init(&schema);

	size_t num_types = jso_schema_keyword_get_types(
			&schema, "uikey", JSO_SCHEMA_KEYWORD_TYPE_NUMBER, types, JSO_VALUE_TYPES_SIZE);

	assert_int_equal(2, num_types);
	assert_int_equal(JSO_TYPE_INT, types[0]);
	assert_int_equal(JSO_TYPE_DOUBLE, types[1]);
}

/* Test getting types for keyword string type. */
static void test_jso_schema_keyword_get_types_for_string(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value_type types[JSO_VALUE_TYPES_SIZE];

	jso_schema_init(&schema);

	size_t num_types = jso_schema_keyword_get_types(
			&schema, "skey", JSO_SCHEMA_KEYWORD_TYPE_STRING, types, JSO_VALUE_TYPES_SIZE);

	assert_int_equal(1, num_types);
	assert_int_equal(JSO_TYPE_STRING, types[0]);
}

/* Test getting types for keyword regexp type. */
static void test_jso_schema_keyword_get_types_for_regexp(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value_type types[JSO_VALUE_TYPES_SIZE];

	jso_schema_init(&schema);

	size_t num_types = jso_schema_keyword_get_types(
			&schema, "rkey", JSO_SCHEMA_KEYWORD_TYPE_REGEXP, types, JSO_VALUE_TYPES_SIZE);

	assert_int_equal(1, num_types);
	assert_int_equal(JSO_TYPE_STRING, types[0]);
}

/* Test getting types for keyword array type. */
static void test_jso_schema_keyword_get_types_for_array(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value_type types[JSO_VALUE_TYPES_SIZE];

	jso_schema_init(&schema);

	size_t num_types = jso_schema_keyword_get_types(
			&schema, "akey", JSO_SCHEMA_KEYWORD_TYPE_ARRAY, types, JSO_VALUE_TYPES_SIZE);

	assert_int_equal(1, num_types);
	assert_int_equal(JSO_TYPE_ARRAY, types[0]);
}

/* Test getting types for keyword array of strings type. */
static void test_jso_schema_keyword_get_types_for_array_of_strings(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value_type types[JSO_VALUE_TYPES_SIZE];

	jso_schema_init(&schema);

	size_t num_types = jso_schema_keyword_get_types(&schema, "askey",
			JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_STRINGS, types, JSO_VALUE_TYPES_SIZE);

	assert_int_equal(1, num_types);
	assert_int_equal(JSO_TYPE_ARRAY, types[0]);
}

/* Test getting types for keyword array of schema objects type. */
static void test_jso_schema_keyword_get_types_for_array_of_schema_objects(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value_type types[JSO_VALUE_TYPES_SIZE];

	jso_schema_init(&schema);

	size_t num_types = jso_schema_keyword_get_types(&schema, "asokey",
			JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS, types, JSO_VALUE_TYPES_SIZE);

	assert_int_equal(1, num_types);
	assert_int_equal(JSO_TYPE_ARRAY, types[0]);
}

/* Test getting types for keyword object type. */
static void test_jso_schema_keyword_get_types_for_object(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value_type types[JSO_VALUE_TYPES_SIZE];

	jso_schema_init(&schema);

	size_t num_types = jso_schema_keyword_get_types(
			&schema, "okey", JSO_SCHEMA_KEYWORD_TYPE_OBJECT, types, JSO_VALUE_TYPES_SIZE);

	assert_int_equal(1, num_types);
	assert_int_equal(JSO_TYPE_OBJECT, types[0]);
}

/* Test getting types for keyword schema object type. */
static void test_jso_schema_keyword_get_types_for_schema_object(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value_type types[JSO_VALUE_TYPES_SIZE];

	jso_schema_init(&schema);

	size_t num_types = jso_schema_keyword_get_types(
			&schema, "sokey", JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT, types, JSO_VALUE_TYPES_SIZE);

	assert_int_equal(1, num_types);
	assert_int_equal(JSO_TYPE_OBJECT, types[0]);
}

/* Test getting types for keyword object of schema objects type. */
static void test_jso_schema_keyword_get_types_for_object_of_schema_objects(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value_type types[JSO_VALUE_TYPES_SIZE];

	jso_schema_init(&schema);

	size_t num_types = jso_schema_keyword_get_types(&schema, "osokey",
			JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS, types, JSO_VALUE_TYPES_SIZE);

	assert_int_equal(1, num_types);
	assert_int_equal(JSO_TYPE_OBJECT, types[0]);
}

/* Test getting types for keyword object of schema objects type. */
static void test_jso_schema_keyword_get_types_for_object_of_schema_objects_or_arrstr(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value_type types[JSO_VALUE_TYPES_SIZE];

	jso_schema_init(&schema);

	size_t num_types = jso_schema_keyword_get_types(&schema, "osokey",
			JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS_OR_ARRAY_OF_STRINGS, types,
			JSO_VALUE_TYPES_SIZE);

	assert_int_equal(2, num_types);
	assert_int_equal(JSO_TYPE_OBJECT, types[0]);
	assert_int_equal(JSO_TYPE_ARRAY, types[1]);
}

/* Test getting types for keyword regexp object of schema objects type. */
static void test_jso_schema_keyword_get_types_for_regexp_object_of_schema_objects(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value_type types[JSO_VALUE_TYPES_SIZE];

	jso_schema_init(&schema);

	size_t num_types = jso_schema_keyword_get_types(&schema, "rosokey",
			JSO_SCHEMA_KEYWORD_TYPE_REGEXP_OBJECT_OF_SCHEMA_OBJECTS, types, JSO_VALUE_TYPES_SIZE);

	assert_int_equal(1, num_types);
	assert_int_equal(JSO_TYPE_OBJECT, types[0]);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_schema_keyword_get_types_for_any),
		cmocka_unit_test(test_jso_schema_keyword_get_types_for_null),
		cmocka_unit_test(test_jso_schema_keyword_get_types_for_bool),
		cmocka_unit_test(test_jso_schema_keyword_get_types_for_int),
		cmocka_unit_test(test_jso_schema_keyword_get_types_for_uint),
		cmocka_unit_test(test_jso_schema_keyword_get_types_for_double),
		cmocka_unit_test(test_jso_schema_keyword_get_types_for_number),
		cmocka_unit_test(test_jso_schema_keyword_get_types_for_string),
		cmocka_unit_test(test_jso_schema_keyword_get_types_for_regexp),
		cmocka_unit_test(test_jso_schema_keyword_get_types_for_array),
		cmocka_unit_test(test_jso_schema_keyword_get_types_for_array_of_strings),
		cmocka_unit_test(test_jso_schema_keyword_get_types_for_array_of_schema_objects),
		cmocka_unit_test(test_jso_schema_keyword_get_types_for_object),
		cmocka_unit_test(test_jso_schema_keyword_get_types_for_schema_object),
		cmocka_unit_test(test_jso_schema_keyword_get_types_for_object_of_schema_objects),
		cmocka_unit_test(test_jso_schema_keyword_get_types_for_object_of_schema_objects_or_arrstr),
		cmocka_unit_test(test_jso_schema_keyword_get_types_for_regexp_object_of_schema_objects),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
