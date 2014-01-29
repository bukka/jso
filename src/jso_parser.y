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

#include <stdio.h>

#include "jso.h"
#include "jso_scanner.h"
#include "jso_parser.h"

#define YYSTYPE jso_value

%}

%pure-parser
%name-prefix "jso_yy"


%token JSO_T_NUL
%token JSO_T_TRUE
%token JSO_T_FALSE
%token JSO_T_LONG
%token JSO_T_DOUBLE
%token JSO_T_STRING
%token JSO_T_ESTRING
%token JSO_T_EOI
%token JSO_T_ERROR


%% /* Rules */

start:
		value
;

object:
		'{' members '}'
;

members:
	/* empty */
	|	member
;

member:
		pair
	|	pair ',' member
;

pair:
	JSO_T_STRING ':' value
;

array:
		'[' elements ']'
;

elements:
	/* empty */
	|	element
;

element:
		value
	|	value ',' element
;

value:
		object
	|	array
	|	JSO_T_STRING
	|	JSO_T_ESTRING
	|	JSO_T_LONG
	|	JSO_T_DOUBLE
	|	JSO_T_NUL
	|	JSO_T_TRUE
	|	JSO_T_FALSE
;

%%

int jso_yylex(jso_value *jv)
{
	return 1;
}

void jso_yyerror(char const *s)
{
  fprintf (stderr, "%s\n", s);
}
