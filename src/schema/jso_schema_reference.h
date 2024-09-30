/*
 * Copyright (c) 2024 Jakub Zelenka. All rights reserved.
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
 * @file jso_schema_reference.h
 * @brief JsonSchema reference handling.
 */

#ifndef JSO_SCHEMA_REFERENCE_H
#define JSO_SCHEMA_REFERENCE_H

#include "jso_schema.h"
#include "jso_schema_uri.h"

jso_schema_reference *jso_schema_reference_create(
		jso_string *ref_uri, jso_schema_uri *base_uri, jso_schema *schema);

void jso_schema_reference_free(jso_schema_reference *ref);

jso_rc jso_schema_reference_resolve(jso_schema_reference *jp, jso_value *doc, jso_value **value);

#endif /* JSO_SCHEMA_REFERENCE_H */
