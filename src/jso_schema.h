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
 * @brief Array of schema values.
 */
typedef struct _jso_schema_array_of_values {
	/** array length */
	size_t len;
	/** array size */
	size_t capacity;
	/** array value */
	jso_schema_value *values[1];
} jso_schema_array_of_values;

/**
 * @brief JsonSchema keyword data.
 *
 * The current size of the data is equal to the largest element which is pointer
 * size.
 */
typedef union _jso_schema_keyword_data {
	/** boolean value */
	jso_bool bval;
	/** integer value */
	jso_int ival;
	/** unsigned integer value */
	jso_uint uval;
	/** double value */
	jso_double dval;
	/** string value */
	jso_string *sval;
	/** array of string values */
	jso_array *asval;
	/** array of schema object values */
	jso_schema_array_of_values *asoval;
	/** schema object value */
	jso_schema_value *soval;
	/** schema object */
	jso_object *osoval;
} jso_schema_keyword_data;

/**
 * @brief JsonSchema keyword type.
 */
typedef enum {
	/** null keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_NULL = 0x0001,
	/** boolean keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN = 0x0002,
	/** integer keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_INTEGER = 0x0004,
	/** integer keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER = 0x0008,
	/** integer keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_NUMBER = 0x0010,
	/** string keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_STRING = 0x0020,
	/** string array keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_ARRAY = 0x0040,
	/** string array keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_STRINGS = 0x0080,
	/** schema object array keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS = 0x0200,
	/** object keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_OBJECT = 0x0800,
	/** schema object keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT = 0x1000,
	/** object of schema objects keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS = 0x2000
} jso_schema_keyword_type;

#define JSO_SCHEMA_KEYWORD_TYPE_LAST JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS

/**
 * @brief Flag stating the presence of keyword.
 */
#define JSO_SCHEMA_KEYWORD_FLAG_PRESENT 0x01

/**
 * @brief Flag specifying that the value must not be empty.
 *
 * An example can be non empty array.
 */
#define JSO_SCHEMA_KEYWORD_FLAG_NOT_EMPTY 0x02

/**
 * @brief Flag specifying that the value is not zero.
 *
 * This is mainly applicable to number unsigned integer. This is an alias for not empty flag.
 */
#define JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO 0x02

/**
 * @brief Flag specifying that the value must be unique.
 *
 * This is mainly applicable to array.
 */
#define JSO_SCHEMA_KEYWORD_FLAG_UNIQUE 0x04

/**
 * @brief Flag specifying that the number is a floating number.
 *
 * This is mainly applicable to number.
 */
#define JSO_SCHEMA_KEYWORD_FLAG_FLOATING 0x08

/**
 * @brief JsonSchema keyword.
 *
 * Union values represent the JsonSchema keywords that can have more than one
 * type. The size is 128 bits on 64bit platform and 96 bits on 32bit platforms.
 */
typedef struct _jso_schema_keyword {
	jso_schema_keyword_data data;
	jso_schema_keyword_type type;
	jso_uint32 flags;
} jso_schema_keyword;

/**
 * @brief Common schema feilds without default value.
 */
#define JSO_SCHEMA_VALUE_COMMON_FIELDS_NO_VALUE() \
	/** parent pointer */ \
	jso_schema_value *parent; \
	/** title keyword */ \
	jso_schema_keyword title; \
	/** description keyword */ \
	jso_schema_keyword description

/**
 * @brief Common schema feilds with default value.
 * @param _value_type type of default value
 */
#define JSO_SCHEMA_VALUE_COMMON_FIELDS() \
	JSO_SCHEMA_VALUE_COMMON_FIELDS_NO_VALUE(); \
	/** default keyword */ \
	jso_schema_keyword default_value

/**
 * @brief JsonSchema common validation keywords.
 */
typedef struct _jso_schema_value_common {
	JSO_SCHEMA_VALUE_COMMON_FIELDS_NO_VALUE();
} jso_schema_value_common;

/**
 * @brief JsonSchema null validation keywords.
 */
typedef jso_schema_value_common jso_schema_value_null;

/**
 * @brief JsonSchema boolean validation keywords.
 */
typedef struct _jso_schema_value_boolean {
	JSO_SCHEMA_VALUE_COMMON_FIELDS();
} jso_schema_value_boolean;

/**
 * @brief JsonSchema integer validation keywords.
 */
typedef struct _jso_schema_value_integer {
	JSO_SCHEMA_VALUE_COMMON_FIELDS();
	/** multipleOf keyword */
	jso_schema_keyword multiple_of;
	/** minimum keyword */
	jso_schema_keyword minimum;
	/** maximum keyword */
	jso_schema_keyword maximum;
	/** exclusiveMinimum keyword */
	jso_schema_keyword exclusive_minimum;
	/** exclusiveMaximum keyword */
	jso_schema_keyword exclusive_maximum;
} jso_schema_value_integer;

/**
 * @brief JsonSchema number validation keywords.
 */
typedef struct _jso_schema_value_number {
	JSO_SCHEMA_VALUE_COMMON_FIELDS();
	/** multipleOf keyword */
	jso_schema_keyword multiple_of;
	/** minimum keyword */
	jso_schema_keyword minimum;
	/** maximum keyword */
	jso_schema_keyword maximum;
	/** exclusiveMinimum keyword */
	jso_schema_keyword exclusive_minimum;
	/** exclusiveMaximum keyword */
	jso_schema_keyword exclusive_maximum;
} jso_schema_value_number;

/**
 * @brief JsonSchema string validation keywords.
 * @todo support pattern
 */
typedef struct _jso_schema_value_string {
	JSO_SCHEMA_VALUE_COMMON_FIELDS();
	/** maxLength keyword */
	jso_schema_keyword max_length;
	/** minLength keyword */
	jso_schema_keyword min_length;
} jso_schema_value_string;

/**
 * @brief JsonSchema array validation keywords.
 */
typedef struct _jso_schema_value_array {
	JSO_SCHEMA_VALUE_COMMON_FIELDS();
	/** additionalItems keyword */
	jso_schema_keyword additional_items;
	/** items keyword */
	jso_schema_keyword items;
	/** uniqueItems keyword */
	jso_schema_keyword unique_items;
	/** maxItems keyword */
	jso_schema_keyword max_items;
	/** minItems keyword */
	jso_schema_keyword min_items;
} jso_schema_value_array;

/**
 * @brief JsonSchema array validation keywords.
 * @todo support pattern_properties and dependencies
 */
typedef struct _jso_schema_value_object {
	JSO_SCHEMA_VALUE_COMMON_FIELDS();
	/** additionalProperties keyword */
	jso_schema_keyword *additional_properties;
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
	JSO_SCHEMA_VALUE_TYPE_NULL,
	/** boolean value type */
	JSO_SCHEMA_VALUE_TYPE_BOOLEAN,
	/** integer value type */
	JSO_SCHEMA_VALUE_TYPE_INTEGER,
	/** numeric value type */
	JSO_SCHEMA_VALUE_TYPE_NUMBER,
	/** string value type */
	JSO_SCHEMA_VALUE_TYPE_STRING,
	/** array value type */
	JSO_SCHEMA_VALUE_TYPE_ARRAY,
	/** object value type */
	JSO_SCHEMA_VALUE_TYPE_OBJECT
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
	/** integer value */
	jso_schema_value_integer *intval;
	/** number value */
	jso_schema_value_number *numval;
	/** string value */
	jso_schema_value_string *strval;
	/** array value */
	jso_schema_value_array *arrval;
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
	JSO_SCHEMA_ERROR_ID,
	JSO_SCHEMA_ERROR_KEYWORD_REQUIRED,
	JSO_SCHEMA_ERROR_ROOT_DATA_TYPE,
	JSO_SCHEMA_ERROR_TYPE_UNKNOWN,
	JSO_SCHEMA_ERROR_VALUE_ALLOC,
	JSO_SCHEMA_ERROR_VALUE_DATA_ALLOC,
	JSO_SCHEMA_ERROR_VALUE_DATA_DEPS,
	JSO_SCHEMA_ERROR_VALUE_DATA_TYPE,
	JSO_SCHEMA_ERROR_VERSION,
} jso_schema_error_type;

/**
 * @brief JsonSchema error.
 */
typedef struct _jso_schema_error {
	/** error message */
	const char *message;
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
	jso_string *id;
	/** schema version */
	jso_schema_version version;
	/** schema error */
	jso_schema_error error;
	/** schema initialized */
	jso_bool is_initialized;
} jso_schema;

/**
 * Get JsonSchema error type.
 *
 * @param _schema shema of type @ref jso_schema
 */
#define JSO_SCHEMA_ERROR_TYPE(_schema) _schema->error.type

/**
 * Get JsonSchema error message.
 *
 * @param _schema shema of type @ref jso_schema
 */
#define JSO_SCHEMA_ERROR_MESSAGE(_schema) _schema->error.message

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
