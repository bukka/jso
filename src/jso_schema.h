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

/**
 * @file jso_schema.h
 * @brief JsonSchema header
 */

#ifndef JSO_SCHEMA_H
#define JSO_SCHEMA_H

#include "jso_types.h"
#include "jso_ht.h"
#include "jso_bitset.h"

typedef struct _jso_schema_value jso_schema_value;

/**
 * @brief Common keyword default bit set ID.
 */
#define JSO_SCHEMA_KEYWORD_COMMON_DEFAULT 1

/**
 * @brief Common keyword description bit set ID.
 */
#define JSO_SCHEMA_KEYWORD_COMMON_DESCRIPTION 2

/**
 * @brief Common keyword title bit set ID.
 */
#define JSO_SCHEMA_KEYWORD_COMMON_TITLE 3

/**
 * @brief Common schema feilds without default value.
 */
#define JSO_SCHEMA_VALUE_COMMON_FIELDS_NO_VALUE() \
	/** parent pointer */ \
	jso_schema_value *parent; \
	/** keywords mask to see what has been set */ \
	jso_bitset keywords; \
	/** title keyword */ \
	jso_string title; \
	/** description keyword */ \
	jso_string description;

/**
 * @brief Common schema feilds with default value.
 * @param _value_type type of default value
 */
#define JSO_SCHEMA_VALUE_COMMON_FIELDS(_value_type) \
	JSO_SCHEMA_VALUE_COMMON_FIELDS_NO_VALUE(); \
	/** default keyword */ \
	_value_type default_value

/**
 * @brief JsonSchema null validation keywords.
 */
typedef struct _jso_schema_value_null {
	JSO_SCHEMA_VALUE_COMMON_FIELDS_NO_VALUE();
} jso_schema_value_null;

/**
 * @brief JsonSchema boolean validation keywords.
 */
typedef struct _jso_schema_value_boolean {
	JSO_SCHEMA_VALUE_COMMON_FIELDS(jso_bool);
} jso_schema_value_boolean;

/**
 * @brief Number keyword multipleOf bit set ID.
 */
#define JSO_SCHEMA_KEYWORD_NUMBER_MULTIPLE_OF 10

/**
 * @brief Number keyword minimum bit set ID.
 */
#define JSO_SCHEMA_KEYWORD_NUMBER_MINIMUM 11

/**
 * @brief Number keyword maximum bit set ID.
 */
#define JSO_SCHEMA_KEYWORD_NUMBER_MAXIMUM 12

/**
 * @brief Number keyword exclusiveMinimum bit set ID.
 */
#define JSO_SCHEMA_KEYWORD_NUMBER_EXCLUSIVE_MINIMUM 13

/**
 * @brief Number keyword exclusiveMaximum bit set ID.
 */
#define JSO_SCHEMA_KEYWORD_NUMBER_EXCLUSIVE_MAXIMUM 14

/**
 * @brief JsonSchema number validation keywords.
 */
typedef struct _jso_schema_value_number {
	JSO_SCHEMA_VALUE_COMMON_FIELDS(jso_number);
	/** multipleOf keyword */
	jso_uint multiple_of;
	/** minimum keyword */
	jso_int minimum;
	/** maximum keyword */
	jso_int maximum;
	/** exclusiveMinimum keyword */
	jso_bool exclusive_minimum;
	/** exclusiveMaximum keyword */
	jso_bool exclusive_maximum;
} jso_schema_value_number;

/**
 * @brief String keyword maxLength bit set ID.
 */
#define JSO_SCHEMA_KEYWORD_STRING_MAX_LENGTH 10

/**
 * @brief String keyword minLength bit set ID.
 */
#define JSO_SCHEMA_KEYWORD_STRING_MIN_LENGTH 11

/**
 * @brief JsonSchema string validation keywords.
 * @todo support pattern
 */
typedef struct _jso_schema_value_string {
	JSO_SCHEMA_VALUE_COMMON_FIELDS(jso_bool);
	/** maxLength keyword */
	jso_uint max_length;
	/** minLength keyword */
	jso_uint min_length;
} jso_schema_value_string;

/**
 * @brief Array keyword additionalItems bit set ID.
 */
#define JSO_SCHEMA_KEYWORD_ARRAY_ADDITIONAL_ITEMS 10

/**
 * @brief Array keyword uniqueItems bit set ID.
 */
#define JSO_SCHEMA_KEYWORD_ARRAY_UNIQUE_ITEMS 11

/**
 * @brief Array keyword items bit set ID.
 */
#define JSO_SCHEMA_KEYWORD_ARRAY_ITEMS 12

/**
 * @brief Array keyword maxItems bit set ID.
 */
#define JSO_SCHEMA_KEYWORD_ARRAY_MAX_ITEMS 13

/**
 * @brief Array keyword minItems bit set ID.
 */
#define JSO_SCHEMA_KEYWORD_ARRAY_MIN_ITEMS 14

/**
 * @brief JsonSchema array validation keywords.
 * @todo support object type alternative for additional_items and items
 */
typedef struct _jso_schema_value_array {
	JSO_SCHEMA_VALUE_COMMON_FIELDS(jso_array *);
	/** additionalItems keyword */
	jso_bool additional_items;
	/** uniqueItems keyword */
	jso_bool unique_items;
	/** items keyword */
	jso_array *items;
	/** maxItems keyword */
	jso_uint max_items;
	/** minItems keyword */
	jso_uint min_items;
} jso_schema_value_array;

/**
 * @brief Object keyword additionalProperties bit set ID.
 */
#define JSO_SCHEMA_KEYWORD_OBJECT_ADDITIONAL_PROPERTIES 10

/**
 * @brief Object keyword maxProperties bit set ID.
 */
#define JSO_SCHEMA_KEYWORD_OBJECT_MAX_PROPERTIES 11

/**
 * @brief Object keyword minProperties bit set ID.
 */
#define JSO_SCHEMA_KEYWORD_OBJECT_MIN_PROPERTIES 12

/**
 * @brief Object keyword required bit set ID.
 */
#define JSO_SCHEMA_KEYWORD_OBJECT_REQUIRED 13

/**
 * @brief Object keyword properties bit set ID.
 */
#define JSO_SCHEMA_KEYWORD_OBJECT_PROPERTIES 14

/**
 * @brief JsonSchema array validation keywords.
 * @todo support pattern_properties and dependencies
 */
typedef struct _jso_schema_value_object {
	JSO_SCHEMA_VALUE_COMMON_FIELDS(jso_object *);
	/** additionalProperties keyword */
	jso_bool additional_properties;
	/** maxProperties keyword */
	jso_uint max_properties;
	/** minProperties keyword */
	jso_uint min_properties;
	/** requiredProperties keyword */
	jso_array *required;
	/** properties keyword */
	jso_ht *properties;
} jso_schema_value_object;

/**
 * @brief JsonSchema value type.
 */
typedef enum {
	/** null value type */
	JSO_SCHEMA_TYPE_NULL,
	/** boolean value type */
	JSO_SCHEMA_TYPE_BOOL,
	/** numeric value type */
	JSO_SCHEMA_TYPE_NUMBER,
	/** string value type */
	JSO_SCHEMA_TYPE_STRING,
	/** array value type */
	JSO_SCHEMA_TYPE_ARRAY,
	/** object value type */
	JSO_SCHEMA_TYPE_OBJECT
} jso_schema_value_type;

/**
 * @brief JsonSchema value data.
 *
 * The current size of the value data is equal to the largest
 * element which is pointer size.
 */
typedef union _jso_schema_value_data {
	/** null value */
	jso_schema_value_null *nulval;
	/** boolean value */
	jso_schema_value_boolean *boolval;
	/** number value */
	jso_schema_value_number *numval;
	/** string value */
	jso_schema_value_string *strval;
	/** array value */
	jso_schema_value_array *arrva;
	/** object value */
	jso_schema_value_object *objval;
} jso_schema_value_data;

/**
 * @brief JsonSchema value data and type.
 */
struct _jso_schema_value {
	/** value data */
	jso_schema_value_data data;
	/** value type */
	jso_schema_value_type type;
};

/**
 * @brief Schema version.
 */
typedef enum _jso_schema_version { JSO_SCHEMA_VERSION_DRAFT_04 } jso_schema_version;

/**
 * @brief Schema error type.
 */
typedef enum _jso_schema_error_type {
	JSO_SCHEMA_ERROR_NONE = 0,
	JSO_SCHEMA_ERROR_VERSION,
	JSO_SCHEMA_ERROR_ID,
} jso_schema_error_type;

/**
 * @brief JsonSchema error.
 */
typedef struct _jso_schema_error {
	/** error message */
	jso_string *message;
	/** error type */
	jso_schema_error_type type;
} jso_schema_error;

/**
 * @brief JsonSchema main structure.
 */
typedef struct _jso_schema {
	/** root value */
	jso_schema_value *root;
	/** schema ID */
	jso_string id;
	/** schema version */
	jso_schema_version version;
	/** schema error */
	jso_schema_error error;
} jso_schema;

/**
 * Allocate new schema.
 *
 * @param data json source data
 * @return New schema instance or NULL on error.
 */
JSO_API jso_schema *jso_schema_alloc();

/**
 * Initialize schema.
 *
 * @param schema schema to initialize
 */
JSO_API void jso_schema_init(jso_schema *schema);

/**
 * Parse data to set up the schema.
 *
 * @param schema schema to parse data to
 * @param data source data
 * @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_schema_parse(jso_schema *schema, jso_value *data);

/**
 * Clear schema.
 *
 * @param schema schema to clear
 */
JSO_API void jso_schema_clear(jso_schema *schema);

/**
 * Free schema.
 *
 * @param schema schema to free
 */
JSO_API void jso_schema_free(jso_schema *schema);

#endif /* JSO_SCHEMA_H */
