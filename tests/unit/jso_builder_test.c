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

#include "../../src/jso_builder.h"
#include "../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* A test case that tests various array building. */
static void test_jso_builder_array(void **state)
{
	(void) state; /* unused */

	jso_builder builder;
	jso_builder_init(&builder);

	assert_null(jso_builder_get_value(&builder));

	jso_string *str = jso_string_create_from_cstr("str");

	assert_int_equal(JSO_SUCCESS, jso_builder_array_start(&builder));
	assert_int_equal(JSO_SUCCESS, jso_builder_array_add_bool(&builder, true));
	assert_int_equal(JSO_SUCCESS, jso_builder_array_add_bool(&builder, false));
	assert_int_equal(JSO_SUCCESS, jso_builder_array_add_int(&builder, 5));
	assert_int_equal(JSO_SUCCESS, jso_builder_array_add_double(&builder, 3.2));
	assert_int_equal(JSO_SUCCESS, jso_builder_array_add_cstr(&builder, "test"));
	assert_int_equal(JSO_SUCCESS, jso_builder_array_add_string(&builder, str));
	assert_int_equal(JSO_SUCCESS, jso_builder_array_add_array_start(&builder));
	assert_int_equal(JSO_SUCCESS, jso_builder_array_add_int(&builder, 10));
	assert_int_equal(JSO_SUCCESS, jso_builder_array_add_object_start(&builder));

	// array cannot be added in object
	assert_int_equal(JSO_FAILURE, jso_builder_array_add_bool(&builder, false));
	assert_int_equal(JSO_FAILURE, jso_builder_array_add_int(&builder, 5));
	assert_int_equal(JSO_FAILURE, jso_builder_array_add_double(&builder, 3.2));
	assert_int_equal(JSO_FAILURE, jso_builder_array_add_cstr(&builder, "test"));
	assert_int_equal(JSO_FAILURE, jso_builder_array_add_string(&builder, str));
	assert_int_equal(JSO_FAILURE, jso_builder_array_add_array_start(&builder));
	assert_int_equal(JSO_FAILURE, jso_builder_array_add_object_start(&builder));
	assert_int_equal(JSO_FAILURE, jso_builder_array_end(&builder));

	assert_int_equal(JSO_SUCCESS, jso_builder_object_end(&builder));
	assert_int_equal(JSO_SUCCESS, jso_builder_array_end(&builder));
	assert_int_equal(JSO_SUCCESS, jso_builder_array_end(&builder));
	// already in root so it should fail
	assert_int_equal(JSO_FAILURE, jso_builder_array_end(&builder));

	jso_value *value = jso_builder_get_value(&builder);
	assert_non_null(value);
	assert_int_equal(JSO_TYPE_ARRAY, JSO_TYPE_P(value));

	size_t i = 0;
	jso_value *item;
	JSO_ARRAY_FOREACH(JSO_ARRVAL_P(value), item)
	{
		switch (i++) {
			case 0:
				assert_int_equal(JSO_TYPE_BOOL, JSO_TYPE_P(item));
				assert_true(JSO_IVAL_P(item));
				break;
			case 1:
				assert_int_equal(JSO_TYPE_BOOL, JSO_TYPE_P(item));
				assert_false(JSO_IVAL_P(item));
				break;
			case 2:
				assert_int_equal(JSO_TYPE_INT, JSO_TYPE_P(item));
				assert_int_equal(5, JSO_IVAL_P(item));
				break;
			case 3:
				assert_int_equal(JSO_TYPE_DOUBLE, JSO_TYPE_P(item));
				assert_int_equal(3.2, JSO_DVAL_P(item));
				break;
			case 4:
				assert_int_equal(JSO_TYPE_STRING, JSO_TYPE_P(item));
				assert_string_equal("test", JSO_SVAL_P(item));
				break;
			case 5:
				assert_int_equal(JSO_TYPE_STRING, JSO_TYPE_P(item));
				assert_ptr_equal(str, JSO_STR_P(item));
				break;
			case 6:
				assert_int_equal(JSO_TYPE_ARRAY, JSO_TYPE_P(item));
				jso_value *inner_item;
				size_t j = 0;
				JSO_ARRAY_FOREACH(JSO_ARRVAL_P(item), inner_item)
				{
					switch (j++) {
						case 0:
							assert_int_equal(JSO_TYPE_INT, JSO_TYPE_P(inner_item));
							assert_int_equal(10, JSO_IVAL_P(inner_item));
							break;
						case 1:
							assert_int_equal(JSO_TYPE_OBJECT, JSO_TYPE_P(inner_item));
							assert_int_equal(0, JSO_OBJECT_COUNT(JSO_OBJVAL_P(inner_item)));
							break;
						default:
							assert_false(true);
							break;
					}
				}
				JSO_ARRAY_FOREACH_END;
				break;
			default:
				assert_false(true);
				break;
		}
	}
	JSO_ARRAY_FOREACH_END;

	jso_value_free(value);
	jso_builder_clear(&builder);
}

/* A test case that tests various object building. */
static void test_jso_builder_object(void **state)
{
	(void) state; /* unused */

	jso_builder builder;
	jso_builder_init(&builder);

	assert_null(jso_builder_get_value(&builder));

	jso_string *str = jso_string_create_from_cstr("str");

	assert_int_equal(JSO_SUCCESS, jso_builder_object_start(&builder));
	assert_int_equal(JSO_SUCCESS, jso_builder_object_add_bool(&builder, "bt", true));
	assert_int_equal(JSO_SUCCESS, jso_builder_object_add_bool(&builder, "bf", false));
	assert_int_equal(JSO_SUCCESS, jso_builder_object_add_int(&builder, "ival", 5));
	assert_int_equal(JSO_SUCCESS, jso_builder_object_add_double(&builder, "dbl", 3.2));
	assert_int_equal(JSO_SUCCESS, jso_builder_object_add_cstr(&builder, "cstr", "test"));
	assert_int_equal(JSO_SUCCESS, jso_builder_object_add_string(&builder, "str", str));
	assert_int_equal(JSO_SUCCESS, jso_builder_object_add_object_start(&builder, "obj"));
	assert_int_equal(JSO_SUCCESS, jso_builder_object_add_int(&builder, "oi", 10));
	assert_int_equal(JSO_SUCCESS, jso_builder_object_add_array_start(&builder, "oa"));

	// object cannot be added in object
	assert_int_equal(JSO_FAILURE, jso_builder_object_add_bool(&builder, "b1", false));
	assert_int_equal(JSO_FAILURE, jso_builder_object_add_int(&builder, "b2", 5));
	assert_int_equal(JSO_FAILURE, jso_builder_object_add_double(&builder, "b3", 3.2));
	assert_int_equal(JSO_FAILURE, jso_builder_object_add_cstr(&builder, "b4", "test"));
	assert_int_equal(JSO_FAILURE, jso_builder_object_add_string(&builder, "b5", str));
	assert_int_equal(JSO_FAILURE, jso_builder_object_add_object_start(&builder, "b6"));
	assert_int_equal(JSO_FAILURE, jso_builder_object_add_array_start(&builder, "b7"));
	assert_int_equal(JSO_FAILURE, jso_builder_object_end(&builder));

	assert_int_equal(JSO_SUCCESS, jso_builder_array_end(&builder));
	assert_int_equal(JSO_SUCCESS, jso_builder_object_end(&builder));
	assert_int_equal(JSO_SUCCESS, jso_builder_object_end(&builder));
	// already in root so it should fail
	assert_int_equal(JSO_FAILURE, jso_builder_object_end(&builder));

	jso_value *value = jso_builder_get_value(&builder);
	assert_non_null(value);
	assert_int_equal(JSO_TYPE_OBJECT, JSO_TYPE_P(value));
	jso_object *obj = JSO_OBJVAL_P(value);
	jso_ht *ht = JSO_OBJECT_HT(obj);
	assert_int_equal(7, JSO_OBJECT_COUNT(obj));

	jso_value *item;

	assert_int_equal(JSO_SUCCESS, jso_ht_get_by_cstr_key(ht, "bt", &item));
	assert_int_equal(JSO_TYPE_BOOL, JSO_TYPE_P(item));
	assert_true(JSO_IVAL_P(item));

	assert_int_equal(JSO_SUCCESS, jso_ht_get_by_cstr_key(ht, "bf", &item));
	assert_int_equal(JSO_TYPE_BOOL, JSO_TYPE_P(item));
	assert_false(JSO_IVAL_P(item));

	assert_int_equal(JSO_SUCCESS, jso_ht_get_by_cstr_key(ht, "ival", &item));
	assert_int_equal(JSO_TYPE_INT, JSO_TYPE_P(item));
	assert_int_equal(5, JSO_IVAL_P(item));

	assert_int_equal(JSO_SUCCESS, jso_ht_get_by_cstr_key(ht, "dbl", &item));
	assert_int_equal(JSO_TYPE_DOUBLE, JSO_TYPE_P(item));
	assert_int_equal(3.2, JSO_DVAL_P(item));

	assert_int_equal(JSO_SUCCESS, jso_ht_get_by_cstr_key(ht, "cstr", &item));
	assert_int_equal(JSO_TYPE_STRING, JSO_TYPE_P(item));
	assert_string_equal("test", JSO_SVAL_P(item));

	assert_int_equal(JSO_SUCCESS, jso_ht_get_by_cstr_key(ht, "str", &item));
	assert_int_equal(JSO_TYPE_STRING, JSO_TYPE_P(item));
	assert_ptr_equal(str, JSO_STR_P(item));

	assert_int_equal(JSO_SUCCESS, jso_ht_get_by_cstr_key(ht, "obj", &item));
	assert_int_equal(JSO_TYPE_OBJECT, JSO_TYPE_P(item));
	jso_value *inner_item;
	jso_object *inner_obj = JSO_OBJVAL_P(item);
	jso_ht *inner_ht = JSO_OBJECT_HT(inner_obj);
	assert_int_equal(2, JSO_OBJECT_COUNT(inner_obj));
	assert_int_equal(JSO_SUCCESS, jso_ht_get_by_cstr_key(inner_ht, "oi", &inner_item));
	assert_int_equal(JSO_TYPE_INT, JSO_TYPE_P(inner_item));
	assert_int_equal(10, JSO_IVAL_P(inner_item));
	assert_int_equal(JSO_SUCCESS, jso_ht_get_by_cstr_key(inner_ht, "oa", &inner_item));
	assert_int_equal(JSO_TYPE_ARRAY, JSO_TYPE_P(inner_item));
	assert_int_equal(0, JSO_ARRAY_LEN(JSO_ARRVAL_P(inner_item)));

	jso_builder_clear_all(&builder);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_builder_array),
		cmocka_unit_test(test_jso_builder_object),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
