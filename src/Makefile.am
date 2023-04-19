AM_CFLAGS = -Wall -std=c11
AM_LDFLAGS = -lm

bin_PROGRAMS = jso
jso_SOURCES =  main.c jso_value.c jso_array.c jso_object.c jso_dg_dtoa.c \
	jso_number.c jso_encoder.c jso_error.c jso_ht.c jso_scanner.c \
	jso_parser.tab.c parser/jso_parser_hooks_decode.c jso_schema.c jso_cli.c \
	io/jso_io.c io/jso_io_file.c io/jso_io_memory.c
include_HEADERS = jso.h jso_types.h jso_value.h jso_array.h jso_object.h \
	jso_dg_dtoa.h jso_bitset.h jso_number.h jso_error.h jso_encoder.h \
	jso_ht.h jso_parser.h jso_parser.tab.h jso_parser_hooks.h jso_mm.h \
	parser/jso_parser_hooks_decode.h jso_scanner.h jso_string.h jso_schema.h \
	jso_tokens.h jso_cli.h jso_io.h io/jso_io_file.h io/jso_io_memory.h

jso_scanner.c: jso_scanner.re jso_scanner.h
	$(RE2C) -t jso_scanner_defs.h -bci jso_scanner.re > jso_scanner.c

jso_parser.tab.c jso_parser.tab.h: jso_parser.y jso_parser.h
	$(YACC) --file-prefix=jso_parser -l -d jso_parser.y
