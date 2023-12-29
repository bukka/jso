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
 * @file jso_schema_validation_value.h
 * @brief JsonSchema validation of values.
 */

#ifndef JSO_SCHEMA_VALIDATION_OBJECT_H
#define JSO_SCHEMA_VALIDATION_OBJECT_H

#include "jso_schema.h"

jso_schema_value *jso_schema_validation_object_find_value(
		jso_schema *schema, jso_schema_value *value, jso_string *key);

jso_rc jso_schema_validation_object_key(
		jso_schema *schema, jso_schema_value *value, jso_string *key);

jso_rc jso_schema_validation_object_update(jso_schema *schema, jso_schema_value *value,
		jso_object *instance_object, jso_string *instance_key, jso_value *instance_value);

jso_rc jso_schema_validation_object_end(jso_schema *schema, jso_schema_value *value);

#endif /* JSO_SCHEMA_VALIDATION_OBJECT_H */
