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

static jso_rc jso_pointer_compile(jso_pointer *jp, jso_string *pointer_value)
{
	jso_ctype *end, *start, *pos, *next;
	pos = start = JSO_STRING_VAL(pointer_value);
	size_t pointer_len = JSO_STRING_LEN(pointer_value);
	if (pointer_len == 0) {
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
	end = start + pointer_len;
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
	jp->pointer_value = jso_string_copy(pointer_value);
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

JSO_API jso_pointer *jso_pointer_create(jso_string *pointer_value)
{
	jso_pointer *jp = jso_calloc(1, sizeof(jso_pointer));
	if (jp == NULL || jso_pointer_compile(jp, pointer_value) == JSO_FAILURE) {
		return NULL;
	}

	return jp;
}

static jso_rc jso_pointer_search(
		jso_pointer *jp, size_t token_pos, jso_value *doc_pos, jso_value **value)
{
	if (token_pos == jp->tokens_count) {
		*value = doc_pos;
		return JSO_SUCCESS;
	}
	jso_string *token = jp->tokens[token_pos];
	jso_value *next;
	switch (JSO_TYPE_P(doc_pos)) {
		case JSO_TYPE_OBJECT:
			if (jso_object_get(JSO_OBJVAL_P(doc_pos), token, &next) == JSO_FAILURE) {
				jso_pointer_error_set(
						jp, JSO_POINTER_ERROR_NOT_FOUND, "JsonPointer value not found in object");
				return JSO_FAILURE;
			}
			return jso_pointer_search(jp, token_pos + 1, next, value);

		case JSO_TYPE_ARRAY: {
			jso_int index;
			if (jso_string_to_int(token, &index) == JSO_FAILURE) {
				jso_pointer_error_set(jp, JSO_POINTER_ERROR_INVALID_ARRAY_INDEX,
						"JsonPointer array index is not a number");
				return JSO_FAILURE;
			}
			if (index < 0) {
				jso_pointer_error_set(jp, JSO_POINTER_ERROR_INVALID_ARRAY_INDEX,
						"JsonPointer array index cannot be negative");
			}
			if (jso_array_index(JSO_ARRVAL_P(doc_pos), (size_t) index, &next) == JSO_FAILURE) {
				jso_pointer_error_set(
						jp, JSO_POINTER_ERROR_NOT_FOUND, "JsonPointer value not found in array");
				return JSO_FAILURE;
			}
			return jso_pointer_search(jp, token_pos + 1, next, value);
		}

		case JSO_TYPE_BOOL:
		case JSO_TYPE_DOUBLE:
		case JSO_TYPE_INT:
		case JSO_TYPE_STRING:
		case JSO_TYPE_NULL:
			jso_pointer_error_set(jp, JSO_POINTER_ERROR_NOT_FOUND,
					"JsonPointer scalar value not in the leave token");
			return JSO_FAILURE;
		default:
			jso_pointer_error_set(
					jp, JSO_POINTER_ERROR_INVALID_VALUE, "JsonPointer value is not supported");
			return JSO_FAILURE;
			break;
	}
	return JSO_FAILURE;
}

JSO_API jso_rc jso_pointer_resolve(
		jso_pointer *jp, jso_value *doc, jso_value **value, jso_ht *value_cache)
{
	return jso_pointer_search(jp, 0, doc, value);
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
		jso_string_free(jp->pointer_value);
		jso_free(jp);
	}
}
