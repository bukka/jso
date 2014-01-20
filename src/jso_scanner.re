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

#include "jso_scanner_defs.h"

#define	YYCTYPE     jso_ctype
#define	YYCURSOR    JSO_IO_CURSOR(s->io)
#define	YYLIMIT     JSO_IO_LIMIT(s->io)
#define	YYMARKER    JSO_IO_MARKER(s->io)
#define	YYCTXMARKER JSO_IO_CTXMARKER(s->io)

#define YYGETCONDITION()        s->state
#define YYSETCONDITION(yystate) s->state = yystate

#define	YYFILL(n)   { JSO_IO_READ(s->io, n); \
	if (JSO_IO_ERROR(s->io)) return JSO_TOKEN(ERROR); \
	if (JSO_IO_END(s->io)) return JSO_TOKEN(EOI); }

#define JSO_TOKEN(token)  JSO_T_##token
#define	JSO_TOKEN_RETURN(token)  return JSO_TOKEN(token)
#define JSO_CONDITION_SET(condition) YYSETCONDITION(yyc##condition)
#define JSO_CONDITION_GOTO(condition) goto yyc_##condition

void jso_scanner_init(jso_scanner *s, jso_io *io)
{
	memset(s, 0, sizeof (jso_scanner));
	s->io = io;
	JSO_CONDITION_SET(JS);
}

void jso_scanner_pre_esc_copy(jso_scanner *s)
{
	if (JSO_IO_STR_LENGTH(s->io)) {
		size_t len = JSO_IO_STR_LENGTH(s->io);
		memcpy(s->pstr, JSO_IO_STR_GET_START(s->io), len * sizeof(jso_ctype));
		s->pstr += len;
	}
}

int jso_scan(jso_scanner *s)
{
std:
	JSO_IO_RESET_TOKEN(s->io);

/*!re2c
	re2c:indent:top = 1;
	re2c:yyfill:enable = 0;

	DIGIT   = [0-9];
	DIGITNZ = [1-9];
	UINT    = "0" | ( DIGITNZ DIGIT* ) ;
	INT     = "-"? UINT ;
	HEX     = DIGIT | [a-fA-F] ;
	HEXNZ   = DIGITNZ | [a-fA-F] ;
	HEX2    = HEX{2} ;
	HEX4    = HEX{4} ;
	FLOAT   = INT "." UINT ;
	EXP     = ( INT | FLOAT ) [eE] [+-]? UINT ;
	WS      = [ \t]+ ;
	NL      = "\r"? "\n" ;
	EOI     = "\000";
	ANY     = [^] ;
	ESCPREF = "\\" ;
	ESCSYM  = ( "\"" | "\\" | "/" | [bfnrt] ) ;
	ESC     = ESCPREF ESCSYM ;
	UTFSYM  = "u" ;
	UTFPREF = ESCPREF UTFSYM ;
	UTF     = UTFPREF HEX4 ;
	UTF1    = UTFPREF "00" HEX2 ;
	UTF2    = UTFPREF ( ( "0" HEXNZ ) | ( HEXNZ HEX ) ) HEX2 ;

	<JS>"null"               { JSO_TOKEN_RETURN(NUL); }
	<JS>"true"               { JSO_TOKEN_RETURN(TRUE); }
	<JS>"false"              { JSO_TOKEN_RETURN(FALSE); }
	<JS>"{"                  { return '{'; }
	<JS>"}"                  { return '}'; }
	<JS>"["                  { return '['; }
	<JS>"]"                  { return ']'; }
	<JS>":"                  { return ':'; }
	<JS>","                  { return ','; }
	<JS>INT                  {
		char *tailptr;
		JSO_VALUE_SET_LONG(s->value, strtol((char *) JSO_IO_TOKEN(s->io), &tailptr, 10));
		JSO_TOKEN_RETURN(LONG);
	}
	<JS>FLOAT|EXP            {
		char *tailptr;
		JSO_VALUE_SET_DOUBLE(s->value, strtod((char *) JSO_IO_TOKEN(s->io), &tailptr));
		JSO_TOKEN_RETURN(DOUBLE);
	}
	<JS>WS|NL                { goto std; }
	<JS>EOI                  { JSO_TOKEN_RETURN(EOI); }
	<JS>["]                  {
		JSO_IO_STR_SET_START(s->io);
		JSO_IO_STR_CLEAR_ESC(s->io);
		JSO_CONDITION_SET(STR_P1);
		JSO_CONDITION_GOTO(STR_P1);
	}

	<STR_P1>UTF1             {
		JSO_IO_STR_ADD_ESC(s->io, 4);
		JSO_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>UTF2             {
		JSO_IO_STR_ADD_ESC(s->io, 3);
		JSO_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>ESC              {
		JSO_IO_STR_ADD_ESC(s->io, 1);
		JSO_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>ESCPREF           { JSO_TOKEN_RETURN(ERROR); }
	<STR_P1>["]              {
		jso_ctype *str;
		size_t len = JSO_IO_STR_LENGTH(s->io) - JSO_IO_STR_GET_ESC(s->io);
		if (len == 0) {
			JSO_CONDITION_SET(JS);
			JSO_TOKEN_RETURN(ESTRING);
		}
		str = jso_malloc((len + 1) * sizeof(jso_ctype));
		str[len] = 0;
		JSO_VALUE_SET_STRING(s->value, str, len);
		if (JSO_IO_STR_GET_ESC(s->io)) {
			s->pstr = JSO_SVAL(s->value);
			JSO_IO_CURSOR(s->io) = JSO_IO_STR_GET_START(s->io);
			JSO_CONDITION_SET(STR_P2);
			JSO_CONDITION_GOTO(STR_P2);
		} else {
			memcpy(JSO_SVAL(s->value), JSO_IO_STR_GET_START(s->io), len * sizeof(jso_ctype));
			JSO_CONDITION_SET(JS);
			JSO_TOKEN_RETURN(STRING);
		}
	}
	<STR_P1>ANY              { JSO_CONDITION_GOTO(STR_P1); }

	<STR_P2>UTFPREF/HEX4     {
		int code = 0;
		jso_scanner_pre_esc_copy(s);

		if (code > 255) {
			JSO_IO_STR_SET_START_AFTER(s->io, 2);
		} else {
			JSO_IO_STR_SET_START_AFTER(s->io, 1);
		}
		JSO_CONDITION_GOTO(STR_P2);
	}
	<STR_P2>ESCPREF          {
		char esc;
		jso_scanner_pre_esc_copy(s);
		switch (*JSO_IO_CURSOR(s->io)) {
			case 'b':
				esc = '\b';
				break;
			case 'f':
				esc = '\f';
				break;
			case 'n':
				esc = '\n';
				break;
			case 'r':
				esc = '\r';
				break;
			case 't':
				esc = '\t';
				break;
			case '\\':
			case '/':
			case '"':
				esc = *JSO_IO_CURSOR(s->io);
				break;
			default:
				JSO_TOKEN_RETURN(ERROR);
		}
		*s->pstr = esc;
		s->pstr++;
		++YYCURSOR;
		JSO_IO_STR_SET_START(s->io);
		JSO_CONDITION_GOTO(STR_P2);
	}
	<STR_P2>["] => JS        {
		jso_scanner_pre_esc_copy(s);
		JSO_TOKEN_RETURN(STRING);
	}
	<STR_P2>ANY              { JSO_CONDITION_GOTO(STR_P2); }

	<*>ANY                   { JSO_TOKEN_RETURN(ERROR); }
*/

}

