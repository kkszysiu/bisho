AC_PREREQ(2.53)
AC_INIT(bisho, 0.2)
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)

AC_CANONICAL_HOST

AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_INSTALL
AC_ISC_POSIX
AC_HEADER_STDC
AM_PROG_CC_C_O

AC_SUBST(LIBEXECDIR)

GETTEXT_PACKAGE=bisho
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [The name of the gettext package.])
AC_SUBST(GETTEXT_PACKAGE)
AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.40], [no-xml])

# TODO: bit nasty, should we use gnome-common?
CFLAGS="$CFLAGS -Wall"

PKG_CHECK_MODULES(DEPS, mojito-client gtk+-2.0 gconf-2.0 mux gnome-keyring-1 libsoup-2.4 rest)

AC_OUTPUT([
        Makefile
        tools/Makefile
        po/Makefile.in
])
