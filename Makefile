PKGS=mojito-client gtk+-2.0 gconf-2.0
CPPFLAGS=`pkg-config --cflags $(PKGS)`
CFLAGS=-g -Wall
LDFLAGS=`pkg-config --libs $(PKGS)`

bisho: Makefile main.c service-info.c service-info.h
	$(LINK.c) -o $@ $(filter %.c, $^)

clean:
	rm -f bisho
