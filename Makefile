CC = gcc
CFLAGS =  -ggdb -m64 -rdynamic
PFLAGS = -pg
PROG = FACT-BETA
PARSERV = 3

SRCS = main.c malloc_replacements.c functions.c string.c \
	management.c scroll.c math.c mem.c shell.c \
	comparisons.c parser.c primitives.c eval.c \
	errorman.c statements.c utilities.c execfile.c \
	mpc_functions.c modules.c

LIBS = -lgc -lgmp -ldl

all: $(PROG)
	cd ./stdlib ; make ; 

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -D PARSING=$(PARSERV) -o $(PROG) $(SRCS) $(LIBS)

install:
	cp ./FACT-GCC /usr/bin/FACT-BETA
	chmod +x /usr/bin/FACT-BETA                   # this is to make sure that FACT is an executable
	rm -r /usr/include/FACT/                      # clear them out
	rm -r /etc/FACT
	cp -r ./includes /usr/include/FACT/           # install the development headers
	mkdir /etc/FACT                               # make the folder containing all the data
	mkdir /etc/FACT/modules
	mkdir /etc/FACT/include
	cp ./stdlib/stdlib.soft /etc/FACT/modules/    # install the stdlib so
	cp ./stdincludes/stdlib.ft /etc/FACT/include/ # install the stdlib ft

clean:
	rm -f $(PROG)
	cd ./stdlib ; make clean ;
profile:	$(SRCS)	
	$(CC) $(CFLAGS) -D PARSING=$(PARSERV) -o $(PROG) $(SRCS) $(LIBS) $(PFLAGS)

#libtest:
#	gcc -shared -ldl -fPIC -ggdb -rdynamic lib_test.c -o lib_test.so