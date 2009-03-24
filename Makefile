PKGS=mojito-client gtk+-2.0
CPPFLAGS=`pkg-config --cflags $(PKGS)`
CFLAGS=-g -Wall
LDFLAGS=`pkg-config --libs $(PKGS)`

all: main
