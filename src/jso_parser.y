%code top {
/*
 * Copyright (c) 2012-2025 Jakub Zelenka. All rights reserved.
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
#include "jso_parser.h"
#include "jso_parser_hooks.h"

#include <stdio.h>
#include <string.h>

#define YYDEBUG 0

#if YYDEBUG
int jso_yydebug = 1;
#endif

}

%locations
%define api.pure
%define api.prefix {jso_yy}
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
%token <value> JSO_T_ENOMEM
%token <value> JSO_T_EOI
%token <value> JSO_T_ERROR

%type <value> start object key value values array errlex
%type <object> members member
%type <pair> pair
%type <array> elements element

%destructor { jso_value_free(&$$); } <value>
%destructor { jso_object_free($$); } <object>
%destructor { jso_value_free(&$$.key); jso_value_free(&$$.val); } <pair>
%destructor { jso_array_free($$); } <array>

%code {
int jso_yylex(union YYSTYPE *value, YYLTYPE *location, jso_parser *parser);
void jso_yyerror(YYLTYPE *location, jso_parser *parser, char const *msg);

#define JSO_PARSER_SET_LOC(_loc) \
	JSO_ASSERT_EQ(sizeof(YYLTYPE), sizeof(jso_parser_location)); \
	parser->location = (jso_parser_location *) &_loc

#define JSO_PARSER_SET_ERROR(_etype, _loc) \
	JSO_VALUE_SET_ERROR(parser->result, jso_error_new(_etype, \
			_loc->first_column, _loc->first_line, _loc->last_column, _loc->last_line)); \

#define JSO_PARSER_ERROR(_etype) \
	YYLTYPE *_loc = (YYLTYPE *) parser->location; \
	JSO_PARSER_SET_ERROR(_etype, _loc); \
	YYERROR

#define JSO_PARSER_HOOK_CHECK(_name, _loc, _etype_exp) \
	do { \
		if (parser->hooks._name != NULL) { \
			jso_error_type _etype = _etype_exp; \
			if (_etype != JSO_ERROR_NONE) { \
				JSO_PARSER_SET_LOC(_loc); \
				JSO_PARSER_ERROR(_etype); \
			} \
		} \
	} while(0)

#define JSO_PARSER_DEPTH_DEC() jso_parser_depth_decrease(parser)
#define JSO_PARSER_DEPTH_INC(_loc) \
	jso_error_type _etype = jso_parser_depth_increase(parser); \
	do { \
		if (_etype != JSO_ERROR_NONE) { \
			JSO_PARSER_SET_LOC(_loc); \
			JSO_PARSER_ERROR(_etype); \
		} \
	} while(0)

#define JSO_PARSER_HOOK(_name, _loc, ...) \
		JSO_PARSER_HOOK_CHECK(_name, _loc, parser->hooks._name(parser, __VA_ARGS__))

#define JSO_PARSER_HOOK_0(_name, _loc) \
	JSO_PARSER_HOOK_CHECK(_name, _loc, parser->hooks._name(parser))

}

%% /* Rules */

start:
		value JSO_T_EOI
			{
				$$ = $1;
				parser->result = $1;
				JSO_USE($2);
				YYACCEPT;
			}
	|	value errlex
			{
				JSO_USE_2($$, $1, $2);
			}
;

object:
		'{'
			{
				JSO_PARSER_DEPTH_INC(@1);
				JSO_PARSER_HOOK_0(object_start, @1);
			}
		members
			{
				JSO_PARSER_DEPTH_DEC();
				JSO_PARSER_HOOK_0(object_end, @3);
				JSO_VALUE_SET_OBJECT($$, $3);
			}
;

members:
		'}'
			{
				JSO_PARSER_HOOK(object_create, @1, &$$);
			}
	|	member '}'
;

member:
		pair
			{
				JSO_PARSER_HOOK(object_create, @1, &$$);
				JSO_PARSER_HOOK(object_update, @1, $$, JSO_STR($1.key), &$1.val);
			}
	|	member ',' pair
			{
				JSO_PARSER_HOOK(object_update, @3, $1, JSO_STR($3.key), &$3.val);
				$$ = $1;
			}
	|	member errlex
			{
				JSO_USE_2($$, $1, $2);
			}
;

pair:
		key ':' value
			{
				$$.key = $1;
				$$.val = $3;
			}
	|	key errlex
			{
				JSO_USE_2($$, $1, $2);
			}
;

array:
		'['
			{
				JSO_PARSER_DEPTH_INC(@1);
				JSO_PARSER_HOOK_0(array_start, @1);
			}
		elements
			{
				JSO_PARSER_DEPTH_DEC();
				JSO_PARSER_HOOK_0(array_end, @3);
				JSO_VALUE_SET_ARRAY($$, $3);
			}
;

elements:
		']'
			{
				JSO_PARSER_HOOK(array_create, @1, &$$);
			}
	|	element ']'
;

element:
		value
			{
				JSO_PARSER_HOOK(array_create, @1, &$$);
				JSO_PARSER_HOOK(array_append, @1, $$, &$1);
			}
	|	element ',' value
			{
				JSO_PARSER_HOOK(array_append, @3, $1, &$3);
				$$ = $1;
			}
	|	element errlex
			{
				JSO_USE_2($$, $1, $2);
			}
;

key:
		JSO_T_STRING
			{
				JSO_PARSER_HOOK(object_key, @1, JSO_STR($1));
				$$ = $1;
			}
	|	JSO_T_ESTRING
			{
				JSO_PARSER_HOOK(object_key, @1, JSO_STR($1));
				$$ = $1;
			}
;

value:
		values
			{
				JSO_PARSER_HOOK(value, @1, &$1);
				$$ = $1;
			}

values:
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
		JSO_T_ERROR
			{
				parser->result = $1;
				JSO_USE($$);
				YYERROR;
			}
;

%%

JSO_API int jso_yylex(union YYSTYPE *value, YYLTYPE *location, jso_parser *parser)
{
	int token = jso_scan(&parser->scanner);
	value->value = parser->scanner.value;
	location->first_column = JSO_SCANNER_LOCATION(parser->scanner, first_column);
	location->first_line = JSO_SCANNER_LOCATION(parser->scanner, first_line);
	location->last_column = JSO_SCANNER_LOCATION(parser->scanner, last_column);
	location->last_line = JSO_SCANNER_LOCATION(parser->scanner, last_line);
	return token;
}

void jso_yyerror(YYLTYPE *location, jso_parser *parser, char const *msg)
{
	JSO_PARSER_SET_ERROR(JSO_ERROR_SYNTAX, location);
}
