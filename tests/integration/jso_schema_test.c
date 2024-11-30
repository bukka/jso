/*
 * Copyright (c) 2024 Jakub Zelenka. All rights reserved.
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
#include "../../src/jso_schema.h"
#include "../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>

#define assert_jso_schema_result_success(_call) \
	{ \
		jso_rc rc = _call; \
		if (rc == JSO_FAILURE) { \
			fprintf(stderr, "[  ERROR   ] Schema error message: %s\n", \
					JSO_SCHEMA_ERROR_MESSAGE(&schema)); \
		} \
		assert_int_equal(JSO_SUCCESS, rc); \
	} \
	assert_int_equal(JSO_SCHEMA_ERROR_NONE, JSO_SCHEMA_ERROR_TYPE(&schema))

#define assert_jso_schema_validation_success(_call) \
	result = _call; \
	if (result != JSO_SCHEMA_VALIDATION_VALID) { \
		fprintf(stderr, "[  ERROR   ] Schema error message: %s\n", \
				JSO_SCHEMA_ERROR_MESSAGE(&schema)); \
	} \
	assert_int_equal(JSO_SCHEMA_VALIDATION_VALID, result); \
	assert_int_equal(JSO_SCHEMA_ERROR_NONE, JSO_SCHEMA_ERROR_TYPE(&schema))

#define assert_jso_schema_validation_failure(_call) \
	result = _call; \
	if (result == JSO_SCHEMA_VALIDATION_ERROR) { \
		fprintf(stderr, "[  ERROR   ] Schema error message: %s\n", \
				JSO_SCHEMA_ERROR_MESSAGE(&schema)); \
	} else if (result == JSO_SCHEMA_VALIDATION_VALID) { \
		fprintf(stderr, "[  ERROR   ] Schema validation valid but expected invalid\n"); \
	} \
	assert_int_equal(JSO_SCHEMA_VALIDATION_INVALID, result); \
	assert_true(jso_schema_error_is_validation(&schema))

/* A test for simple boolean value. */
static void test_jso_schema_boolean(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "boolean");
	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_value instance;

	JSO_VALUE_SET_BOOL(instance, true);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));

	JSO_VALUE_SET_INT(instance, 12);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));

	jso_value_clear(&instance);
	jso_schema_clear(&schema);
}

/* A test for a simple string value limited with max and min length. */
static void test_jso_schema_string_with_lengths(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "string");
	jso_builder_object_add_int(&builder, "minLength", 4);
	jso_builder_object_add_int(&builder, "maxLength", 6);

	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_value instance;
	jso_string *sv;

	sv = jso_string_create_from_cstr("valid");
	JSO_VALUE_SET_STRING(instance, sv);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	sv = jso_string_create_from_cstr("too long");
	JSO_VALUE_SET_STRING(instance, sv);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	sv = jso_string_create_from_cstr("sho");
	JSO_VALUE_SET_STRING(instance, sv);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	JSO_VALUE_SET_INT(instance, 5);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	jso_schema_clear(&schema);
}

/* A test for a simple string value with regular expression pattern. */
static void test_jso_schema_string_with_pattern(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "string");
	jso_builder_object_add_cstr(&builder, "pattern", "^(\\([0-9]{3}\\))?[0-9]{3}-[0-9]{4}$");

	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_value instance;
	jso_string *sv;

	sv = jso_string_create_from_cstr("555-1212");
	JSO_VALUE_SET_STRING(instance, sv);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	sv = jso_string_create_from_cstr("(888)555-1212");
	JSO_VALUE_SET_STRING(instance, sv);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	sv = jso_string_create_from_cstr("(888)555-1212 ext. 532");
	JSO_VALUE_SET_STRING(instance, sv);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	sv = jso_string_create_from_cstr("(800)FLOWERS");
	JSO_VALUE_SET_STRING(instance, sv);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	jso_schema_clear(&schema);
}

/* A test for a simple integer type. */
static void test_jso_schema_integer(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "integer");

	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_value instance;
	jso_string *sv;

	JSO_VALUE_SET_INT(instance, 14);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	JSO_VALUE_SET_DOUBLE(instance, 14.0);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	JSO_VALUE_SET_DOUBLE(instance, 14.2);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	JSO_VALUE_SET_BOOL(instance, true);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	sv = jso_string_create_from_cstr("200");
	JSO_VALUE_SET_STRING(instance, sv);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	jso_schema_clear(&schema);
}

/* A test for a simple number type with multipleOf. */
static void test_jso_schema_number_multiple(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "number");
	jso_builder_object_add_int(&builder, "multipleOf", 10);

	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_value instance;

	JSO_VALUE_SET_INT(instance, 0);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	JSO_VALUE_SET_INT(instance, 10);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	JSO_VALUE_SET_INT(instance, 20);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	JSO_VALUE_SET_INT(instance, 23);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	JSO_VALUE_SET_BOOL(instance, true);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	jso_schema_clear(&schema);
}

/* A test for a simple number type with range. */
static void test_jso_schema_number_range(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "number");
	jso_builder_object_add_int(&builder, "minimum", 0);
	jso_builder_object_add_int(&builder, "maximum", 100);
	jso_builder_object_add_bool(&builder, "exclusiveMaximum", true);

	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_value instance;

	JSO_VALUE_SET_INT(instance, 0);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	JSO_VALUE_SET_INT(instance, 10);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	JSO_VALUE_SET_INT(instance, 99);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	JSO_VALUE_SET_INT(instance, 100);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	JSO_VALUE_SET_INT(instance, -1);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	jso_schema_clear(&schema);
}

/* A test for a simple null type. */
static void test_jso_schema_null(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "null");

	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_value instance;

	JSO_VALUE_SET_NULL(instance);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	JSO_VALUE_SET_INT(instance, 100);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	JSO_VALUE_SET_BOOL(instance, true);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	jso_schema_clear(&schema);
}

/* A test for an object type with properties. */
static void test_jso_schema_object_props(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "object");
	// properties
	jso_builder_object_add_object_start(&builder, "properties");
	// number property
	jso_builder_object_add_object_start(&builder, "number");
	jso_builder_object_add_cstr(&builder, "type", "number");
	jso_builder_object_end(&builder);
	// street_name property
	jso_builder_object_add_object_start(&builder, "street_name");
	jso_builder_object_add_cstr(&builder, "type", "string");
	jso_builder_object_end(&builder);
	// end properties and root object
	jso_builder_object_end(&builder);
	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_value instance;

	// valid
	jso_builder_object_start(&builder);
	jso_builder_object_add_int(&builder, "number", 10);
	jso_builder_object_add_cstr(&builder, "street_name", "Pennsylvania");
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid with extra property
	jso_builder_object_start(&builder);
	jso_builder_object_add_int(&builder, "number", 10);
	jso_builder_object_add_cstr(&builder, "street_name", "Pennsylvania");
	jso_builder_object_add_cstr(&builder, "city", "Prague");
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// invalid number type
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "number", "10");
	jso_builder_object_add_cstr(&builder, "street_name", "Pennsylvania");
	assert_jso_schema_validation_failure(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// not an object
	JSO_VALUE_SET_INT(instance, 100);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	// not an object
	JSO_VALUE_SET_BOOL(instance, true);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	jso_schema_clear(&schema);
}

/* A test for an object type with pattern properties. */
static void test_jso_schema_object_pattern_props(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "object");
	// properties
	jso_builder_object_add_object_start(&builder, "patternProperties");
	// number property
	jso_builder_object_add_object_start(&builder, "^S_");
	jso_builder_object_add_cstr(&builder, "type", "string");
	jso_builder_object_end(&builder);
	// street_name property
	jso_builder_object_add_object_start(&builder, "^I_");
	jso_builder_object_add_cstr(&builder, "type", "integer");
	jso_builder_object_end(&builder);
	// end properties and root object
	jso_builder_object_end(&builder);
	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_builder_object_start(&builder);
	jso_builder_object_add_int(&builder, "I_val", 10);
	jso_builder_object_add_cstr(&builder, "S_val", "Pennsylvania");
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "I_val", "10");
	jso_builder_object_add_cstr(&builder, "S_val", "Pennsylvania");
	assert_jso_schema_validation_failure(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_schema_clear(&schema);
}

/* A test for an object type with additional properties with false value. */
static void test_jso_schema_object_additional_props_false(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "object");
	// properties
	jso_builder_object_add_object_start(&builder, "properties");
	// number property
	jso_builder_object_add_object_start(&builder, "number");
	jso_builder_object_add_cstr(&builder, "type", "number");
	jso_builder_object_end(&builder);
	// street_name property
	jso_builder_object_add_object_start(&builder, "street_name");
	jso_builder_object_add_cstr(&builder, "type", "string");
	jso_builder_object_end(&builder);
	// end properties
	jso_builder_object_end(&builder);
	// additional properties
	jso_builder_object_add_bool(&builder, "additionalProperties", false);
	// end root
	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid
	jso_builder_object_start(&builder);
	jso_builder_object_add_int(&builder, "number", 10);
	jso_builder_object_add_cstr(&builder, "street_name", "Pennsylvania");
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// invalid with extra property
	jso_builder_object_start(&builder);
	jso_builder_object_add_int(&builder, "number", 10);
	jso_builder_object_add_cstr(&builder, "street_name", "Pennsylvania");
	jso_builder_object_add_cstr(&builder, "city", "Prague");
	assert_jso_schema_validation_failure(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_schema_clear(&schema);
}

/* A test for an object type with additional properties with type. */
static void test_jso_schema_object_additional_props_type(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "object");
	// properties
	jso_builder_object_add_object_start(&builder, "properties");
	// number property
	jso_builder_object_add_object_start(&builder, "number");
	jso_builder_object_add_cstr(&builder, "type", "number");
	jso_builder_object_end(&builder);
	// street_name property
	jso_builder_object_add_object_start(&builder, "street_name");
	jso_builder_object_add_cstr(&builder, "type", "string");
	jso_builder_object_end(&builder);
	// end properties
	jso_builder_object_end(&builder);
	// additional properties
	jso_builder_object_add_object_start(&builder, "additionalProperties");
	jso_builder_object_add_cstr(&builder, "type", "string");
	jso_builder_object_end(&builder);
	// end root
	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid
	jso_builder_object_start(&builder);
	jso_builder_object_add_int(&builder, "number", 10);
	jso_builder_object_add_cstr(&builder, "street_name", "Pennsylvania");
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid with extra string property
	jso_builder_object_start(&builder);
	jso_builder_object_add_int(&builder, "number", 10);
	jso_builder_object_add_cstr(&builder, "street_name", "Pennsylvania");
	jso_builder_object_add_cstr(&builder, "city", "Prague");
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// invalid with extra property that is not string
	jso_builder_object_start(&builder);
	jso_builder_object_add_int(&builder, "number", 10);
	jso_builder_object_add_cstr(&builder, "street_name", "Pennsylvania");
	jso_builder_object_add_int(&builder, "age", 18);
	assert_jso_schema_validation_failure(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_schema_clear(&schema);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_schema_boolean),
		cmocka_unit_test(test_jso_schema_string_with_lengths),
		cmocka_unit_test(test_jso_schema_string_with_pattern),
		cmocka_unit_test(test_jso_schema_integer),
		cmocka_unit_test(test_jso_schema_number_multiple),
		cmocka_unit_test(test_jso_schema_number_range),
		cmocka_unit_test(test_jso_schema_null),
		cmocka_unit_test(test_jso_schema_object_props),
		cmocka_unit_test(test_jso_schema_object_pattern_props),
		cmocka_unit_test(test_jso_schema_object_additional_props_false),
		cmocka_unit_test(test_jso_schema_object_additional_props_type),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
