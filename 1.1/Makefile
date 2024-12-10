CC = gcc
CFLAGS = -Wall -Werror -Wfatal-errors

DEPS = ilist.c plist.c utils.c specific.c

all: Dandi

Dandi: main.c $(DEPS)
	$(CC) $(CFLAGS) -o Dandelifeon $(DEPS) main.c

clean:
	$(RM) Dandelifeon
