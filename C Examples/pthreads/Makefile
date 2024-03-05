CC=gcc
CFLAGS=-Wall -Wextra -g -pthread

BINS=$(patsubst %.c,%,$(wildcard *.c))

all: $(BINS)

clean:
	rm -f $(BINS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<
