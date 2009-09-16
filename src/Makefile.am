bin_PROGRAMS = bisho

bisho_SOURCES = \
	main.c \
	bisho-window.c bisho-window.h \
	bisho-pane.c bisho-pane.h \
	bisho-pane-oauth.c bisho-pane-oauth.h \
	bisho-pane-flickr.c bisho-pane-flickr.h \
	bisho-utils.c bisho-utils.h \
	entry.c entry.h \
	service-info.c service-info.h \
	mux-banner.c mux-banner.h \
	mux-expander.c mux-expander.h \
	mux-expanding-item.c mux-expanding-item.h \
	mux-frame.c mux-frame.h \
	mux-icon-button.c mux-icon-button.h \
	mux-window.c mux-window.h

bisho_CPPFLAGS = $(DEPS_CFLAGS) \
	-DLIBEXECDIR=\"@libexecdir@\" \
	-DLOCALEDIR=\""$(datadir)/locale"\"  \
	-Wall -Wmissing-declarations
bisho_LDADD = $(DEPS_LIBS)
