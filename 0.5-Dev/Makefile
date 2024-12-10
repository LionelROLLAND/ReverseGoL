CC = gcc
CFLAGS = -Wall -Werror -Wfatal-errors

all: Dandi

Dandi: Dandelifeon_origin.c $(DEPS)
	$(CC) $(CFLAGS) -o Dandelifeon Dandelifeon_origin.c

clean:
	$(RM) Dandelifeon
