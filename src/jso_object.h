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
 * @file jso_object.h
 * @brief Object header
 */

#ifndef JSO_OBJECT_H
#define JSO_OBJECT_H

#include "jso_types.h"
#include "jso_ht.h"

/**
 * @brief Object structure.
 */
struct _jso_object {
	/** embedded hash table */
	jso_ht ht;
};

/**
 * @brief Object iteration function callback.
 */
typedef jso_rc (*jso_object_callback)(jso_string *key, jso_value *val);

/**
 * @brief Object iteration function callback with argument.
 */
typedef jso_rc (*jso_object_with_arg_callback)(jso_string *key, jso_value *val, void *arg);

/**
 * Allocate object.
 *
 * @return New initialized object or NULL on error.
 */
JSO_API jso_object *jso_object_alloc();

/**
 * Free object.
 *
 * @param obj object
 */
JSO_API void jso_object_free(jso_object *obj);

/**
 * Add value with a key to the object.
 *
 * @param obj object
 * @param key key of the value
 * @param value value to add
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_object_add(jso_object *obj, jso_string *key, jso_value *val);

/**
 * Call callback for each value in the object.
 *
 * @param obj object
 * @param cbk callback function
 */
JSO_API void jso_object_apply(jso_object *obj, jso_object_callback cbk);

/**
 * Call callback with extra argument for each value in the object.
 *
 * @param obj object
 * @param cbk callback function
 * @param arg callback argument
 */
JSO_API void jso_object_apply_with_arg(
		jso_object *obj, jso_object_with_arg_callback cbk, void *arg);

/**
 * Print object with supplied indent.
 *
 * @param obj object
 * @param indent indent for each printed value
 */
JSO_API void jso_object_print(jso_object *obj, jso_uint indent);

/**
 * @brief Macro to start iteration of the object.
 * @param _obj object
 * @param _key entry key
 * @param _val entry value pointer
 */
#define JSO_OBJECT_FOREACH(_obj, _key, _val) \
	jso_ht *_ht = &_obj->ht; \
	JSO_HT_FOREACH(_ht, _key, _val)

/**
 * @brief Macro to end iteration of the object.
 */
#define JSO_OBJECT_FOREACH_END JSO_HT_FOREACH_END

#endif /* JSO_OBJECT_H */
