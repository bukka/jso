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

#include "jso_schema.h"

#include "jso.h"

jso_schema_validation_result jso_schema_validate_instance(
		jso_schema_validation_stream *stream, jso_value *instance)
{
	jso_value *val;
	jso_schema_validation_result res;

	switch (JSO_TYPE_P(instance)) {
		case JSO_TYPE_ARRAY:
			if ((res = jso_schema_validation_stream_array_start(stream))
					!= JSO_SCHEMA_VALIDATION_VALID) {
				return res;
			}
			jso_array *array = JSO_ARRVAL_P(instance);
			JSO_ARRAY_FOREACH(array, val)
			{
				if ((res = jso_schema_validate_instance(stream, val))
						!= JSO_SCHEMA_VALIDATION_VALID) {
					return res;
				}
				if ((res = jso_schema_validation_stream_array_append(stream, array, val))
						!= JSO_SCHEMA_VALIDATION_VALID) {
					return res;
				}
			}
			JSO_ARRAY_FOREACH_END;
			if ((res = jso_schema_validation_stream_array_end(stream))
					!= JSO_SCHEMA_VALIDATION_VALID) {
				return res;
			}
			if ((res = jso_schema_validation_stream_value(stream, instance))
					!= JSO_SCHEMA_VALIDATION_VALID) {
				return res;
			}
			break;
		case JSO_TYPE_OBJECT: {
			jso_string *key;
			if ((res = jso_schema_validation_stream_object_start(stream))
					!= JSO_SCHEMA_VALIDATION_VALID) {
				return res;
			}
			jso_object *object = JSO_OBJVAL_P(instance);
			JSO_OBJECT_FOREACH(object, key, val)
			{
				if ((res = jso_schema_validation_stream_object_key(stream, key))
						!= JSO_SCHEMA_VALIDATION_VALID) {
					return res;
				}
				if ((res = jso_schema_validate_instance(stream, val))
						!= JSO_SCHEMA_VALIDATION_VALID) {
					return res;
				}
				if ((res = jso_schema_validation_stream_object_update(stream, object, key, val))
						!= JSO_SCHEMA_VALIDATION_VALID) {
					return res;
				}
			}
			JSO_OBJECT_FOREACH_END;
			if ((res = jso_schema_validation_stream_object_end(stream))
					!= JSO_SCHEMA_VALIDATION_VALID) {
				return res;
			}
			if ((res = jso_schema_validation_stream_value(stream, instance))
					!= JSO_SCHEMA_VALIDATION_VALID) {
				return res;
			}
		} break;
		default:
			return jso_schema_validation_stream_value(stream, instance);
	}

	return JSO_SCHEMA_VALIDATION_VALID;
}

JSO_API jso_schema_validation_result jso_schema_validate(jso_schema *schema, jso_value *instance)
{
	jso_schema_validation_stream stream;

	jso_schema_validation_stream_init(schema, &stream, 32);

	jso_schema_validation_result result = jso_schema_validate_instance(&stream, instance);

	if (result == JSO_SCHEMA_VALIDATION_VALID) {
		result = jso_schema_validation_stream_final_result(&stream);
	}

	jso_schema_validation_stream_clear(&stream);

	return result;
}