INSTALL = install
DESTDIR ?= /
PREFIX = ?= $(DESTDIR)/usr
MAKE = make
PATH_INST = $(PREFIX)/bin

all:
	$(MAKE) -C src all
        mkdir -p ./bin
	mv ./src/lidown ./bin/

intstall:
	echo "not a stable build so not installing yet"

uninstall:
	echo "dude its not even installed"

clean:
	rm -rf bin
