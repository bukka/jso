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

#include "../../../src/schema/jso_schema_keyword_object.h"
#include "../../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* Wrapping. */

/* Wrapper for jso_object_add. */
jso_rc __wrap_jso_object_add(jso_object *obj, jso_string *key, jso_value *val)
{
	function_called();
	check_expected_ptr(obj);
	check_expected_ptr(key);

	jso_schema_value *schema_value = mock_ptr_type(jso_schema_value *);
	assert_ptr_equal(schema_value, JSO_SVVAL_P(val));

	return mock_type(jso_rc);
}

/* Wrapper for jso_object_alloc. */
jso_object *__wrap_jso_object_alloc()
{
	function_called();

	return mock_ptr_type(jso_object *);
}

/* Wrapper for jso_object_free. */
void __wrap_jso_object_free(jso_object *obj)
{
	function_called();
	check_expected_ptr(obj);
}

/* Wrapper for jso_object_add. */
jso_rc __wrap_jso_object_resize(jso_object *obj, size_t size)
{
	function_called();
	check_expected_ptr(obj);
	check_expected(size);

	return mock_type(jso_rc);
}

/* Wrapper for jso_schema_data_get. */
jso_value *__wrap_jso_schema_data_get(jso_schema *schema, jso_value *data, const char *key,
		jso_value_type type, jso_uint32 keyword_flags, jso_bool error_on_invalid_type,
		jso_value *val)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected(key);
	check_expected(type);
	check_expected(keyword_flags);
	check_expected(error_on_invalid_type);
	check_expected_ptr(val);

	return mock_ptr_type(jso_value *);
}

/* Wrapper for jso_schema_keyword_get_regexp_code. */
jso_re_code *__wrap_jso_schema_keyword_get_regexp_code(
		jso_schema *schema, const char *keyword_key, jso_string *object_key, jso_value *val)
{
	function_called();
	check_expected_ptr(schema);
	check_expected(keyword_key);
	check_expected_ptr(object_key);
	check_expected_ptr(val);

	return mock_ptr_type(jso_re_code *);
}

/* Wrapper for jso_schema_keyword_validate_array_of_strings. */
jso_rc __wrap_jso_schema_keyword_validate_array_of_strings(
		jso_schema *schema, const char *key, jso_array *arr, jso_uint32 keyword_flags)
{
	function_called();
	check_expected_ptr(schema);
	check_expected(key);
	check_expected_ptr(arr);
	check_expected(keyword_flags);

	return mock_type(jso_rc);
}

/* Wrapper for jso_schema_value_free. */
extern void __real_jso_schema_value_free(jso_schema_value *val);
void __wrap_jso_schema_value_free(jso_schema_value *val)
{
	function_called();
	check_expected_ptr(val);
	__real_jso_schema_value_free(val);
}

/* Wrapper for jso_schema_value_parse. */
jso_schema_value *__wrap_jso_schema_value_parse(
		jso_schema *schema, jso_value *data, jso_schema_value *parent)
{
	function_called();
	check_expected_ptr(schema);
	check_expected_ptr(data);
	check_expected_ptr(parent);

	return mock_ptr_type(jso_schema_value *);
}

/* Helper funcitons */
static void jso_test_clear_schema(jso_schema *schema)
{
	expect_function_call(__wrap_jso_schema_value_free);
	expect_value(__wrap_jso_schema_value_free, val, schema->root);
	jso_schema_clear(schema);
}

/* Tests for jso_schema_keyword_get_object. */

/* Test getting of object if all good. */
static void test_jso_schema_keyword_get_object_when_value_ok(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_object object;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);
	jso_object_init(&object);

	JSO_VALUE_SET_OBJECT(val, &object);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "okey");
	expect_value(__wrap_jso_schema_data_get, type, JSO_TYPE_OBJECT);
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_TRUE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, &val);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_object(
			&schema, &data, "okey", JSO_TRUE, 0, &keyword, &val, &parent);

	assert_non_null(schema_keyword);
	assert_true(JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) & JSO_SCHEMA_KEYWORD_FLAG_PRESENT);
	assert_ptr_equal(&object, JSO_SCHEMA_KEYWORD_DATA_OBJ_P(schema_keyword));
	assert_int_equal(JSO_OBJECT_REFCOUNT(&object), 1);

	jso_test_clear_schema(&schema);
}

/* Test getting of object if no data. */
static void test_jso_schema_keyword_get_object_when_value_not_found(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_object object;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);
	jso_object_init(&object);

	JSO_VALUE_SET_OBJECT(val, &object);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "okey");
	expect_value(__wrap_jso_schema_data_get, type, JSO_TYPE_OBJECT);
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_FALSE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, NULL);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_object(
			&schema, &data, "okey", JSO_FALSE, 0, &keyword, &val, &parent);

	assert_null(schema_keyword);

	jso_test_clear_schema(&schema);
}

/* Tests for jso_schema_keyword_get_schema_object. */

/* Test getting of schema object if all good. */
static void test_jso_schema_keyword_get_schema_object_when_value_ok(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_object object;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;
	jso_schema_value schema_value;

	jso_schema_init(&schema);
	jso_object_init(&object);

	JSO_VALUE_SET_OBJECT(val, &object);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "sokey");
	expect_value(__wrap_jso_schema_data_get, type, JSO_TYPE_OBJECT);
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_TRUE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, &val);

	expect_function_call(__wrap_jso_schema_value_parse);
	expect_value(__wrap_jso_schema_value_parse, schema, &schema);
	expect_value(__wrap_jso_schema_value_parse, data, &val);
	expect_value(__wrap_jso_schema_value_parse, parent, &parent);
	will_return(__wrap_jso_schema_value_parse, &schema_value);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_schema_object(
			&schema, &data, "sokey", JSO_TRUE, 0, &keyword, &val, &parent);

	assert_non_null(schema_keyword);
	assert_true(JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) & JSO_SCHEMA_KEYWORD_FLAG_PRESENT);
	assert_ptr_equal(&schema_value, JSO_SCHEMA_KEYWORD_DATA_SCHEMA_OBJ_P(schema_keyword));

	jso_test_clear_schema(&schema);
}

/* Test getting of schema object if all good. */
static void test_jso_schema_keyword_get_schema_object_when_parsing_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_object object;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);
	jso_object_init(&object);

	JSO_VALUE_SET_OBJECT(val, &object);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "sokey");
	expect_value(__wrap_jso_schema_data_get, type, JSO_TYPE_OBJECT);
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_TRUE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, &val);

	expect_function_call(__wrap_jso_schema_value_parse);
	expect_value(__wrap_jso_schema_value_parse, schema, &schema);
	expect_value(__wrap_jso_schema_value_parse, data, &val);
	expect_value(__wrap_jso_schema_value_parse, parent, &parent);
	will_return(__wrap_jso_schema_value_parse, NULL);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_schema_object(
			&schema, &data, "sokey", JSO_TRUE, 0, &keyword, &val, &parent);

	assert_null(schema_keyword);

	jso_test_clear_schema(&schema);
}

/* Test getting of schema object if no data. */
static void test_jso_schema_keyword_get_schema_object_when_value_not_found(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_object object;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);
	jso_object_init(&object);

	JSO_VALUE_SET_OBJECT(val, &object);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "sokey");
	expect_value(__wrap_jso_schema_data_get, type, JSO_TYPE_OBJECT);
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_FALSE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, NULL);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_schema_object(
			&schema, &data, "sokey", JSO_FALSE, 0, &keyword, &val, &parent);

	assert_null(schema_keyword);

	jso_test_clear_schema(&schema);
}

/* Tests for jso_schema_keyword_get_object_of_schema_objects. */

/* Test getting of object of schema objects if all good. */
static void test_jso_schema_keyword_get_object_of_schema_objects_when_all_ok(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_string *objkey;
	jso_object object, schema_obj, sobj1, sobj2;
	jso_value data, val, ov1, ov2, *item;
	jso_schema_value parent, sov[2];
	jso_schema_keyword keyword;

	jso_schema_init(&schema);
	jso_object_init(&object);
	jso_object_init(&sobj1);
	jso_object_init(&sobj2);

	JSO_VALUE_SET_OBJECT(ov1, &sobj1);
	JSO_VALUE_SET_OBJECT(ov2, &sobj2);

	jso_string *kov1 = jso_string_create_from_cstr("o1");
	jso_string *kov2 = jso_string_create_from_cstr("o2");

	// hash table needs to be set as jso_object_add is mocked
	jso_ht_set(JSO_OBJECT_HT(&object), kov1, &ov1, true);
	jso_ht_set(JSO_OBJECT_HT(&object), kov2, &ov2, true);

	JSO_VALUE_SET_OBJECT(val, &object);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "sokey");
	expect_value(__wrap_jso_schema_data_get, type, JSO_TYPE_OBJECT);
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_TRUE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, &val);

	expect_function_call(__wrap_jso_object_alloc);
	will_return(__wrap_jso_object_alloc, &schema_obj);

	expect_function_call(__wrap_jso_object_resize);
	expect_value(__wrap_jso_object_resize, obj, &schema_obj);
	expect_value(__wrap_jso_object_resize, size, 2);
	will_return(__wrap_jso_object_resize, JSO_SUCCESS);

	int i = 0;
	JSO_OBJECT_FOREACH(&object, objkey, item)
	{
		expect_function_call(__wrap_jso_schema_value_parse);
		expect_value(__wrap_jso_schema_value_parse, schema, &schema);
		expect_value(__wrap_jso_schema_value_parse, data, item);
		expect_value(__wrap_jso_schema_value_parse, parent, &parent);
		will_return(__wrap_jso_schema_value_parse, &sov[i]);

		expect_function_call(__wrap_jso_object_add);
		expect_value(__wrap_jso_object_add, obj, &schema_obj);
		expect_value(__wrap_jso_object_add, key, objkey);
		will_return(__wrap_jso_object_add, &sov[i]);
		will_return(__wrap_jso_object_add, JSO_SUCCESS);

		i++;
	}
	JSO_OBJECT_FOREACH_END;

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_object_of_schema_objects(
			&schema, &data, "sokey", JSO_TRUE, 0, &keyword, &val, &parent);

	assert_non_null(schema_keyword);
	assert_true(JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) & JSO_SCHEMA_KEYWORD_FLAG_PRESENT);
	assert_ptr_equal(&schema_obj, JSO_SCHEMA_KEYWORD_DATA_OBJ_SCHEMA_OBJ_P(schema_keyword));

	jso_string_free(kov1);
	jso_string_free(kov2);
	jso_test_clear_schema(&schema);
}

/* Test getting of object of schema objects if parsing value fails. */
static void test_jso_schema_keyword_get_object_of_schema_objects_when_parse_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_string *objkey;
	jso_object object, schema_obj, sobj1, sobj2;
	jso_value data, val, ov1, ov2, *item;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);
	jso_object_init(&object);
	jso_object_init(&sobj1);
	jso_object_init(&sobj2);

	JSO_VALUE_SET_OBJECT(ov1, &sobj1);
	JSO_VALUE_SET_OBJECT(ov2, &sobj2);

	jso_string *kov1 = jso_string_create_from_cstr("o1");
	jso_string *kov2 = jso_string_create_from_cstr("o2");

	// hash table needs to be set as jso_object_add is mocked
	jso_ht_set(JSO_OBJECT_HT(&object), kov1, &ov1, true);
	jso_ht_set(JSO_OBJECT_HT(&object), kov2, &ov2, true);

	JSO_VALUE_SET_OBJECT(val, &object);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "sokey");
	expect_value(__wrap_jso_schema_data_get, type, JSO_TYPE_OBJECT);
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_TRUE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, &val);

	expect_function_call(__wrap_jso_object_alloc);
	will_return(__wrap_jso_object_alloc, &schema_obj);

	expect_function_call(__wrap_jso_object_resize);
	expect_value(__wrap_jso_object_resize, obj, &schema_obj);
	expect_value(__wrap_jso_object_resize, size, 2);
	will_return(__wrap_jso_object_resize, JSO_SUCCESS);

	JSO_OBJECT_FOREACH(&object, objkey, item)
	{
		(void) objkey;
		expect_function_call(__wrap_jso_schema_value_parse);
		expect_value(__wrap_jso_schema_value_parse, schema, &schema);
		expect_value(__wrap_jso_schema_value_parse, data, item);
		expect_value(__wrap_jso_schema_value_parse, parent, &parent);
		will_return(__wrap_jso_schema_value_parse, NULL);
		break;
	}
	JSO_OBJECT_FOREACH_END;

	expect_function_call(__wrap_jso_object_free);
	expect_value(__wrap_jso_object_free, obj, &schema_obj);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_object_of_schema_objects(
			&schema, &data, "sokey", JSO_TRUE, 0, &keyword, &val, &parent);

	assert_null(schema_keyword);

	jso_string_free(kov1);
	jso_string_free(kov2);
	jso_test_clear_schema(&schema);
}

/* Test getting of object of schema objects if parsing value fails. */
static void test_jso_schema_keyword_get_object_of_schema_objects_when_item_not_object(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_string *objkey;
	jso_object object, schema_obj, sobj1;
	jso_value data, val, ov1, ov2, *item;
	jso_schema_value parent, sov;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);
	jso_object_init(&object);
	jso_object_init(&sobj1);

	jso_string *item2 = jso_string_create_from_cstr("item");

	JSO_VALUE_SET_OBJECT(ov1, &sobj1);
	JSO_VALUE_SET_STRING(ov2, item2);

	jso_string *kov1 = jso_string_create_from_cstr("o1");
	jso_string *kov2 = jso_string_create_from_cstr("o2");

	// hash table needs to be set as jso_object_add is mocked
	jso_ht_set(JSO_OBJECT_HT(&object), kov1, &ov1, true);
	jso_ht_set(JSO_OBJECT_HT(&object), kov2, &ov2, true);

	JSO_VALUE_SET_OBJECT(val, &object);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "sokey");
	expect_value(__wrap_jso_schema_data_get, type, JSO_TYPE_OBJECT);
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_TRUE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, &val);

	expect_function_call(__wrap_jso_object_alloc);
	will_return(__wrap_jso_object_alloc, &schema_obj);

	expect_function_call(__wrap_jso_object_resize);
	expect_value(__wrap_jso_object_resize, obj, &schema_obj);
	expect_value(__wrap_jso_object_resize, size, 2);
	will_return(__wrap_jso_object_resize, JSO_SUCCESS);

	JSO_OBJECT_FOREACH(&object, objkey, item)
	{
		expect_function_call(__wrap_jso_schema_value_parse);
		expect_value(__wrap_jso_schema_value_parse, schema, &schema);
		expect_value(__wrap_jso_schema_value_parse, data, item);
		expect_value(__wrap_jso_schema_value_parse, parent, &parent);
		will_return(__wrap_jso_schema_value_parse, &sov);

		expect_function_call(__wrap_jso_object_add);
		expect_value(__wrap_jso_object_add, obj, &schema_obj);
		expect_value(__wrap_jso_object_add, key, objkey);
		will_return(__wrap_jso_object_add, &sov);
		will_return(__wrap_jso_object_add, JSO_SUCCESS);

		break;
	}
	JSO_OBJECT_FOREACH_END;

	expect_function_call(__wrap_jso_object_free);
	expect_value(__wrap_jso_object_free, obj, &schema_obj);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_object_of_schema_objects(
			&schema, &data, "sokey", JSO_TRUE, 0, &keyword, &val, &parent);

	assert_null(schema_keyword);
	assert_int_equal(JSO_SCHEMA_ERROR_VALUE_DATA_TYPE, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal("Object value for keyword sokey must be a schema object",
			JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_string_free(kov1);
	jso_string_free(kov2);
	jso_test_clear_schema(&schema);
}

/* Test getting of object of schema objects if resizing object fails. */
static void test_jso_schema_keyword_get_object_of_schema_objects_when_resize_fails(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_object object, schema_obj, sobj1, sobj2;
	jso_value data, val, ov1, ov2;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);
	jso_object_init(&object);
	jso_object_init(&sobj1);
	jso_object_init(&sobj2);

	JSO_VALUE_SET_OBJECT(ov1, &sobj1);
	JSO_VALUE_SET_OBJECT(ov2, &sobj2);

	jso_string *kov1 = jso_string_create_from_cstr("o1");
	jso_string *kov2 = jso_string_create_from_cstr("o2");

	// hash table needs to be set as jso_object_add is mocked
	jso_ht_set(JSO_OBJECT_HT(&object), kov1, &ov1, true);
	jso_ht_set(JSO_OBJECT_HT(&object), kov2, &ov2, true);

	JSO_VALUE_SET_OBJECT(val, &object);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "sokey");
	expect_value(__wrap_jso_schema_data_get, type, JSO_TYPE_OBJECT);
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_TRUE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, &val);

	expect_function_call(__wrap_jso_object_alloc);
	will_return(__wrap_jso_object_alloc, &schema_obj);

	expect_function_call(__wrap_jso_object_resize);
	expect_value(__wrap_jso_object_resize, obj, &schema_obj);
	expect_value(__wrap_jso_object_resize, size, 2);
	will_return(__wrap_jso_object_resize, JSO_FAILURE);

	expect_function_call(__wrap_jso_object_free);
	expect_value(__wrap_jso_object_free, obj, &schema_obj);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_object_of_schema_objects(
			&schema, &data, "sokey", JSO_TRUE, 0, &keyword, &val, &parent);

	assert_null(schema_keyword);
	assert_int_equal(JSO_SCHEMA_ERROR_KEYWORD_ALLOC, JSO_SCHEMA_ERROR_TYPE(&schema));
	assert_string_equal(
			"Allocating object for keyword sokey failed", JSO_SCHEMA_ERROR_MESSAGE(&schema));

	jso_string_free(kov1);
	jso_string_free(kov2);
	jso_test_clear_schema(&schema);
}

/* Test getting of object of schema objects if no data. */
static void test_jso_schema_keyword_get_object_of_schema_objects_when_data_not_found(void **state)
{
	(void) state; /* unused */

	jso_schema schema;
	jso_object object;
	jso_value data, val;
	jso_schema_value parent;
	jso_schema_keyword keyword;

	jso_schema_init(&schema);
	jso_object_init(&object);

	JSO_VALUE_SET_OBJECT(val, &object);

	expect_function_call(__wrap_jso_schema_data_get);
	expect_value(__wrap_jso_schema_data_get, schema, &schema);
	expect_value(__wrap_jso_schema_data_get, data, &data);
	expect_string(__wrap_jso_schema_data_get, key, "sokey");
	expect_value(__wrap_jso_schema_data_get, type, JSO_TYPE_OBJECT);
	expect_value(__wrap_jso_schema_data_get, keyword_flags, 0);
	expect_value(__wrap_jso_schema_data_get, error_on_invalid_type, JSO_FALSE);
	expect_value(__wrap_jso_schema_data_get, val, &val);
	will_return(__wrap_jso_schema_data_get, NULL);

	jso_schema_keyword *schema_keyword = jso_schema_keyword_get_object_of_schema_objects(
			&schema, &data, "sokey", JSO_FALSE, 0, &keyword, &val, &parent);

	assert_null(schema_keyword);

	jso_test_clear_schema(&schema);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_schema_keyword_get_object_when_value_ok),
		cmocka_unit_test(test_jso_schema_keyword_get_object_when_value_not_found),
		cmocka_unit_test(test_jso_schema_keyword_get_schema_object_when_value_ok),
		cmocka_unit_test(test_jso_schema_keyword_get_schema_object_when_parsing_fails),
		cmocka_unit_test(test_jso_schema_keyword_get_schema_object_when_value_not_found),
		cmocka_unit_test(test_jso_schema_keyword_get_object_of_schema_objects_when_all_ok),
		cmocka_unit_test(test_jso_schema_keyword_get_object_of_schema_objects_when_parse_fails),
		cmocka_unit_test(test_jso_schema_keyword_get_object_of_schema_objects_when_resize_fails),
		cmocka_unit_test(test_jso_schema_keyword_get_object_of_schema_objects_when_data_not_found),
		cmocka_unit_test(test_jso_schema_keyword_get_object_of_schema_objects_when_item_not_object),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
		// cmocka_unit_test(),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
