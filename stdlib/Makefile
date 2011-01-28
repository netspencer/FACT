CC = gcc
BIT = 64 # Set to either 64 or 32
CFLAGS = -Wall -ggdb -m$(BIT) -shared -ldl -fPIC
LIBNAME = stdlib.soft

SRCS = stdlib.c file_io.c lock.c variadic.c casts.c random.c

all: $(LIBNAME)

$(LIBNAME):	$(SRCS)
	$(CC) $(CFLAGS) -o $(LIBNAME) $(SRCS)

clean:
	rm -f $(LIBNAME)
