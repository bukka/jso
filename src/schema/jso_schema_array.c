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

#include "jso_schema_array.h"
#include "jso_schema_value.h"

#include "jso.h"

jso_schema_array *jso_schema_array_alloc(size_t size)
{
	jso_schema_array *arr
			= jso_calloc(1, sizeof(jso_schema_array) + size * sizeof(jso_schema_value *));
	arr->capacity = size;
	return arr;
}

jso_rc jso_schema_array_append(jso_schema_array *arr, jso_schema_value *val)
{
	if (arr->len < arr->capacity) {
		arr->values[arr->len++] = val;
		return JSO_SUCCESS;
	}
	return JSO_FAILURE;
}

void jso_schema_array_free(jso_schema_array *arr)
{
	for (size_t i = 0; i < arr->len; i++) {
		jso_schema_value_free(arr->values[i]);
	}
	jso_free(arr);
}