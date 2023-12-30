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

#include "jso.h"

#include "jso_schema.h"

#include "jso_schema_value.h"
#include "jso_schema_validation_composition.h"
#include "jso_schema_validation_array.h"
#include "jso_schema_validation_object.h"
#include "jso_schema_validation_stack.h"

inline jso_rc jso_schema_validation_stream_stack_init(
		jso_schema_validation_stream *stream, size_t capacity)
{
	return jso_schema_validation_stack_init(&stream->stack, capacity);
}

inline jso_schema_validation_position *jso_schema_validation_stream_stack_push_basic(
		jso_schema_validation_stream *stream, jso_schema_value *current_value,
		jso_schema_validation_position *parent)
{
	return jso_schema_validation_stack_push_basic(&stream->stack, current_value, parent);
}

inline jso_schema_validation_position *jso_schema_validation_stream_stack_push_separator(
		jso_schema_validation_stream *stream)
{
	return jso_schema_validation_stack_push_separator(&stream->stack);
}

inline jso_bool jso_schema_validation_stream_should_terminate(
		jso_schema *schema, jso_schema_validation_position *pos)
{
	return pos->validation_result == JSO_FAILURE && jso_schema_error_is_fatal(schema);
}

inline jso_schema_validation_position *jso_schema_validation_stream_stack_pop(
		jso_schema_validation_stream *stream)
{
	return jso_schema_validation_stack_pop(&stream->stack);
}

JSO_API void jso_schema_validation_stream_clear(jso_schema_validation_stream *stream)
{
	jso_schema_validation_stack_clear(&stream->stack);
}

void jso_schema_validation_stream_stack_layer_iterator_start(
		jso_schema_validation_stream *stream, jso_schema_validation_stack_layer_iterator *iterator)
{
	jso_schema_validation_stack_layer_iterator_start(&stream->stack, iterator);
}

jso_schema_validation_position *jso_schema_validation_stream_stack_layer_iterator_next(
		jso_schema_validation_stream *stream, jso_schema_validation_stack_layer_iterator *iterator)
{
	return jso_schema_validation_stack_layer_iterator_next(&stream->stack, iterator);
}

void jso_schema_validation_stream_stack_layer_remove(jso_schema_validation_stream *stream)
{
	jso_schema_validation_stack_layer_remove(&stream->stack);
}

JSO_API jso_rc jso_schema_validation_stream_init(
		jso_schema *schema, jso_schema_validation_stream *stream, size_t stack_capacity)
{
	JSO_ASSERT_GE(stack_capacity, 1);

	if (jso_schema_validation_stream_stack_init(stream, stack_capacity) == JSO_FAILURE) {
		return JSO_FAILURE;
	}
	if (jso_schema_validation_stream_stack_push_basic(stream, schema->root, NULL) == NULL) {
		jso_schema_validation_stream_clear(stream);
		return JSO_FAILURE;
	}
	stream->root_schema = schema;

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_object_start(jso_schema_validation_stream *stream)
{
	jso_schema_validation_stack_layer_iterator iterator;
	jso_schema_validation_position *pos;
	jso_schema *schema = stream->root_schema;

	jso_schema_validation_stream_stack_layer_iterator_start(stream, &iterator);
	if (jso_schema_validation_stream_stack_push_separator(stream) == NULL) {
		return JSO_FAILURE;
	}
	while ((pos = jso_schema_validation_stream_stack_layer_iterator_next(stream, &iterator))) {
		jso_schema_value *value = pos->current_value;
		if (JSO_SCHEMA_VALUE_TYPE_P(value) == JSO_SCHEMA_VALUE_TYPE_OBJECT) {
			pos->validation_result = jso_schema_validation_composition_check(stream, value);

		} else {
			pos->validation_result = JSO_FAILURE;
		}

		if (jso_schema_validation_stream_should_terminate(schema, pos)) {
			return JSO_FAILURE;
		}
	}

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_object_key(
		jso_schema_validation_stream *stream, jso_string *key)
{
	jso_schema_validation_stack_layer_iterator iterator;
	jso_schema_validation_position *pos;
	jso_schema *schema = stream->root_schema;

	jso_schema_validation_stream_stack_layer_iterator_start(stream, &iterator);
	while ((pos = jso_schema_validation_stream_stack_layer_iterator_next(stream, &iterator))) {
		++pos->index;
		pos->validation_result
				= jso_schema_validation_object_key(schema, pos->current_value, key, pos->index);
		if (jso_schema_validation_stream_should_terminate(schema, pos)) {
			return JSO_FAILURE;
		}
		pos->object_key = key;
		if (jso_schema_validation_object_push_values(stream, pos->current_value, key)) {
			return JSO_FAILURE;
		}
	}

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_object_update(jso_schema_validation_stream *stream,
		jso_object *instance_object, jso_string *instance_key, jso_value *instance_item)
{
	// currently there is nothing to validate
	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_object_end(jso_schema_validation_stream *stream)
{

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_array_start(jso_schema_validation_stream *stream)
{

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_array_append(
		jso_schema_validation_stream *stream, jso_array *instance_array, jso_value *instance_item)
{

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_array_end(jso_schema_validation_stream *stream)
{

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_value(
		jso_schema_validation_stream *stream, jso_value *instance)
{
	jso_schema_validation_stack_layer_iterator iterator;
	jso_schema_validation_position *pos;
	jso_schema *schema = stream->root_schema;

	jso_schema_validation_stream_stack_layer_iterator_start(stream, &iterator);
	while ((pos = jso_schema_validation_stream_stack_layer_iterator_next(stream, &iterator))) {
		jso_schema_value *value = pos->current_value;
		jso_schema_value_type value_type = JSO_SCHEMA_VALUE_TYPE_P(value);
		if (value_type == JSO_SCHEMA_VALUE_TYPE_OBJECT || value_type == JSO_SCHEMA_VALUE_TYPE_ARRAY
				|| (pos->validation_result = jso_schema_validation_composition_check(stream, value))
						== JSO_SUCCESS) {
			pos->validation_result = jso_schema_value_validate(schema, value, instance);
		}
		if (jso_schema_validation_stream_should_terminate(schema, pos)) {
			return JSO_FAILURE;
		}
	}
	jso_schema_validation_stream_stack_layer_remove(stream);

	return JSO_SUCCESS;
}