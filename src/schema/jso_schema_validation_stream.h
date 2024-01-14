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

/**
 * @file jso_schema_validation_stream.h
 * @brief JsonSchema validation stream private functionality.
 */

#ifndef JSO_SCHEMA_VALIDATION_STREAM_H
#define JSO_SCHEMA_VALIDATION_STREAM_H

#include "jso_schema.h"
#include "jso_schema_validation_stack.h"

static inline jso_rc jso_schema_validation_stream_stack_init(
		jso_schema_validation_stream *stream, size_t capacity)
{
	return jso_schema_validation_stack_init(&stream->stack, capacity);
}

static inline jso_schema_validation_position *jso_schema_validation_stream_stack_push_basic(
		jso_schema_validation_stream *stream, jso_schema_value *current_value,
		jso_schema_validation_position *parent)
{
	return jso_schema_validation_stack_push_basic(&stream->stack, current_value, parent);
}

static inline jso_schema_validation_position *jso_schema_validation_stream_stack_push_composed(
		jso_schema_validation_stream *stream, jso_schema_value *current_value,
		jso_schema_validation_position *parent,
		jso_schema_validation_composition_type composition_type)
{
	return jso_schema_validation_stack_push_composed(
			&stream->stack, current_value, parent, composition_type);
}

static inline jso_schema_validation_position *jso_schema_validation_stream_stack_push_separator(
		jso_schema_validation_stream *stream)
{
	return jso_schema_validation_stack_push_separator(&stream->stack);
}

static inline jso_bool jso_schema_validation_stream_should_terminate(
		jso_schema *schema, jso_schema_validation_position *pos)
{
	return pos->validation_result == JSO_FAILURE && jso_schema_error_is_fatal(schema);
}

#endif /* JSO_SCHEMA_VALIDATION_STACK_H */
