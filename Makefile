CC = gcc
BIT = 64 # Set to either 64 or 32
CFLAGS = -ggdb -m$(BIT) -rdynamic
PROG = FACT-BETA

SRCS = main.c malloc_replacements.c functions.c string.c \
	management.c math.c mem.c shell.c \
	comparisons.c parser.c primitives.c eval.c \
	errorman.c statements.c utilities.c execfile.c \
	mpc_functions.c modules.c error_check.c bytecode.c

LIBS = -lgc -lgmp -ldl 

all: $(PROG)
	cd ./stdlib ; make ; 

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

help:
	@echo To install FACT-BETA completely, in your shell enter these three commands:
	@echo sudo make install_headers
	@echo make
	@echo sudo make install

install_headers:
	if test -d /usr/include/FACT; \
	then rm -r /usr/include/FACT/; \
	fi
	cp -r ./includes /usr/include/FACT/           # install the development headers

install:
	cp ./$(PROG) /usr/bin/$(PROG)
	chmod +x /usr/bin/$(PROG)                     # this is to make sure that FACT is an executable
	if test -d /etc/FACT; \
	then rm -r /etc/FACT; \
	fi
	mkdir /etc/FACT                               # make the folder containing all the data
	mkdir /etc/FACT/modules
	mkdir /etc/FACT/include
	cp ./stdlib/stdlib.soft /etc/FACT/modules/    # install the stdlib so
	cp ./stdincludes/stdlib.ft /etc/FACT/include/ # install the stdlib ft

clean:
	rm -f $(PROG)
	cd ./stdlib ; make clean ;
