AC_PREREQ([2.60])
AC_INIT([libpatts],[1.0.1],[support@delwink.com])

AC_CONFIG_SRCDIR([src/patts.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES])
AM_SILENT_RULES([yes])
AM_PROG_AR
LT_INIT
AC_PROG_CC

PKG_CHECK_MODULES([sqon], [sqon])
PKG_CHECK_MODULES([jansson], [jansson])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 include/Makefile
])
AC_OUTPUT
