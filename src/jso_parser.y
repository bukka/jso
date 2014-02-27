%code top {
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
#include "jso_parser.h"

#define YYDEBUG 0

#if YYDEBUG
int jso_yydebug = 1;
#endif

}

%pure-parser
%name-prefix "jso_yy"
%lex-param  { jso_parser *parser  }
%parse-param { jso_parser *parser }

%union {
	jso_value value;
	struct {
		jso_value key;
		jso_value val;
	} pair;
	jso_object *object;
	jso_array *array;
}

%token <value> JSO_T_NUL
%token <value> JSO_T_TRUE
%token <value> JSO_T_FALSE
%token <value> JSO_T_LONG
%token <value> JSO_T_DOUBLE
%token <value> JSO_T_STRING
%token <value> JSO_T_ESTRING
%token <value> JSO_T_EOI
%token <value> JSO_T_ERROR

%type <value> start object key value array errlex
%type <object> members member
%type <pair> pair
%type <array> elements element

%destructor { jso_value_free(&$$); } <value>
%destructor { jso_object_free($$); } <object>
%destructor { jso_value_free(&$$.key); jso_value_free(&$$.val); } <pair>
%destructor { jso_array_free($$); } <array>

%code {
int jso_yylex(union YYSTYPE *value, jso_parser *parser);
void jso_yyerror(jso_parser *parser, char const *msg);
}

%% /* Rules */

start:
		value JSO_T_EOI         { $$ = $1; parser->result = $1; JSO_USE($2); YYACCEPT; }
	|	value errlex            { JSO_USE_2($$, $1, $2); }
;

object:
		'{' members '}'         { JSO_VALUE_SET_OBJECT($$, $2); }
;

members:
		/* empty */             { $$ = jso_object_alloc(); }
	|	member
;

member:
		pair                    { $$ = jso_object_alloc(); jso_object_add($$, &$1.key, &$1.val); }
	|	member ',' pair         { jso_object_add($1, &$3.key, &$3.val); $$ = $1; }
	|	member errlex           { JSO_USE_2($$, $1, $2); }
;

pair:
		key ':' value           { $$.key = $1; $$.val = $3; }
	|	key errlex              { JSO_USE_2($$, $1, $2); }
;

array:
		'[' elements ']'        { JSO_VALUE_SET_ARRAY($$, $2); }
;

elements:
		/* empty */             { $$ = jso_array_alloc(); }
	|	element
;

element:
		value                   { $$ = jso_array_alloc(); jso_array_append($$, &$1); }
	|	element ',' value       { jso_array_append($1, &$3); $$ = $1; }
	|	element errlex          { JSO_USE_2($$, $1, $2); }
;

key:
		JSO_T_STRING
	|	JSO_T_ESTRING
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
	|	errlex
;

errlex:
		JSO_T_ERROR             { JSO_USE_1($$, $1); YYERROR; }
;

%%

int jso_yylex(union YYSTYPE *value, jso_parser *parser)
{
	int token = jso_scan(&parser->scanner);
	value->value = parser->scanner.value;
	return token;
}

void jso_yyerror(jso_parser *parser, char const *msg)
{
  fprintf (stderr, "%s\n", msg);
}
