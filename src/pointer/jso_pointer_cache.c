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

#include "jso_pointer.h"
#include "jso.h"

JSO_API jso_pointer_cache *jso_pointer_cache_alloc()
{
	return jso_calloc(1, sizeof(jso_pointer_cache));
}

JSO_API void jso_pointer_cache_init(jso_pointer_cache *jpc)
{
	memset(jpc, 0, sizeof(jso_pointer_cache));
}

JSO_API void jso_pointer_cache_free(jso_pointer_cache *jpc)
{
	jso_ht_free(&(jpc->data));
}

JSO_API void jso_pointer_cache_clear(jso_pointer_cache *jpc)
{
	jso_ht_clear(&(jpc->data));
}

JSO_API jso_rc jso_pointer_cache_set(
		jso_pointer_cache *jpc, jso_string *uri, jso_pointer *jp, jso_bool free_old)
{
	jso_value value;
	JSO_VALUE_SET_POINTER(value, jp);

	return jso_ht_set(&(jpc->data), uri, &value, free_old);
}

JSO_API jso_rc jso_pointer_cache_get(jso_pointer_cache *jpc, jso_string *uri, jso_pointer **jp)
{
	jso_value *value;

	if (jso_ht_get(&(jpc->data), uri, &value) == JSO_SUCCESS) {
		JSO_ASSERT_EQ(JSO_TYPE_POINTER, JSO_TYPE_P(value));
		*jp = JSO_PTRVAL_P(value);
		return JSO_SUCCESS;
	}

	return JSO_FAILURE;
}
