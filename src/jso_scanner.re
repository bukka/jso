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

#define JSO_SCANNER_COPY_ESC() jso_scanner_copy_string(s, 0)
#define JSO_SCANNER_COPY_UTF() jso_scanner_copy_string(s, 5)


static void jso_scanner_copy_string(jso_scanner *s, size_t esc_size)
{
	size_t len = JSO_IO_STR_LENGTH(s->io) - esc_size;
	if (len) {
		memcpy(s->pstr, JSO_IO_STR_GET_START(s->io), len * sizeof(jso_ctype));
		s->pstr += len;
	}
}

static int jso_hex_to_int(jso_ctype c)
{
	if (c >= '0' && c <= '9')
	{
		return c - '0';
	}
	else if (c >= 'A' && c <= 'F')
	{
		return c - ('A' - 10);
	}
	else if (c >= 'a' && c <= 'f')
	{
		return c - ('a' - 10);
	}
	else
	{
		/* this should never happened */
		return -1;
	}
}

static int jso_ucs2_to_int(jso_scanner *s, int size)
{
	int i, code = 0;
	jso_ctype *pc = JSO_IO_CURSOR(s->io) - 1;
	for (i = 0; i < size; i++) {
		code |= jso_hex_to_int(*(pc--)) << (i * 4);
	}
	return code;
}

void jso_scanner_init(jso_scanner *s, jso_io *io)
{
	memset(s, 0, sizeof (jso_scanner));
	s->io = io;
	JSO_CONDITION_SET(JS);
}

int jso_scan(jso_scanner *s)
{
std:
	JSO_IO_RESET_TOKEN(s->io);

/*!re2c
	re2c:indent:top = 1;
	re2c:yyfill:enable = 0;

	DIGIT   = [0-9] ;
	DIGITNZ = [1-9] ;
	UINT    = "0" | ( DIGITNZ DIGIT* ) ;
	INT     = "-"? UINT ;
	HEX     = DIGIT | [a-fA-F] ;
	HEXNZ   = DIGITNZ | [a-fA-F] ;
	HEX7    = [0-7] ;
	HEXC    = DIGIT | [a-cA-C] ;
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
	UCS2    = UTFPREF HEX{4} ;
	UTF1    = UTFPREF "00" HEX7 HEX ;
	UTF2    = UTFPREF "0" HEX7 HEX{2} ;
	UTF3    = UTFPREF ( ( ( HEXC | [efEF] ) HEX ) | ( [dD] HEX7 ) ) HEX{2} ;
	UTF4    = UTFPREF [dD] [89abAB] HEX{2} UTFPREF [dD] [c-fC-F] HEX{2} ;

	<JS>"{"                  { return '{'; }
	<JS>"}"                  { return '}'; }
	<JS>"["                  { return '['; }
	<JS>"]"                  { return ']'; }
	<JS>":"                  { return ':'; }
	<JS>","                  { return ','; }
	<JS>"null"               {
		JSO_VALUE_SET_NULL(s->value);
		JSO_TOKEN_RETURN(NUL);
	}
	<JS>"true"               {
		JSO_VALUE_SET_BOOL(s->value, JSO_TRUE);
		JSO_TOKEN_RETURN(TRUE);
	}
	<JS>"false"              {
		JSO_VALUE_SET_BOOL(s->value, JSO_FALSE);
		JSO_TOKEN_RETURN(FALSE);
	}
	<JS>INT                  {
		char *tailptr;
		JSO_VALUE_SET_INT(s->value, strtol((char *) JSO_IO_TOKEN(s->io), &tailptr, 10));
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
		JSO_IO_STR_ADD_ESC(s->io, 5);
		JSO_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>UTF2             {
		JSO_IO_STR_ADD_ESC(s->io, 4);
		JSO_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>UTF3             {
		JSO_IO_STR_ADD_ESC(s->io, 3);
		JSO_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>UTF4             {
		JSO_IO_STR_ADD_ESC(s->io, 8);
		JSO_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>UCS2             {
		JSO_TOKEN_RETURN(ERROR);
	}
	<STR_P1>ESC              {
		JSO_IO_STR_ADD_ESC(s->io, 1);
		JSO_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>ESCPREF           {
		JSO_TOKEN_RETURN(ERROR);
	}
	<STR_P1>["]              {
		jso_ctype *str;
		size_t len = JSO_IO_STR_LENGTH(s->io) - JSO_IO_STR_GET_ESC(s->io);
		if (len == 0) {
			JSO_CONDITION_SET(JS);
			JSO_VALUE_SET_EMPTY_STRING(s->value);
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

	<STR_P2>UTF1             {
		int utf16 = jso_ucs2_to_int(s, 2);
		JSO_SCANNER_COPY_UTF();
		*(s->pstr++) = (jso_ctype) utf16;
		JSO_IO_STR_SET_START(s->io);
		JSO_CONDITION_GOTO(STR_P2);
	}
	<STR_P2>UTF2             {
		int utf16 = jso_ucs2_to_int(s, 3);
		JSO_SCANNER_COPY_UTF();
		*(s->pstr++) = (jso_ctype) (0xc0 | (utf16 >> 6));
		*(s->pstr++) = (jso_ctype) (0x80 | (utf16 & 0x3f));
		JSO_IO_STR_SET_START(s->io);
		JSO_CONDITION_GOTO(STR_P2);
	}
	<STR_P2>UTF3             {
		int utf16 = jso_ucs2_to_int(s, 4);
		JSO_SCANNER_COPY_UTF();
		*(s->pstr++) = (jso_ctype) (0xe0 | (utf16 >> 12));
		*(s->pstr++) = (jso_ctype) (0x80 | ((utf16 >> 6) & 0x3f));
		*(s->pstr++) = (jso_ctype) (0x80 | (utf16 & 0x3f));
		JSO_IO_STR_SET_START(s->io);
		JSO_CONDITION_GOTO(STR_P2);
	}
	<STR_P2>ESCPREF          {
		char esc;
		JSO_SCANNER_COPY_ESC();
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
		*(s->pstr++) = esc;
		++YYCURSOR;
		JSO_IO_STR_SET_START(s->io);
		JSO_CONDITION_GOTO(STR_P2);
	}
	<STR_P2>["] => JS        {
		JSO_SCANNER_COPY_ESC();
		JSO_TOKEN_RETURN(STRING);
	}
	<STR_P2>ANY              { JSO_CONDITION_GOTO(STR_P2); }

	<*>ANY                   { JSO_TOKEN_RETURN(ERROR); }
*/

}

