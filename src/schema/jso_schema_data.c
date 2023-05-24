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

#include "jso.h"

jso_value *jso_schema_data_get_value_fast(
		jso_schema *schema, jso_value *data, const char *key, jso_uint32 keyword_flags)
{
	jso_value *val;

	if (jso_ht_get_by_cstr_key(JSO_OBJHVAL_P(data), key, &val) == JSO_FAILURE) {
		if (keyword_flags & JSO_SCHEMA_KEYWORD_FLAG_REQUIRED) {
			jso_schema_error_format(
					schema, JSO_SCHEMA_ERROR_KEYWORD_REQUIRED, "Keyword %s is required", key);
		}
		return NULL;
	}

	return val;
}

jso_rc jso_schema_data_check_type(jso_schema *schema, jso_value *data, const char *key,
		jso_value *val, jso_value_type type, jso_value_type primary_type,
		jso_bool error_on_invalid_type)
{
	if ((JSO_TYPE_P(val) & type) == 0) {
		if (error_on_invalid_type) {
			if (type != primary_type) {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_DATA_TYPE,
						"Invalid type for %s - expected either %s or %s but given %s", key,
						jso_value_type_to_string(primary_type), jso_value_type_to_string(type),
						jso_value_type_to_string(JSO_TYPE_P(val)));
			} else {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_DATA_TYPE,
						"Invalid type for %s - expected %s but given %s", key,
						jso_value_type_to_string(type), jso_value_type_to_string(JSO_TYPE_P(val)));
			}
		}
		return JSO_FAILURE;
	}

	return JSO_SUCCESS;
}

jso_value *jso_schema_data_get(jso_schema *schema, jso_value *data, const char *key,
		jso_value_type type, jso_uint32 keyword_flags, jso_bool error_on_invalid_type,
		jso_value *val)
{
	val = jso_schema_data_get_value(schema, data, key, keyword_flags, val);
	if (val == NULL) {
		return NULL;
	}

	return jso_schema_data_check_type(schema, data, key, val, type, type, error_on_invalid_type)
					== JSO_SUCCESS
			? val
			: NULL;
}