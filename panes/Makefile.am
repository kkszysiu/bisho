panedir = $(pkglibdir)
pane_LTLIBRARIES = libflickr.la liboauth.la

AM_CPPFLAGS = $(DEPS_CFLAGS) -DLIBEXECDIR=\"@libexecdir@\"
AM_LDFLAGS = -module -avoid-version

libflickr_la_SOURCES = flickr.c flickr.h

liboauth_la_SOURCES = oauth.c oauth.h

