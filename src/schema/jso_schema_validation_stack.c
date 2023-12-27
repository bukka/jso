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
#include "jso_schema_validation_stack.h"

jso_rc jso_schema_validation_stack_init(jso_schema_validation_stack *stack, size_t capacity)
{
	stack->positions = jso_malloc(capacity * sizeof(jso_schema_validation_position));
	if (stack->positions == NULL) {
		return JSO_FAILURE;
	}
	stack->capacity = capacity;
	stack->size = 0;

	return JSO_SUCCESS;
}

void jso_schema_validation_stack_clear(jso_schema_validation_stack *stack)
{
	jso_free(stack->positions);
}

static jso_rc jso_schema_validation_stack_resize_if_needed(jso_schema_validation_stack *stack)
{
	size_t capacity = stack->capacity;
	if (stack->size < capacity) {
		return JSO_SUCCESS;
	}

	size_t new_capacity = capacity * 2;
	jso_schema_validation_position *positions
			= jso_realloc(stack->positions, sizeof(jso_schema_validation_position) * new_capacity);
	if (stack->positions == NULL) {
		return JSO_FAILURE;
	}
	stack->positions = positions;
	stack->capacity = new_capacity;
	return JSO_SUCCESS;
}

static inline jso_schema_validation_position *jso_schema_validation_stack_next(
		jso_schema_validation_stack *stack)
{
	jso_schema_validation_position *position = &stack->positions[stack->size];
	// clear position before use
	memset(position, 0, sizeof(jso_schema_validation_position));

	return position;
}

jso_schema_validation_position *jso_schema_validation_stack_push_basic(
		jso_schema_validation_stack *stack, jso_schema_value *current_value,
		jso_schema_validation_position *parent)
{
	if (jso_schema_validation_stack_resize_if_needed(stack) == JSO_FAILURE) {
		return NULL;
	}

	jso_schema_validation_position *next = jso_schema_validation_stack_next(stack);
	next->current_value = current_value;
	next->parent = parent;

	return next;
}

jso_schema_validation_position *jso_schema_validation_stack_pop(jso_schema_validation_stack *stack)
{
	if (stack->size == 0) {
		return NULL;
	}

	return &stack->positions[--stack->size];
}

void jso_schema_validation_stack_layer_iterator_start(
		jso_schema_validation_stack *stack, jso_schema_validation_stack_layer_iterator *iterator)
{
	JSO_ASSERT_GT(stack->size, 0);
	jso_schema_validation_position *last_position = &stack->positions[stack->size - 1];
	iterator->start = iterator->index = last_position->layer_start;
}

jso_schema_validation_position *jso_schema_validation_stack_layer_iterator_next(
		jso_schema_validation_stack *stack, jso_schema_validation_stack_layer_iterator *iterator)
{
	if (iterator->index + 1 >= stack->size) {
		return NULL;
	}
	jso_schema_validation_position *pos = &stack->positions[++iterator->index];
	if (JSO_SCHEMA_VALIDATION_POSITION_IS_SENTINEL(pos)) {
		return NULL;
	}
	return pos;
}
