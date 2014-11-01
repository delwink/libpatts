SUBDIRS = include
ACLOCAL_AMFLAGS = -I m4 --install
dist_doc_DATA = README.md
licensedir = $(datadir)/licenses/$(PACKAGE_NAME)
dist_license_DATA = COPYING
pkgconfigdir = $(libdir)/pkgconfig
dist_pkgconfig_DATA = patts.pc

lib_LTLIBRARIES = libpatts.la
libpatts_la_LDFLAGS = -version-info 0:0:0
libpatts_la_SOURCES = src/admin.c src/conn.c src/get.c src/query.c
libpatts_la_CFLAGS = -Wall -Wextra -std=c11 -Iinclude/patts `pkg-config --libs --cflags icu-uc icu-io cquel` `mysql_config --cflags --libs`

AM_CFLAGS = $(DEPS_CFLAGS)
AM_LIBS = $(DEPS_LIBS)
