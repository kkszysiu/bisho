panedir = $(pkglibdir)
pane_LTLIBRARIES = libflickr.la
libflickr_la_SOURCES = flickr.c flickr.h
libflickr_la_CPPFLAGS = $(DEPS_CFLAGS) -DLIBEXECDIR=\"@libexecdir@\"
libflickr_la_LIBADD = $(top_builddir)/src/libbisho.la $(DEPS_LIBS)
libflickr_la_LDFLAGS = -module -avoid-version
