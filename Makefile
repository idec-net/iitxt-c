CFLAGS+=-ggdb -std=c11
INSTALL=install
INSTALL_DATA=$(INSTALL) -m 644
prefix=/usr
bindir=$(prefix)/bin
docdir=$(prefix)/share/doc/iitxt-c

all:
	${CC} ${CFLAGS} view.c -o view
	${CC} ${CFLAGS} write.c -o write
	${CC} ${CFLAGS} send.c -o send -lcurl
	${CC} ${CFLAGS} webfetch.c -o webfetch -lcurl

clean:
	${RM} view write send webfetch

install:
	mkdir -p ${DESTDIR}${bindir}
	mkdir -p ${DESTDIR}${docdir}
	mkdir -p ${DESTDIR}/etc/xdg/iitxt-c
	$(INSTALL) view write send webfetch ${DESTDIR}${bindir}
	$(INSTALL_DATA) README.md ${DESTDIR}${docdir}
	$(INSTALL_DATA) config.default.cfg ${DESTDIR}/etc/xdg/iitxt-c/

uninstall:
	rm ${DESTDIR}${bindir}/view
	rm ${DESTDIR}${bindir}/write
	rm ${DESTDIR}${bindir}/send
	rm ${DESTDIR}${bindir}/webfetch
	rm ${DESTDIR}${docdir}/README.md
	rmdir ${DESTDIR}${docdir}
	rm ${DESTDIR}/etc/xdg/iitxt-c/config.default.cfg
	rmdir ${DESTDIR}/etc/xdg/iitxt-c
