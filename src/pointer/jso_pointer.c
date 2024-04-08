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

static jso_rc jso_pointer_compile(jso_pointer *jp, jso_string *uri, jso_pointer_cache *cache)
{
	jp->cache = cache;
	jp->uri = jso_string_copy(uri);
	// TODO: implement tokenization

	return JSO_SUCCESS;
}

JSO_API jso_pointer *jso_pointer_alloc(jso_string *uri, jso_pointer_cache *cache)
{
	jso_pointer *jp = jso_calloc(1, sizeof(jso_pointer));
	if (jp == NULL || jso_pointer_compile(jp, uri, cache) == JSO_FAILURE) {
		return NULL;
	}
	return jp;
}

JSO_API jso_rc jso_pointer_init(jso_pointer *jp, jso_string *uri, jso_pointer_cache *cache)
{
	memset(jp, 0, sizeof(jso_pointer));
	return jso_pointer_compile(jp, uri, cache);
}

JSO_API jso_rc jso_pointer_resolve(jso_pointer *jp, jso_value *doc, jso_value **value)
{
	// TODO: Implement resolving
	return JSO_SUCCESS;
}

JSO_API void jso_pointer_free(jso_pointer *jp)
{
	jso_pointer_clear(jp);
	jso_free(jp);
}

JSO_API void jso_pointer_clear(jso_pointer *jp)
{
	for (size_t i = 0; i < jp->tokens_count; i++) {
		jso_string_free(jp->tokens[i]);
	}
	jso_string_free(jp->uri);
}
