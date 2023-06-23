#include "../../src/jso_array.h"
#include "../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* A test case that tests appending array item. */
static void jso_test_array_append(void **state)
{
	(void) state; /* unused */

	jso_value val1, val2, val3, *val;
	JSO_VALUE_SET_INT(val1, 1);
	JSO_VALUE_SET_INT(val2, 2);
	JSO_VALUE_SET_INT(val3, 3);

	jso_array *arr = jso_array_alloc();
	jso_array_append(arr, &val1);
	jso_array_append(arr, &val2);
	jso_array_append(arr, &val3);

	jso_int i = 1;
	JSO_ARRAY_FOREACH(arr, val)
	{
		assert_int_equal(i++, JSO_IVAL_P(val));
	}
	JSO_ARRAY_FOREACH_END;

	jso_array_free(arr);
}

/* A test case that tests pushing array item. */
static void jso_test_array_push(void **state)
{
	(void) state; /* unused */

	jso_value val1, val2, val3, *val;
	JSO_VALUE_SET_INT(val1, 1);
	JSO_VALUE_SET_INT(val2, 2);
	JSO_VALUE_SET_INT(val3, 3);

	jso_array *arr = jso_array_alloc();
	jso_array_push(arr, &val1);
	jso_array_push(arr, &val2);
	jso_array_push(arr, &val3);

	jso_int i = 3;
	JSO_ARRAY_FOREACH(arr, val)
	{
		assert_int_equal(i--, JSO_IVAL_P(val));
	}
	JSO_ARRAY_FOREACH_END;

	jso_array_free(arr);
}

/* A test case that tests poping array item. */
static void jso_test_array_pop(void **state)
{
	(void) state; /* unused */

	jso_value val1, val2, val3, *val;
	JSO_VALUE_SET_INT(val1, 1);
	JSO_VALUE_SET_INT(val2, 2);
	JSO_VALUE_SET_INT(val3, 3);

	jso_array *arr = jso_array_alloc();
	jso_array_push(arr, &val1);
	jso_array_push(arr, &val2);
	jso_array_push(arr, &val3);

	jso_int i = 2;
	jso_array_pop(arr);
	assert_int_equal(2, JSO_ARRAY_LEN(arr));
	JSO_ARRAY_FOREACH(arr, val)
	{
		assert_int_equal(i--, JSO_IVAL_P(val));
	}
	JSO_ARRAY_FOREACH_END;

	jso_array_free(arr);
}

/* A test case that tests applying function for all array items. */
static void jso_test_array_apply_callback(size_t idx, jso_value *val)
{
	assert_int_equal(idx + 1, JSO_IVAL_P(val));
}

static void jso_test_array_apply(void **state)
{
	(void) state; /* unused */

	jso_value val1, val2, val3;
	JSO_VALUE_SET_INT(val1, 1);
	JSO_VALUE_SET_INT(val2, 2);
	JSO_VALUE_SET_INT(val3, 3);

	jso_array *arr = jso_array_alloc();
	jso_array_append(arr, &val1);
	jso_array_append(arr, &val2);
	jso_array_append(arr, &val3);

	jso_array_apply(arr, jso_test_array_apply_callback);
}

/* A test case that tests applying function with args for all array items. */
static void jso_test_array_apply_with_args_callback(size_t idx, jso_value *val, void *arg)
{
	int *expected_val = (int *) arg;
	assert_int_equal(*expected_val, JSO_IVAL_P(val));
	*expected_val = *expected_val - 1;
}

static void jso_test_array_apply_with_args(void **state)
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

	jso_int i = 3;
	jso_array_apply_with_arg(arr, jso_test_array_apply_with_args_callback, &i);
	assert_int_equal(0, i);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(jso_test_array_append),
		cmocka_unit_test(jso_test_array_push),
		cmocka_unit_test(jso_test_array_pop),
		cmocka_unit_test(jso_test_array_apply),
		cmocka_unit_test(jso_test_array_apply_with_args),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}