/*jso_schema_validation_value_callback
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

#include "jso_schema_validation_result.h"

#include "jso.h"

void jso_schema_validation_result_propagate(jso_schema_validation_position *pos)
{
	jso_schema_validation_position *parent_pos = pos->parent;
	if (parent_pos->position_type == JSO_SCHEMA_VALIDATION_POSITION_BASIC) {
		if (pos->validation_result == JSO_FAILURE) {
			jso_schema_validation_set_final_result(parent_pos, JSO_FAILURE);
		}
	} else {
		JSO_ASSERT_EQ(parent_pos->position_type, JSO_SCHEMA_VALIDATION_POSITION_COMPOSED);
		switch (parent_pos->composition_type) {
			case JSO_SCHEMA_VALIDATION_COMPOSITION_REF:
				jso_schema_validation_set_final_result(parent_pos, pos->validation_result);
				break;
			case JSO_SCHEMA_VALIDATION_COMPOSITION_ALL:
				if (pos->validation_result == JSO_FAILURE) {
					jso_schema_validation_set_final_result(parent_pos, JSO_FAILURE);
				} else {
					parent_pos->count++;
				}
				break;
			case JSO_SCHEMA_VALIDATION_COMPOSITION_ANY:
				if (pos->validation_result == JSO_SUCCESS) {
					parent_pos->count++;
					jso_schema_validation_set_final_result(parent_pos, JSO_SUCCESS);
				}
				break;
			case JSO_SCHEMA_VALIDATION_COMPOSITION_ONE:
				if (pos->validation_result == JSO_SUCCESS) {
					parent_pos->count++;
					if (parent_pos->count > 1) {
						jso_schema_validation_set_final_result(parent_pos, JSO_FAILURE);
					}
				}
				break;
			default:
				JSO_ASSERT_EQ(parent_pos->composition_type, JSO_SCHEMA_VALIDATION_COMPOSITION_NOT);
				if (pos->validation_result == JSO_SUCCESS) {
					jso_schema_validation_set_final_result(parent_pos, JSO_FAILURE);
				} else {
					jso_schema_validation_set_final_result(parent_pos, JSO_SUCCESS);
				}
				break;
		}
	}
}