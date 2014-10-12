SUBDIRS = include
ACLOCAL_AMFLAGS = -I m4 --install
dist_doc_DATA = README.md

EXTRA_DIST = m4/NOTES

lib_LTLIBRARIES = libpatts.la
libpatts_la_SOURCES = src/admin.c src/conn.c src/get.c
libpatts_la_CFLAGS = -Wall -Wextra -std=c11 -Iinclude/patts `pkg-config --libs --cflags icu-uc icu-io` `mysql_config --cflags --libs`

AM_CFLAGS = $(DEPS_CFLAGS)
AM_LIBS = $(DEPS_LIBS)
