CC = gcc
CFLAGS = -Wall -ggdb -m64
PFLAGS = -pg
PROG = FACT-GCC
PARSERV = 3

SRCS = main.c malloc_replacements.c functions.c string.c \
	management.c scroll.c math.c mem.c shell.c \
	comparisons.c parser.c primitives.c eval.c \
	errorman.c statements.c utilities.c execfile.c \
	mpc_functions.c modules.c

LIBS = -lgc -lgmp -ldl

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -D PARSING=$(PARSERV) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG)

profile:	$(SRCS)	
	$(CC) $(CFLAGS) -D PARSING=$(PARSERV) -o $(PROG) $(SRCS) $(LIBS) $(PFLAGS)