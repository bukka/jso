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

/* A test case that sets item in hash table and loop through them. */
static void test_jso_ht_set(void **state)
{
	(void) state; /* unused */

	jso_value val1, val2, val3, val4, val5, *val;
	JSO_VALUE_SET_INT(val1, 1);
	JSO_VALUE_SET_INT(val2, 2);
	JSO_VALUE_SET_INT(val3, 3);
	JSO_VALUE_SET_INT(val4, 4);
	JSO_VALUE_SET_INT(val5, 5);

	jso_string *key;
	jso_string *key1 = jso_string_create_from_cstr("key");
	jso_string *key2 = jso_string_create_from_cstr("second key");
	jso_string *key3 = jso_string_create_from_cstr("third key");
	jso_string *key4 = jso_string_create_from_cstr("key");
	jso_string *key5 = jso_string_create_from_cstr("key");

	jso_ht *ht = jso_ht_alloc();

	jso_ht_set(ht, key1, &val1, false);
	jso_ht_set(ht, key2, &val2, false);
	jso_ht_set(ht, key3, &val3, false);
	jso_ht_set(ht, key4, &val4, false);
	jso_ht_set(ht, key5, &val5, false);
	jso_ht_set(ht, key1, &val3, true);

	assert_int_equal(3, ht->count);

	jso_int i = 1;
	JSO_HT_FOREACH(ht, key, val)
	{
		switch (i++) {
			case 1:
				assert_true(jso_string_equals_to_cstr(key, "key"));
				assert_int_equal(3, JSO_IVAL_P(val));
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
	JSO_HT_FOREACH_END;

	jso_ht_free(ht);

	// check the key4 is not freed
	assert_true(jso_string_equals_to_cstr(key4, "key"));
	jso_string_free(key4);
}

/* A test case that gets item from hash table. */
static void test_jso_ht_get(void **state)
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

	jso_ht *ht = jso_ht_alloc();

	// get should return false on no value in hash table
	assert_true(jso_ht_get(ht, key1, &val) == JSO_FAILURE);
	assert_null(val);

	jso_ht_set(ht, key1, &val1, false);
	jso_ht_set(ht, key2, &val2, false);
	jso_ht_set(ht, key3, &val3, false);

	// get should return false on no value in hash table
	assert_true(jso_ht_get(ht, key2, &val) == JSO_SUCCESS);
	assert_int_equal(2, JSO_IVAL_P(val));

	// get should return false on no value in hash table
	assert_true(jso_ht_get(ht, key4, &val) == JSO_FAILURE);
	// value should not change
	assert_int_equal(2, JSO_IVAL_P(val));

	jso_ht_free(ht);
	jso_string_free(key4);
}

/* A test case that gets item by C string key from hash table. */
static void test_jso_ht_get_by_cstr_key(void **state)
{
	(void) state; /* unused */

	jso_value val1, val2, val3, *val = NULL;
	JSO_VALUE_SET_INT(val1, 1);
	JSO_VALUE_SET_INT(val2, 2);
	JSO_VALUE_SET_INT(val3, 3);

	jso_string *key1 = jso_string_create_from_cstr("key");
	jso_string *key2 = jso_string_create_from_cstr("second key");
	jso_string *key3 = jso_string_create_from_cstr("third key");

	jso_ht *ht = jso_ht_alloc();

	// get should return false on no value in hash table
	assert_true(jso_ht_get_by_cstr_key(ht, "key", &val) == JSO_FAILURE);
	assert_null(val);

	jso_ht_set(ht, key1, &val1, false);
	jso_ht_set(ht, key2, &val2, false);
	jso_ht_set(ht, key3, &val3, false);

	// get should return false on no value in hash table
	assert_true(jso_ht_get_by_cstr_key(ht, "second key", &val) == JSO_SUCCESS);
	assert_int_equal(2, JSO_IVAL_P(val));

	// get should return false on no value in hash table
	assert_true(jso_ht_get_by_cstr_key(ht, "unknown key", &val) == JSO_FAILURE);
	// value should not change
	assert_int_equal(2, JSO_IVAL_P(val));

	jso_ht_free(ht);
}

/* A test case that checks whether item is in hash table. */
static void test_jso_ht_has(void **state)
{
	(void) state; /* unused */

	jso_value val1, val2, val3;
	JSO_VALUE_SET_INT(val1, 1);
	JSO_VALUE_SET_INT(val2, 2);
	JSO_VALUE_SET_INT(val3, 3);

	jso_string *key1 = jso_string_create_from_cstr("key");
	jso_string *key2 = jso_string_create_from_cstr("second key");
	jso_string *key3 = jso_string_create_from_cstr("third key");
	jso_string *key4 = jso_string_create_from_cstr("no key");

	jso_ht *ht = jso_ht_alloc();

	// get should return false on no value in hash table
	assert_false(jso_ht_has(ht, key1));

	jso_ht_set(ht, key1, &val1, false);
	jso_ht_set(ht, key2, &val2, false);
	jso_ht_set(ht, key3, &val3, false);

	// get should return false on no value in hash table
	assert_true(jso_ht_has(ht, key2));

	// get should return false on no value in hash table
	assert_false(jso_ht_has(ht, key4));

	jso_ht_free(ht);
	jso_string_free(key4);
}

/* A test case that clones hash table. */
static void test_jso_ht_clone(void **state)
{
	(void) state; /* unused */

	jso_value val1, val2, val3, *val = NULL;
	JSO_VALUE_SET_INT(val1, 1);
	JSO_VALUE_SET_INT(val2, 2);
	JSO_VALUE_SET_INT(val3, 3);

	jso_string *key1 = jso_string_create_from_cstr("key");
	jso_string *key2 = jso_string_create_from_cstr("second key");
	jso_string *key3 = jso_string_create_from_cstr("third key");

	jso_ht *ht_src = jso_ht_alloc();
	jso_ht *ht_dest = jso_ht_alloc();

	jso_ht_set(ht_src, key1, &val1, false);
	jso_ht_set(ht_src, key2, &val2, false);
	jso_ht_set(ht_src, key3, &val3, false);

	assert_true(jso_ht_clone(ht_src, ht_dest) == JSO_SUCCESS);

	// assert that all values have been copied
	assert_int_equal(3, ht_dest->count);
	assert_true(jso_ht_get_by_cstr_key(ht_dest, "key", &val) == JSO_SUCCESS);
	assert_int_equal(1, JSO_IVAL_P(val));
	assert_true(jso_ht_get_by_cstr_key(ht_dest, "second key", &val) == JSO_SUCCESS);
	assert_int_equal(2, JSO_IVAL_P(val));
	assert_true(jso_ht_get_by_cstr_key(ht_dest, "third key", &val) == JSO_SUCCESS);
	assert_int_equal(3, JSO_IVAL_P(val));

	jso_ht_free(ht_src);

	// check all values and keys are still in dest after src is freed
	jso_int i = 1;
	jso_string *key;
	JSO_HT_FOREACH(ht_dest, key, val)
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
	JSO_HT_FOREACH_END;

	jso_ht_free(ht_dest);
}

/* A test case that resizes hash table. */
static void test_jso_ht_resize(void **state)
{
	(void) state; /* unused */

	jso_value val1, val2, val3, *val = NULL;
	JSO_VALUE_SET_INT(val1, 1);
	JSO_VALUE_SET_INT(val2, 2);
	JSO_VALUE_SET_INT(val3, 3);

	jso_string *key1 = jso_string_create_from_cstr("key");
	jso_string *key2 = jso_string_create_from_cstr("second key");
	jso_string *key3 = jso_string_create_from_cstr("third key");

	jso_ht *ht = jso_ht_alloc();
	jso_ht_set(ht, key1, &val1, false);
	jso_ht_set(ht, key2, &val2, false);
	jso_ht_set(ht, key3, &val3, false);

	// it should fail when trying to reduce capacity
	assert_true(jso_ht_resize(ht, 0) == JSO_FAILURE);

	// it should succeed if increasing capacity
	assert_true(jso_ht_resize(ht, ht->capacity + 4) == JSO_SUCCESS);

	// check all values and keys are still present and in correct order after resizing
	jso_int i = 1;
	jso_string *key;
	JSO_HT_FOREACH(ht, key, val)
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
	JSO_HT_FOREACH_END;

	jso_ht_free(ht);
}

int main(void)
{
	// clang-format off
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_ht_set),
		cmocka_unit_test(test_jso_ht_get),
		cmocka_unit_test(test_jso_ht_get_by_cstr_key),
		cmocka_unit_test(test_jso_ht_has),
		cmocka_unit_test(test_jso_ht_clone),
		cmocka_unit_test(test_jso_ht_resize),
	};
	// clang-format on

	return cmocka_run_group_tests(tests, NULL, NULL);
}
