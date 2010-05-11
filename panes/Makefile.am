panedir = $(MODULESDIR)
pane_LTLIBRARIES = libflickr.la liboauth.la

AM_CPPFLAGS = $(DEPS_CFLAGS) -DLIBEXECDIR=\"@libexecdir@\" -I$(top_srcdir)/src
AM_LDFLAGS = -module -avoid-version ../src/libbisho-common.la

libflickr_la_SOURCES = flickr.c flickr.h

liboauth_la_SOURCES = oauth.c oauth.h

