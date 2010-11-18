CC = gcc
CFLAGS = -Wall -ggdb -m64 -shared -ldl -fPIC
LIBNAME = stdlib.soft

SRCS = stdlib.c file_io.c # more will be added, I guarantee it. Maybe

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