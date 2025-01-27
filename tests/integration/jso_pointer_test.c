/*
 * Copyright (c) 2024-2025 Jakub Zelenka. All rights reserved.
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
#include "../../src/jso_builder.h"
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

/* A test for point resolving. */
static void test_jso_pointer_resolve(void **state)
{
	(void) state; /* unused */

	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "key_str", "str1");
	jso_builder_object_add_int(&builder, "key_int", 11);
	jso_builder_object_add_object_start(&builder, "key_obj");
	jso_builder_object_add_int(&builder, "nested_key_int", 111);
	jso_builder_object_add_object_start(&builder, "nested_key_obj");
	jso_builder_object_add_cstr(&builder, "deep_key_str", "str_deep");
	jso_builder_object_end(&builder); // nested_key_obj
	jso_builder_object_add_array_start(&builder, "nested_key_arr");
	jso_builder_array_add_int(&builder, 112);
	jso_builder_array_add_cstr(&builder, "data");
	jso_builder_array_add_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "arr_key", "arr_val");
	jso_builder_object_end(&builder); // arr boj
	jso_builder_array_add_array_start(&builder);
	jso_builder_array_add_int(&builder, 114);
	jso_builder_array_end(&builder); // nested arr
	jso_builder_array_end(&builder); // arra
	jso_builder_object_end(&builder); // key_obj
	jso_builder_object_end(&builder); // root

	jso_value *root = jso_builder_get_value(&builder);
	jso_value *value;
	jso_string *str;
	jso_pointer *pointer;

	str = jso_string_create_from_cstr("/key_str");
	pointer = jso_pointer_create(str);
	assert_int_equal(JSO_SUCCESS, jso_pointer_resolve(pointer, root, &value));
	assert_int_equal(JSO_TYPE_STRING, JSO_TYPE_P(value));
	assert_true(jso_string_equals_to_cstr(JSO_STR_P(value), "str1"));
	jso_string_free(str);
	jso_pointer_free(pointer);

	str = jso_string_create_from_cstr("/key_int");
	pointer = jso_pointer_create(str);
	assert_int_equal(JSO_SUCCESS, jso_pointer_resolve(pointer, root, &value));
	assert_int_equal(JSO_TYPE_INT, JSO_TYPE_P(value));
	assert_int_equal(11, JSO_IVAL_P(value));
	jso_string_free(str);
	jso_pointer_free(pointer);

	str = jso_string_create_from_cstr("/key_obj/nested_key_int");
	pointer = jso_pointer_create(str);
	assert_int_equal(JSO_SUCCESS, jso_pointer_resolve(pointer, root, &value));
	assert_int_equal(JSO_TYPE_INT, JSO_TYPE_P(value));
	assert_int_equal(111, JSO_IVAL_P(value));
	jso_string_free(str);
	jso_pointer_free(pointer);

	str = jso_string_create_from_cstr("/key_obj/nested_key_obj/deep_key_str");
	pointer = jso_pointer_create(str);
	assert_int_equal(JSO_SUCCESS, jso_pointer_resolve(pointer, root, &value));
	assert_int_equal(JSO_TYPE_STRING, JSO_TYPE_P(value));
	assert_true(jso_string_equals_to_cstr(JSO_STR_P(value), "str_deep"));
	jso_string_free(str);
	jso_pointer_free(pointer);

	str = jso_string_create_from_cstr("/key_obj/nested_key_arr/0");
	pointer = jso_pointer_create(str);
	assert_int_equal(JSO_SUCCESS, jso_pointer_resolve(pointer, root, &value));
	assert_int_equal(JSO_TYPE_INT, JSO_TYPE_P(value));
	assert_int_equal(112, JSO_IVAL_P(value));
	jso_string_free(str);
	jso_pointer_free(pointer);

	str = jso_string_create_from_cstr("/key_obj/nested_key_arr/1");
	pointer = jso_pointer_create(str);
	assert_int_equal(JSO_SUCCESS, jso_pointer_resolve(pointer, root, &value));
	assert_int_equal(JSO_TYPE_STRING, JSO_TYPE_P(value));
	assert_true(jso_string_equals_to_cstr(JSO_STR_P(value), "data"));
	jso_string_free(str);
	jso_pointer_free(pointer);

	str = jso_string_create_from_cstr("/key_obj/nested_key_arr/2/arr_key");
	pointer = jso_pointer_create(str);
	assert_int_equal(JSO_SUCCESS, jso_pointer_resolve(pointer, root, &value));
	assert_int_equal(JSO_TYPE_STRING, JSO_TYPE_P(value));
	assert_true(jso_string_equals_to_cstr(JSO_STR_P(value), "arr_val"));
	jso_string_free(str);
	jso_pointer_free(pointer);

	str = jso_string_create_from_cstr("/key_obj/nested_key_arr/3/0");
	pointer = jso_pointer_create(str);
	assert_int_equal(JSO_SUCCESS, jso_pointer_resolve(pointer, root, &value));
	assert_int_equal(JSO_TYPE_INT, JSO_TYPE_P(value));
	assert_int_equal(114, JSO_IVAL_P(value));
	jso_string_free(str);
	jso_pointer_free(pointer);

	str = jso_string_create_from_cstr("/key_unknown");
	pointer = jso_pointer_create(str);
	assert_int_equal(JSO_FAILURE, jso_pointer_resolve(pointer, root, &value));
	assert_int_equal(JSO_POINTER_ERROR_NOT_FOUND, JSO_POINTER_ERROR_TYPE(pointer));
	jso_string_free(str);
	jso_pointer_free(pointer);

	str = jso_string_create_from_cstr("/key_obj/nested_key_arr/8");
	pointer = jso_pointer_create(str);
	assert_int_equal(JSO_FAILURE, jso_pointer_resolve(pointer, root, &value));
	assert_int_equal(JSO_POINTER_ERROR_NOT_FOUND, JSO_POINTER_ERROR_TYPE(pointer));
	jso_string_free(str);
	jso_pointer_free(pointer);

	str = jso_string_create_from_cstr("/key_obj/nested_key_arr/val");
	pointer = jso_pointer_create(str);
	assert_int_equal(JSO_FAILURE, jso_pointer_resolve(pointer, root, &value));
	assert_int_equal(JSO_POINTER_ERROR_INVALID_ARRAY_INDEX, JSO_POINTER_ERROR_TYPE(pointer));
	jso_string_free(str);
	jso_pointer_free(pointer);

	jso_builder_clear_all(&builder);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_pointer_create_simple),
		cmocka_unit_test(test_jso_pointer_create_with_valid_simple_escapes),
		cmocka_unit_test(test_jso_pointer_create_with_valid_mixed_escapes),
		cmocka_unit_test(test_jso_pointer_create_with_invalid_escape_number),
		cmocka_unit_test(test_jso_pointer_create_with_invalid_escape_end),
		cmocka_unit_test(test_jso_pointer_resolve),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
