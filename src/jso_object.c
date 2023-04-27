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

#include <stdlib.h>
#include "jso.h"

/* allocate and init object */
JSO_API jso_object *jso_object_alloc()
{
	return jso_calloc(1, sizeof(jso_object));
}

/* free object and its elements */
JSO_API void jso_object_free(jso_object *obj)
{
	if (!obj)
		return;
	if (JSO_OBJECT_REFCOUNT(obj) > 0) {
		--JSO_OBJECT_REFCOUNT(obj);
		return;
	}
	jso_ht_clear(&obj->ht);
	jso_free(obj);
}

/* resize object */
JSO_API jso_rc jso_object_resize(jso_object *obj, size_t size)
{
	return jso_ht_resize(&obj->ht, size);
}

/* add new element to the object obj */
JSO_API jso_rc jso_object_add(jso_object *obj, jso_string *key, jso_value *val)
{
	return jso_ht_set(&obj->ht, key, val, true);
}

/* get element of the object obj */
JSO_API jso_rc jso_object_get(jso_object *obj, jso_string *key, jso_value **val)
{
	return jso_ht_get(&obj->ht, key, val);
}

/* call cbk function for each element in object */
JSO_API void jso_object_apply(jso_object *obj, jso_object_callback cbk)
{
	jso_string *key;
	jso_value *val;

	JSO_OBJECT_FOREACH(obj, key, val)
	{
		cbk(key, val);
	}
	JSO_OBJECT_FOREACH_END;
}

/* call cbk function with arg for each element in object */
JSO_API void jso_object_apply_with_arg(jso_object *obj, jso_object_with_arg_callback cbk, void *arg)
{
	jso_string *key;
	jso_value *val;

	JSO_OBJECT_FOREACH(obj, key, val)
	{
		cbk(key, val, arg);
	}
	JSO_OBJECT_FOREACH_END;
}
