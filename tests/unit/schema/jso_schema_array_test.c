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

#include "../../../src/schema/jso_schema_array.h"
#include "../../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

void __wrap_jso_schema_value_free(jso_schema_value *val)
{
    check_expected_ptr(val);
}

/* A test to schema value append and and free calls */
static void jso_test_schema_array_append_and_free(void **state)
{
	(void) state; /* unused */

	jso_schema_value sv1, sv2;
    jso_schema_value *psv1 = &sv1, *psv2 = &sv2;

	jso_schema_array *arr = jso_schema_array_alloc(2);
	jso_schema_array_append(arr, psv1);
	jso_schema_array_append(arr, psv2);

    assert_int_equal(2, arr->len);
    assert_ptr_equal(psv1, arr->values[0]);
    assert_ptr_equal(psv2, arr->values[1]);

	expect_value(__wrap_jso_schema_value_free, val, psv1);
    expect_value(__wrap_jso_schema_value_free, val, psv2);

	jso_schema_array_free(arr);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(jso_test_schema_array_append_and_free),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
