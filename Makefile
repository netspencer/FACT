CC = gcc
CFLAGS = -Wall -ggdb -m64
PROG = FACT-GCC
PARSERV = 3

SRCS = main.c gmp_mallocs.c functions.c string.c \
	management.c scroll.c math.c mem.c shell.c \
	operators.c parser.c primitives.c eval.c \
	errorman.c statements.c utilities.c execfile.c

LIBS = -lgc -lgmp

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -D PARSING=$(PARSERV) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG)