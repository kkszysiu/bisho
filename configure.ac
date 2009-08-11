AC_PREREQ(2.53)
AC_INIT(bisho, 0.11)
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(src/config.h)

AC_CANONICAL_HOST

AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_INSTALL
AC_ISC_POSIX
AC_HEADER_STDC
AM_PROG_CC_C_O

AS_AC_EXPAND(BINDIR, $bindir)

AC_SUBST(LIBEXECDIR)

GETTEXT_PACKAGE=bisho
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [The name of the gettext package.])
AC_SUBST(GETTEXT_PACKAGE)
AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.40], [no-xml])

PKG_CHECK_MODULES(DEPS, mojito-client mojito-keystore gtk+-2.0 gconf-2.0 gnome-keyring-1 libsoup-2.4 rest >= 0.6 unique-1.0)

AM_GCONF_SOURCE_2

AC_OUTPUT([
        Makefile
        data/Makefile
        data/callback.schemas
        src/Makefile
        po/Makefile.in
])
