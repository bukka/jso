%{
/*
 * Copyright (c) 2012-2014 Jakub Zelenka. All rights reserved.
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

#include "jso.h"

%}

%token JSO_T_NULL
%token JSO_T_TRUE
%token JSO_T_FALSE
%token JSO_T_INT
%token JSO_T_FLOAT
%token JSO_T_STR
%token JSO_T_ESTR
%token JSO_T_EOI
%token JSO_T_ERROR

%% /* Rules */

start:
		object
	|	array
	|   value
;

object:
		'{' '}'
	|	'{' members '}'
;

members:
		pair
	|	pair ',' members
;

pair:
	JSO_T_STR ':' value
;

array:
		'[' ']'
	|	'[' elements ']'
;

elements:
		value
	|	value ',' elements
;

value:
		object
	|	array
	|	JSO_T_STR
	|	JSO_T_ESTR
	|	JSO_T_INT
	|	JSO_T_FLOAT
	|	JSO_T_NULL
	|	JSO_T_TRUE
	|	JSO_T_FALSE
;

