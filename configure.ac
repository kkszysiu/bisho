AC_PREREQ(2.53)
AC_INIT(bisho, 0.0)
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)

AC_CANONICAL_HOST

AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_INSTALL
AC_ISC_POSIX
AC_HEADER_STDC
AM_PROG_CC_C_O

# TODO: bit nasty, should we use gnome-common?
CFLAGS="$CFLAGS -Wall"

PKG_CHECK_MODULES(DEPS, mojito-client gtk+-2.0 gconf-2.0 mux)

AC_OUTPUT([
        Makefile
        tools/Makefile
])
