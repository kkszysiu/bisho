AM_CPPFLAGS = \
	$(DEPS_CFLAGS) \
	-DLIBEXECDIR=\"@libexecdir@\" \
	-DPKGLIBDIR=\"$(pkglibdir)\" \
	-DLOCALEDIR=\""$(datadir)/locale"\"  \
	-Wall -Wmissing-declarations
AM_LDFLAGS = \
	$(DEPS_LIBS)


lib_LTLIBRARIES = libbisho-common.la
libbisho_common_ladir = $(pkgincludedir)
libbisho_common_la_HEADERS = \
	bisho-pane.h \
	service-info.h \
	mux-label.h
libbisho_common_la_SOURCES = \
	bisho-window.c bisho-window.h \
	bisho-frame.c bisho-frame.h \
	bisho-pane-username.c bisho-pane-username.h \
	bisho-utils.c bisho-utils.h \
	mux-expander.c mux-expander.h \
	mux-expanding-item.c mux-expanding-item.h \
	bisho-pane.c bisho-pane.h \
	bisho-module.c bisho-module.h \
	service-info.c service-info.h \
	mux-label.c mux-label.h

bin_PROGRAMS = bisho
bisho_SOURCES = main.c
bisho_LDADD = libbisho-common.la

ccmodulesdir = $(EXTENSIONSDIR)
ccmodules_LTLIBRARIES = libbisho.la
libbisho_la_SOURCES = \
	module.c \
	bisho-cc-panel.c bisho-cc-panel.h
libbisho_la_CPPFLAGS = $(AM_CPPFLAGS) $(CC_CFLAGS)
libbisho_la_LIBADD = $(CC_LIBS) libbisho-common.la
libbisho_la_LDFLAGS = $(AM_LDFLAGS) -module -avoid-version
