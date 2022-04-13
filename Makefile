INSTALL = install
DESTDIR ?= /
PREFIX = ?= $(DESTDIR)/usr
MAKE = make
PATH_INST = $(PREFIX)/bin

all:
	$(MAKE) -C src all
	mv ./src/lidown ./

intstall:
	echo "not a stable build so not installing yet"

uninstall:
	echo "dude its not even installed"

clean:
	rm -rf lidown
