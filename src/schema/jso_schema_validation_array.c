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

#include "jso_schema_validation_array.h"
#include "jso_schema_validation_error.h"
#include "jso_schema_validation_result.h"
#include "jso_schema_validation_stack.h"

#include "jso_schema_array.h"
#include "jso_schema_error.h"
#include "jso_schema_keyword.h"

#include "jso.h"

static inline jso_schema_validation_result jso_schema_validation_array_push_value(
		jso_schema_validation_stack *stack, jso_schema_value *current_value,
		jso_schema_validation_position *pos)
{
	return jso_schema_validation_stack_push_basic(stack, current_value, pos) == NULL
			? JSO_SCHEMA_VALIDATION_ERROR
			: JSO_SCHEMA_VALIDATION_VALID;
}

static jso_schema_value *jso_schema_validation_array_find_item(jso_schema_validation_stack *stack,
		jso_schema_validation_position *pos, jso_schema_value_array *arrval)
{
	if (JSO_SCHEMA_KEYWORD_TYPE(arrval->items) == JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT) {
		return JSO_SCHEMA_KEYWORD_DATA_SCHEMA_OBJ(arrval->items);
	}
	JSO_ASSERT_EQ(JSO_SCHEMA_KEYWORD_TYPE(arrval->items),
			JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS);
	jso_schema_array *items = JSO_SCHEMA_KEYWORD_DATA_ARR_SCHEMA_OBJ(arrval->items);
	size_t index = pos->count;
	return jso_schema_array_get(items, index);
}

jso_schema_validation_result jso_schema_validation_array_start(
		jso_schema_validation_stack *stack, jso_schema_validation_position *pos)
{
	jso_schema_value *value = pos->current_value;
	jso_schema_value_array *arrval = JSO_SCHEMA_VALUE_DATA_ARR_P(value);

	if (pos->is_final_validation_result) {
		return JSO_SCHEMA_VALIDATION_VALID;
	}

	if (JSO_SCHEMA_KW_IS_SET(arrval->items)) {
		jso_schema_value *item = jso_schema_validation_array_find_item(stack, pos, arrval);
		if (item != NULL) {
			return jso_schema_validation_array_push_value(stack, item, pos);
		}
		if (JSO_SCHEMA_KW_IS_SET(arrval->additional_items)
				&& JSO_SCHEMA_KEYWORD_TYPE(arrval->additional_items)
						== JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT) {
			return jso_schema_validation_array_push_value(
					stack, JSO_SCHEMA_KEYWORD_DATA_SCHEMA_OBJ(arrval->additional_items), pos);
		}
	}

	return JSO_SCHEMA_VALIDATION_VALID;
}

jso_schema_validation_result jso_schema_validation_array_append(
		jso_schema_validation_stack *stack, jso_schema_validation_position *pos)
{
	jso_schema *schema = stack->root_schema;
	jso_schema_value *value = pos->current_value;
	jso_schema_value_array *arrval = JSO_SCHEMA_VALUE_DATA_ARR_P(value);

	if (pos->is_final_validation_result) {
		return JSO_SCHEMA_VALIDATION_VALID;
	}

	if (JSO_SCHEMA_KW_IS_SET(arrval->max_items)) {
		jso_uint max_items = JSO_SCHEMA_KEYWORD_DATA_UINT(arrval->max_items);
		size_t arrlen = pos->count;
		if (arrlen > max_items) {
			jso_schema_validation_set_final_result(pos, JSO_SCHEMA_VALIDATION_INVALID);
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Array number of items is %zu which is greater than max number of items %lu",
					arrlen, max_items);
			return JSO_SCHEMA_VALIDATION_INVALID;
		}
	}

	if (JSO_SCHEMA_KW_IS_SET(arrval->items)) {
		jso_schema_value *item = jso_schema_validation_array_find_item(stack, pos, arrval);
		if (item != NULL) {
			return jso_schema_validation_array_push_value(stack, item, pos);
		}
		if (JSO_SCHEMA_KW_IS_SET(arrval->additional_items)) {
			if (JSO_SCHEMA_KEYWORD_TYPE(arrval->additional_items)
					== JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT) {
				return jso_schema_validation_array_push_value(
						stack, JSO_SCHEMA_KEYWORD_DATA_SCHEMA_OBJ(arrval->additional_items), pos);
			}
			JSO_ASSERT_EQ(JSO_SCHEMA_KEYWORD_TYPE(arrval->additional_items),
					JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
			if (!JSO_SCHEMA_KEYWORD_DATA_BOOL(arrval->additional_items)) {
				jso_schema_array *items = JSO_SCHEMA_KEYWORD_DATA_ARR_SCHEMA_OBJ(arrval->items);
				JSO_ASSERT_GT(pos->count, 0);
				if (jso_schema_array_get(items, pos->count - 1) == NULL) {
					jso_schema_validation_set_final_result(pos, JSO_SCHEMA_VALIDATION_INVALID);
					jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
							"Array additional items are not allowed and number of items is lower");
					return JSO_SCHEMA_VALIDATION_INVALID;
				}
			}
		}
	}

	return JSO_SCHEMA_VALIDATION_VALID;
}

jso_schema_validation_result jso_schema_validation_array_value(
		jso_schema *schema, jso_schema_value *value, jso_value *instance)
{
	if (JSO_TYPE_P(instance) != JSO_TYPE_ARRAY) {
		return jso_schema_validation_value_type_error(schema, JSO_TYPE_ARRAY, JSO_TYPE_P(instance));
	}

	jso_schema_value_array *arrval = JSO_SCHEMA_VALUE_DATA_ARR_P(value);

	if (JSO_SCHEMA_KW_IS_SET(arrval->min_items)) {
		jso_uint kw_uval = JSO_SCHEMA_KEYWORD_DATA_UINT(arrval->min_items);
		size_t arrlen = JSO_ARRAY_LEN(JSO_ARRVAL_P(instance));
		if (arrlen < kw_uval) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Array number of items is %zu which is lower than minimum number of items %lu",
					arrlen, kw_uval);
			return JSO_SCHEMA_VALIDATION_INVALID;
		}
	}

	if (JSO_SCHEMA_KW_IS_SET(arrval->unique_items)
			&& JSO_SCHEMA_KEYWORD_DATA_BOOL(arrval->unique_items)
			&& !jso_array_is_unique(JSO_ARRVAL_P(instance))) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD, "Array is not unique");
		return JSO_SCHEMA_VALIDATION_INVALID;
	}

	return JSO_SCHEMA_VALIDATION_VALID;
}