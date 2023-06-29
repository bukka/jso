
#include "../../src/jso_ht.h"
#include "../../src/jso.h"

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* A test case that adds items to object and loop through them. */
static void jso_test_object_add(void **state)
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
static void jso_test_object_get(void **state)
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
static void jso_test_object_resize(void **state)
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

int main(void)
{
	// clang-format off
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(jso_test_object_add),
		cmocka_unit_test(jso_test_object_get),
		cmocka_unit_test(jso_test_object_resize),
	};
	// clang-format on

	return cmocka_run_group_tests(tests, NULL, NULL);
}
