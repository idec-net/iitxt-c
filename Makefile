CC="gcc"
CFLAGS="-ggdb"

all:
	${CC} ${CFLAGS} viewer.c -o viewer
	${CC} ${CFLAGS} writer.c -o writer
	${CC} ${CFLAGS} sender.c -o sender -lcurl
	${CC} ${CFLAGS} webfetch.c -o webfetch -lcurl

clean:
	rm -f viewer writer sender webfetch