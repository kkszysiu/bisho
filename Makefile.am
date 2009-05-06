SUBDIRS = tools po

bin_PROGRAMS = bisho

bisho_SOURCES = \
	main.c \
	bisho-window.c bisho-window.h \
	bisho-oauth-pane.c bisho-oauth-pane.h \
	bisho-utils.c bisho-utils.h \
	entry.c entry.h \
	service-info.c service-info.h

bisho_CPPFLAGS = $(DEPS_CFLAGS)
bisho_LDFLAGS = $(DEPS_LIBS)

desktopdir = $(datadir)/applications
desktop_in_files = bisho.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

CLEANFILES = $(desktop_DATA)
EXTRA_DIST = $(desktop_in_files)
