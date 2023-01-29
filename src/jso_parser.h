/*
 * Copyright (c) 2012-2023 Jakub Zelenka. All rights reserved.
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

#ifndef JSO_PARSER_H
#define JSO_PARSER_H

#include "jso.h"
#include "jso_scanner.h"

/**
 * @brief Parser state.
 */
typedef struct _jso_parser {
	jso_scanner scanner;
	jso_value result;
	jso_uint depth;
	jso_uint max_depth;
} jso_parser;

/**
 * @brief Initialize parser.
 *
 * @param parser Parser instance.
 */
void jso_parser_init(jso_parser *parser);

/**
 * @brief Initialize parser.
 *
 * @param parser Parser instance.
 * @return int Parsing result.
 */
int jso_yyparse(jso_parser *parser);

#endif /* JSO_SCANNER_H */
