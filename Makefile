CC          = gcc
BIT         = 64 # Set to either 64 or 32
CFLAGS      = -g3 -gdwarf-2 -m$(BIT) -rdynamic
PROG        = FACT-BETA
INCLUDE_DIR = ./Include
API_INC_DIR = ./API_Include

SRCS = main.c malloc_replacements.c functions.c 	    \
	management.c math.c mem.c shell.c BIFs.c            \
	comparisons.c parser.c primitives.c eval.c          \
	errorman.c statements.c utilities.c execfile.c      \
	mpc_functions.c modules.c bytecode.c sprout.c 

LIBS = -lgc -lgmp -ldl -lpthread

all: $(PROG)
	cd ./stdlib ; make ; 

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS) -I$(INCLUDE_DIR)

help:
	@echo To install FACT-BETA completely, in your shell enter these three commands:
	@echo sudo make install_headers
	@echo make
	@echo sudo make install

install_headers:
	if test -d /usr/include/FACT; \
	then rm -r /usr/include/FACT/; \
	fi
	cp -r $(API_INC_DIR) /usr/include/FACT/ # install the development headers

install:
	cp ./$(PROG) /usr/bin/$(PROG)
	chmod +x /usr/bin/$(PROG) # this is to make sure that FACT is an executable
	if test -d /etc/FACT; \
	then rm -r /etc/FACT; \
	fi
	mkdir /etc/FACT # make the folder containing all the data
	mkdir /etc/FACT/modules
	mkdir /etc/FACT/include
	cp ./stdlib/stdlib.soft /etc/FACT/modules/    # install the stdlib so
	cp ./stdincludes/stdlib.ft /etc/FACT/include/ # install the stdlib ft

clean:
	rm -f $(PROG)
	cd ./stdlib ; make clean ;
