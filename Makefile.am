SUBDIRS = include
ACLOCAL_AMFLAGS = -I m4 --install
dist_doc_DATA = README
licensedir = $(datadir)/licenses/$(PACKAGE_NAME)
dist_license_DATA = COPYING
pkgconfigdir = $(libdir)/pkgconfig
dist_pkgconfig_DATA = patts.pc

lib_LTLIBRARIES = libpatts.la
libpatts_la_SOURCES = src/admin.c src/get.c src/internal.c src/patts.c\
src/setup.c src/user.c

libpatts_la_LDFLAGS = -version-info 1:0:0

libpatts_la_CFLAGS = -Wall -Wextra -std=c11 -Iinclude/patts

libpatts_la_LIBADD = $(sqon_LIBS) $(jansson_LIBS)

AM_CFLAGS = $(DEPS_CFLAGS)
AM_LIBS = $(DEPS_LIBS)
