SUBDIRS = tools

bin_PROGRAMS = bisho

%.c: %.list Makefile.am
	glib-genmarshal --body --prefix=mojito_marshal $< > $@

%.h: %.list Makefile.am
	glib-genmarshal --header --prefix=mojito_marshal $< > $@

%-ginterface.c: ${datadir}/mojito/dbus/%.xml Makefile.am
	python $(top_srcdir)/tools/glib-ginterface-gen.py --include='"marshals.h"' --filename=$(basename $@) $< Mojito_

%-bindings.h: ${datadir}/mojito/dbus/%.xml
	dbus-binding-tool --mode=glib-client --output=$@ --prefix=$(subst -,_,$*) $^

bisho_SOURCES = \
	main.c \
	bisho-window.c bisho-window.h \
	entry.c entry.h \
	service-info.c service-info.h \
	mojito-keyfob-callout-ginterface.c mojito-keyfob-callout-ginterface.h \
	mojito-keyfob-bindings.h \
	marshals.c marshals.h

bisho_CPPFLAGS = $(DEPS_CFLAGS)
bisho_LDFLAGS = $(DEPS_LIBS)

desktopdir = $(datadir)/applications
dist_desktop_DATA = bisho.desktop

BUILT_SOURCES = \
	mojito-keyfob-callout-ginterface.c mojito-keyfob-callout-ginterface.h \
	mojito-keyfob-bindings.h \
	marshals.c marshals.h

CLEANFILES = $(BUILT_SOURCES)
