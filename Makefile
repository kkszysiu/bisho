CPPFLAGS=`pkg-config --cflags mojito-client gtk+-2.0`
CFLAGS=-g -Wall
LDFLAGS=`pkg-config --libs mojito-client gtk+-2.0`

all: main
