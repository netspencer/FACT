CC = gcc
BIT = 32 # Set to either 64 or 32
CFLAGS = -Wall -ggdb -m$(BIT) -shared -ldl -fPIC
LIBNAME = stdlib.soft

SRCS = stdlib.c file_io.c lock.c # more will be added, I guarantee it. Maybe

all: $(LIBNAME)

$(LIBNAME):	$(SRCS)
	$(CC) $(CFLAGS) -o $(LIBNAME) $(SRCS)

#install:
#	cp ./FACT-GCC /usr/bin/FACT
#	chmod +x /usr/bin/FACT # this is to make sure that FACT is an executable
#	cp -r ./includes /usr/include/FACT/ # install the development headers 

clean:
	rm -f $(LIBNAME)

#profile:	$(SRCS)	
#	$(CC) $(CFLAGS) -D PARSING=$(PARSERV) -o $(PROG) $(SRCS) $(LIBS) $(PFLAGS)