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

#include "jso_schema_validation_error.h"
#include "jso_schema_validation_object.h"
#include "jso_schema_validation_result.h"
#include "jso_schema_validation_stack.h"

#include "jso_schema_error.h"
#include "jso_schema_keyword.h"
#include "jso_schema_value.h"

#include "jso_re.h"
#include "jso.h"

jso_rc jso_schema_validation_object_key(
		jso_schema_validation_stack *stack, jso_schema_validation_position *pos, jso_string *key)
{
	if (pos->is_final_validation_result) {
		return JSO_SUCCESS;
	}

	jso_schema *schema = stack->root_schema;
	jso_schema_value *value = pos->current_value;
	jso_schema_value_object *objval = JSO_SCHEMA_VALUE_DATA_OBJ_P(value);

	if (JSO_SCHEMA_KW_IS_SET(objval->max_properties)) {
		jso_uint kw_uval = JSO_SCHEMA_KEYWORD_DATA_UINT(objval->max_properties);
		size_t objlen = pos->count;
		if (objlen > kw_uval) {
			jso_schema_validation_set_final_result(pos, JSO_FAILURE);
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Object number of properties is %zu which is greater than maximum number of "
					"properties %lu",
					objlen, kw_uval);
			return JSO_FAILURE;
		}
	}

	bool found = false;

	if (JSO_SCHEMA_KW_IS_SET(objval->properties)) {
		jso_value *result;
		jso_object *props = JSO_SCHEMA_KEYWORD_DATA_OBJ_SCHEMA_OBJ(objval->max_properties);
		if (jso_object_get(props, key, &result) == JSO_SUCCESS) {
			JSO_ASSERT_EQ(JSO_TYPE_P(result), JSO_TYPE_SCHEMA_VALUE);
			if (jso_schema_validation_stack_push_basic(stack, JSO_SVVAL_P(result), pos) == NULL) {
				return JSO_FAILURE;
			}
			found = true;
		}
	}

	if (JSO_SCHEMA_KW_IS_SET(objval->pattern_properties)) {
		jso_string *key;
		jso_value *val;
		jso_object *pattern_props = JSO_SCHEMA_KEYWORD_DATA_OBJ_SCHEMA_OBJ(objval->max_properties);
		JSO_OBJECT_FOREACH(pattern_props, key, val)
		{
			JSO_ASSERT_EQ(JSO_TYPE_P(val), JSO_TYPE_SCHEMA_VALUE);
			jso_schema_value *schema_value = JSO_SVVAL_P(val);
			jso_re_code *code = JSO_SCHEMA_VALUE_REGEXP_P(schema_value);
			JSO_ASSERT_NOT_NULL(code);
			jso_re_match_data *match_data = jso_re_match_data_create(code);
			int match_result = jso_re_match(key, code, match_data);
			jso_re_match_data_free(match_data);
			if (match_result > 0) {
				if (jso_schema_validation_stack_push_basic(stack, schema_value, pos) == NULL) {
					return JSO_FAILURE;
				}
				found = true;
			}
		}
		JSO_OBJECT_FOREACH_END;
	}

	if (!found && JSO_SCHEMA_KW_IS_SET(objval->additional_properties)
			&& !JSO_SCHEMA_KEYWORD_DATA_BOOL(objval->additional_properties)) {
		jso_schema_validation_set_final_result(pos, JSO_FAILURE);
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
				"Object does not allow additional properties but added property with key %s which "
				"is is not found in properties or matches any pattern property",
				JSO_STRING_VAL(key));
		return JSO_FAILURE;
	}

	return JSO_SUCCESS;
}

jso_rc jso_schema_validation_object_pre_value(jso_schema_validation_stack *stack,
		jso_schema_validation_position *pos, jso_value *instance)
{
	jso_schema *schema = stack->root_schema;

	if (JSO_TYPE_P(instance) != JSO_TYPE_OBJECT) {
		jso_schema_validation_set_final_result(pos, JSO_FAILURE);
		return jso_schema_validation_value_type_error(
				schema, JSO_TYPE_OBJECT, JSO_TYPE_P(instance));
	}

	jso_schema_value_object *objval = JSO_SCHEMA_VALUE_DATA_OBJ_P(pos->current_value);

	if (JSO_SCHEMA_KW_IS_SET(objval->dependencies)) {
		jso_string *key;
		jso_value *val;
		jso_object *dependencies = JSO_SCHEMA_KEYWORD_DATA_OBJ_SCHEMA_OBJ(objval->dependencies);
		jso_object *instance_obj = JSO_OBJVAL_P(instance);
		jso_schema_validation_stack_mark(stack);
		JSO_OBJECT_FOREACH(dependencies, key, val)
		{
			if (JSO_TYPE_P(val) == JSO_TYPE_ARRAY) {
				jso_value *item;
				JSO_ARRAY_FOREACH(JSO_ARRVAL_P(val), item)
				{
					JSO_ASSERT_EQ(JSO_TYPE_P(item), JSO_TYPE_STRING);
					if (!jso_object_has(instance_obj, JSO_STR_P(item))) {
						jso_schema_validation_set_final_result(pos, JSO_FAILURE);
						jso_schema_validation_stack_reset(stack);
						jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
								"Object key %s is required by dependency %s but it is not present",
								JSO_SVAL_P(item), JSO_STRING_VAL(key));
						return JSO_FAILURE;
					}
				}
				JSO_ARRAY_FOREACH_END;
			} else {
				JSO_ASSERT_EQ(JSO_TYPE_P(val), JSO_TYPE_SCHEMA_VALUE);
				if (jso_schema_validation_stack_push_basic(stack, JSO_SVVAL_P(val), pos) == NULL) {
					return JSO_FAILURE;
				}
			}
		}
		JSO_OBJECT_FOREACH_END;
	}

	return JSO_SUCCESS;
}

jso_rc jso_schema_validation_object_value(
		jso_schema *schema, jso_schema_value *value, jso_value *instance)
{
	jso_schema_value_object *objval = JSO_SCHEMA_VALUE_DATA_OBJ_P(value);

	if (JSO_SCHEMA_KW_IS_SET(objval->min_properties)) {
		jso_uint kw_uval = JSO_SCHEMA_KEYWORD_DATA_UINT(objval->min_properties);
		size_t objlen = JSO_OBJECT_COUNT(JSO_OBJVAL_P(instance));
		if (objlen < kw_uval) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Object number of properties is %zu which is lower than minimum number of "
					"properties %lu",
					objlen, kw_uval);
			return JSO_FAILURE;
		}
	}

	if (JSO_SCHEMA_KW_IS_SET(objval->required)) {
		jso_value *item;
		jso_object *instance_object = JSO_OBJVAL_P(instance);
		JSO_ARRAY_FOREACH(JSO_SCHEMA_KEYWORD_DATA_ARR_STR(objval->required), item)
		{
			if (!jso_object_has(instance_object, JSO_STR_P(item))) {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
						"Object does not have requier property with key %s", JSO_SVAL_P(item));
				return JSO_FAILURE;
			}
		}
		JSO_ARRAY_FOREACH_END;
	}

	return JSO_SUCCESS;
}
