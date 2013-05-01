# Makefile for JSO

CC = gcc
CFLAGS = -pedantic -ansi -Wall -g
RM = rm -f

OBJS = main.o jso_scanner.o jso_stream.o

%.o : %.c
	$(CC) -c $(CFLAGS) $<


.PHONY: all
all: jso_scanner.c jso

jso_scanner.c: jso_scanner.re
	re2c -i jso_scanner.re > jso_scanner.c

jso: $(OBJS)
	$(CC) -o $@ $(OBJS)

main.o: main.c jso_scanner.h jso_basics.h jso_tokens.h jso_stream.h
jso_scanner.o: jso_scanner.c jso_scanner.h jso_basics.h jso_tokens.h jso_stream.h
jso_stream.o: jso_stream.c jso_stream.h jso_basics.h

.PHONY: clean
clean:
	$(RM) *.o
	$(RM) jso_scanner.c
	$(RM) jso

# end of Makefile
