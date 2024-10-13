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
#include <setjmp.h>
#include <cmocka.h>

/* A test for simple boolean value. */
static void test_jso_schema_bool(void **state)
{
	(void) state; /* unused */

	jso_builder builder;
	jso_builder_init(&builder);

	// build schema
	jso_builder_object_start(&builder);
	jso_builder_object_add_cstr(&builder, "type", "bool");
	jso_builder_object_end(&builder);

	jso_schema schema;
	jso_schema_init(&schema);
	assert_int_equal(
			JSO_SUCCESS, jso_schema_parse(&schema, jso_builder_get_value(builder.current)));
	jso_builder_clear(&builder);

	jso_value instance;

	JSO_VALUE_SET_BOOL(instance, true);
	assert_int_equal(JSO_SUCCESS, jso_schema_validate(&schema, &instance));

	jso_value_free(&instance);
	jso_schema_clear(&schema);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_schema_bool),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
