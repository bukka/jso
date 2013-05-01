/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:  Jakub Zelenka <jakub.php@gmail.com>                        |
   +----------------------------------------------------------------------+
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "jso_scanner.h"
#include "jso_stream.h"

#define	JSO_SCANNER_BSIZE 4096

#define	YYCTYPE     char
#define	YYCURSOR    s->cursor
#define	YYLIMIT     s->limit
#define	YYMARKER    s->ptr
#define	YYFILL(n)   { int fill_rc = fill(s, n); \
		if (fill_rc != JSO_STREAM_SUCCESS && s->ptr == s->limit) \
			return fill_rc == JSO_STREAM_EOI ? JSO_TOK(EOI) : JSO_TOK(ERROR); }

#define JSO_TOK(_token)  JSO_T_ ## _token
#define	JSO_RTOK(_token)  return JSO_TOK(_token)

int fill(jso_scanner *s, int n)
{
	size_t size;
	int rc;

	size = JSO_SCANNER_BSIZE;
	rc = jso_stream_read(s->stream, &s->ptr, &size);
	if (rc == JSO_STREAM_SUCCESS) {
		s->cursor = s->ptr;
		s->limit = s->ptr + size;
	}
	return rc;
}

void jso_scanner_init(jso_scanner *scanner, jso_stream *stream)
{
	memset(scanner, 0, sizeof (jso_scanner));
	scanner->stream = stream;
}

int jso_scan(jso_scanner *s)
{
std:
/*!re2c
DIGIT   = [0-9];
OCT     = "0" DIGIT+;
INT     = "0" | ( [1-9] DIGIT* ) ;
HEX     = "0x" [a-fA-F0-9]+ ;
ALPHA   = [a-zA-Z_$] ;
ALNUM   = ALPHA | DIGIT ;
IDENT   = ALPHA ALNUM+ ;
WS      = [ \t]+ ;
NL      = "\r"? "\n" ;
EOI     = "\000";
*/

/*!re2c
	"undefined"      { JSO_RTOK(UNDEFINED); }
	"null"           { JSO_RTOK(NUL); }
	"NaN"            { JSO_RTOK(NAN); }
	"Infinity"       { JSO_RTOK(INFINITY); }
	"{"              { JSO_RTOK(LBRACE); }
	"}"              { JSO_RTOK(RBRACE); }
	"("              { JSO_RTOK(LPAR); }
	")"              { JSO_RTOK(RPAR); }
	";"              { JSO_RTOK(SEMICOLON); }
	":"              { JSO_RTOK(COLON); }
	","              { JSO_RTOK(COMMA); }
	INT              { JSO_RTOK(INT); }
	IDENT            { JSO_RTOK(IDENT); }
	EOI              { JSO_RTOK(EOI); }

	WS|NL            { goto std; }
	[^]              { JSO_RTOK(ERROR); }
*/

}

