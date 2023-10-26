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

#include "../../../src/schema/jso_schema_keyword_single.h"
#include "../../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* Wrapping. */

/* Wrapper for jso_schema_keyword_get_any. */
void __wrap_jso_schema_keyword_get_any(jso_schema *schema, jso_value *data, const char *key,
		jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected(error_on_invalid_type);
	check_expected(keyword_flags);
	check_expected_ptr(schema_keyword);
	check_expected_ptr(val);
	check_expected_ptr(parent);
}

/* Wrapper for jso_schema_keyword_get_null. */
void __wrap_jso_schema_keyword_get_null(jso_schema *schema, jso_value *data, const char *key,
		jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected(error_on_invalid_type);
	check_expected(keyword_flags);
	check_expected_ptr(schema_keyword);
	check_expected_ptr(val);
	check_expected_ptr(parent);
}

/* Wrapper for jso_schema_keyword_get_bool. */
void __wrap_jso_schema_keyword_get_bool(jso_schema *schema, jso_value *data, const char *key,
		jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected(error_on_invalid_type);
	check_expected(keyword_flags);
	check_expected_ptr(schema_keyword);
	check_expected_ptr(val);
	check_expected_ptr(parent);
}

/* Wrapper for jso_schema_keyword_get_int. */
void __wrap_jso_schema_keyword_get_int(jso_schema *schema, jso_value *data, const char *key,
		jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected(error_on_invalid_type);
	check_expected(keyword_flags);
	check_expected_ptr(schema_keyword);
	check_expected_ptr(val);
	check_expected_ptr(parent);
}

/* Wrapper for jso_schema_keyword_get_uint. */
void __wrap_jso_schema_keyword_get_uint(jso_schema *schema, jso_value *data, const char *key,
		jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected(error_on_invalid_type);
	check_expected(keyword_flags);
	check_expected_ptr(schema_keyword);
	check_expected_ptr(val);
	check_expected_ptr(parent);
}

/* Wrapper for jso_schema_keyword_get_number. */
void __wrap_jso_schema_keyword_get_number(jso_schema *schema, jso_value *data, const char *key,
		jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected(error_on_invalid_type);
	check_expected(keyword_flags);
	check_expected_ptr(schema_keyword);
	check_expected_ptr(val);
	check_expected_ptr(parent);
}
/* Wrapper for jso_schema_keyword_get_string. */
void __wrap_jso_schema_keyword_get_string(jso_schema *schema, jso_value *data, const char *key,
		jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected(error_on_invalid_type);
	check_expected(keyword_flags);
	check_expected_ptr(schema_keyword);
	check_expected_ptr(val);
	check_expected_ptr(parent);
}

/* Wrapper for jso_schema_keyword_get_regexp. */
void __wrap_jso_schema_keyword_get_regexp(jso_schema *schema, jso_value *data, const char *key,
		jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected(error_on_invalid_type);
	check_expected(keyword_flags);
	check_expected_ptr(schema_keyword);
	check_expected_ptr(val);
	check_expected_ptr(parent);
}

/* Wrapper for jso_schema_keyword_get_array. */
void __wrap_jso_schema_keyword_get_array(jso_schema *schema, jso_value *data, const char *key,
		jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected(error_on_invalid_type);
	check_expected(keyword_flags);
	check_expected_ptr(schema_keyword);
	check_expected_ptr(val);
	check_expected_ptr(parent);
}

/* Wrapper for jso_schema_keyword_get_array_of_strings. */
void __wrap_jso_schema_keyword_get_array_of_strings(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected(error_on_invalid_type);
	check_expected(keyword_flags);
	check_expected_ptr(schema_keyword);
	check_expected_ptr(val);
	check_expected_ptr(parent);
}

/* Wrapper for jso_schema_keyword_get_array_of_schema_objects. */
void __wrap_jso_schema_keyword_get_array_of_schema_objects(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected(error_on_invalid_type);
	check_expected(keyword_flags);
	check_expected_ptr(schema_keyword);
	check_expected_ptr(val);
	check_expected_ptr(parent);
}

/* Wrapper for jso_schema_keyword_get_object. */
void __wrap_jso_schema_keyword_get_object(jso_schema *schema, jso_value *data, const char *key,
		jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected(error_on_invalid_type);
	check_expected(keyword_flags);
	check_expected_ptr(schema_keyword);
	check_expected_ptr(val);
	check_expected_ptr(parent);
}

/* Wrapper for jso_schema_keyword_get_schema_object. */
void __wrap_jso_schema_keyword_get_schema_object(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected(error_on_invalid_type);
	check_expected(keyword_flags);
	check_expected_ptr(schema_keyword);
	check_expected_ptr(val);
	check_expected_ptr(parent);
}

/* Wrapper for jso_schema_keyword_get_object_of_schema_objects. */
void __wrap_jso_schema_keyword_get_object_of_schema_objects(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected(error_on_invalid_type);
	check_expected(keyword_flags);
	check_expected_ptr(schema_keyword);
	check_expected_ptr(val);
	check_expected_ptr(parent);
}

/* Wrapper for jso_schema_keyword_get_object_of_schema_objects_or_array_of_strings. */
void __wrap_jso_schema_keyword_get_object_of_schema_objects_or_array_of_strings(jso_schema *schema,
		jso_value *data, const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected(error_on_invalid_type);
	check_expected(keyword_flags);
	check_expected_ptr(schema_keyword);
	check_expected_ptr(val);
	check_expected_ptr(parent);
}

/* Wrapper for jso_schema_keyword_get_regexp_object_of_schema_objects. */
void __wrap_jso_schema_keyword_get_regexp_object_of_schema_objects(jso_schema *schema,
		jso_value *data, const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected(error_on_invalid_type);
	check_expected(keyword_flags);
	check_expected_ptr(schema_keyword);
	check_expected_ptr(val);
	check_expected_ptr(parent);
}

/* Tests for jso_schema_keyword_get. */

/* Test freeing keyword with any type. */
static void test_jso_schema_keyword_get_for_any(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_keyword keyword;
	jso_schema_value parent;
	const char *key = "single";

	expect_function_call(__wrap_jso_schema_keyword_get_any);
	expect_value(__wrap_jso_schema_keyword_get_any, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_any, data, &data);
	expect_value(__wrap_jso_schema_keyword_get_any, key, key);
	expect_value(__wrap_jso_schema_keyword_get_any, error_on_invalid_type, true);
	expect_value(__wrap_jso_schema_keyword_get_any, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_any, schema_keyword, &keyword);
	expect_value(__wrap_jso_schema_keyword_get_any, val, &val);
	expect_value(__wrap_jso_schema_keyword_get_any, parent, &parent);

	jso_schema_keyword_get_ex(
			&schema, &data, key, &parent, JSO_SCHEMA_KEYWORD_TYPE_ANY, 0, true, &keyword, &val);
}

/* Test freeing keyword with null type. */
static void test_jso_schema_keyword_get_for_null(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_keyword keyword;
	jso_schema_value parent;
	const char *key = "single";

	expect_function_call(__wrap_jso_schema_keyword_get_null);
	expect_value(__wrap_jso_schema_keyword_get_null, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_null, data, &data);
	expect_value(__wrap_jso_schema_keyword_get_null, key, key);
	expect_value(__wrap_jso_schema_keyword_get_null, error_on_invalid_type, true);
	expect_value(__wrap_jso_schema_keyword_get_null, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_null, schema_keyword, &keyword);
	expect_value(__wrap_jso_schema_keyword_get_null, val, &val);
	expect_value(__wrap_jso_schema_keyword_get_null, parent, &parent);

	jso_schema_keyword_get_ex(
			&schema, &data, key, &parent, JSO_SCHEMA_KEYWORD_TYPE_NULL, 0, true, &keyword, &val);
}

/* Test freeing keyword with bool type. */
static void test_jso_schema_keyword_get_for_bool(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_keyword keyword;
	jso_schema_value parent;
	const char *key = "single";

	expect_function_call(__wrap_jso_schema_keyword_get_bool);
	expect_value(__wrap_jso_schema_keyword_get_bool, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_bool, data, &data);
	expect_value(__wrap_jso_schema_keyword_get_bool, key, key);
	expect_value(__wrap_jso_schema_keyword_get_bool, error_on_invalid_type, true);
	expect_value(__wrap_jso_schema_keyword_get_bool, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_bool, schema_keyword, &keyword);
	expect_value(__wrap_jso_schema_keyword_get_bool, val, &val);
	expect_value(__wrap_jso_schema_keyword_get_bool, parent, &parent);

	jso_schema_keyword_get_ex(
			&schema, &data, key, &parent, JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN, 0, true, &keyword, &val);
}

/* Test freeing keyword with int type. */
static void test_jso_schema_keyword_get_for_int(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_keyword keyword;
	jso_schema_value parent;
	const char *key = "single";

	expect_function_call(__wrap_jso_schema_keyword_get_int);
	expect_value(__wrap_jso_schema_keyword_get_int, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_int, data, &data);
	expect_value(__wrap_jso_schema_keyword_get_int, key, key);
	expect_value(__wrap_jso_schema_keyword_get_int, error_on_invalid_type, true);
	expect_value(__wrap_jso_schema_keyword_get_int, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_int, schema_keyword, &keyword);
	expect_value(__wrap_jso_schema_keyword_get_int, val, &val);
	expect_value(__wrap_jso_schema_keyword_get_int, parent, &parent);

	jso_schema_keyword_get_ex(
			&schema, &data, key, &parent, JSO_SCHEMA_KEYWORD_TYPE_INTEGER, 0, true, &keyword, &val);
}

/* Test freeing keyword with uint type. */
static void test_jso_schema_keyword_get_for_uint(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_keyword keyword;
	jso_schema_value parent;
	const char *key = "single";

	expect_function_call(__wrap_jso_schema_keyword_get_uint);
	expect_value(__wrap_jso_schema_keyword_get_uint, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_uint, data, &data);
	expect_value(__wrap_jso_schema_keyword_get_uint, key, key);
	expect_value(__wrap_jso_schema_keyword_get_uint, error_on_invalid_type, true);
	expect_value(__wrap_jso_schema_keyword_get_uint, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_uint, schema_keyword, &keyword);
	expect_value(__wrap_jso_schema_keyword_get_uint, val, &val);
	expect_value(__wrap_jso_schema_keyword_get_uint, parent, &parent);

	jso_schema_keyword_get_ex(&schema, &data, key, &parent,
			JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER, 0, true, &keyword, &val);
}

/* Test freeing keyword with double type. */
static void test_jso_schema_keyword_get_for_double(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_keyword keyword;
	jso_schema_value parent;
	const char *key = "single";

	expect_function_call(__wrap_jso_schema_keyword_get_number);
	expect_value(__wrap_jso_schema_keyword_get_number, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_number, data, &data);
	expect_value(__wrap_jso_schema_keyword_get_number, key, key);
	expect_value(__wrap_jso_schema_keyword_get_number, error_on_invalid_type, true);
	expect_value(__wrap_jso_schema_keyword_get_number, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_number, schema_keyword, &keyword);
	expect_value(__wrap_jso_schema_keyword_get_number, val, &val);
	expect_value(__wrap_jso_schema_keyword_get_number, parent, &parent);

	jso_schema_keyword_get_ex(
			&schema, &data, key, &parent, JSO_SCHEMA_KEYWORD_TYPE_DOUBLE, 0, true, &keyword, &val);
}

/* Test freeing keyword with number type. */
static void test_jso_schema_keyword_get_for_number(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_keyword keyword;
	jso_schema_value parent;
	const char *key = "single";

	JSO_SCHEMA_KEYWORD_TYPE(keyword) = JSO_SCHEMA_KEYWORD_TYPE_NUMBER;

	expect_function_call(__wrap_jso_schema_keyword_get_number);
	expect_value(__wrap_jso_schema_keyword_get_number, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_number, data, &data);
	expect_value(__wrap_jso_schema_keyword_get_number, key, key);
	expect_value(__wrap_jso_schema_keyword_get_number, error_on_invalid_type, true);
	expect_value(__wrap_jso_schema_keyword_get_number, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_number, schema_keyword, &keyword);
	expect_value(__wrap_jso_schema_keyword_get_number, val, &val);
	expect_value(__wrap_jso_schema_keyword_get_number, parent, &parent);

	jso_schema_keyword_get_ex(
			&schema, &data, key, &parent, JSO_SCHEMA_KEYWORD_TYPE_NUMBER, 0, true, &keyword, &val);
}

/* Test freeing keyword with string type. */
static void test_jso_schema_keyword_get_for_string(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_keyword keyword;
	jso_schema_value parent;
	const char *key = "single";

	expect_function_call(__wrap_jso_schema_keyword_get_string);
	expect_value(__wrap_jso_schema_keyword_get_string, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_string, data, &data);
	expect_value(__wrap_jso_schema_keyword_get_string, key, key);
	expect_value(__wrap_jso_schema_keyword_get_string, error_on_invalid_type, true);
	expect_value(__wrap_jso_schema_keyword_get_string, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_string, schema_keyword, &keyword);
	expect_value(__wrap_jso_schema_keyword_get_string, val, &val);
	expect_value(__wrap_jso_schema_keyword_get_string, parent, &parent);

	jso_schema_keyword_get_ex(
			&schema, &data, key, &parent, JSO_SCHEMA_KEYWORD_TYPE_STRING, 0, true, &keyword, &val);
}

/* Test freeing keyword with regexp type. */
static void test_jso_schema_keyword_get_for_regexp(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_keyword keyword;
	jso_schema_value parent;
	const char *key = "single";

	expect_function_call(__wrap_jso_schema_keyword_get_regexp);
	expect_value(__wrap_jso_schema_keyword_get_regexp, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_regexp, data, &data);
	expect_value(__wrap_jso_schema_keyword_get_regexp, key, key);
	expect_value(__wrap_jso_schema_keyword_get_regexp, error_on_invalid_type, true);
	expect_value(__wrap_jso_schema_keyword_get_regexp, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_regexp, schema_keyword, &keyword);
	expect_value(__wrap_jso_schema_keyword_get_regexp, val, &val);
	expect_value(__wrap_jso_schema_keyword_get_regexp, parent, &parent);

	jso_schema_keyword_get_ex(
			&schema, &data, key, &parent, JSO_SCHEMA_KEYWORD_TYPE_REGEXP, 0, true, &keyword, &val);
}

/* Test freeing keyword with array type. */
static void test_jso_schema_keyword_get_for_array(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_keyword keyword;
	jso_schema_value parent;
	const char *key = "single";

	expect_function_call(__wrap_jso_schema_keyword_get_array);
	expect_value(__wrap_jso_schema_keyword_get_array, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_array, data, &data);
	expect_value(__wrap_jso_schema_keyword_get_array, key, key);
	expect_value(__wrap_jso_schema_keyword_get_array, error_on_invalid_type, true);
	expect_value(__wrap_jso_schema_keyword_get_array, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_array, schema_keyword, &keyword);
	expect_value(__wrap_jso_schema_keyword_get_array, val, &val);
	expect_value(__wrap_jso_schema_keyword_get_array, parent, &parent);

	jso_schema_keyword_get_ex(
			&schema, &data, key, &parent, JSO_SCHEMA_KEYWORD_TYPE_ARRAY, 0, true, &keyword, &val);
}

/* Test freeing keyword with array of strings type. */
static void test_jso_schema_keyword_get_for_array_of_strings(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_keyword keyword;
	jso_schema_value parent;
	const char *key = "single";

	expect_function_call(__wrap_jso_schema_keyword_get_array_of_strings);
	expect_value(__wrap_jso_schema_keyword_get_array_of_strings, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_array_of_strings, data, &data);
	expect_value(__wrap_jso_schema_keyword_get_array_of_strings, key, key);
	expect_value(__wrap_jso_schema_keyword_get_array_of_strings, error_on_invalid_type, true);
	expect_value(__wrap_jso_schema_keyword_get_array_of_strings, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_array_of_strings, schema_keyword, &keyword);
	expect_value(__wrap_jso_schema_keyword_get_array_of_strings, val, &val);
	expect_value(__wrap_jso_schema_keyword_get_array_of_strings, parent, &parent);

	jso_schema_keyword_get_ex(&schema, &data, key, &parent,
			JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_STRINGS, 0, true, &keyword, &val);
}

/* Test freeing keyword with array of schema objects type. */
static void test_jso_schema_keyword_get_for_array_of_schema_objects(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_keyword keyword;
	jso_schema_value parent;
	const char *key = "single";

	expect_function_call(__wrap_jso_schema_keyword_get_array_of_schema_objects);
	expect_value(__wrap_jso_schema_keyword_get_array_of_schema_objects, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_array_of_schema_objects, data, &data);
	expect_value(__wrap_jso_schema_keyword_get_array_of_schema_objects, key, key);
	expect_value(
			__wrap_jso_schema_keyword_get_array_of_schema_objects, error_on_invalid_type, true);
	expect_value(__wrap_jso_schema_keyword_get_array_of_schema_objects, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_array_of_schema_objects, schema_keyword, &keyword);
	expect_value(__wrap_jso_schema_keyword_get_array_of_schema_objects, val, &val);
	expect_value(__wrap_jso_schema_keyword_get_array_of_schema_objects, parent, &parent);

	jso_schema_keyword_get_ex(&schema, &data, key, &parent,
			JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS, 0, true, &keyword, &val);
}

/* Test freeing keyword with object type. */
static void test_jso_schema_keyword_get_for_object(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_keyword keyword;
	jso_schema_value parent;
	const char *key = "single";

	expect_function_call(__wrap_jso_schema_keyword_get_object);
	expect_value(__wrap_jso_schema_keyword_get_object, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_object, data, &data);
	expect_value(__wrap_jso_schema_keyword_get_object, key, key);
	expect_value(__wrap_jso_schema_keyword_get_object, error_on_invalid_type, true);
	expect_value(__wrap_jso_schema_keyword_get_object, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_object, schema_keyword, &keyword);
	expect_value(__wrap_jso_schema_keyword_get_object, val, &val);
	expect_value(__wrap_jso_schema_keyword_get_object, parent, &parent);

	jso_schema_keyword_get_ex(
			&schema, &data, key, &parent, JSO_SCHEMA_KEYWORD_TYPE_OBJECT, 0, true, &keyword, &val);
}

/* Test freeing keyword with schema object type. */
static void test_jso_schema_keyword_get_for_schema_object(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_keyword keyword;
	jso_schema_value parent;
	const char *key = "single";

	expect_function_call(__wrap_jso_schema_keyword_get_schema_object);
	expect_value(__wrap_jso_schema_keyword_get_schema_object, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_schema_object, data, &data);
	expect_value(__wrap_jso_schema_keyword_get_schema_object, key, key);
	expect_value(__wrap_jso_schema_keyword_get_schema_object, error_on_invalid_type, true);
	expect_value(__wrap_jso_schema_keyword_get_schema_object, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_schema_object, schema_keyword, &keyword);
	expect_value(__wrap_jso_schema_keyword_get_schema_object, val, &val);
	expect_value(__wrap_jso_schema_keyword_get_schema_object, parent, &parent);

	jso_schema_keyword_get_ex(&schema, &data, key, &parent, JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT,
			0, true, &keyword, &val);
}

/* Test freeing keyword with object of schema objects type. */
static void test_jso_schema_keyword_get_for_object_of_schema_objects(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_keyword keyword;
	jso_schema_value parent;
	const char *key = "single";

	expect_function_call(__wrap_jso_schema_keyword_get_object_of_schema_objects);
	expect_value(__wrap_jso_schema_keyword_get_object_of_schema_objects, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_object_of_schema_objects, data, &data);
	expect_value(__wrap_jso_schema_keyword_get_object_of_schema_objects, key, key);
	expect_value(
			__wrap_jso_schema_keyword_get_object_of_schema_objects, error_on_invalid_type, true);
	expect_value(__wrap_jso_schema_keyword_get_object_of_schema_objects, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_object_of_schema_objects, schema_keyword, &keyword);
	expect_value(__wrap_jso_schema_keyword_get_object_of_schema_objects, val, &val);
	expect_value(__wrap_jso_schema_keyword_get_object_of_schema_objects, parent, &parent);

	jso_schema_keyword_get_ex(&schema, &data, key, &parent,
			JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS, 0, true, &keyword, &val);
}

/* Test freeing keyword with object of schema objects or_array of strings type. */
static void test_jso_schema_keyword_get_for_object_of_schema_objects_or_arrstr(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_keyword keyword;
	jso_schema_value parent;
	const char *key = "single";

	expect_function_call(
			__wrap_jso_schema_keyword_get_object_of_schema_objects_or_array_of_strings);
	expect_value(__wrap_jso_schema_keyword_get_object_of_schema_objects_or_array_of_strings, schema,
			&schema);
	expect_value(__wrap_jso_schema_keyword_get_object_of_schema_objects_or_array_of_strings, data,
			&data);
	expect_value(
			__wrap_jso_schema_keyword_get_object_of_schema_objects_or_array_of_strings, key, key);
	expect_value(__wrap_jso_schema_keyword_get_object_of_schema_objects_or_array_of_strings,
			error_on_invalid_type, true);
	expect_value(__wrap_jso_schema_keyword_get_object_of_schema_objects_or_array_of_strings,
			keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_object_of_schema_objects_or_array_of_strings,
			schema_keyword, &keyword);
	expect_value(
			__wrap_jso_schema_keyword_get_object_of_schema_objects_or_array_of_strings, val, &val);
	expect_value(__wrap_jso_schema_keyword_get_object_of_schema_objects_or_array_of_strings, parent,
			&parent);

	jso_schema_keyword_get_ex(&schema, &data, key, &parent,
			JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS_OR_ARRAY_OF_STRINGS, 0, true, &keyword,
			&val);
}

/* Test freeing keyword with regexp_object_of_schema_objects type. */
static void test_jso_schema_keyword_get_for_regexp_object_of_schema_objects(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_value data, val;
	jso_schema_keyword keyword;
	jso_schema_value parent;
	const char *key = "single";

	expect_function_call(__wrap_jso_schema_keyword_get_regexp_object_of_schema_objects);
	expect_value(__wrap_jso_schema_keyword_get_regexp_object_of_schema_objects, schema, &schema);
	expect_value(__wrap_jso_schema_keyword_get_regexp_object_of_schema_objects, data, &data);
	expect_value(__wrap_jso_schema_keyword_get_regexp_object_of_schema_objects, key, key);
	expect_value(__wrap_jso_schema_keyword_get_regexp_object_of_schema_objects,
			error_on_invalid_type, true);
	expect_value(__wrap_jso_schema_keyword_get_regexp_object_of_schema_objects, keyword_flags, 0);
	expect_value(__wrap_jso_schema_keyword_get_regexp_object_of_schema_objects, schema_keyword,
			&keyword);
	expect_value(__wrap_jso_schema_keyword_get_regexp_object_of_schema_objects, val, &val);
	expect_value(__wrap_jso_schema_keyword_get_regexp_object_of_schema_objects, parent, &parent);

	jso_schema_keyword_get_ex(&schema, &data, key, &parent,
			JSO_SCHEMA_KEYWORD_TYPE_REGEXP_OBJECT_OF_SCHEMA_OBJECTS, 0, true, &keyword, &val);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_schema_keyword_get_for_any),
		cmocka_unit_test(test_jso_schema_keyword_get_for_null),
		cmocka_unit_test(test_jso_schema_keyword_get_for_bool),
		cmocka_unit_test(test_jso_schema_keyword_get_for_int),
		cmocka_unit_test(test_jso_schema_keyword_get_for_uint),
		cmocka_unit_test(test_jso_schema_keyword_get_for_double),
		cmocka_unit_test(test_jso_schema_keyword_get_for_number),
		cmocka_unit_test(test_jso_schema_keyword_get_for_string),
		cmocka_unit_test(test_jso_schema_keyword_get_for_regexp),
		cmocka_unit_test(test_jso_schema_keyword_get_for_array),
		cmocka_unit_test(test_jso_schema_keyword_get_for_array_of_strings),
		cmocka_unit_test(test_jso_schema_keyword_get_for_array_of_schema_objects),
		cmocka_unit_test(test_jso_schema_keyword_get_for_object),
		cmocka_unit_test(test_jso_schema_keyword_get_for_schema_object),
		cmocka_unit_test(test_jso_schema_keyword_get_for_object_of_schema_objects),
		cmocka_unit_test(test_jso_schema_keyword_get_for_object_of_schema_objects_or_arrstr),
		cmocka_unit_test(test_jso_schema_keyword_get_for_regexp_object_of_schema_objects),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
