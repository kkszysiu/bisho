PKGS=mojito-client gtk+-2.0 gconf-2.0
CPPFLAGS=`pkg-config --cflags $(PKGS)`
CFLAGS=-g -Wall
LDFLAGS=`pkg-config --libs $(PKGS)`

bisho: main.c service-info.c
	$(LINK.c) -o $@ $^
