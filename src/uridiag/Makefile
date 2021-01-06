
CFLAGS=-Wall

all:	uridiag

install: all
	install -m 755 uridiag /usr/bin/uridiag

uridiag:	uridiag.c fftsg.c
	cc -Wall uridiag.c fftsg.c -o uridiag -lusb -lasound -lpthread -lm


