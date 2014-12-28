AM_CFLAGS = -ansi -Wall

bin_PROGRAMS = jso
jso_SOURCES =  main.c jso.c jso_cli.c jso_array.c jso_object.c jso_scanner.c jso_parser.tab.c io/jso_file.c
include_HEADERS = jso.h jso_types.h jso_cli.h jso_array.h jso_object.h jso_parser.h jso_parser.tab.h jso_scanner.h jso_tokens.h jso_io.h io/jso_file.h

jso_scanner.c: jso_scanner.re
	re2c -t jso_scanner_defs.h -bci jso_scanner.re > jso_scanner.c

jso_parser.tab.c: jso_parser.y
	$(YACC) -d jso_parser.y
