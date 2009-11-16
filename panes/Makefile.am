panedir = $(pkglibdir)
pane_LTLIBRARIES = libflickr.la
libflickr_la_SOURCES = flickr.c flickr.h
libflickr_la_CPPFLAGS = $(DEPS_CFLAGS) -DLIBEXECDIR=\"@libexecdir@\"
libflickr_la_LIBADD = $(DEPS_LIBS)
libflickr_la_LDFLAGS = -module -avoid-version
