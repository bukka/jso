/*
 * Copyright (c) 2012-2014 Jakub Zelenka. All rights reserved.
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

/* jso object member */
typedef struct _jso_object_member {
	jso_string key;
	jso_value val;
	struct _jso_object_member *next;
} jso_object_member;

/* jso object structure */
struct _jso_object {
	jso_object_member *head;
	jso_object_member *tail;
};

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
	jso_object_member *tmp, *member = obj->head;
	while (member) {
		jso_value_free(&member->val);
		jso_free(member->key.val);
		tmp = member->next;
		jso_free(member);
		member = tmp;
	}
	jso_free(obj);
}

/* add new element to the object obj */
JSO_API int jso_object_add(jso_object *obj, jso_value *key, jso_value *val)
{
	jso_object_member *member;
	member = jso_malloc(sizeof (jso_object_member));
	if (!member)
		return JSO_FALSE;
	member->key = key->data.str;
	member->val = *val;
	member->next = NULL;
	if (!obj->head) {
		obj->head = obj->tail = member;
	} else {
		obj->tail->next = member;
		obj->tail = member;
	}
	return JSO_TRUE;
}

/* call cbk function for each element in object */
JSO_API void jso_object_foreach(jso_object *obj, jso_object_callback cbk)
{
	jso_object_member *member = obj->head;
	while (member) {
		cbk(&member->key, &member->val);
		member = member->next;
	}
}

/* call cbk function with arg for each element in object */
JSO_API void jso_object_foreach_with_arg(jso_object *obj, jso_object_with_arg_callback cbk, void *arg)
{
	jso_object_member *member = obj->head;
	while (member) {
		cbk(&member->key, &member->val, arg);
		member = member->next;
	}
}