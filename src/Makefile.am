bin_PROGRAMS = bisho

bisho_SOURCES = \
	main.c \
	bisho-window.c bisho-window.h \
	bisho-oauth-pane.c bisho-oauth-pane.h \
	bisho-flickr-pane.c bisho-flickr-pane.h \
	bisho-utils.c bisho-utils.h \
	entry.c entry.h \
	service-info.c service-info.h

bisho_CPPFLAGS = $(DEPS_CFLAGS) \
	-DLIBEXECDIR=\"@libexecdir@\" \
	-DLOCALEDIR=\""$(datadir)/locale"\"  \
	-Wall -Wmissing-declarations
bisho_LDADD = $(DEPS_LIBS)
