CC = gcc
CFLAGS = -Wall -Werror -Wfatal-errors

all: Dandi

Dandi: Dandelifeon_debug.c $(DEPS)
	$(CC) $(CFLAGS) -o Dandelifeon Dandelifeon_debug.c

clean:
	$(RM) Dandelifeon
