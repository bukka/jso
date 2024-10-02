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

#include "jso_schema_error.h"
#include "jso_schema_reference.h"

#include "jso_pointer.h"
#include "jso.h"

jso_schema_reference *jso_schema_reference_create(
		jso_schema *schema, jso_string *ref_uri, jso_schema_uri *base_uri)
{
	jso_schema_reference *ref = jso_calloc(1, sizeof(jso_schema_reference));
	if (ref == NULL) {
		jso_schema_error_format(
				schema, JSO_SCHEMA_ERROR_REFERENCE_ALLOC, "Allocation of schema reference failed");
		return NULL;
	}
	if (jso_schema_uri_set(schema, &ref->uri, base_uri, ref_uri) == JSO_FAILURE) {
		jso_free(ref);
		return NULL;
	}
	ref->schema = schema;
	return ref;
}

void jso_schema_reference_free(jso_schema_reference *ref)
{
	// TODO: change ref count because of caching
	jso_schema_uri_clear(&ref->uri);
	jso_free(ref);
}

jso_rc jso_schema_reference_resolve(jso_schema_reference *jp, jso_value *doc, jso_value **value)
{
	// TODO: check cache and then do resolving if not found and save result to cache
	return JSO_SUCCESS;
}