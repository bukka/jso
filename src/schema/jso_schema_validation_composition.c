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

#include "jso_schema_validation_composition.h"
#include "jso_schema_validation_stream.h"

#include "jso_schema_array.h"
#include "jso_schema_keyword.h"
#include "jso_schema_value.h"

#include "jso.h"

static jso_rc jso_schema_validation_composition_push_keyword_schema_objects_ex(
		jso_schema_validation_stream *stream, jso_schema_validation_position *pos,
		jso_schema_keyword *keyword, jso_schema_validation_composition_type composition_type)
{
	if (!JSO_SCHEMA_KEYWORD_IS_PRESENT_P(keyword)) {
		return JSO_SUCCESS;
	}
	jso_schema_value *value;
	jso_schema_array *array = JSO_SCHEMA_KEYWORD_DATA_ARR_SCHEMA_OBJ_P(keyword);
	JSO_SCHEMA_ARRAY_FOREACH(array, value)
	{
		if (jso_schema_validation_stream_stack_push_composed(stream, value, pos, composition_type)
				== NULL) {
			return JSO_FAILURE;
		}
	}
	JSO_SCHEMA_ARRAY_FOREACH_END;

	return JSO_SUCCESS;
}

static inline jso_rc jso_schema_validation_composition_push_keyword_schema_objects(
		jso_schema_validation_stream *stream, jso_schema_validation_position *pos,
		jso_schema_keyword *keyword, jso_schema_validation_composition_type composition_type)
{
	if (!JSO_SCHEMA_KEYWORD_IS_PRESENT_P(keyword)) {
		return JSO_SUCCESS;
	}

	return jso_schema_validation_composition_push_keyword_schema_objects_ex(
			stream, pos, keyword, composition_type);
}

static inline jso_rc jso_schema_validation_composition_push_keyword_schema_object(
		jso_schema_validation_stream *stream, jso_schema_validation_position *pos,
		jso_schema_keyword *keyword, jso_schema_validation_composition_type composition_type)
{
	if (!JSO_SCHEMA_KEYWORD_IS_PRESENT_P(keyword)) {
		return JSO_SUCCESS;
	}

	jso_schema_value *value = JSO_SCHEMA_KEYWORD_DATA_SCHEMA_OBJ_P(keyword);
	if (jso_schema_validation_stream_stack_push_composed(stream, value, pos, composition_type)
			== NULL) {
		return JSO_FAILURE;
	}

	return JSO_SUCCESS;
}

jso_rc jso_schema_validation_composition_check(
		jso_schema_validation_stream *stream, jso_schema_validation_position *pos)
{
	jso_schema_value_common *data = JSO_SCHEMA_VALUE_DATA_COMMON_P(pos->current_value);

	if (jso_schema_validation_composition_push_keyword_schema_objects(
				stream, pos, &data->all_of, JSO_SCHEMA_VALIDATION_COMPOSITION_ALL)
			== JSO_FAILURE) {
		return JSO_FAILURE;
	}
	if (jso_schema_validation_composition_push_keyword_schema_objects(
				stream, pos, &data->any_of, JSO_SCHEMA_VALIDATION_COMPOSITION_ANY)
			== JSO_FAILURE) {
		return JSO_FAILURE;
	}
	if (jso_schema_validation_composition_push_keyword_schema_objects(
				stream, pos, &data->one_of, JSO_SCHEMA_VALIDATION_COMPOSITION_ONE)
			== JSO_FAILURE) {
		return JSO_FAILURE;
	}
	if (jso_schema_validation_composition_push_keyword_schema_object(
				stream, pos, &data->not, JSO_SCHEMA_VALIDATION_COMPOSITION_NOT)
			== JSO_FAILURE) {
		return JSO_FAILURE;
	}

	return JSO_SUCCESS;
}