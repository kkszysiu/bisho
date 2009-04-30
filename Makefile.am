SUBDIRS = tools

bin_PROGRAMS = bisho

bisho_SOURCES = \
	main.c \
	entry.c entry.h \
	service-info.c service-info.h
bisho_CPPFLAGS = $(DEPS_CFLAGS)
bisho_LDFLAGS = $(DEPS_LIBS)

desktopdir = $(datadir)/applications
dist_desktop_DATA = bisho.desktop
