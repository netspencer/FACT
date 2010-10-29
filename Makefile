CC = gcc
CFLAGS =  -ggdb -m64 -rdynamic
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

install:
	cp ./FACT-GCC /usr/bin/FACT
	chmod +x /usr/bin/FACT # this is to make sure that FACT is an executable
	rm -r /usr/include/FACT/ # clear it out
	cp -r ./includes /usr/include/FACT/ # install the development headers 

clean:
	rm -f $(PROG)

profile:	$(SRCS)	
	$(CC) $(CFLAGS) -D PARSING=$(PARSERV) -o $(PROG) $(SRCS) $(LIBS) $(PFLAGS)

#libtest:
#	gcc -shared -ldl -fPIC -ggdb -rdynamic lib_test.c -o lib_test.so