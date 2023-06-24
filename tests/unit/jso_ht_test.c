
#include "../../src/jso_ht.h"
#include "../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* A test case that sets item in array and loop through them. */
static void jso_test_ht_set(void **state)
{
	(void) state; /* unused */

	jso_value val1, val2, val3, val4, val5, *val;
	JSO_VALUE_SET_INT(val1, 1);
	JSO_VALUE_SET_INT(val2, 2);
	JSO_VALUE_SET_INT(val3, 3);
	JSO_VALUE_SET_INT(val3, 4);
	JSO_VALUE_SET_INT(val3, 5);

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
	jso_ht_set(ht, key5, &val5, true);
	jso_ht_set(ht, key1, &val3, false);

	assert_int_equal(3, ht->count);

	jso_int i = 1;
	JSO_HT_FOREACH(ht, key, val)
	{
		switch (i++) {
			case 1:
				assert_true(jso_string_equals_to_cstr(key, "key"));
				assert_int_equal(5, JSO_IVAL_P(val));
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

	// check the key1 is not freed
	assert_true(jso_string_equals_to_cstr(key1, "key"));
	jso_string_free(key4);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(jso_test_ht_set),
		// cmocka_unit_test(jso_test_ht_get),
		// cmocka_unit_test(jso_test_ht_get_by_cstr_key),
		// cmocka_unit_test(jso_test_ht_copy),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
