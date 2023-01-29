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

#ifndef JSO_OBJECT_H
#define JSO_OBJECT_H

#include "jso_types.h"
#include "jso_ht.h"

/* jso object structure */
struct _jso_object {
	jso_ht ht;
};

/* callback function for iterating object */
typedef jso_rc (*jso_object_callback)(jso_string *key, jso_value *val);
typedef jso_rc (*jso_object_with_arg_callback)(jso_string *key, jso_value *val, void *arg);

/* object functions */
JSO_API jso_object *jso_object_alloc();
JSO_API void jso_object_free(jso_object *obj);
JSO_API jso_rc jso_object_add(jso_object *obj, jso_value *key, jso_value *val);
JSO_API void jso_object_apply(jso_object *obj, jso_object_callback cbk);
JSO_API void jso_object_apply_with_arg(
		jso_object *obj, jso_object_with_arg_callback cbk, void *arg);
JSO_API void jso_object_print(jso_object *obj, jso_uint indent);

#define JSO_OBJECT_FOREACH(_obj, _key, _val) \
	jso_ht *_ht = &_obj->ht; \
	JSO_HT_FOREACH(_ht, _key, _val)

#define JSO_OBJECT_FOREACH_END JSO_HT_FOREACH_END

#endif /* JSO_OBJECT_H */
