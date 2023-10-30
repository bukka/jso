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

#include "../../../src/schema/jso_schema_value.h"
#include "../../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

/* Tests for jso_schema_value_allocate. */

/* Test allocating schema value. */
static void test_jso_schema_value_allocate(void **state)
{
	(void) state; /* unused */

	jso_schema schema;

	jso_schema_init(&schema);

	jso_schema_value *value = jso_schema_value_alloc(&schema, "null");

	assert_non_null(value);

	jso_schema_value_free(value);
}

/* Tests for jso_schema_value_allocate. */

/* Test allocating schema value data. */
static void test_jso_schema_value_data_allocate(void **state)
{
	(void) state; /* unused */

	jso_schema schema;

	jso_schema_init(&schema);

	jso_schema_value_null *value
			= jso_schema_value_data_alloc(sizeof(jso_schema_value_null), &schema, "null");

	assert_non_null(value);

	jso_free(value);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jso_schema_value_allocate),
		cmocka_unit_test(test_jso_schema_value_data_allocate),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
