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

/**
 * @file jso_pointer.h
 * @brief JSO pointer header
 */

#ifndef JSO_POINTER_H
#define JSO_POINTER_H

#include "jso_types.h"
#include "jso_ht.h"

/**
 * @brief JSO pointer cache
 */
typedef struct _jso_pointer_cache {
	jso_ht data;
} jso_pointer_cache;

/**
 * Allocate new JSO pointer cache.
 *
 * @return New initialized JSO pointer cache.
 */
JSO_API jso_pointer_cache *jso_pointer_cache_alloc();

/**
 * Initialize JSO pointer cache.
 *
 * @param jpc JSO pointer cache
 */
JSO_API void jso_pointer_cache_init(jso_pointer_cache *jpc);

/**
 * Free JSO pointer cache.
 *
 * @param jpc JSO pointer cache
 */
JSO_API void jso_pointer_cache_free(jso_pointer_cache *jpc);

/**
 * Clear JSO pointer cache (delete all items).
 *
 * @param jpc JSO pointer cache
 */
JSO_API void jso_pointer_cache_clear(jso_pointer_cache *jpc);

/**
 * Set pointer result with the URI to the cache.
 *
 * @param jpc JSO pointer cache
 * @param uri JSO pointer URI
 * @param value JSO pointer value
 * @param free_old whether to free old value if value for URI alrady exists.
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_pointer_cache_set(jso_pointer_cache *jpc, jso_string *uri, jso_value *value, jso_bool free_old);

/**
 * Return pointer result.
 *
 * @param jpc JSO pointer cache
 * @param uri JSO pointer URI
 * @param value Cached JSO pointer value
 * @return @ref JSO_SUCCESS on success (if found), otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_pointer_cache_get(jso_pointer_cache *jpc, jso_string *uri, jso_value **value);


/**
 * @brief Pointer error type.
 */
typedef enum _jso_pointer_error_type {
	JSO_POINTER_ERROR_NONE = 0,
	JSO_POINTER_ERROR_NOT_FOUND,
	JSO_POINTER_ERROR_INVALID,
} jso_pointer_error_type;

/**
 * @brief Pointer error.
 */
typedef struct _jso_pointer_error {
	/** error message */
	char *message;
	/** error type */
	jso_pointer_error_type type;
} jso_pointer_error;

/**
 * @brief Pointer structure.
 */
typedef struct _jso_pointer {
	jso_string *uri;
	size_t base_uri_len;
	jso_string *tokens;
	size_t tokens_count;
	jso_pointer_cache *cache;
	jso_pointer_error error;
} jso_pointer;

/**
 * Get pointer error type.
 *
 * @param _pointer shema of type @ref jso_pointer
 */
#define JSO_POINTER_ERROR_TYPE(_pointer) (_pointer)->error.type

/**
 * Get pointer error message.
 *
 * @param _pointer shema of type @ref jso_pointer
 */
#define JSO_POINTER_ERROR_MESSAGE(_pointer) (_pointer)->error.message

/**
 * Check if pointer error is set.
 *
 * @param pointer pointer of type @ref jso_pointer
 * @return JSO_TRUE if error set, otherwise JSO_FALSE
 */
static inline bool jso_pointer_error_is_set(jso_pointer *pointer)
{
	return JSO_POINTER_ERROR_TYPE(pointer) != JSO_POINTER_ERROR_NONE;
}

/**
 * Allocate new JSO pointer.
 *
 * @param uri JSO pointer URI
 * @param cache Pointer cache
 * @return New initialized JSO pointer.
 */
JSO_API jso_pointer *jso_pointer_alloc(jso_string *uri, jso_pointer_cache *cache);

/**
 * Initialize JSO pointer.
 *
 * @param jp JSO pointer
 * @param uri JSO pointer URI
 * @param cache Pointer cache
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_pointer_init(jso_pointer *jp, jso_string *uri, jso_pointer_cache *cache);

/**
 * Initialize JSO pointer.
 *
 * @param jp JSO pointer
 * @param uri JSO pointer URI
 * @param cache Pointer cache
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_pointer_init(jso_pointer *jp, jso_string *uri, jso_pointer_cache *cache);

/**
 * Resolve 
 * 
 * @param jp JSO pointer
 * @param doc Document to search value in.
 * @param value Resolved value if the resolving was successful
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE. 
 */
JSO_API jso_rc jso_pointer_resolve(jso_pointer *jp, jso_value *doc, jso_value **value);

/**
 * Free JSO pointer.
 *
 * @param ht JSO pointer
 */
JSO_API void jso_pointer_free(jso_pointer *jp);

/**
 * Clear JSO pointer (delete all items).
 *
 * @param ht JSO pointer
 */
JSO_API void jso_pointer_clear(jso_pointer *jp);

#endif /* JSO_POINTER_H */