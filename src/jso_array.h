/*
 * Copyright (c) 2012-2023 Jakub Zelenka. All rights reserved.
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
 * @file jso_array.h
 * @brief Array header
 */

#ifndef JSO_ARRAY_H
#define JSO_ARRAY_H

#include "jso_types.h"

/**
 * @brief Array element.
 */
typedef struct _jso_array_element {
	jso_value val;
	struct _jso_array_element *next;
} jso_array_element;

/**
 * @brief Array main structure.
 *
 * Currently array is implemented as a linked list.
 */
struct _jso_array {
	/* array first element */
	jso_array_element *head;
	/* array last element */
	jso_array_element *tail;
	/* array length */
	size_t len;
	/* array reference count */
	jso_uint16 refcount;
	/* array flags */
	jso_uint16 flags;
};

/**
 * Get length of the supplied array.
 *
 * @param _arr pointer to @ref jso_array
 * @return Array length.
 */
#define JSO_ARRAY_LEN(_arr) (_arr)->len

/**
 * Get array flags of the supplied array.
 *
 * @param _arr pointer to @ref jso_array
 * @return String flags.
 */
#define JSO_ARRAY_FLAGS(_arr) (_arr)->flags

/**
 * Get reference count of the supplied array.
 *
 * @param _arr pointer to @ref jso_array
 * @return References count value.
 */
#define JSO_ARRAY_REFCOUNT(_arr) (_arr)->refcount

/**
 * @brief Array iteration function callback.
 */
typedef jso_rc (*jso_array_callback)(size_t idx, jso_value *val);

/**
 * @brief Array iteration function callback with argument.
 */
typedef jso_rc (*jso_array_with_arg_callback)(size_t idx, jso_value *val, void *arg);

/**
 * Allocate array.
 *
 * @return New initialized array.
 */
JSO_API jso_array *jso_array_alloc();

/**
 * Free array.
 *
 * @param arr array
 */
JSO_API void jso_array_free(jso_array *arr);

/**
 * Append value to the end of the array.
 *
 * @param arr array
 * @param val value
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_array_append(jso_array *arr, jso_value *val);

/**
 * Push value to the beginning of the array.
 *
 * @param arr array
 * @param val value
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_array_push(jso_array *arr, jso_value *val);

/**
 * Remove value from the beginning of the array.
 *
 * @param arr array
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_array_pop(jso_array *arr);

/**
 * Call callback for each value in the array.
 *
 * @param arr array
 * @param cbk callback function
 */
JSO_API void jso_array_apply(jso_array *arr, jso_array_callback cbk);

/**
 * Call callback with extra argument for each value in the array.
 *
 * @param arr array
 * @param cbk callback function
 * @param arg callback argument
 */
JSO_API void jso_array_apply_with_arg(jso_array *arr, jso_array_with_arg_callback cbk, void *arg);

/**
 * Print array with supplied indent.
 *
 * @param arr array
 * @param indent indent for each printed value
 */
JSO_API void jso_array_print(jso_array *arr, jso_uint indent);

/**
 * Copy array.
 *
 * @param str array to copy
 * @return The copied array.
 */
static inline jso_array *jso_array_copy(jso_array *arr)
{
	++JSO_ARRAY_REFCOUNT(arr);
	return arr;
}

/**
 * @brief Macro to start iteration of the array.
 * @param _arr array
 * @param _val value pointer
 */
#define JSO_ARRAY_FOREACH(_arr, _val) \
	do { \
		struct _jso_array_element *_array_el; \
		for (_array_el = _arr->head; _array_el; _array_el = _array_el->next) { \
			_val = &_array_el->val;

/**
 * @brief Macro to end iteration of the array.
 */
#define JSO_ARRAY_FOREACH_END \
	} \
	} \
	while (0)

#endif /* JSO_ARRAY_H */
