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

#define JSO_SCHEMA_KEYWORD_COMMON_DEFAULT 1
#define JSO_SCHEMA_KEYWORD_COMMON_DESCRIPTION 2
#define JSO_SCHEMA_KEYWORD_COMMON_TITLE 3

#define JSO_SCHEMA_VALUE_COMMON_FIELDS_NO_VALUE() \
	jso_bitset keywords; \
	jso_string title; \
	jso_string description;

#define JSO_SCHEMA_VALUE_COMMON_FIELDS(_value_type) \
	JSO_SCHEMA_VALUE_COMMON_FIELDS_NO_VALUE(); \
	_value_type default_value

typedef struct _jso_schema_value_null {
	JSO_SCHEMA_VALUE_COMMON_FIELDS_NO_VALUE();
} jso_schema_value_null;

typedef struct _jso_schema_value_boolean {
	JSO_SCHEMA_VALUE_COMMON_FIELDS(jso_bool);
} jso_schema_value_boolean;

#define JSO_SCHEMA_KEYWORD_NUMBER_MULTIPLE_OF 10
#define JSO_SCHEMA_KEYWORD_NUMBER_MINIMUM 11
#define JSO_SCHEMA_KEYWORD_NUMBER_MAXIMUM 12
#define JSO_SCHEMA_KEYWORD_NUMBER_EXCLUSIVE_MINIMUM 13
#define JSO_SCHEMA_KEYWORD_NUMBER_EXCLUSIVE_MAXIMUM 14

typedef struct _jso_schema_value_number {
	JSO_SCHEMA_VALUE_COMMON_FIELDS(jso_number);
	jso_uint multiple_of;
	jso_int minimum;
	jso_int maximum;
	jso_bool exclusive_minimum;
	jso_bool exclusive_maximum;
} jso_schema_value_number;

#define JSO_SCHEMA_KEYWORD_STRING_MAX_LENGTH 10
#define JSO_SCHEMA_KEYWORD_STRING_MIN_LENGTH 11

typedef struct _jso_schema_value_string {
	JSO_SCHEMA_VALUE_COMMON_FIELDS(jso_bool);
	jso_uint max_length;
	jso_uint min_length;
	// TODO: support pattern
} jso_schema_value_string;

#define JSO_SCHEMA_KEYWORD_ARRAY_ADDITIONAL_ITEMS 10
#define JSO_SCHEMA_KEYWORD_ARRAY_UNIQUE_ITEMS 11
#define JSO_SCHEMA_KEYWORD_ARRAY_ITEMS 12
#define JSO_SCHEMA_KEYWORD_ARRAY_MAX_ITEMS 13
#define JSO_SCHEMA_KEYWORD_ARRAY_MIN_ITEMS 14

typedef struct _jso_schema_value_array {
	JSO_SCHEMA_VALUE_COMMON_FIELDS(jso_array *);
	jso_bool additional_items; // TODO: support object type alternative
	jso_bool unique_items;
	jso_array *items; // TODO: support object type alternative
	jso_uint max_items;
	jso_uint min_items;
} jso_schema_value_array;

#define JSO_SCHEMA_KEYWORD_OBJECT_ADDITIONAL_PROPERTIES 10
#define JSO_SCHEMA_KEYWORD_OBJECT_MAX_PROPERTIES 11
#define JSO_SCHEMA_KEYWORD_OBJECT_MIN_PROPERTIES 12
#define JSO_SCHEMA_KEYWORD_OBJECT_REQUIRED 13
#define JSO_SCHEMA_KEYWORD_OBJECT_PROPERTIES 14

typedef struct _jso_schema_value_object {
	JSO_SCHEMA_VALUE_COMMON_FIELDS(jso_object *);
	jso_bool additional_properties;
	jso_uint max_properties;
	jso_uint min_properties;
	jso_array *required;
	jso_ht *properties;
	// TODO: support pattern_properties
	// TODO: support dependencies
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
	JSO_TYPE_STRING,
	/** array value type */
	JSO_TYPE_ARRAY,
	/** object value type */
	JSO_TYPE_OBJECT
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
	/** array value (reference) */
	jso_schema_value_array *arrva;
	/** object value (reference) */
	jso_schema_value_object *objval;
} jso_schema_value_data;

/**
 * @brief JsonSchema value data and type.
 */
typedef struct _jso_schema_value {
	/** value data */
	jso_schema_value_data data;
	/** value type */
	jso_schema_value_type type;
} jso_schema_value;

/**
 * @brief JsonSchema main structure.
 */
typedef struct _jso_schema {
	jso_schema_value *root;
	jso_string id;
} jso_schema;

#endif /* JSO_SCHEMA_H */
