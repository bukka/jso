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

#define	JSO_SCANNER_BSIZE 4096

#define	YYCTYPE     jso_ctype
#define	YYCURSOR    JSO_IO_CURSOR(s->io)
#define	YYLIMIT     JSO_IO_LIMIT(s->io)
#define	YYMARKER    JSO_MARKER(s->io)
#define	YYFILL(n)   { JSO_IO_READ(s->io, n); \
	if (JSO_IO_ERROR(s->io)) return JSO_TOK(ERROR); \
	if (JSO_IO_END(s->io)) return JSO_TOK(EOI); }

#define JSO_TOK(token)  JSO_T_##token
#define	JSO_RTOK(token)  return JSO_TOK(token)

void jso_scanner_init(jso_scanner *scanner, jso_io *io)
{
	memset(scanner, 0, sizeof (jso_scanner));
	scanner->io = io;
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

