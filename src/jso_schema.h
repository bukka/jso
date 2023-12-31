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
#include "jso_re.h"
#include "jso_bitset.h"

/**
 * @brief Array of schema values.
 */
typedef struct _jso_schema_array {
	/** array length */
	size_t len;
	/** array size */
	size_t capacity;
	/** array value */
	jso_schema_value *values[1];
} jso_schema_array;

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
	/** regular expression value */
	jso_re_code *reval;
	/** array of any values */
	jso_array *aval;
	/** array of string values */
	jso_array *asval;
	/** array of schema object values */
	jso_schema_array *asoval;
	/** schema object value */
	jso_schema_value *soval;
	/** object of schema objects */
	jso_object *osoval;
	/** any object */
	jso_object *oval;
	/** any value */
	jso_value *vval;
} jso_schema_keyword_data;

/**
 * @brief JsonSchema keyword type.
 */
typedef enum {
	/** any keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_ANY,
	/** null keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_NULL,
	/** boolean keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN,
	/** integer keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_INTEGER,
	/** integer keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER,
	/** double floating number keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_DOUBLE,
	/** integer keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_NUMBER,
	/** string keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_STRING,
	/** regular expression keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_REGEXP,
	/** string array keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_ARRAY,
	/** string array keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_STRINGS,
	/** schema object array keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS,
	/** object keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_OBJECT,
	/** schema object keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT,
	/** object of schema objects keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS,
	/** object of schema objects or array of strings keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS_OR_ARRAY_OF_STRINGS,
	/** object with regural expression keys of schema objects keyword type */
	JSO_SCHEMA_KEYWORD_TYPE_REGEXP_OBJECT_OF_SCHEMA_OBJECTS
} jso_schema_keyword_type;

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
 * @brief Flag specifying that the value is required.
 */
#define JSO_SCHEMA_KEYWORD_FLAG_REQUIRED 0x04

/**
 * @brief Flag specifying that the value must be unique.
 *
 * This is mainly applicable to array.
 */
#define JSO_SCHEMA_KEYWORD_FLAG_UNIQUE 0x08

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
 * Get keyword boolean data for the supplied pointer to keyword.
 *
 * @param _pkw pointer to keyword
 * @return Boolean keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_BOOL_P(_pkw) (_pkw)->data.bval

/**
 * Get keyword integer data for the supplied pointer to keyword.
 *
 * @param _pkw pointer to keyword
 * @return Integer keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_INT_P(_pkw) (_pkw)->data.ival

/**
 * Get keyword unsigned integer data for the supplied pointer to keyword.
 *
 * @param _pkw pointer to keyword
 * @return Unsigned integer keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_UINT_P(_pkw) (_pkw)->data.uval

/**
 * Get keyword double data for the supplied pointer to keyword.
 *
 * @param _pkw pointer to keyword
 * @return Double keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_DOUBLE_P(_pkw) (_pkw)->data.dval

/**
 * Get keyword string data for the supplied pointer to keyword.
 *
 * @param _pkw pointer to keyword
 * @return String keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_STR_P(_pkw) (_pkw)->data.sval

/**
 * Get keyword regular expression code for the supplied pointer to keyword.
 *
 * @param _pkw pointer to keyword
 * @return Regular expression code keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_RE_P(_pkw) (_pkw)->data.reval

/**
 * Get keyword array data for the supplied pointer to keyword.
 *
 * @param _pkw pointer to keyword
 * @return Array keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_ARR_P(_pkw) (_pkw)->data.aval

/**
 * Get keyword array of strings data for the supplied pointer to keyword.
 *
 * @param _pkw pointer to keyword
 * @return Array of string keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_ARR_STR_P(_pkw) (_pkw)->data.asval

/**
 * Get keyword array of schema objects data for the supplied pointer to keyword.
 *
 * @param _pkw pointer to keyword
 * @return Array of schema objects keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_ARR_SCHEMA_OBJ_P(_pkw) (_pkw)->data.asoval

/**
 * Get keyword schema object data for the supplied pointer to keyword.
 *
 * @param _pkw pointer to keyword
 * @return Schema object keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_SCHEMA_OBJ_P(_pkw) (_pkw)->data.soval

/**
 * Get keyword object of schema objects data for the supplied pointer to keyword.
 *
 * @param _pkw pointer to keyword
 * @return Object of schema objects keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_OBJ_SCHEMA_OBJ_P(_pkw) (_pkw)->data.osoval

/**
 * Get keyword object data for the supplied pointer to keyword.
 *
 * @param _pkw pointer to keyword
 * @return Object keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_OBJ_P(_pkw) (_pkw)->data.oval

/**
 * Get any keyword data for the supplied pointer to keyword.
 *
 * @param _pkw pointer to keyword
 * @return Any keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_ANY_P(_pkw) (_pkw)->data.vval

/**
 * Get keyword boolean data for the supplied keyword.
 *
 * @param _kw keyword
 * @return Boolean keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_BOOL(_kw) _kw.data.bval

/**
 * Get keyword integer data for the supplied keyword.
 *
 * @param _kw keyword
 * @return Integer keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_INT(_kw) _kw.data.ival

/**
 * Get keyword unsigned integer data for the supplied keyword.
 *
 * @param _kw keyword
 * @return Unsigned integer keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_UINT(_kw) _kw.data.uval

/**
 * Get keyword double data for the supplied keyword.
 *
 * @param _kw keyword
 * @return Double keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_DOUBLE(_kw) _kw.data.dval

/**
 * Get keyword string data for the supplied keyword.
 *
 * @param _kw keyword
 * @return String keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_STR(_kw) _kw.data.sval

/**
 * Get keyword regular expression code for the supplied keyword.
 *
 * @param _kw keyword
 * @return Regular expression code keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_RE(_kw) _kw.data.reval

/**
 * Get keyword array data for the supplied keyword.
 *
 * @param _kw keyword
 * @return Array keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_ARR(_kw) _kw.data.aval

/**
 * Get keyword array of strings data for the supplied keyword.
 *
 * @param _kw keyword
 * @return Array of string keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_ARR_STR(_kw) _kw.data.asval

/**
 * Get keyword array of schema objects data for the supplied keyword.
 *
 * @param _kw keyword
 * @return Array of schema objects keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_ARR_SCHEMA_OBJ(_kw) _kw.data.asoval

/**
 * Get keyword schema object data for the supplied keyword.
 *
 * @param _kw keyword
 * @return Schema object kenumvalyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_SCHEMA_OBJ(_kw) _kw.data.soval

/**
 * Get keyword object of schema objects data for the supplied keyword.
 *
 * @param _kw keyword
 * @return Object of schema objects keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_OBJ_SCHEMA_OBJ(_kw) _kw.data.osoval

/**
 * Get keyword object data for the supplied keyword.
 *
 * @param _kw keyword
 * @return Object keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_OBJ(_kw) _kw.data.oval

/**
 * Get any keyword data for the supplied keyword.
 *
 * @param _kw keyword
 * @return Any keyword data value.
 */
#define JSO_SCHEMA_KEYWORD_DATA_ANY(_kw) _kw.data.vval

/**
 * Get schema keyword type for the supplied schema keyword pointer.
 *
 * @param _pkw schema keyword pointer
 * @return The schema keyword type of @ref jso_schema_keyword_type
 */
#define JSO_SCHEMA_KEYWORD_TYPE_P(_pkw) _pkw->type

/**
 * Get schema keyword type for the supplied schema keyword.
 *
 * @param _kw schema keyword
 * @return The schema keyword type of @ref jso_schema_keyword_type
 */
#define JSO_SCHEMA_KEYWORD_TYPE(_kw) _kw.type

/**
 * Get schema keyword flags for the supplied schema keyword pointer.
 *
 * @param _pkw schema keyword pointer
 * @return The schema keyword type of @ref jso_schema_keyword_type
 */
#define JSO_SCHEMA_KEYWORD_FLAGS_P(_pkw) _pkw->flags

/**
 * Get schema keyword flags for the supplied schema keyword.
 *
 * @param _kw schema keyword
 * @return The schema keyword type of @ref jso_schema_keyword_type
 */
#define JSO_SCHEMA_KEYWORD_FLAGS(_kw) _kw.flags

/**
 * Check present flag for the supplied schema keyword pointer.
 *
 * @param _pkw schema keyword pointer
 * @return JSO_TRUE if keyword is present, otherwise JSO_FALSE
 */
#define JSO_SCHEMA_KEYWORD_IS_PRESENT_P(_pkw) (_pkw->flags & JSO_SCHEMA_KEYWORD_FLAG_PRESENT)

/**
 * Check present flag for the supplied schema keyword.
 *
 * @param _kw schema keyword
 * @return JSO_TRUE if keyword is present, otherwise JSO_FALSE
 */
#define JSO_SCHEMA_KEYWORD_IS_PRESENT(_kw) (_kw.flags & JSO_SCHEMA_KEYWORD_FLAG_PRESENT)

/**
 * @brief Common schema feilds without default value.
 */
#define JSO_SCHEMA_VALUE_COMMON_FIELDS() \
	/** parent pointer */ \
	jso_schema_value *parent; \
	/** regural expression if used by patternProperties */ \
	jso_re_code *re; \
	/** enum keyword */ \
	jso_schema_keyword enum_elements; \
	/** allOf keyword */ \
	jso_schema_keyword all_of; \
	/** anyOf keyword */ \
	jso_schema_keyword any_of; \
	/** oneOf keyword */ \
	jso_schema_keyword one_of; \
	/** not keyword */ \
	jso_schema_keyword not ; \
	/** definitions keyword */ \
	jso_schema_keyword definitions; \
	/** title keyword */ \
	jso_schema_keyword title; \
	/** description keyword */ \
	jso_schema_keyword description; \
	/** default keyword */ \
	jso_schema_keyword default_value

/**
 * @brief JsonSchema common validation keywords.
 */
typedef struct _jso_schema_value_common {
	JSO_SCHEMA_VALUE_COMMON_FIELDS();
} jso_schema_value_common;

/**
 * @brief JsonSchema any validation keywords.
 */
typedef jso_schema_value_common jso_schema_value_any;

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
	/** pattern keyword */
	jso_schema_keyword pattern;
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
	jso_schema_keyword additional_properties;
	/** maxProperties keyword */
	jso_schema_keyword max_properties;
	/** minProperties keyword */
	jso_schema_keyword min_properties;
	/** requiredProperties keyword */
	jso_schema_keyword required;
	/** properties keyword */
	jso_schema_keyword properties;
	/** pattern properties keyword */
	jso_schema_keyword pattern_properties;
	/** dependencies keyword */
	jso_schema_keyword dependencies;
} jso_schema_value_object;

/**
 * @brief JsonSchema value type.
 */
typedef enum {
	/** any value type */
	JSO_SCHEMA_VALUE_TYPE_ANY = 0,
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
	/** common value pointer */
	jso_schema_value_common *comval;
	/** any value */
	jso_schema_value_any *anyval;
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
 * @brief Flag specifying that all value keywords are not set.
 */
#define JSO_SCHEMA_VALUE_FLAG_NOT_EMPTY 0x01

/**
 * @brief JsonSchema value data and type.
 */
struct _jso_schema_value {
	/** value data */
	jso_schema_value_data data;
	/** value type */
	jso_schema_value_type type;
	/** value flags */
	jso_uint32 flags;
};

/**
 * Get common schema value data for the supplied pointer to schema value.
 *
 * @param _psv pointer to schema value
 * @return Common schema value data.
 */
#define JSO_SCHEMA_VALUE_DATA_COMMON_P(_psv) (_psv)->data.comval

/**
 * Get any schema value data for the supplied pointer to schema value.
 *
 * @param _psv pointer to schema value
 * @return Any schema value data.
 */
#define JSO_SCHEMA_VALUE_DATA_ANY_P(_psv) (_psv)->data.anyval

/**
 * Get null schema value data for the supplied pointer to schema value.
 *
 * @param _psv pointer to schema value
 * @return Null schema value data.
 */
#define JSO_SCHEMA_VALUE_DATA_NULL_P(_psv) (_psv)->data.nulval

/**
 * Get boolean schema value data for the supplied pointer to schema value.
 *
 * @param _psv pointer to schema value
 * @return Boolean schema value data.
 */
#define JSO_SCHEMA_VALUE_DATA_BOOL_P(_psv) (_psv)->data.boolval

/**
 * Get integer schema value data for the supplied pointer to schema value.
 *
 * @param _psv pointer to schema value
 * @return Integer schema value data.
 */
#define JSO_SCHEMA_VALUE_DATA_INT_P(_psv) (_psv)->data.intval

/**
 * Get number schema value data for the supplied pointer to schema value.
 *
 * @param _psv pointer to schema value
 * @return Number schema value data.
 */
#define JSO_SCHEMA_VALUE_DATA_NUM_P(_psv) (_psv)->data.numval

/**
 * Get string schema value data for the supplied pointer to schema value.
 *
 * @param _psv pointer to schema value
 * @return String schema value data.
 */
#define JSO_SCHEMA_VALUE_DATA_STR_P(_psv) (_psv)->data.strval

/**
 * Get array schema value data for the supplied pointer to schema value.
 *
 * @param _psv pointer to schema value
 * @return Array schema value data.
 */
#define JSO_SCHEMA_VALUE_DATA_ARR_P(_psv) (_psv)->data.arrval

/**
 * Get object schema value data for the supplied pointer to schema value.
 *
 * @param _psv pointer to schema value
 * @return Object schema value data.
 */
#define JSO_SCHEMA_VALUE_DATA_OBJ_P(_psv) (_psv)->data.objval

/**
 * Get common schema value data for the supplied schema value.
 *
 * @param _sv schema value
 * @return Common schema value data.
 */
#define JSO_SCHEMA_VALUE_DATA_COMMON(_sv) _sv.data.comval

/**
 * Get any schema value data for the supplied schema value.
 *
 * @param _sv schema value
 * @return Any schema value data.
 */
#define JSO_SCHEMA_VALUE_DATA_ANY(_sv) _sv.data.anyval

/**
 * Get null schema value data for the supplied schema value.
 *
 * @param _sv schema value
 * @return Null schema value data.
 */
#define JSO_SCHEMA_VALUE_DATA_NULL(_sv) _sv.data.nulval

/**
 * Get boolean schema value data for the supplied schema value.
 *
 * @param _sv schema value
 * @return Boolean schema value data.
 */
#define JSO_SCHEMA_VALUE_DATA_BOOL(_sv) _sv.data.boolval

/**
 * Get integer schema value data for the supplied schema value.
 *
 * @param _sv schema value
 * @return Integer schema value data.
 */
#define JSO_SCHEMA_VALUE_DATA_INT(_sv) _sv.data.intval

/**
 * Get number schema value data for the supplied schema value.
 *
 * @param _sv schema value
 * @return Number schema value data.
 */
#define JSO_SCHEMA_VALUE_DATA_NUM(_sv) _sv.data.numval

/**
 * Get string schema value data for the supplied schema value.
 *
 * @param _sv schema value
 * @return String schema value data.
 */
#define JSO_SCHEMA_VALUE_DATA_STR(_sv) _sv.data.strval

/**
 * Get array schema value data for the supplied schema value.
 *
 * @param _sv schema value
 * @return Array schema value data.
 */
#define JSO_SCHEMA_VALUE_DATA_ARR(_sv) _sv.data.arrval

/**
 * Get object schema value data for the supplied schema value.
 *
 * @param _sv schema value
 * @return Object schema value data.
 */
#define JSO_SCHEMA_VALUE_DATA_OBJ(_sv) _sv.data.objval

/**
 * Get schema value regular expression for the supplied schema value pointer.
 *
 * @param _psv schema value pointer
 * @return The schema regular expression of type @ref jso_re_code.
 */
#define JSO_SCHEMA_VALUE_REGEXP_P(_psv) JSO_SCHEMA_VALUE_DATA_COMMON_P(_psv)->re

/**
 * Get schema value regular expression for the supplied schema value.
 *
 * @param _sv schema value
 * @return The schema regular expression of type @ref jso_re_code.
 */
#define JSO_SCHEMA_VALUE_REGEXP(_sv) JSO_SCHEMA_VALUE_DATA_COMMON(_sv)->re

/**
 * Get schema value type for the supplied schema value pointer.
 *
 * @param _psv schema value pointer
 * @return The schema keyword type of @ref jso_schema_value_type.
 */
#define JSO_SCHEMA_VALUE_TYPE_P(_psv) _psv->type

/**
 * Get schema value type for the supplied schema value.
 *
 * @param _sv schema value
 * @return The schema value type of @ref jso_schema_value_type.
 */
#define JSO_SCHEMA_VALUE_TYPE(_sv) _sv.type

/**
 * Get schema value flags for the supplied schema value pointer.
 *
 * @param _psv schema value pointer
 * @return The schema value flags
 */
#define JSO_SCHEMA_VALUE_FLAGS_P(_psv) _psv->flags

/**
 * Get schema value flags for the supplied schema value.
 *
 * @param _sv schema value
 * @return The schema value flags
 */
#define JSO_SCHEMA_VALUE_FLAGS(_psv) _psv.flags

/**
 * Check whether not empty flag is set from the supplied schema value pointer.
 *
 * @param _psv schema value pointer
 * @return JSO_TRUE if value is not empty, otherwise JSO_FALSE
 */
#define JSO_SCHEMA_VALUE_IS_NOT_EMPTY_P(_psv) (_psv->flags & JSO_SCHEMA_VALUE_FLAG_NOT_EMPTY)

/**
 * Check whether not empty flag is set from the supplied schema value.
 *
 * @param _sv schema value
 * @return JSO_TRUE if value is not empty, otherwise JSO_FALSE
 */
#define JSO_SCHEMA_VALUE_IS_NOT_EMPTY(_sv) (_sv.flags & JSO_SCHEMA_VALUE_FLAG_NOT_EMPTY)

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
	JSO_SCHEMA_ERROR_KEYWORD_TYPE,
	JSO_SCHEMA_ERROR_ROOT_DATA_TYPE,
	JSO_SCHEMA_ERROR_TYPE_INVALID,
	JSO_SCHEMA_ERROR_VALUE_ALLOC,
	JSO_SCHEMA_ERROR_VALUE_DATA_ALLOC,
	JSO_SCHEMA_ERROR_VALUE_DATA_DEPS,
	JSO_SCHEMA_ERROR_VALUE_DATA_TYPE,
	JSO_SCHEMA_ERROR_KEYWORD_ALLOC,
	JSO_SCHEMA_ERROR_KEYWORD_PREP,
	JSO_SCHEMA_ERROR_VALIDATION_ALLOC,
	JSO_SCHEMA_ERROR_VALIDATION_TYPE,
	JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
	JSO_SCHEMA_ERROR_VERSION,
} jso_schema_error_type;

/**
 * @brief JsonSchema error.
 */
typedef struct _jso_schema_error {
	/** error message */
	char *message;
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
#define JSO_SCHEMA_ERROR_TYPE(_schema) (_schema)->error.type

/**
 * Get JsonSchema error message.
 *
 * @param _schema shema of type @ref jso_schema
 */
#define JSO_SCHEMA_ERROR_MESSAGE(_schema) (_schema)->error.message

/**
 * Check if schema error is set.
 *
 * @param schema shema of type @ref jso_schema
 * @return JSO_TRUE if error set, otherwise JSO_FALSE
 */
static inline bool jso_schema_error_is_set(jso_schema *schema)
{
	return JSO_SCHEMA_ERROR_TYPE(schema) != JSO_SCHEMA_ERROR_NONE;
}

/**
 * Check if schema error is a fatal error that requires quick termination.
 *
 * @param schema shema of type @ref jso_schema
 * @return JSO_TRUE if fatal error, otherwise JSO_FALSE
 */
static inline bool jso_schema_error_is_fatal(jso_schema *schema)
{
	jso_schema_error_type type = JSO_SCHEMA_ERROR_TYPE(schema);
	return (type == JSO_SCHEMA_ERROR_VALUE_ALLOC || type == JSO_SCHEMA_ERROR_VALUE_DATA_ALLOC
			|| type == JSO_SCHEMA_ERROR_KEYWORD_ALLOC || type == JSO_SCHEMA_ERROR_VALIDATION_ALLOC);
}

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
 * @return JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
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

/**
 * Free schema value.
 *
 * @param val scheam value
 */
JSO_API void jso_schema_value_free(jso_schema_value *val);

/**
 * @brief Schema validation position type.
 */
typedef enum _jso_schema_validation_position_type {
	JSO_SCHEMA_VALIDATION_POSITION_BASIC = 0,
	JSO_SCHEMA_VALIDATION_POSITION_COMPOSED,
	JSO_SCHEMA_VALIDATION_POSITION_SENTINEL,
} jso_schema_validation_position_type;

typedef struct _jso_schema_validation_position jso_schema_validation_position;

/**
 * @brief Check if validation position type is a sentinel
 */
#define JSO_SCHEMA_VALIDATION_POSITION_IS_SENTINEL(_pos) \
	(_pos->type == JSO_SCHEMA_VALIDATION_POSITION_SENTINEL)

/**
 * @brief JsonSchema validation position structure used during the stream validation.
 */
struct _jso_schema_validation_position {
	/** position type */
	jso_schema_validation_position_type type;
	/** validation result */
	jso_rc validation_result;
	/** schema parent validation position */
	jso_schema_validation_position *parent;
	/** schema value currently processed */
	jso_schema_value *current_value;
	/** schema object key if object is being processed */
	jso_string *object_key;
	/** schema array index if array is being processed or size of object if object is being
	 * processed */
	size_t index;
	/** start of the current layer */
	size_t layer_start;
};

/**
 * @brief JsonSchema validation stack.
 */
typedef struct _jso_schema_validation_stack {
	/** validation positions */
	jso_schema_validation_position *positions;
	/** last separator position */
	jso_schema_validation_position *last_separator;
	/** allocated capacity */
	size_t capacity;
	/** used stack size */
	size_t size;
} jso_schema_validation_stack;

/**
 * @brief JsonSchema validation stream structure.
 */
typedef struct _jso_schema_validation_stream {
	/** root schema, TODO: move to stack so allocation errors can be reported */
	jso_schema *root_schema;
	/** validation stack of positions */
	jso_schema_validation_stack stack;
} jso_schema_validation_stream;

JSO_API jso_rc jso_schema_validate(jso_schema *schema, jso_value *instance);

JSO_API jso_rc jso_schema_validation_stream_init(
		jso_schema *schema, jso_schema_validation_stream *stream, size_t positions_capacity);

JSO_API void jso_schema_validation_stream_clear(jso_schema_validation_stream *stream);

JSO_API jso_rc jso_schema_validation_stream_object_start(jso_schema_validation_stream *stream);

JSO_API jso_rc jso_schema_validation_stream_object_key(
		jso_schema_validation_stream *stream, jso_string *str);

JSO_API jso_rc jso_schema_validation_stream_object_update(jso_schema_validation_stream *stream,
		jso_object *instance_object, jso_string *instance_key, jso_value *instance_item);

JSO_API jso_rc jso_schema_validation_stream_object_end(jso_schema_validation_stream *stream);

JSO_API jso_rc jso_schema_validation_stream_array_start(jso_schema_validation_stream *stream);

JSO_API jso_rc jso_schema_validation_stream_array_append(
		jso_schema_validation_stream *stream, jso_array *instance_array, jso_value *instance_item);

JSO_API jso_rc jso_schema_validation_stream_array_end(jso_schema_validation_stream *stream);

JSO_API jso_rc jso_schema_validation_stream_value(
		jso_schema_validation_stream *stream, jso_value *value);

#endif /* JSO_SCHEMA_H */
