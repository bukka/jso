/*
 * Copyright (c) 2023 Jakub Zelenka. All rights reserved.
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

#include "jso_schema_validation_object.h"

#include "jso_schema_error.h"
#include "jso_schema_keyword.h"
#include "jso_schema_value.h"

#include "jso.h"

jso_rc jso_schema_validation_object_push_values(
		jso_schema_validation_stack *stack, jso_schema_value *value, jso_string *key)
{
	return JSO_SUCCESS;
}

jso_rc jso_schema_validation_object_key(
		jso_schema *schema, jso_schema_value *value, jso_string *key, size_t num_properties)
{
	// TODO: validate if key is allowed (when additionalProperties=false) and maxProperties based on
	// index
	return JSO_SUCCESS;
}

jso_rc jso_schema_validation_object_update(jso_schema *schema, jso_schema_value *value,
		jso_object *instance_object, jso_string *instance_key, jso_value *instance_value)
{
	return JSO_SUCCESS;
}

jso_rc jso_schema_validation_object_end(jso_schema *schema, jso_schema_value *value)
{
	return JSO_SUCCESS;
}
