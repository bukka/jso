/*
 * Copyright (c) 2012-2015 Jakub Zelenka. All rights reserved.
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
#include <stdio.h>
#include <string.h>
#include "jso.h"
#include "jso_cli.h"
#include "jso_scanner.h"
#include "jso_parser.h"
#include "jso_encoder.h"
#include "io/jso_io_file.h"

static jso_rc jso_cli_param_callback_depth(const char *value, jso_cli_options *options);
static jso_rc jso_cli_param_callback_output(const char *value, jso_cli_options *options);
static jso_rc jso_cli_param_callback_help(jso_cli_options *options);


const jso_cli_param jso_cli_default_params[] = {
	JSO_CLI_PARAM_ENTRY_VALUE("depth",         'd',    jso_cli_param_callback_depth)
	JSO_CLI_PARAM_ENTRY_VALUE("output-type",   'o',    jso_cli_param_callback_output)
	JSO_CLI_PARAM_ENTRY_FLAG( "help",          'h',    jso_cli_param_callback_help)
	JSO_CLI_PARAM_ENTRY_END
};

JSO_API void jso_cli_register_params(jso_cli_ctx *ctx, const jso_cli_param *params)
{
	ctx->params = params;
}

JSO_API void jso_cli_register_default_params(jso_cli_ctx *ctx)
{
	jso_cli_register_params(ctx, jso_cli_default_params);
}

JSO_API jso_rc jso_cli_parse_io(jso_io *io, jso_cli_options *options)
{
	jso_rc rc;
	jso_parser parser;

	/* init scanner */
	jso_parser_init(&parser);
	jso_scanner_init(&parser.scanner, io);

	/* set max depth (0 = unlimited) */
	parser.max_depth = options->max_depth;

	/* parse */
	if (jso_yyparse(&parser) == 0) {

		/* print result */
		if (options->output_type == JSO_OUTPUT_DEBUG) {
			jso_value_print_debug(&parser.result);
		} else {
			jso_io *os = jso_io_file_open_stream(stdout);
			jso_encoder_options enc_options;
			enc_options.max_depth = JSO_ENCODER_DEPTH_UNLIMITED; /* unlimited */
			enc_options.pretty    = options->output_type == JSO_OUTPUT_PRETTY;
			jso_encode(&parser.result, os, &enc_options);
			JSO_IO_FREE(os);
		}

		rc = JSO_SUCCESS;
	} else {
		rc = JSO_FAILURE;
	}

	/* free resources */
	jso_value_free(&parser.result);
	JSO_IO_FREE(io);

	return rc;
}

JSO_API jso_rc jso_cli_parse_file(const char *file_path, jso_cli_options *options)
{
	jso_io *io;
	off_t filesize;
	size_t bytes_to_read, bytes_read_once, bytes_read_total = 0;

	/* get file size */
	filesize = jso_io_file_size(file_path);
	if (filesize < 0) {
		JSO_IO_PRINTF(options->es, "Getting file size for file '%s' failed\n", file_path);
		return JSO_FAILURE;
	}
	bytes_to_read = (size_t) filesize;

	/* open file */
	io = jso_io_file_open(file_path, "r");
	if (!io) {
		JSO_IO_PRINTF(options->es, "Opening the file '%s' failed\n", file_path);
		return JSO_FAILURE;
	}
	/* read the whole file into the buffer */
	do {
		bytes_read_once = JSO_IO_READ(io, bytes_to_read - bytes_read_total);
		bytes_read_total += bytes_read_once;
	} while (bytes_read_once);

	/* check the read data */
	if (bytes_read_total == 0) {
		JSO_IO_PRINTF(options->es, "No data in the file '%s'\n", file_path);
		return JSO_FAILURE;
	}
	/* check the read data */
	if (bytes_read_total < bytes_to_read) {
		JSO_IO_PRINTF(options->es, "Only %lu of %lu read from the file '%s'\n",
				bytes_read_total, bytes_to_read, file_path);
		return JSO_FAILURE;
	}

	if (!JSO_IO_CURSOR(io)) {
		JSO_IO_PRINTF(options->es, "Cursor is not set\n");
		return JSO_FAILURE;
	}

	return jso_cli_parse_io(io, options);
}

static jso_rc jso_cli_param_callback_depth(const char *value, jso_cli_options *options)
{
	if (!value) {
		JSO_IO_PRINTF(options->es, "Option depth requires value\n");
		return JSO_FAILURE;
	}

	options->max_depth = atoi(value);

	return JSO_SUCCESS;
}

static jso_rc jso_cli_param_callback_output(const char *value, jso_cli_options *options)
{
	if (!value) {
		JSO_IO_PRINTF(options->es, "Option output requires value\n");
		return JSO_FAILURE;
	}

	if (options->output_type != JSO_OUTPUT_NONE) {
		return JSO_FAILURE;
	}

	if (!strncmp(value, "minimal", JSO_MAX(strlen(value), sizeof("minimal") - 1))) {
		options->output_type = JSO_OUTPUT_MINIMAL;
	} else if (!strncmp(value, "pretty", JSO_MAX(strlen(value), sizeof("pretty") - 1))) {
		options->output_type = JSO_OUTPUT_PRETTY;
	} else if (!strncmp(value, "debug", JSO_MAX(strlen(value), sizeof("debug") - 1))) {
		options->output_type = JSO_OUTPUT_DEBUG;
	} else {
		JSO_IO_PRINTF(options->es, "Unknown output value %s\n", value);
		return JSO_FAILURE;
	}

	return JSO_SUCCESS;
}

static jso_rc jso_cli_param_callback_help(jso_cli_options *options)
{
	options->output_type = JSO_OUTPUT_HELP;

	return JSO_SUCCESS;
}

JSO_API void jso_cli_options_init_pre(jso_cli_options *options)
{
	options->max_depth = 0;
	options->output_type = JSO_OUTPUT_NONE;
	options->is = NULL;
	options->os = jso_io_file_open_stream(stdout);
	options->es = jso_io_file_open_stream(stderr);
}

JSO_API void jso_cli_options_init_post(jso_cli_options *options)
{
	if (options->output_type == JSO_OUTPUT_NONE)
		options->output_type = JSO_OUTPUT_PRETTY;
	if (!options->is)
		options->is = jso_io_file_open_stream(stdin);
}

JSO_API jso_rc jso_cli_options_destroy(jso_cli_options *options)
{
	jso_rc rc = JSO_SUCCESS;

	if (options->is)
		rc = JSO_IO_FREE(options->is);
	if (options->os) {
		rc = JSO_IO_FREE(options->os) == JSO_FAILURE || rc == JSO_FAILURE ?
				JSO_FAILURE : JSO_SUCCESS;
	}
	if (options->es) {
		rc = JSO_IO_FREE(options->es) == JSO_FAILURE || rc == JSO_FAILURE ?
				JSO_FAILURE : JSO_SUCCESS;
	}

	return rc;
}

static const jso_cli_param *jso_cli_find_param(jso_cli_ctx *ctx,
		const char *long_name, size_t long_name_len, char short_name)
{
	int i;
	const jso_cli_param *param = &ctx->params[0];

	for (i = 1; param->long_name; i++) {
		if (long_name) {
			if (!strncmp(param->long_name, long_name, long_name_len)) {
				return param;
			}
		} else if (param->short_name && param->short_name == short_name) {
			return param;
		}

		param = &ctx->params[i];
	}

	return NULL;
}

static inline const jso_cli_param *jso_cli_find_param_by_long_name(
		jso_cli_ctx *ctx, const char *long_name, size_t long_name_len)
{
	return jso_cli_find_param(ctx, long_name, long_name_len, 0);
}

static inline const jso_cli_param *jso_cli_find_param_by_short_name(
		jso_cli_ctx *ctx, char short_name)
{
	return jso_cli_find_param(ctx, NULL, 0, short_name);
}

static const jso_cli_param *jso_cli_parse_long_option(
		jso_cli_ctx *ctx, jso_cli_options *options,
		const char *arg, size_t arg_len, const char **pvalue)
{
	const jso_cli_param *param;
	const char *long_name, *value;
	char *long_name_dup;
	size_t long_name_len;

	if (arg_len == 2) {
		JSO_IO_PRINTF(options->es, "The long option must have name\n");
		return NULL;
	}

	long_name = &arg[2];
	value = strchr(long_name, '=');
	if (value) {
		long_name_len = (size_t) (value - long_name);
		*pvalue = value + 1;
	} else {
		long_name_len = strlen(long_name);
		*pvalue = NULL;
	}

	param = jso_cli_find_param_by_long_name(ctx, long_name, long_name_len);
	if (param) {
		return param;
	}

	long_name_dup = jso_malloc(long_name_len + 1);
	strncpy(long_name_dup, long_name, long_name_len);
	long_name_dup[long_name_len] = '\0';
	JSO_IO_PRINTF(options->es, "Unknown option: %s\n", long_name_dup);
	jso_free(long_name_dup);

	return NULL;
}

static const jso_cli_param *jso_cli_parse_short_option(
		jso_cli_ctx *ctx, jso_cli_options *options,
		const char *arg, size_t arg_len, const char **pvalue)
{
	const jso_cli_param *param;
	char short_name;

	if (arg_len > 2) {
		JSO_IO_PRINTF(options->es, "The grouping of short parameters is not allowed yet\n");
		return NULL;
	}

	*pvalue = NULL;
	short_name = arg[1];

	param = jso_cli_find_param_by_short_name(ctx, short_name);
	if (param) {
		return param;
	}

	JSO_IO_PRINTF(options->es, "Unknown option: %c\n", short_name);

	return NULL;
}

JSO_API jso_rc jso_cli_parse_args_ex(int argc, const char *argv[], jso_cli_ctx *ctx)
{
	int i;
	jso_rc rc;
	const char *file_path = NULL, *value;
	size_t arg_len;
	const jso_cli_param *param;
	jso_cli_options options;

	/* pre-initialize options */
	jso_cli_options_init_pre(&options);

	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			arg_len = strlen(argv[i]);

			if (arg_len == 1) {
				JSO_IO_PRINTF(options.es, "Input stdin is not supported yet\n");
				return JSO_FAILURE;
			}

			param = (argv[i][1] == '-') ?
				jso_cli_parse_long_option(ctx, &options, &argv[i][0], arg_len, &value) :
				jso_cli_parse_short_option(ctx, &options, &argv[i][0], arg_len, &value);

			if (!param) {
				return JSO_FAILURE;
			}

			if (param->has_value) {
				if (!value) {
					if (i + 1 == argc || argv[i + 1][0] == '-') {
						JSO_IO_PRINTF(options.es, "No value for option: %s\n", param->long_name);
						return JSO_FAILURE;
					}
					value = &argv[i + 1][0];
				}

				if (param->callback.callback_value(value, &options) == JSO_FAILURE) {
					return JSO_FAILURE;
				}
			} else {
				if (value) {
					JSO_IO_PRINTF(options.es, "No value should be added to option: %s\n", param->long_name);
					return JSO_FAILURE;
				}

				if (param->callback.callback_flag(&options) == JSO_FAILURE) {
					return JSO_FAILURE;
				}
			}


		} else {
			file_path = &argv[i][0];
		}
	}

	if (!file_path) {
		JSO_IO_PRINTF(options.es, "File path not specified\n");
		return JSO_FAILURE;
	}

	/* post-initialize options */
	jso_cli_options_init_post(&options);

	/* parse file */
	rc = jso_cli_parse_file(file_path, &options);

	/* destroy options */
	jso_cli_options_destroy(&options);

	return rc;
}

JSO_API jso_rc jso_cli_parse_args(int argc, const char *argv[])
{
	jso_cli_ctx ctx;

	jso_cli_register_default_params(&ctx);

	return jso_cli_parse_args_ex(argc, argv, &ctx);
}
