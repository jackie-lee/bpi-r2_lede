# FNC

ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = foreign

MAINTAINERCLEANFILES = \
    config.log config.status \
    $(srcdir)/Makefile.in \
    $(srcdir)/config.h.in $(srcdir)/config.h.in~ $(srcdir)/configure \
    $(srcdir)/build-aux/install-sh $(srcdir)/missing \
    $(srcdir)/depcomp $(srcdir)/aclocal.m4 \
    $(srcdir)/config.guess $(srcdir)/config.sub \
    $(srcdir)/fnc.spec
BUILT_SOURCES = $(top_srcdir)/.version
$(top_srcdir)/.version:
	echo $(VERSION) > $(top_srcdir)/.version

SUBDIRS = src
dist_doc_DATA = AUTHORS

release:
	./autogen.sh
	make dist
