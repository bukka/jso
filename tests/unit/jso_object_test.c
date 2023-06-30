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

#include "../../src/jso.h"

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* A test case that adds items to object and loop through them. */
static void test_jso_object_add(void **state)
{
	(void) state; /* unused */

	jso_value val1, val2, val3, val4, *val;
	JSO_VALUE_SET_INT(val1, 1);
	JSO_VALUE_SET_INT(val2, 2);
	JSO_VALUE_SET_INT(val3, 3);
	JSO_VALUE_SET_INT(val4, 4);

	jso_string *key;
	jso_string *key1 = jso_string_create_from_cstr("key");
	jso_string *key2 = jso_string_create_from_cstr("second key");
	jso_string *key3 = jso_string_create_from_cstr("third key");
	jso_string *key4 = jso_string_create_from_cstr("key");

	jso_object *obj = jso_object_alloc();

	jso_object_add(obj, key1, &val1);
	jso_object_add(obj, key2, &val2);
	jso_object_add(obj, key3, &val3);
	jso_object_add(obj, key4, &val4);

	assert_int_equal(3, JSO_OBJECT_COUNT(obj));

	jso_int i = 1;
	JSO_OBJECT_FOREACH(obj, key, val)
	{
		switch (i++) {
			case 1:
				assert_true(jso_string_equals_to_cstr(key, "key"));
				assert_int_equal(4, JSO_IVAL_P(val));
				break;
			case 2:
				assert_true(jso_string_equals_to_cstr(key, "second key"));
				assert_int_equal(2, JSO_IVAL_P(val));
				break;
			case 3:
				assert_true(jso_string_equals_to_cstr(key, "third key"));
				assert_int_equal(3, JSO_IVAL_P(val));
				break;
		}
	}
	JSO_OBJECT_FOREACH_END;

	jso_object_free(obj);
}

/* A test case that gets item from object. */
static void test_jso_object_get(void **state)
{
	(void) state; /* unused */

	jso_value val1, val2, val3, *val = NULL;
	JSO_VALUE_SET_INT(val1, 1);
	JSO_VALUE_SET_INT(val2, 2);
	JSO_VALUE_SET_INT(val3, 3);

	jso_string *key1 = jso_string_create_from_cstr("key");
	jso_string *key2 = jso_string_create_from_cstr("second key");
	jso_string *key3 = jso_string_create_from_cstr("third key");
	jso_string *key4 = jso_string_create_from_cstr("no key");

	jso_object *obj = jso_object_alloc();

	// get should return false on no value in object
	assert_true(jso_object_get(obj, key1, &val) == JSO_FAILURE);
	assert_null(val);

	jso_object_add(obj, key1, &val1);
	jso_object_add(obj, key2, &val2);
	jso_object_add(obj, key3, &val3);

	// get should return false on no value in hash table
	assert_true(jso_object_get(obj, key2, &val) == JSO_SUCCESS);
	assert_int_equal(2, JSO_IVAL_P(val));

	// get should return false on no value in hash table
	assert_true(jso_object_get(obj, key4, &val) == JSO_FAILURE);
	// value should not change
	assert_int_equal(2, JSO_IVAL_P(val));

	jso_object_free(obj);
	jso_string_free(key4);
}

/* A test case that resizes hash table. */
static void test_jso_object_resize(void **state)
{
	(void) state; /* unused */

	jso_value val1, val2, val3, *val = NULL;
	JSO_VALUE_SET_INT(val1, 1);
	JSO_VALUE_SET_INT(val2, 2);
	JSO_VALUE_SET_INT(val3, 3);

	jso_string *key1 = jso_string_create_from_cstr("key");
	jso_string *key2 = jso_string_create_from_cstr("second key");
	jso_string *key3 = jso_string_create_from_cstr("third key");

	jso_object *obj = jso_object_alloc();
	jso_object_add(obj, key1, &val1);
	jso_object_add(obj, key2, &val2);
	jso_object_add(obj, key3, &val3);

	// it should fail when trying to reduce capacity
	assert_true(jso_object_resize(obj, 0) == JSO_FAILURE);

	// it should succeed if increasing capacity
	assert_true(jso_object_resize(obj, JSO_OBJECT_CAPACITY(obj) + 4) == JSO_SUCCESS);

	// check all values and keys are still present and in correct order after resizing
	jso_int i = 1;
	jso_string *key;
	JSO_OBJECT_FOREACH(obj, key, val)
	{
		switch (i++) {
			case 1:
				assert_true(jso_string_equals_to_cstr(key, "key"));
				assert_int_equal(1, JSO_IVAL_P(val));
				break;
			case 2:
				assert_true(jso_string_equals_to_cstr(key, "second key"));
				assert_int_equal(2, JSO_IVAL_P(val));
				break;
			case 3:
				assert_true(jso_string_equals_to_cstr(key, "third key"));
				assert_int_equal(3, JSO_IVAL_P(val));
				break;
		}
	}
	JSO_OBJECT_FOREACH_END;

	jso_object_free(obj);
}

/* A test case that tests applying function for all object key value pairs. */
static jso_int object_apply_mask = 0;

static void jso_test_object_apply_callback(jso_string *key, jso_value *val)
{
	jso_int ival = JSO_IVAL_P(val);
	switch (ival) {
		case 1:
			assert_true(jso_string_equals_to_cstr(key, "key"));
			break;
		case 2:
			assert_true(jso_string_equals_to_cstr(key, "second key"));
			break;
		case 3:
			assert_true(jso_string_equals_to_cstr(key, "third key"));
			break;
		default:
			// make sure no other value is returned
			assert_true(false);
	}
	object_apply_mask |= (1 << (ival - 1));
}

static void test_jso_object_apply(void **state)
{
	(void) state; /* unused */

	jso_value val1, val2, val3;
	JSO_VALUE_SET_INT(val1, 1);
	JSO_VALUE_SET_INT(val2, 2);
	JSO_VALUE_SET_INT(val3, 3);

	jso_string *key1 = jso_string_create_from_cstr("key");
	jso_string *key2 = jso_string_create_from_cstr("second key");
	jso_string *key3 = jso_string_create_from_cstr("third key");

	jso_object *obj = jso_object_alloc();
	jso_object_add(obj, key1, &val1);
	jso_object_add(obj, key2, &val2);
	jso_object_add(obj, key3, &val3);

	object_apply_mask = 0;
	jso_object_apply(obj, jso_test_object_apply_callback);
	assert_int_equal(7, object_apply_mask);
	object_apply_mask = 0;

	jso_object_free(obj);
}

/* A test case that tests applying function with args for all object key value pairs. */
static void jso_test_object_apply_with_arg_callback(jso_string *key, jso_value *val, void *arg)
{

	jso_int ival = JSO_IVAL_P(val);
	switch (ival) {
		case 1:
			assert_true(jso_string_equals_to_cstr(key, "key"));
			break;
		case 2:
			assert_true(jso_string_equals_to_cstr(key, "second key"));
			break;
		case 3:
			assert_true(jso_string_equals_to_cstr(key, "third key"));
			break;
		default:
			// make sure no other value is returned
			assert_true(false);
	}
	int *obj_mask = (int *) arg;
	*obj_mask |= (1 << (ival - 1));
}

static void test_jso_object_apply_with_arg(void **state)
{
	(void) state; /* unused */

	jso_value val1, val2, val3;
	JSO_VALUE_SET_INT(val1, 1);
	JSO_VALUE_SET_INT(val2, 2);
	JSO_VALUE_SET_INT(val3, 3);

	jso_string *key1 = jso_string_create_from_cstr("key");
	jso_string *key2 = jso_string_create_from_cstr("second key");
	jso_string *key3 = jso_string_create_from_cstr("third key");

	jso_object *obj = jso_object_alloc();
	jso_object_add(obj, key1, &val1);
	jso_object_add(obj, key2, &val2);
	jso_object_add(obj, key3, &val3);

	int object_mask = 0;
	jso_object_apply_with_arg(obj, jso_test_object_apply_with_arg_callback, &object_mask);
	assert_int_equal(7, object_mask);

	jso_object_free(obj);
}

/* A test case that tests whether two objects are equal. */
static void test_jso_object_equals(void **state)
{
	(void) state; /* unused */

	jso_value val11, val21, val31, val41, val12, val22, val32, val42;
	JSO_VALUE_SET_INT(val11, 1);
	JSO_VALUE_SET_INT(val21, 2);
	JSO_VALUE_SET_INT(val31, 3);
	JSO_VALUE_SET_INT(val41, 3);
	JSO_VALUE_SET_INT(val12, 1);
	JSO_VALUE_SET_INT(val22, 2);
	JSO_VALUE_SET_INT(val32, 3);
	JSO_VALUE_SET_INT(val42, 4);

	jso_string *key11 = jso_string_create_from_cstr("key");
	jso_string *key21 = jso_string_create_from_cstr("second key");
	jso_string *key31 = jso_string_create_from_cstr("third key");
	jso_string *key41 = jso_string_create_from_cstr("fourth key");
	jso_string *key12 = jso_string_create_from_cstr("key");
	jso_string *key22 = jso_string_create_from_cstr("second key");
	jso_string *key32 = jso_string_create_from_cstr("third key");
	jso_string *key42 = jso_string_create_from_cstr("fourth key");

	jso_object *obj1 = jso_object_alloc();
	jso_object_add(obj1, key11, &val11);
	jso_object_add(obj1, key21, &val21);
	jso_object_add(obj1, key31, &val31);

	jso_object *obj2 = jso_object_alloc();
	jso_object_add(obj2, key12, &val12);
	jso_object_add(obj2, key22, &val22);

	assert_false(jso_object_equals(obj1, obj2));

	jso_object_add(obj2, key32, &val32);
	assert_true(jso_object_equals(obj1, obj2));

	jso_object_add(obj1, key41, &val41);
	jso_object_add(obj2, key42, &val42);
	assert_false(jso_object_equals(obj1, obj2));

	jso_object_free(obj1);
	jso_object_free(obj2);
}

/* A test to check whether object copying works by increasing reference */
static void test_jso_object_copy(void **state)
{
	(void) state; /* unused */

	jso_value val1, val2, val3;
	JSO_VALUE_SET_INT(val1, 1);
	JSO_VALUE_SET_INT(val2, 2);
	JSO_VALUE_SET_INT(val3, 3);

	jso_array *arr = jso_array_alloc();
	jso_array_push(arr, &val1);
	jso_array_push(arr, &val2);
	jso_array_push(arr, &val3);

	assert_int_equal(0, JSO_ARRAY_REFCOUNT(arr));
	assert_ptr_equal(arr, jso_array_copy(arr));
	assert_int_equal(1, JSO_ARRAY_REFCOUNT(arr));
	assert_ptr_equal(arr, jso_array_copy(arr));
	assert_int_equal(2, JSO_ARRAY_REFCOUNT(arr));
	jso_array_free(arr);
	assert_int_equal(1, JSO_ARRAY_REFCOUNT(arr));
	jso_array_free(arr);
	assert_int_equal(0, JSO_ARRAY_REFCOUNT(arr));
	jso_array_free(arr);
}

int main(void)
{
	// clang-format off
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_object_add),
		cmocka_unit_test(test_jso_object_get),
		cmocka_unit_test(test_jso_object_resize),
		cmocka_unit_test(test_jso_object_apply),
		cmocka_unit_test(test_jso_object_apply_with_arg),
		cmocka_unit_test(test_jso_object_equals),
		cmocka_unit_test(test_jso_object_copy),
	};
	// clang-format on

	return cmocka_run_group_tests(tests, NULL, NULL);
}
