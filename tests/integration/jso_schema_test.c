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
#include "../../src/schema/jso_schema_error.h"
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
	assert_true(jso_schema_error_is_validation(&schema)); \
	jso_schema_error_free(&schema)

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

	// valid with no props
	jso_builder_object_start(&builder);
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid with all props
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
	// pattern properties
	jso_builder_object_add_object_start(&builder, "patternProperties");
	// string properties
	jso_builder_object_add_object_start(&builder, "^S_");
	jso_builder_object_add_cstr(&builder, "type", "string");
	jso_builder_object_end(&builder);
	// integer properties
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

/* A test for an object type with all properties with non overlapping types. */
static void test_jso_schema_object_all_props_non_overlap(void **state)
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
	jso_builder_object_add_object_start(&builder, "builtin");
	jso_builder_object_add_cstr(&builder, "type", "number");
	jso_builder_object_end(&builder);
	// end properties
	jso_builder_object_end(&builder);
	// pattern properties
	jso_builder_object_add_object_start(&builder, "patternProperties");
	// string properties
	jso_builder_object_add_object_start(&builder, "^S_");
	jso_builder_object_add_cstr(&builder, "type", "string");
	jso_builder_object_end(&builder);
	// integer properties
	jso_builder_object_add_object_start(&builder, "^I_");
	jso_builder_object_add_cstr(&builder, "type", "integer");
	jso_builder_object_end(&builder);
	// end pattern properties
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

	// valid with property
	jso_builder_object_start(&builder);
	jso_builder_object_add_int(&builder, "builtin", 10);
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid with pattern property
	jso_builder_object_start(&builder);
	jso_builder_object_add_int(&builder, "I_val", 10);
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid with extra string property
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "keyword", "value");
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// invalid with extra property that is not string
	jso_builder_object_start(&builder);
	jso_builder_object_add_int(&builder, "keyword", 10);
	assert_jso_schema_validation_failure(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_schema_clear(&schema);
}

/* A test for an object type with required properties. */
static void test_jso_schema_object_required_props(void **state)
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
	// name property
	jso_builder_object_add_object_start(&builder, "name");
	jso_builder_object_add_cstr(&builder, "type", "string");
	jso_builder_object_end(&builder);
	// email property
	jso_builder_object_add_object_start(&builder, "email");
	jso_builder_object_add_cstr(&builder, "type", "string");
	jso_builder_object_end(&builder);
	// address property
	jso_builder_object_add_object_start(&builder, "address");
	jso_builder_object_add_cstr(&builder, "type", "string");
	jso_builder_object_end(&builder);
	// telephone property
	jso_builder_object_add_object_start(&builder, "telephone");
	jso_builder_object_add_cstr(&builder, "type", "string");
	jso_builder_object_end(&builder);
	// end properties
	jso_builder_object_end(&builder);
	// required
	jso_builder_object_add_array_start(&builder, "required");
	jso_builder_array_add_cstr(&builder, "name");
	jso_builder_array_add_cstr(&builder, "email");
	jso_builder_array_end(&builder);
	// end root
	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid with all required properties
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "name", "William Shakespeare");
	jso_builder_object_add_cstr(&builder, "email", "bill@stratford-upon-avon.co.uk");
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid with all required and extra properties
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "name", "William Shakespeare");
	jso_builder_object_add_cstr(&builder, "email", "bill@stratford-upon-avon.co.uk");
	jso_builder_object_add_cstr(&builder, "address", "Henley Street, Stratford-upon-Avon");
	jso_builder_object_add_cstr(&builder, "authorship", "in question");
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// invalid with missing email
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "name", "William Shakespeare");
	jso_builder_object_add_cstr(&builder, "address", "Henley Street, Stratford-upon-Avon");
	assert_jso_schema_validation_failure(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// invalid with null email
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "name", "William Shakespeare");
	jso_builder_object_add_cstr(&builder, "address", "Henley Street, Stratford-upon-Avon");
	jso_builder_object_add_null(&builder, "email");
	assert_jso_schema_validation_failure(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_schema_clear(&schema);
}

/* A test for an object type with size requirements. */
static void test_jso_schema_object_size(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "object");
	jso_builder_object_add_int(&builder, "minProperties", 2);
	jso_builder_object_add_int(&builder, "maxProperties", 3);
	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// invalid with empty object
	jso_builder_object_start(&builder);
	assert_jso_schema_validation_failure(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// invalid with 1 property
	jso_builder_object_start(&builder);
	jso_builder_object_add_int(&builder, "a", 0);
	assert_jso_schema_validation_failure(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid with 2 properties
	jso_builder_object_start(&builder);
	jso_builder_object_add_int(&builder, "a", 0);
	jso_builder_object_add_int(&builder, "b", 1);
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// // valid with 3 properties
	jso_builder_object_start(&builder);
	jso_builder_object_add_int(&builder, "a", 0);
	jso_builder_object_add_int(&builder, "b", 1);
	jso_builder_object_add_int(&builder, "c", 2);
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// invalid with 4 properties
	jso_builder_object_start(&builder);
	jso_builder_object_add_int(&builder, "a", 0);
	jso_builder_object_add_int(&builder, "b", 1);
	jso_builder_object_add_int(&builder, "c", 2);
	jso_builder_object_add_int(&builder, "d", 3);
	assert_jso_schema_validation_failure(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_schema_clear(&schema);
}

/* A test for an array type with items. */
static void test_jso_schema_array_items(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "array");
	jso_builder_object_add_object_start(&builder, "items");
	jso_builder_object_add_cstr(&builder, "type", "number");
	jso_builder_object_end(&builder);
	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid empty array
	jso_builder_array_start(&builder);
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid with number items
	jso_builder_array_start(&builder);
	jso_builder_array_add_int(&builder, 0);
	jso_builder_array_add_int(&builder, 1);
	jso_builder_array_add_int(&builder, 2);
	jso_builder_array_add_int(&builder, 3);
	jso_builder_array_add_int(&builder, 4);
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// invalid with one string item between number items
	jso_builder_array_start(&builder);
	jso_builder_array_add_int(&builder, 0);
	jso_builder_array_add_int(&builder, 1);
	jso_builder_array_add_cstr(&builder, "2");
	jso_builder_array_add_int(&builder, 3);
	jso_builder_array_add_int(&builder, 4);
	assert_jso_schema_validation_failure(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// invalid with empty object
	jso_builder_object_start(&builder);
	assert_jso_schema_validation_failure(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_schema_clear(&schema);
}

/* A test for an array tuple - items with different types. */
static void test_jso_schema_array_tuple(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "array");
	jso_builder_object_add_array_start(&builder, "items");
	// first item number
	jso_builder_array_add_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "number");
	jso_builder_object_end(&builder);
	// second item string
	jso_builder_array_add_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "string");
	jso_builder_object_end(&builder);
	// third item boolean
	jso_builder_array_add_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "boolean");
	jso_builder_object_end(&builder);
	// end
	jso_builder_array_end(&builder);
	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid empty array
	jso_builder_array_start(&builder);
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid with all items
	jso_builder_array_start(&builder);
	jso_builder_array_add_int(&builder, 3);
	jso_builder_array_add_cstr(&builder, "street");
	jso_builder_array_add_bool(&builder, true);
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid with not all items
	jso_builder_array_start(&builder);
	jso_builder_array_add_int(&builder, 3);
	jso_builder_array_add_cstr(&builder, "street");
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// invalid with more items
	jso_builder_array_start(&builder);
	jso_builder_array_add_int(&builder, 3);
	jso_builder_array_add_cstr(&builder, "street");
	jso_builder_array_add_bool(&builder, true);
	jso_builder_array_add_cstr(&builder, "street");
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// invalid with last item type incorrect
	jso_builder_array_start(&builder);
	jso_builder_array_add_int(&builder, 3);
	jso_builder_array_add_cstr(&builder, "street");
	jso_builder_array_add_cstr(&builder, "street2");
	jso_builder_array_add_bool(&builder, true);
	assert_jso_schema_validation_failure(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// invalid with first item type incorrect
	jso_builder_array_start(&builder);
	jso_builder_array_add_cstr(&builder, "street");
	jso_builder_array_add_cstr(&builder, "street2");
	jso_builder_array_add_bool(&builder, true);
	assert_jso_schema_validation_failure(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_schema_clear(&schema);
}

/* A test for an array tuple with no additional items allowed. */
static void test_jso_schema_array_additional_false(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "array");
	jso_builder_object_add_array_start(&builder, "items");
	// first item number
	jso_builder_array_add_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "number");
	jso_builder_object_end(&builder);
	// second item string
	jso_builder_array_add_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "string");
	jso_builder_object_end(&builder);
	// third item boolean
	jso_builder_array_add_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "boolean");
	jso_builder_object_end(&builder);
	// end
	jso_builder_array_end(&builder);
	jso_builder_object_add_bool(&builder, "additionalItems", false);
	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid empty array
	jso_builder_array_start(&builder);
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid with all items
	jso_builder_array_start(&builder);
	jso_builder_array_add_int(&builder, 3);
	jso_builder_array_add_cstr(&builder, "street");
	jso_builder_array_add_bool(&builder, true);
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid with not all items
	jso_builder_array_start(&builder);
	jso_builder_array_add_int(&builder, 3);
	jso_builder_array_add_cstr(&builder, "street");
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// invalid with more items
	jso_builder_array_start(&builder);
	jso_builder_array_add_int(&builder, 3);
	jso_builder_array_add_cstr(&builder, "street");
	jso_builder_array_add_bool(&builder, true);
	jso_builder_array_add_cstr(&builder, "street");
	assert_jso_schema_validation_failure(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_schema_clear(&schema);
}

/* A test for an array tuple with only string additional items allowed. */
static void test_jso_schema_array_additional_string(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "array");
	jso_builder_object_add_array_start(&builder, "items");
	// first item number
	jso_builder_array_add_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "number");
	jso_builder_object_end(&builder);
	// second item string
	jso_builder_array_add_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "string");
	jso_builder_object_end(&builder);
	// third item boolean
	jso_builder_array_add_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "boolean");
	jso_builder_object_end(&builder);
	// array end
	jso_builder_array_end(&builder);
	// additional items
	jso_builder_object_add_object_start(&builder, "additionalItems");
	jso_builder_object_add_cstr(&builder, "type", "string");
	jso_builder_object_end(&builder);
	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid empty array
	jso_builder_array_start(&builder);
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid with all items
	jso_builder_array_start(&builder);
	jso_builder_array_add_int(&builder, 3);
	jso_builder_array_add_cstr(&builder, "street");
	jso_builder_array_add_bool(&builder, true);
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid with not all items
	jso_builder_array_start(&builder);
	jso_builder_array_add_int(&builder, 3);
	jso_builder_array_add_cstr(&builder, "street");
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid with more string items
	jso_builder_array_start(&builder);
	jso_builder_array_add_int(&builder, 3);
	jso_builder_array_add_cstr(&builder, "street");
	jso_builder_array_add_bool(&builder, true);
	jso_builder_array_add_cstr(&builder, "street2");
	jso_builder_array_add_cstr(&builder, "street3");
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// invalid with more items where there is not only string
	jso_builder_array_start(&builder);
	jso_builder_array_add_int(&builder, 3);
	jso_builder_array_add_cstr(&builder, "street");
	jso_builder_array_add_bool(&builder, true);
	jso_builder_array_add_bool(&builder, true);
	assert_jso_schema_validation_failure(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_schema_clear(&schema);
}

/* A test for an array length - min and max items. */
static void test_jso_schema_array_length(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "array");
	jso_builder_object_add_int(&builder, "minItems", 2);
	jso_builder_object_add_int(&builder, "maxItems", 3);
	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// invalid with 0 items
	jso_builder_array_start(&builder);
	assert_jso_schema_validation_failure(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// invalid with 1 item
	jso_builder_array_start(&builder);
	jso_builder_array_add_int(&builder, 1);
	assert_jso_schema_validation_failure(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid with 2 items
	jso_builder_array_start(&builder);
	jso_builder_array_add_int(&builder, 1);
	jso_builder_array_add_int(&builder, 2);
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid with 3 items
	jso_builder_array_start(&builder);
	jso_builder_array_add_int(&builder, 1);
	jso_builder_array_add_int(&builder, 2);
	jso_builder_array_add_int(&builder, 3);
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// invalid with 4 item
	jso_builder_array_start(&builder);
	jso_builder_array_add_int(&builder, 1);
	jso_builder_array_add_int(&builder, 2);
	jso_builder_array_add_int(&builder, 3);
	jso_builder_array_add_int(&builder, 4);
	assert_jso_schema_validation_failure(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_schema_clear(&schema);
}

/* A test for an array uniqueness. */
static void test_jso_schema_array_unique(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "array");
	jso_builder_object_add_bool(&builder, "uniqueItems", true);
	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid for empty array
	jso_builder_array_start(&builder);
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// valid with 2 items
	jso_builder_array_start(&builder);
	jso_builder_array_add_int(&builder, 1);
	jso_builder_array_add_int(&builder, 2);
	jso_builder_array_add_int(&builder, 3);
	jso_builder_array_add_int(&builder, 4);
	jso_builder_array_add_int(&builder, 5);
	assert_jso_schema_validation_success(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	// invalid for not unique items
	jso_builder_array_start(&builder);
	jso_builder_array_add_int(&builder, 1);
	jso_builder_array_add_int(&builder, 2);
	jso_builder_array_add_int(&builder, 3);
	jso_builder_array_add_int(&builder, 3);
	jso_builder_array_add_int(&builder, 4);
	assert_jso_schema_validation_failure(
			jso_schema_validate(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_schema_clear(&schema);
}

/* A test for an enum without type specified and using single type (string) values. */
static void test_jso_schema_enum_anytype_strings(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_array_start(&builder, "enum");
	jso_builder_array_add_cstr(&builder, "red");
	jso_builder_array_add_cstr(&builder, "green");
	jso_builder_array_add_cstr(&builder, "blue");
	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_value instance;
	jso_string *sv;

	sv = jso_string_create_from_cstr("red");
	JSO_VALUE_SET_STRING(instance, sv);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	sv = jso_string_create_from_cstr("blue");
	JSO_VALUE_SET_STRING(instance, sv);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	sv = jso_string_create_from_cstr("black");
	JSO_VALUE_SET_STRING(instance, sv);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	JSO_VALUE_SET_INT(instance, 5);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	jso_schema_clear(&schema);
}

/* A test for an enum without type specified and using multiple types values. */
static void test_jso_schema_enum_anytype_mixed(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_array_start(&builder, "enum");
	jso_builder_array_add_cstr(&builder, "on");
	jso_builder_array_add_cstr(&builder, "true");
	jso_builder_array_add_bool(&builder, true);
	jso_builder_array_add_int(&builder, 1);
	jso_builder_array_add_double(&builder, 1.0);
	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_value instance;
	jso_string *sv;

	sv = jso_string_create_from_cstr("on");
	JSO_VALUE_SET_STRING(instance, sv);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	JSO_VALUE_SET_INT(instance, 1);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	JSO_VALUE_SET_DOUBLE(instance, 1.0);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	sv = jso_string_create_from_cstr("off");
	JSO_VALUE_SET_STRING(instance, sv);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	JSO_VALUE_SET_INT(instance, 5);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	jso_schema_clear(&schema);
}

/* A test for a basic allOf schema composition. */
static void test_jso_schema_all_of_basic(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_array_start(&builder, "allOf");
	jso_builder_array_add_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "string");
	jso_builder_object_end(&builder);
	jso_builder_array_add_object_start(&builder);
	jso_builder_object_add_int(&builder, "maxLength", 5);
	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_value instance;
	jso_string *sv;

	sv = jso_string_create_from_cstr("short");
	JSO_VALUE_SET_STRING(instance, sv);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	sv = jso_string_create_from_cstr("too long");
	JSO_VALUE_SET_STRING(instance, sv);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	jso_schema_clear(&schema);
}

/* A test for a basic anyOf schema composition. */
static void test_jso_schema_any_of_basic(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_array_start(&builder, "anyOf");
	jso_builder_array_add_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "string");
	jso_builder_object_add_int(&builder, "maxLength", 5);
	jso_builder_object_end(&builder);
	jso_builder_array_add_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "number");
	jso_builder_object_add_int(&builder, "minimum", 0);
	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_value instance;
	jso_string *sv;

	sv = jso_string_create_from_cstr("short");
	JSO_VALUE_SET_STRING(instance, sv);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	sv = jso_string_create_from_cstr("too long");
	JSO_VALUE_SET_STRING(instance, sv);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	JSO_VALUE_SET_INT(instance, 12);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	JSO_VALUE_SET_INT(instance, -4);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));
	jso_value_clear(&instance);

	jso_schema_clear(&schema);
}

/* A test for a basic oneOf schema composition. */
static void test_jso_schema_one_of_basic(void **state)
{
	(void) state; /* unused */

	jso_schema_validation_result result;
	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_array_start(&builder, "oneOf");
	jso_builder_array_add_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "number");
	jso_builder_object_add_int(&builder, "multipleOf", 5);
	jso_builder_object_end(&builder);
	jso_builder_array_add_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "number");
	jso_builder_object_add_int(&builder, "multipleOf", 3);
	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_jso_schema_result_success(jso_schema_parse(&schema, jso_builder_get_value(&builder)));
	jso_builder_clear(&builder);

	jso_value instance;

	JSO_VALUE_SET_INT(instance, 10);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));

	JSO_VALUE_SET_INT(instance, 9);
	assert_jso_schema_validation_success(jso_schema_validate(&schema, &instance));

	JSO_VALUE_SET_INT(instance, 2);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));

	JSO_VALUE_SET_INT(instance, 15);
	assert_jso_schema_validation_failure(jso_schema_validate(&schema, &instance));

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
		cmocka_unit_test(test_jso_schema_object_all_props_non_overlap),
		cmocka_unit_test(test_jso_schema_object_required_props),
		cmocka_unit_test(test_jso_schema_object_size),
		cmocka_unit_test(test_jso_schema_array_items),
		cmocka_unit_test(test_jso_schema_array_tuple),
		cmocka_unit_test(test_jso_schema_array_additional_false),
		cmocka_unit_test(test_jso_schema_array_additional_string),
		cmocka_unit_test(test_jso_schema_array_length),
		cmocka_unit_test(test_jso_schema_array_unique),
		cmocka_unit_test(test_jso_schema_enum_anytype_strings),
		cmocka_unit_test(test_jso_schema_enum_anytype_mixed),
		cmocka_unit_test(test_jso_schema_all_of_basic),
		cmocka_unit_test(test_jso_schema_any_of_basic),
		cmocka_unit_test(test_jso_schema_one_of_basic),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
