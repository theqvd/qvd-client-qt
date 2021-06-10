CPUS=$(shell grep ^processor /proc/cpuinfo | wc -l )
PREFIX=/usr/local
DESTDIR=/

BINDIR=bin
LIBDIR=lib64

.PHONY: all

all: build/QVD_Client

.PHONY: clean

clean:
	rm -rf build

.PHONY: install
install: install_files
	ldconfig

.PHONY: install_files
install_files:
	mkdir -p $(DESTDIR)/$(PREFIX)/$(BINDIR) $(DESTDIR)/$(PREFIX)/$(LIBDIR)
	install -m 755 build/gui/QVD_Client $(DESTDIR)/$(PREFIX)/$(BINDIR)
	install -m 755 build/libqvdclient/libqvdclient.so.1.0.0 $(DESTDIR)/$(PREFIX)/$(LIBDIR)
	ln -s libqvdclient.so.1.0.0 $(DESTDIR)/$(PREFIX)/$(LIBDIR)/libqvdclient.so.1

build/QVD_Client: build/Makefile
	cd build && make -j$(CPUS)

build/Makefile:
	#mkdir -p build && cd build && qmake ../QVD_Client.pro
	mkdir -p build && cd build && cmake ../



