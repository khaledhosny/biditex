AC_INIT([BiDiTeX],
        [0.0.1],
        [https://github.com/khaledhosny/biditex/issues],
        [biditex],
        [https://github.com/khaledhosny/biditex])

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_SILENT_RULES([yes])

AC_PROG_CC
AC_PROG_RANLIB
PKG_PROG_PKG_CONFIG([0.20])

PKG_CHECK_MODULES(FRIBIDI, fribidi, , AC_MSG_ERROR([FriBiDi is not found]))

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
