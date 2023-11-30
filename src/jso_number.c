/*
 * Copyright (c) 2012-2023 Jakub Zelenka. All rights reserved.
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

#include "jso_number.h"
#include "jso_types.h"

#include "jso_dg_dtoa.h"

#include <math.h>

JSO_API jso_rc jso_number_string_from_double(jso_number_string *nstr, double val)
{
	return JSO_SUCCESS;
}

JSO_API double jso_number_cstr_to_double(char *str)
{
	return 0.0;
}

JSO_API bool jso_number_eq(jso_number *num1, jso_number *num2) 
{
	if (num1->is_int && num2->is_int) {
		return num1->ival == num2->ival;
	} else if (num1->is_int && !num2->is_int) {
		return num1->ival == num2->dval;
	} else if (!num1->is_int && num2->is_int) {
		return num1->dval == num2->ival;
	} else {
		return num1->dval == num2->dval;
	}
}

JSO_API bool jso_number_ge(jso_number *num1, jso_number *num2)
{
	if (num1->is_int && num2->is_int) {
		return num1->ival >= num2->ival;
	} else if (num1->is_int && !num2->is_int) {
		return num1->ival >= num2->dval;
	} else if (!num1->is_int && num2->is_int) {
		return num1->dval >= num2->ival;
	} else {
		return num1->dval >= num2->dval;
	}
}


JSO_API bool jso_number_gt(jso_number *num1, jso_number *num2)
{
	if (num1->is_int && num2->is_int) {
		return num1->ival > num2->ival;
	} else if (num1->is_int && !num2->is_int) {
		return num1->ival > num2->dval;
	} else if (!num1->is_int && num2->is_int) {
		return num1->dval > num2->ival;
	} else {
		return num1->dval > num2->dval;
	}
}

JSO_API bool jso_number_le(jso_number *num1, jso_number *num2)
{
	if (num1->is_int && num2->is_int) {
		return num1->ival <= num2->ival;
	} else if (num1->is_int && !num2->is_int) {
		return num1->ival <= num2->dval;
	} else if (!num1->is_int && num2->is_int) {
		return num1->dval <= num2->ival;
	} else {
		return num1->dval <= num2->dval;
	}
}

JSO_API bool jso_number_lt(jso_number *num1, jso_number *num2)
{
	if (num1->is_int && num2->is_int) {
		return num1->ival < num2->ival;
	} else if (num1->is_int && !num2->is_int) {
		return num1->ival < num2->dval;
	} else if (!num1->is_int && num2->is_int) {
		return num1->dval < num2->ival;
	} else {
		return num1->dval < num2->dval;
	}
}

JSO_API bool jso_number_is_multiple_of(jso_number *num, jso_number *multiple_of)
{
	if (num->is_int && multiple_of->is_int) {
		return num->ival % multiple_of->ival == 0;
	} else if (num->is_int && !multiple_of->is_int) {
		double mult = multiple_of->dval;
		return nearbyint(mult) == mult && num->ival % (jso_int) mult == 0;
	} else if (!num->is_int && multiple_of->is_int) {
		double n = num->dval;
		return nearbyint(n) == n && (jso_int) n % multiple_of->ival == 0;
	} else {
		return fmod(num->dval, multiple_of->dval) == 0.0;
	}
}