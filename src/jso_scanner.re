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
#define	YYMARKER    JSO_IO_MARKER(s->io)
#define	YYFILL(n)   { JSO_IO_READ(s->io, n); \
	if (JSO_IO_ERROR(s->io)) return JSO_TOKEN(ERROR); \
	if (JSO_IO_END(s->io)) return JSO_TOKEN(EOI); }

#define JSO_TOKEN(token)  JSO_T_##token
#define	JSO_TOKEN_RETURN(token)  return JSO_TOKEN(token)

void jso_scanner_init(jso_scanner *scanner, jso_io *io)
{
	memset(scanner, 0, sizeof (jso_scanner));
	scanner->io = io;
}

int jso_scan(jso_scanner *s)
{
std:
	JSO_IO_RESET_TOKEN(s->io);

/*!re2c
	re2c:indent:top = 2;

	DIGIT   = [0-9];
	UINT    = "0" | ( [1-9] DIGIT* ) ;
	INT     = "-"? UINT ;
	FLOAT   = INT "." UINT ;
	EXP     = ( INT | FLOAT ) [eE] [+-]? UINT ;
	WS      = [ \t]+ ;
	NL      = "\r"? "\n" ;
	EOI     = "\000";
	ANY     = [^] ;

	"null"           { JSO_TOKEN_RETURN(NUL); }
	"true"           { JSO_TOKEN_RETURN(TRUE); }
	"false"          { JSO_TOKEN_RETURN(FALSE); }
	"{"              { return '{'; }
	"}"              { return '}'; }
	"["              { return '['; }
	"]"              { return ']'; }
	":"              { return ':'; }
	","              { return ','; }
	INT              {
		char *tailptr;
		JSO_VALUE_SET_LONG(s->value, strtol((char *) JSO_IO_TOKEN(s->io), &tailptr, 10));
		JSO_TOKEN_RETURN(LONG);
	}
	FLOAT|EXP        {
		char *tailptr;
		JSO_VALUE_SET_DOUBLE(s->value, strtod((char *) JSO_IO_TOKEN(s->io), &tailptr));
		JSO_TOKEN_RETURN(DOUBLE);
	}
	EOI              { JSO_TOKEN_RETURN(EOI); }
	WS|NL            { goto std; }
	ANY              { JSO_TOKEN_RETURN(ERROR); }
*/

}

