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

#include "jso_schema_array.h"
#include "jso_schema_data.h"
#include "jso_schema_error.h"
#include "jso_schema_keyword.h"
#include "jso_schema_keyword_freer.h"
#include "jso_schema_value.h"

#include "jso.h"

jso_schema_value *jso_schema_value_alloc(jso_schema *schema, const char *type_name)
{
	jso_schema_value *value = jso_calloc(1, sizeof(jso_schema_value));
	if (value == NULL) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_ALLOC,
				"Allocating value for type %s failed", type_name);
		return NULL;
	}

	return value;
}

jso_schema_value *jso_schema_value_init(jso_schema *schema, jso_value *data,
		jso_schema_value *parent, const char *type_name, size_t value_size,
		jso_schema_value_type value_type)
{
	jso_schema_value *value = jso_schema_value_alloc(schema, type_name);
	if (value == NULL) {
		return NULL;
	}

	jso_schema_value_common *value_data = jso_calloc(1, value_size);
	if (value_data == NULL) {
		jso_free(value);
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_ALLOC,
				"Allocating value data for type %s failed", type_name);
		return NULL;
	}
	value_data->parent = parent;

	// set default keyword
	JSO_SCHEMA_KW_SET_ANY_EX(schema, data, default, value, value_data, default_value);

	// set metadata keywords
	JSO_SCHEMA_KW_SET_STR(schema, data, description, value, value_data);
	JSO_SCHEMA_KW_SET_STR(schema, data, title, value, value_data);

	// set other common keywords
	JSO_SCHEMA_KW_SET_ARR_EX(schema, data, enum, value, value_data, enum_elements);
	JSO_SCHEMA_KW_SET_ARR_OF_SCHEMA_OBJS_EX(schema, data, allOf, value, value_data, all_of);
	JSO_SCHEMA_KW_SET_ARR_OF_SCHEMA_OBJS_EX(schema, data, anyOf, value, value_data, any_of);
	JSO_SCHEMA_KW_SET_ARR_OF_SCHEMA_OBJS_EX(schema, data, oneOf, value, value_data, one_of);
	JSO_SCHEMA_KW_SET_SCHEMA_OBJ(schema, data, not, value, value_data);
	JSO_SCHEMA_KW_SET_OBJ_OF_SCHEMA_OBJS(schema, data, definitions, value, value_data);

	JSO_SCHEMA_VALUE_DATA_COMMON_P(value) = value_data;
	JSO_SCHEMA_VALUE_TYPE_P(value) = value_type;

	return value;
}
