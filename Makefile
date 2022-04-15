INSTALL = install
DESTDIR ?= /
PREFIX = ?= $(DESTDIR)/usr
MAKE = make
PATH_INST = $(PREFIX)/bin

all:
	mkdir bin
	$(MAKE) -C src all
	mv ./src/lidown ./bin/

intstall:
	echo "not a stable build so not installing yet"

uninstall:
	echo "dude its not even installed"

clean:
	rm -rf bin
