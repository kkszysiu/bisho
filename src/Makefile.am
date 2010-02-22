libbisho_ladir = $(pkgincludedir)
libbisho_la_HEADERS = \
	bisho-pane.h \
	service-info.h \
	mux-label.h


bin_PROGRAMS = bisho
bisho_SOURCES = \
	main.c \
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
bisho_CPPFLAGS = $(DEPS_CFLAGS) \
	-DLIBEXECDIR=\"@libexecdir@\" \
	-DPKGLIBDIR=\"$(pkglibdir)\" \
	-DLOCALEDIR=\""$(datadir)/locale"\"  \
	-Wall -Wmissing-declarations
bisho_LDADD = $(DEPS_LIBS)


ccmodulesdir = $(EXTENSIONSDIR)
ccmodules_LTLIBRARIES = libbisho.la
libbisho_la_SOURCES = \
	module.c \
	bisho-cc-panel.c bisho-cc-panel.h \
	bisho-cc-page.c bisho-cc-page.h \
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
libbisho_la_CPPFLAGS = \
	$(DEPS_CFLAGS) \
	$(CC_CFLAGS) \
	-DLIBEXECDIR=\"@libexecdir@\" \
	-DPKGLIBDIR=\"$(pkglibdir)\" \
	-DLOCALEDIR=\""$(datadir)/locale"\"  \
	-Wall -Wmissing-declarations
libbisho_la_LIBADD = $(DEPS_LIBS) $(CC_LIBS)
