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

#include "../../../src/schema/jso_schema_data.h"
#include "../../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* tests for jso_schema_data_type_error */

static void test_jso_schema_data_type_error_with_no_type(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_schema_init(&schema);

	jso_value val;
	JSO_VALUE_SET_INT(val, 1);

	const jso_value_type types[] = { JSO_TYPE_BOOL };

	jso_schema_data_type_error(&schema, "tkey", &val, types, 0);

	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_TYPE, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("Invalid type for tkey - expected no type but given int",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
}

static void test_jso_schema_data_type_error_with_one_type(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_schema_init(&schema);

	jso_value val;
	JSO_VALUE_SET_INT(val, 1);

	const jso_value_type types[] = { JSO_TYPE_BOOL };

	jso_schema_data_type_error(&schema, "tkey", &val, types, 1);

	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_TYPE, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("Invalid type for tkey - expected bool but given int",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
}

static void test_jso_schema_data_type_error_with_two_types(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_schema_init(&schema);

	jso_value val;
	JSO_VALUE_SET_INT(val, 1);

	const jso_value_type types[] = { JSO_TYPE_BOOL, JSO_TYPE_DOUBLE };

	jso_schema_data_type_error(&schema, "tkey", &val, types, 2);

	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_TYPE, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("Invalid type for tkey - expected either bool or double but given int",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
}

static void test_jso_schema_data_type_error_with_three_types(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_schema_init(&schema);

	jso_value val;
	JSO_VALUE_SET_INT(val, 1);

	const jso_value_type types[] = { JSO_TYPE_BOOL, JSO_TYPE_ARRAY, JSO_TYPE_OBJECT };

	jso_schema_data_type_error(&schema, "tkey", &val, types, 3);

	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_TYPE, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal(
			"Invalid type for tkey - expected either bool, array or object but given int",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_schema_clear(&schema);
}

/* tests for jso_schema_data_get_value_fast */

jso_rc __wrap_jso_ht_get_by_cstr_key(jso_ht *ht, const char *key, jso_value **value)
{
	check_expected_ptr(ht);
	check_expected(key);

	jso_rc rc = mock_type(jso_rc);
	if (rc == JSO_SUCCESS) {
		jso_value *val = mock_ptr_type(jso_value *);
		*value = val;
	}
	return rc;
}

static void test_jso_schema_data_get_value_fast_if_found(void **state)
{
	jso_schema schema;
	jso_schema_init(&schema);

	jso_object *obj = jso_object_alloc();
	jso_value data;
	JSO_VALUE_SET_OBJECT(data, obj);

	jso_value val;
	JSO_VALUE_SET_INT(val, 1);

	expect_value(__wrap_jso_ht_get_by_cstr_key, ht, &obj->ht);
	expect_string(__wrap_jso_ht_get_by_cstr_key, key, "hkey");

	will_return(__wrap_jso_ht_get_by_cstr_key, JSO_SUCCESS);
	will_return(__wrap_jso_ht_get_by_cstr_key, &val);

	jso_value *rv = jso_schema_data_get_value_fast(&schema, &data, "hkey", 0);

	assert_true(jso_value_equals(rv, &val));

	jso_object_free(obj);
	jso_schema_clear(&schema);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_schema_data_type_error_with_no_type),
		cmocka_unit_test(test_jso_schema_data_type_error_with_one_type),
		cmocka_unit_test(test_jso_schema_data_type_error_with_two_types),
		cmocka_unit_test(test_jso_schema_data_type_error_with_three_types),
		cmocka_unit_test(test_jso_schema_data_get_value_fast_if_found),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
