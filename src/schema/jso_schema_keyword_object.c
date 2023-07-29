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

#include "jso_schema_data.h"
#include "jso_schema_error.h"
#include "jso_schema_keyword.h"
#include "jso_schema_keyword_array.h"
#include "jso_schema_keyword_object.h"
#include "jso_schema_keyword_regexp.h"
#include "jso_schema_value.h"

#include "jso.h"

#include <math.h>

jso_schema_keyword *jso_schema_keyword_get_schema_object(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get(
			schema, data, key, JSO_TYPE_OBJECT, keyword_flags, error_on_invalid_type, val);
	if (val != NULL) {
		jso_schema_value *schema_value = jso_schema_value_parse(schema, val, parent);
		if (schema_value != NULL) {
			JSO_SCHEMA_KEYWORD_DATA_SCHEMA_OBJ_P(schema_keyword) = schema_value;
			return schema_keyword;
		}
	}

	return NULL;
}

static inline jso_schema_keyword *jso_schema_keyword_get_custom_object(jso_schema *schema,
		jso_value *data, const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent,
		jso_bool regexp_key, jso_bool can_be_array_of_strings)
{
	val = jso_schema_data_get(
			schema, data, key, JSO_TYPE_OBJECT, keyword_flags, error_on_invalid_type, val);
	if (val == NULL) {
		return NULL;
	}
	jso_value objval;
	jso_string *objkey;
	jso_value *item;
	jso_object *obj = JSO_OBJVAL_P(val);
	jso_object *schema_obj = jso_object_alloc();
	jso_object_resize(schema_obj, JSO_OBJECT_COUNT(obj));
	JSO_OBJECT_FOREACH(obj, objkey, item)
	{
		if (can_be_array_of_strings && JSO_TYPE_P(item) == JSO_TYPE_ARRAY) {
			jso_array *arr = JSO_ARRVAL_P(val);
			if (jso_schema_keyword_validate_array_of_strings(schema, key, arr, keyword_flags)
					== JSO_FAILURE) {
				return NULL;
			}
			JSO_VALUE_SET_ARRAY(objval, jso_array_copy(arr));
		} else {
			if (JSO_TYPE_P(item) != JSO_TYPE_OBJECT) {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_DATA_TYPE,
						"Object value for keyword %s must be a schema object%s", key,
						can_be_array_of_strings ? " or array of strings" : "");
				jso_object_free(schema_obj);
				return NULL;
			}
			jso_schema_value *schema_value = jso_schema_value_parse(schema, item, parent);
			if (schema_value == NULL) {
				jso_object_free(schema_obj);
				return NULL;
			}

			if (regexp_key) {
				jso_re_code *code = jso_schema_keyword_get_regexp_code(schema, key, objkey, val);
				if (code == NULL) {
					jso_schema_value_free(schema_value);
					jso_object_free(schema_obj);
					return NULL;
				}

				JSO_SCHEMA_VALUE_REGEXP_P(schema_value) = code;
			}

			JSO_VALUE_SET_SCHEMA_VALUE(objval, schema_value);
		}

		jso_object_add(schema_obj, jso_string_copy(objkey), &objval);
	}
	JSO_OBJECT_FOREACH_END;
	JSO_SCHEMA_KEYWORD_DATA_OBJ_SCHEMA_OBJ_P(schema_keyword) = schema_obj;
	return schema_keyword;
}

jso_schema_keyword *jso_schema_keyword_get_object_of_schema_objects(jso_schema *schema,
		jso_value *data, const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	return jso_schema_keyword_get_custom_object(schema, data, key, error_on_invalid_type,
			keyword_flags, schema_keyword, val, parent, false, false);
}

jso_schema_keyword *jso_schema_keyword_get_object_of_schema_objects_or_array_of_strings(
		jso_schema *schema, jso_value *data, const char *key, jso_bool error_on_invalid_type,
		jso_uint32 keyword_flags, jso_schema_keyword *schema_keyword, jso_value *val,
		jso_schema_value *parent)
{
	return jso_schema_keyword_get_custom_object(schema, data, key, error_on_invalid_type,
			keyword_flags, schema_keyword, val, parent, false, true);
}

jso_schema_keyword *jso_schema_keyword_get_regexp_object_of_schema_objects(jso_schema *schema,
		jso_value *data, const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	return jso_schema_keyword_get_custom_object(schema, data, key, error_on_invalid_type,
			keyword_flags, schema_keyword, val, parent, true, false);
}

jso_schema_keyword *jso_schema_keyword_get_object(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get(
			schema, data, key, JSO_TYPE_OBJECT, keyword_flags, error_on_invalid_type, val);
	if (val == NULL) {
		return NULL;
	}
	JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) = keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_DATA_OBJ_P(schema_keyword) = jso_object_copy(JSO_OBJVAL_P(val));
	return schema_keyword;
}

void jso_schema_keyword_free_object(jso_schema_keyword *schema_keyword)
{
	jso_object_free(JSO_SCHEMA_KEYWORD_DATA_OBJ_P(schema_keyword));
}

void jso_schema_keyword_free_schema_object(jso_schema_keyword *schema_keyword)
{
	jso_schema_value_free(JSO_SCHEMA_KEYWORD_DATA_SCHEMA_OBJ_P(schema_keyword));
}

void jso_schema_keyword_free_object_of_schema_objects(jso_schema_keyword *schema_keyword)
{
	jso_object_free(JSO_SCHEMA_KEYWORD_DATA_OBJ_SCHEMA_OBJ_P(schema_keyword));
}