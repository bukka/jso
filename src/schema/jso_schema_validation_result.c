/*jso_schema_validation_value_callback
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

#include "jso_schema_validation_result.h"

#include "jso.h"

void jso_schema_validation_result_propagate(jso_schema_validation_position *pos)
{
	// TODO:
	// - loop through parents and update result
	// 	 - mainly for failures bul also for for success if `not` present
	// - special rules should apply for compositions
	//   - anyOf success should set final result flag
	//   - think how allOf and oneOf can be finalized
	//     - how do we know that it is the last one and do we need to know it?
	//   - not should maybe have some flag that will get set during composition check for position
	//   and its children
	// - should we also add some checks whether root_value has final_result set?
	while (pos) {
		pos = pos->parent;
	}
}