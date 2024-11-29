/*
 * Copyright (c) 2023-2024 Jakub Zelenka. All rights reserved.
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
#include "jso_schema_validation_result.h"
#include "jso_schema_validation_stack.h"
#include "jso_schema_validation_stream.h"
#include "jso_schema_validation_value.h"

JSO_API void jso_schema_validation_stream_clear(jso_schema_validation_stream *stream)
{
	jso_schema_validation_stack_clear(&stream->stack);
}

JSO_API jso_rc jso_schema_validation_stream_init(
		jso_schema *schema, jso_schema_validation_stream *stream, size_t stack_capacity)
{
	JSO_ASSERT_GE(stack_capacity, 1);
	jso_schema_validation_stack *stack = JSO_STREAM_VALIDATION_STREAM_STACK_P(stream);

	if (jso_schema_validation_stack_init(schema, stack, stack_capacity) == JSO_FAILURE) {
		return JSO_FAILURE;
	}
	if (jso_schema_validation_stack_push_basic(stack, schema->root, NULL) == NULL) {
		jso_schema_validation_stream_clear(stream);
		return JSO_FAILURE;
	}

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_object_start(jso_schema_validation_stream *stream)
{
	jso_schema_validation_stack_layer_iterator iterator;
	jso_schema_validation_position *pos;
	jso_schema_validation_stack *stack = JSO_STREAM_VALIDATION_STREAM_STACK_P(stream);
	jso_schema *schema = stack->root_schema;

	jso_schema_validation_stack_layer_iterator_start(stack, &iterator);
	while ((pos = jso_schema_validation_stack_layer_iterator_next(stack, &iterator))) {
		jso_schema_value *value = pos->current_value;
		if (JSO_SCHEMA_VALUE_TYPE_P(value) == JSO_SCHEMA_VALUE_TYPE_OBJECT) {
			pos->validation_result = jso_schema_validation_composition_check(stack, pos);
			if (jso_schema_validation_stream_should_terminate(schema, pos)) {
				return JSO_FAILURE;
			}
		} else {
			pos->validation_result = JSO_FAILURE;
		}
	}

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_object_key(
		jso_schema_validation_stream *stream, jso_string *key)
{
	jso_schema_validation_stack_layer_iterator iterator;
	jso_schema_validation_position *pos;
	jso_schema_validation_stack *stack = JSO_STREAM_VALIDATION_STREAM_STACK_P(stream);
	jso_schema *schema = stack->root_schema;

	jso_schema_validation_stack_layer_iterator_start(stack, &iterator);
	if (jso_schema_validation_stack_push_separator(stack) == NULL) {
		return JSO_FAILURE;
	}
	while ((pos = jso_schema_validation_stack_layer_iterator_next(stack, &iterator))) {
		++pos->count;
		pos->validation_result = jso_schema_validation_object_key(stack, pos, key);
		if (jso_schema_validation_stream_should_terminate(schema, pos)) {
			return JSO_FAILURE;
		}
		pos->object_key = key;
	}

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_object_update(jso_schema_validation_stream *stream,
		jso_object *instance_object, jso_string *instance_key, jso_value *instance_item)
{
	// currently there is nothing to do
	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_object_end(jso_schema_validation_stream *stream)
{
	// currently there is nothing to do
	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_array_start(jso_schema_validation_stream *stream)
{
	jso_schema_validation_stack_layer_iterator iterator;
	jso_schema_validation_position *pos;
	jso_schema_validation_stack *stack = JSO_STREAM_VALIDATION_STREAM_STACK_P(stream);
	jso_schema *schema = stack->root_schema;

	jso_schema_validation_stack_layer_iterator_start(stack, &iterator);
	if (jso_schema_validation_stack_push_separator(stack) == NULL) {
		return JSO_FAILURE;
	}
	while ((pos = jso_schema_validation_stack_layer_iterator_next(stack, &iterator))) {
		jso_schema_value *value = pos->current_value;
		if (JSO_SCHEMA_VALUE_TYPE_P(value) == JSO_SCHEMA_VALUE_TYPE_ARRAY) {
			pos->validation_result = jso_schema_validation_composition_check(stack, pos);
			if (jso_schema_validation_stream_should_terminate(schema, pos)) {
				return JSO_FAILURE;
			}
			// We do not need to check termination because validation on start should never fail
			// so it needs to be some termintating error. This needs to be amended if this
			// assumption changes.
			if (jso_schema_validation_array_start(stack, pos) == JSO_FAILURE) {
				return JSO_FAILURE;
			}
		} else {
			pos->validation_result = JSO_FAILURE;
		}
	}

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_array_append(
		jso_schema_validation_stream *stream, jso_array *instance_array, jso_value *instance_item)
{
	jso_schema_validation_stack_layer_iterator iterator;
	jso_schema_validation_position *pos;
	jso_schema_validation_stack *stack = JSO_STREAM_VALIDATION_STREAM_STACK_P(stream);

	jso_schema_validation_stack_layer_iterator_start(stack, &iterator);
	if (jso_schema_validation_stack_push_separator(stack) == NULL) {
		return JSO_FAILURE;
	}
	while ((pos = jso_schema_validation_stack_layer_iterator_next(stack, &iterator))) {
		++pos->count;
		if (jso_schema_validation_array_append(stack, pos) == JSO_FAILURE) {
			if (jso_schema_validation_stream_should_terminate(stack->root_schema, pos)) {
				return JSO_FAILURE;
			}
			jso_schema_validation_result_propagate(pos);
		}
	}

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_array_end(jso_schema_validation_stream *stream)
{
	// currently there is nothing to do
	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_value(
		jso_schema_validation_stream *stream, jso_value *instance)
{
	jso_schema_validation_stack_layer_iterator iterator;
	jso_schema_validation_position *pos;
	jso_schema_validation_stack *stack = JSO_STREAM_VALIDATION_STREAM_STACK_P(stream);
	jso_schema *schema = stack->root_schema;

	jso_value_type instance_type = JSO_TYPE_P(instance);
	if (instance_type != JSO_TYPE_ARRAY) {
		jso_schema_validation_stack_layer_iterator_start(stack, &iterator);
		while ((pos = jso_schema_validation_stack_layer_iterator_next(stack, &iterator))) {
			if (instance_type == JSO_TYPE_OBJECT) {
				if (jso_schema_validation_object_pre_value(stack, pos, instance) == JSO_FAILURE) {
					if (jso_schema_validation_stream_should_terminate(schema, pos)) {
						return JSO_FAILURE;
					}
					jso_schema_validation_result_propagate(pos);
				}
			} else if (jso_schema_validation_composition_check(stack, pos) == JSO_FAILURE) {
				return JSO_FAILURE;
			}
		}
	}
	jso_schema_validation_stack_layer_reverse_iterator_start(stack, &iterator);
	while ((pos = jso_schema_validation_stack_layer_reverse_iterator_next(stack, &iterator))) {
		if (!pos->is_final_validation_result) {
			jso_schema_value *value = pos->current_value;
			pos->validation_result = jso_schema_validation_value(schema, value, instance);
			if (jso_schema_validation_stream_should_terminate(schema, pos)) {
				return JSO_FAILURE;
			}
			jso_schema_validation_result_propagate(pos);
		}
	}
	jso_schema_validation_stack_layer_remove(stack);

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_final_result(jso_schema_validation_stream *stream)
{
	jso_schema_validation_position *pos = jso_schema_validation_stack_root_position(
			JSO_STREAM_VALIDATION_STREAM_STACK_P(stream));

	if (!pos->is_final_validation_result) {
		return JSO_FAILURE;
	}
	return pos->validation_result;
}