CC=gcc
CFLAGS=-ggdb -std=c11

all:
	${CC} ${CFLAGS} view.c -o view
	${CC} ${CFLAGS} write.c -o write
	${CC} ${CFLAGS} send.c -o send -lcurl
	${CC} ${CFLAGS} webfetch.c -o webfetch -lcurl

clean:
	rm -f view write send webfetch
