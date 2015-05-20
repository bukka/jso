AM_CFLAGS = -Wall -std=c99

bin_PROGRAMS = jso
jso_SOURCES =  main.c jso.c jso_cli.c jso_array.c jso_object.c \
	jso_encoder.c jso_scanner.c jso_parser.tab.c io/jso_io_file.c
include_HEADERS = jso.h jso_types.h jso_cli.h jso_array.h jso_object.h \
	jso_encoder.h jso_parser.h jso_parser.tab.h jso_scanner.h jso_tokens.h \
	jso_io.h io/jso_io_file.h

jso_scanner.c: jso_scanner.re
	$(RE2C) -t jso_scanner_defs.h -bci jso_scanner.re > jso_scanner.c

jso_parser.tab.c: jso_parser.y
	$(YACC) --file-prefix=jso_parser -d jso_parser.y
