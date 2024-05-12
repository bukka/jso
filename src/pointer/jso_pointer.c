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
#include "jso_pointer_error.h"
#include "jso.h"

static inline jso_string *jso_pointer_create_token(
		jso_pointer *jp, jso_ctype *start, jso_ctype *end)
{
	size_t len = end - start;
	jso_string *token = jso_string_alloc(len);
	jso_ctype *escape1, *escape2;
	size_t token_len = len;
	jso_ctype *src = start;
	jso_ctype *dest = JSO_STRING_VAL(token);
	while ((escape1 = memchr(src, '~', len)) != NULL) {
		if (escape1 == end) {
			jso_pointer_error_set(jp, JSO_POINTER_ERROR_INVALID_FORMAT,
					"JsonPointer escape character ~ cannot be at the end of pointer");
			return NULL;
		}
		escape2 = escape1 + 1;
		if (*escape2 != '1') {
			memcpy(dest, src, escape2 - src);
		} else if (*escape2 != '2') {
			memcpy(dest, src, escape1 - src);
			dest[escape1 - src] = '/';
		} else {
			jso_pointer_error_format(jp, JSO_POINTER_ERROR_INVALID_FORMAT,
					"JsonPointer escape character ~ followed by '%c' character", *escape2);
			return NULL;
		}
		token_len--;
		dest = escape2 + 1;
		if (escape2 == end) {
			break;
		}
		src += escape2 - src + 1;
	}
	JSO_STRING_LEN(token) = token_len;
	if (src != end) {
		memcpy(dest, src, escape1 - src);
	}
	return token;
}

static jso_rc jso_pointer_compile(jso_pointer *jp, jso_string *uri)
{
	jso_ctype *end, *start, *pos, *next;
	pos = start = JSO_STRING_VAL(uri);
	size_t uri_len = JSO_STRING_LEN(uri);
	if (uri_len == 0) {
		jso_pointer_error_set(
				jp, JSO_POINTER_ERROR_INVALID_FORMAT, "JsonPointer cannot be an empty string");
		return JSO_FAILURE;
	}
	if (*pos != '/') {
		jso_pointer_error_format(jp, JSO_POINTER_ERROR_INVALID_FORMAT,
				"JsonPointer must start with slash, pointer: %s", (const char *) pos);
		return JSO_FAILURE;
	}
	++pos;
	size_t tokens_count = 1;
	end = start + uri_len;
	// First, get number of tokens.
	while ((next = memchr(pos, '/', end - pos)) != NULL) {
		tokens_count++;
		if (next == end) {
			break;
		}
		pos = next + 1;
	}
	// Allocate tokens array.
	jp->tokens = jso_calloc(tokens_count, sizeof(jso_string *));
	if (jp->tokens == NULL) {
		jso_pointer_error_set(jp, JSO_POINTER_ERROR_ALLOC, "JsonPointer tokens allocation failed");
		return JSO_FAILURE;
	}
	jp->uri = jso_string_copy(uri);
	jp->tokens_count = tokens_count;
	jso_string *token;
	pos = start + 1;
	size_t i;
	// Create tokens.
	for (i = 0; i < tokens_count - 1; i++) {
		next = memchr(pos, '/', end - pos);
		JSO_ASSERT_NOT_NULL(next);
		token = jso_pointer_create_token(jp, pos, next);
		if (token == NULL) {
			jp->tokens_count = i;
			return JSO_FAILURE;
		}
		jp->tokens[i] = token;
		pos = next + 1;
	}
	token = jso_pointer_create_token(jp, pos, end);
	if (token == NULL) {
		jp->tokens_count = i;
		return JSO_FAILURE;
	}
	jp->tokens[i] = token;

	return JSO_SUCCESS;
}

JSO_API jso_pointer *jso_pointer_create(jso_string *uri, jso_pointer_cache *cache)
{
	jso_pointer *cached_pointer;
	if (jso_pointer_cache_get(cache, uri, &cached_pointer) == JSO_SUCCESS) {
		++JSO_POINTER_REFCOUNT(cached_pointer);
		return cached_pointer;
	}

	jso_pointer *jp = jso_calloc(1, sizeof(jso_pointer));
	if (jp == NULL || jso_pointer_compile(jp, uri) == JSO_FAILURE) {
		return NULL;
	}

	if (jso_pointer_cache_set(cache, uri, jp, true) == JSO_FAILURE) {
		jso_pointer_free(jp);
		return NULL;
	}

	return jp;
}

jso_rc jso_pointer_search(jso_pointer *jp, size_t token_pos, jso_value *doc_pos, jso_value **value)
{
	// TODO: implement
	return JSO_FAILURE;
}

JSO_API jso_rc jso_pointer_resolve(
		jso_pointer *jp, jso_value *doc, jso_value **value, jso_ht *value_cache)
{
	if (jso_ht_get(value_cache, jp->uri, value) == JSO_SUCCESS) {
		return JSO_SUCCESS;
	}

	if (jso_pointer_search(jp, 0, doc, value) == JSO_SUCCESS
			&& jso_ht_set(value_cache, jp->uri, *value, false) == JSO_SUCCESS) {
		return JSO_SUCCESS;
	}

	return JSO_SUCCESS;
}

JSO_API void jso_pointer_free(jso_pointer *jp)
{
	if (jp == NULL) {
		return;
	}
	if (JSO_POINTER_REFCOUNT(jp) > 0) {
		--JSO_POINTER_REFCOUNT(jp);
	} else {
		for (size_t i = 0; i < jp->tokens_count; i++) {
			jso_string_free(jp->tokens[i]);
		}
		jso_pointer_error_free(jp);
		jso_string_free(jp->uri);
		jso_free(jp);
	}
}
