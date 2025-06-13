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

#include "jso_scanner.h"
#include "jso_parser.h"
#include "jso_parser.tab.h"

#include "jso_scanner_defs.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define	YYCTYPE     jso_ctype
#define	YYCURSOR    JSO_IO_CURSOR(s->io)
#define	YYLIMIT     JSO_IO_LIMIT(s->io)
#define	YYMARKER    JSO_IO_MARKER(s->io)
#define	YYCTXMARKER JSO_IO_CTXMARKER(s->io)

#define YYGETCONDITION()        s->state
#define YYSETCONDITION(yystate) s->state = yystate

#define	YYFILL(n)   { JSO_IO_READ(s->io, n); \
	if (JSO_IO_ERROR(s->io)) return JSO_T_ERROR; \
	if (JSO_IO_END(s->io)) return JSO_T_EOI; }

#define JSO_CONDITION_SET(condition) YYSETCONDITION(yyc##condition)
#define JSO_CONDITION_GOTO(condition) goto yyc_##condition

#define JSO_SCANNER_START_GOTO() goto scanner_start

#define JSO_SCANNER_COPY_ESC() jso_scanner_copy_string(s, 0)
#define JSO_SCANNER_COPY_UTF() jso_scanner_copy_string(s, 5)

#define JSO_SCANNER_LOC(location) JSO_SCANNER_LOCATION(*s, location)

#define JSO_SCANNER_ERROR(etype) \
	do { \
		JSO_VALUE_SET_ERROR(s->value, jso_error_new_ex(etype, &s->loc)); \
		return JSO_T_ERROR; \
	} while(0)


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
	if (c >= '0' && c <= '9') {
		return c - '0';
	} else if (c >= 'A' && c <= 'F') {
		return c - ('A' - 10);
	} else if (c >= 'a' && c <= 'f') {
		return c - ('a' - 10);
	} else {
		/* this should never happened */
		return -1;
	}
}

static int jso_ucs2_to_int_ex(jso_scanner *s, int size, int start)
{
	int i, code = 0;
	jso_ctype *pc = JSO_IO_CURSOR(s->io) - start;
	for (i = 0; i < size; i++) {
		code |= jso_hex_to_int(*(pc--)) << (i * 4);
	}
	return code;
}

static int jso_ucs2_to_int(jso_scanner *s, int size)
{
	return jso_ucs2_to_int_ex(s, size, 1);
}

void jso_scanner_init(jso_scanner *s, jso_io *io)
{
	memset(s, 0, sizeof (jso_scanner));
	s->io = io;
	JSO_SCANNER_LOC(last_column) = 1;
	JSO_SCANNER_LOC(last_line) = 1;
	JSO_CONDITION_SET(JS);
}

int jso_scan(jso_scanner *s)
{
	/* init value to null to prevent double freeing when it's not used in parser */
	JSO_VALUE_SET_NULL(s->value);

scanner_start:
	/* init location values */
	JSO_SCANNER_LOC(first_column) = JSO_SCANNER_LOC(last_column);
	JSO_SCANNER_LOC(first_line) = JSO_SCANNER_LOC(last_line);

	/* set token postion to the cursor position */
	JSO_IO_RESET_TOKEN(s->io);

/*!re2c
	re2c:indent:top = 1;
	re2c:yyfill:enable = 0;

	DIGIT   = [0-9] ;
	DIGITNZ = [1-9] ;
	UINT    = "0" | ( DIGITNZ DIGIT* ) ;
	INT     = "-"? DIGIT+ ;
	HEX     = DIGIT | [a-fA-F] ;
	HEXNZ   = DIGITNZ | [a-fA-F] ;
	HEX7    = [0-7] ;
	HEXC    = DIGIT | [a-cA-C] ;
	FLOAT   = INT "." UINT ;
	EXP     = ( INT | FLOAT ) [eE] [+-]? DIGIT+ ;
	WS      = [ \t\r]+ ;
	NL      = "\r"? "\n" ;
	EOI     = "\000";
	CTRL    = [\x00-\x1F] ;
	UTF8T   = [\x80-\xBF] ;
	UTF8_1  = [\x00-\x7F] ;
	UTF8_2  = [\xC2-\xDF] UTF8T ;
	UTF8_3A = "\xE0" [\xA0-\xBF] UTF8T ;
	UTF8_3B = [\xE1-\xEC] UTF8T{2} ;
	UTF8_3C = "\xED" [\x80-\x9F] UTF8T ;
	UTF8_3D = [\xEE-\xEF] UTF8T{2} ;
	UTF8_3  = UTF8_3A | UTF8_3B | UTF8_3C | UTF8_3D ;
	UTF8_4A = "\xF0"[\x90-\xBF] UTF8T{2} ;
	UTF8_4B = [\xF1-\xF3] UTF8T{3} ;
	UTF8_4C = "\xF4" [\x80-\x8F] UTF8T{2} ;
	UTF8_4  = UTF8_4A | UTF8_4B | UTF8_4C ;
	UTF8    = UTF8_1 | UTF8_2 | UTF8_3 | UTF8_4 ;
	ANY     = [^] ;
	ESCPREF = "\\" ;
	ESCSYM  = ( "\"" | "\\" | "/" | [bfnrt] ) ;
	ESC     = ESCPREF ESCSYM ;
	UTFSYM  = "u" ;
	UTFPREF = ESCPREF UTFSYM ;
	UCS2    = UTFPREF HEX{4} ;
	UTF16_1 = UTFPREF "00" HEX7 HEX ;
	UTF16_2 = UTFPREF "0" HEX7 HEX{2} ;
	UTF16_3 = UTFPREF ( ( ( HEXC | [efEF] ) HEX ) | ( [dD] HEX7 ) ) HEX{2} ;
	UTF16_4 = UTFPREF [dD] [89abAB] HEX{2} UTFPREF [dD] [c-fC-F] HEX{2} ;

	<JS>"{"                  {
		JSO_SCANNER_LOC(last_column)++;
		return '{';
	}
	<JS>"}"                  {
		JSO_SCANNER_LOC(last_column)++;
		return '}';
	}
	<JS>"["                  {
		JSO_SCANNER_LOC(last_column)++;
		return '[';
	}
	<JS>"]"                  {
		JSO_SCANNER_LOC(last_column)++;
		return ']';
	}
	<JS>":"                  {
		JSO_SCANNER_LOC(last_column)++;
		return ':';
	}
	<JS>","                  {
		JSO_SCANNER_LOC(last_column)++;
		return ',';
	}
	<JS>"null"               {
		JSO_SCANNER_LOC(last_column) += 4;
		JSO_VALUE_SET_NULL(s->value);
		return JSO_T_NUL;
	}
	<JS>"true"               {
		JSO_SCANNER_LOC(last_column) += 4;
		JSO_VALUE_SET_BOOL(s->value, JSO_TRUE);
		return JSO_T_TRUE;
	}
	<JS>"false"              {
		JSO_SCANNER_LOC(last_column) += 5;
		JSO_VALUE_SET_BOOL(s->value, JSO_FALSE);
		return JSO_T_FALSE;
	}
	<JS>INT                  {
		jso_bool bigint = 0, negative = JSO_IO_TOKEN(s->io)[0] == '-';
		size_t digits = JSO_IO_TOKEN_LENGTH(s->io);
		JSO_SCANNER_LOC(last_column) += digits;
		digits -= negative;
		if (digits >= JSO_INT_MAX_LENGTH) {
			if (digits == JSO_INT_MAX_LENGTH) {
				int cmp = strncmp((char *) JSO_IO_TOKEN(s->io) + negative, JSO_INT_MAX_DIGITS, JSO_INT_MAX_LENGTH);
				if (!(cmp < 0 || (cmp == 0 && negative))) {
					bigint = 1;
				}
			} else {
				bigint = 1;
			}
		}
		if (bigint) {
			JSO_VALUE_SET_DOUBLE(s->value, strtod((char *) JSO_IO_TOKEN(s->io), NULL));
			return JSO_T_DOUBLE;
		} else {
			JSO_VALUE_SET_INT(s->value, strtol((char *) JSO_IO_TOKEN(s->io), NULL, 10));
			return JSO_T_LONG;
		}
	}
	<JS>FLOAT|EXP            {
		JSO_SCANNER_LOC(last_column) += JSO_IO_TOKEN_LENGTH(s->io);
		JSO_VALUE_SET_DOUBLE(s->value, strtod((char *) JSO_IO_TOKEN(s->io), NULL));
		return JSO_T_DOUBLE;
	}
	<JS>NL                   {
		JSO_SCANNER_LOC(last_line)++;
		JSO_SCANNER_LOC(last_column) = 1;
		JSO_SCANNER_START_GOTO();
	}
	<JS>WS                   {
		JSO_SCANNER_LOC(last_column) += JSO_IO_TOKEN_LENGTH(s->io);
		JSO_SCANNER_START_GOTO();
	}
	<JS>EOI                  {
		if (JSO_IO_END(s->io)) {
			return JSO_T_EOI;
		} else {
			JSO_SCANNER_ERROR(JSO_ERROR_TOKEN);
		}
	}
	<JS>["]                  {
		JSO_SCANNER_LOC(last_column)++;
		JSO_IO_STR_SET_START(s->io);
		JSO_IO_STR_CLEAR_ESC(s->io);
		JSO_CONDITION_SET(STR_P1);
		JSO_CONDITION_GOTO(STR_P1);
	}

	<STR_P1>CTRL             {
		JSO_SCANNER_ERROR(JSO_ERROR_CTRL_CHAR);
	}
	<STR_P1>NL               {
		JSO_SCANNER_LOC(last_line)++;
		JSO_SCANNER_LOC(last_column) = 1;
		JSO_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>UTF16_1          {
		JSO_SCANNER_LOC(last_column) += 6;
		JSO_IO_STR_ADD_ESC(s->io, 5);
		JSO_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>UTF16_2          {
		JSO_SCANNER_LOC(last_column) += 6;
		JSO_IO_STR_ADD_ESC(s->io, 4);
		JSO_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>UTF16_3          {
		JSO_SCANNER_LOC(last_column) += 6;
		JSO_IO_STR_ADD_ESC(s->io, 3);
		JSO_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>UTF16_4          {
		JSO_SCANNER_LOC(last_column) += 12;
		JSO_IO_STR_ADD_ESC(s->io, 8);
		JSO_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>UCS2             {
		JSO_SCANNER_ERROR(JSO_ERROR_UTF16);
	}
	<STR_P1>ESC              {
		JSO_SCANNER_LOC(last_column) += 2;
		JSO_IO_STR_ADD_ESC(s->io, 1);
		JSO_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>ESCPREF           {
		JSO_SCANNER_ERROR(JSO_ERROR_ESCAPE);
	}
	<STR_P1>["]              {
		JSO_SCANNER_LOC(last_column)++;
		size_t len = JSO_IO_STR_LENGTH(s->io) - JSO_IO_STR_GET_ESC(s->io);
		jso_string *str = jso_string_alloc(len);
		if (str == NULL) {
			return JSO_T_ENOMEM;
		}
		JSO_VALUE_SET_STRING(s->value, str);
		if (len == 0) {
			JSO_CONDITION_SET(JS);
			return JSO_T_ESTRING;
		}
		JSO_STRING_LEN(str) = len;
		if (JSO_IO_STR_GET_ESC(s->io)) {
			s->pstr = JSO_SVAL(s->value);
			JSO_IO_CURSOR(s->io) = JSO_IO_STR_GET_START(s->io);
			JSO_CONDITION_SET(STR_P2);
			JSO_CONDITION_GOTO(STR_P2);
		} else {
			memcpy(JSO_SVAL(s->value), JSO_IO_STR_GET_START(s->io), len * sizeof(jso_ctype));
			JSO_CONDITION_SET(JS);
			return JSO_T_STRING;
		}
	}
	<STR_P1>UTF8_1           {
		JSO_SCANNER_LOC(last_column)++;
		JSO_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>UTF8_2           {
		JSO_SCANNER_LOC(last_column) += 2;
		JSO_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>UTF8_3           {
		JSO_SCANNER_LOC(last_column) += 3;
		JSO_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>UTF8_4           {
		JSO_SCANNER_LOC(last_column) += 4;
		JSO_CONDITION_GOTO(STR_P1);
	}
	<STR_P1>ANY              {
		JSO_SCANNER_ERROR(JSO_ERROR_UTF8);
	}

	<STR_P2>UTF16_1             {
		int utf16 = jso_ucs2_to_int(s, 2);
		JSO_SCANNER_COPY_UTF();
		*(s->pstr++) = (jso_ctype) utf16;
		JSO_IO_STR_SET_START(s->io);
		JSO_CONDITION_GOTO(STR_P2);
	}
	<STR_P2>UTF16_2             {
		int utf16 = jso_ucs2_to_int(s, 3);
		JSO_SCANNER_COPY_UTF();
		*(s->pstr++) = (jso_ctype) (0xc0 | (utf16 >> 6));
		*(s->pstr++) = (jso_ctype) (0x80 | (utf16 & 0x3f));
		JSO_IO_STR_SET_START(s->io);
		JSO_CONDITION_GOTO(STR_P2);
	}
	<STR_P2>UTF16_3             {
		int utf16 = jso_ucs2_to_int(s, 4);
		JSO_SCANNER_COPY_UTF();
		*(s->pstr++) = (jso_ctype) (0xe0 | (utf16 >> 12));
		*(s->pstr++) = (jso_ctype) (0x80 | ((utf16 >> 6) & 0x3f));
		*(s->pstr++) = (jso_ctype) (0x80 | (utf16 & 0x3f));
		JSO_IO_STR_SET_START(s->io);
		JSO_CONDITION_GOTO(STR_P2);
	}
	<STR_P2>UTF16_4             {
		int utf32, utf16_hi, utf16_lo;
		utf16_hi = jso_ucs2_to_int(s, 4);
		utf16_lo = jso_ucs2_to_int_ex(s, 4, 7);
		utf32 = ((utf16_lo & 0x3FF) << 10) + (utf16_hi & 0x3FF) + 0x10000;
		jso_scanner_copy_string(s, 11);
		*(s->pstr++) = (jso_ctype) (0xf0 | (utf32 >> 18));
		*(s->pstr++) = (jso_ctype) (0x80 | ((utf32 >> 12) & 0x3f));
		*(s->pstr++) = (jso_ctype) (0x80 | ((utf32 >> 6) & 0x3f));
		*(s->pstr++) = (jso_ctype) (0x80 | (utf32 & 0x3f));
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
				JSO_SCANNER_ERROR(JSO_ERROR_ESCAPE);
		}
		*(s->pstr++) = esc;
		++YYCURSOR;
		JSO_IO_STR_SET_START(s->io);
		JSO_CONDITION_GOTO(STR_P2);
	}
	<STR_P2>["] => JS        {
		JSO_SCANNER_COPY_ESC();
		return JSO_T_STRING;
	}
	<STR_P2>ANY              { JSO_CONDITION_GOTO(STR_P2); }

	<*>ANY                   {
		JSO_SCANNER_ERROR(JSO_ERROR_TOKEN);
	}
*/

}

