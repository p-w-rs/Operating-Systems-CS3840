CC=gcc
CFLAGS=-Wall -Wextra -g
LFLAGS=-lrt

BINS=$(patsubst %.c,%,$(wildcard *.c))

all: $(BINS)

clean:
	rm -f $(BINS) MYFILE

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LFLAGS)
