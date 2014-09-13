ACLOCAL_AMFLAGS = -I m4 --install
dist_doc_DATA = README.md

EXTRA_DIST = m4/NOTES

lib_LTLIBRARIES = libpatts.la
libpatts_la_SOURCES = src/auth.c src/conn.c src/export.c src/upload.c
include_HEADERS = src/auth.h src/conn.h src/export.h src/upload.h

AM_CFLAGS = $(DEPS_CFLAGS)
AM_LIBS = $(DEPS_LIBS)
