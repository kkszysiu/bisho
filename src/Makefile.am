AM_CPPFLAGS = $(DEPS_CFLAGS) \
	-DLIBEXECDIR=\"@libexecdir@\" \
	-DLOCALEDIR=\""$(datadir)/locale"\"  \
	-Wall -Wmissing-declarations

lib_LTLIBRARIES = libbisho.la
libbisho_la_SOURCES = \
	bisho-pane.c bisho-pane.h \
	service-info.c service-info.h \
	mux-label.c mux-label.h
if ! HAVE_INFOBAR
libbisho_la_SOURCES += gtkinfobar.h gtkinfobar.c
endif
libbisho_la_LIBADD = $(DEPS_LIBS)


bin_PROGRAMS = bisho

bisho_SOURCES = \
	main.c \
	bisho-window.c bisho-window.h \
	bisho-pane-flickr.c bisho-pane-flickr.h \
	bisho-pane-oauth.c bisho-pane-oauth.h \
	bisho-pane-username.c bisho-pane-username.h \
	bisho-utils.c bisho-utils.h \
	mux-expander.c mux-expander.h \
	mux-expanding-item.c mux-expanding-item.h
bisho_LDADD = ./libbisho.la $(DEPS_LIBS)
