#
# FNC

ACLOCAL_AMFLAGS = -I $(top_srcdir)/m4
AUTOMAKE_OPTIONS = foreign

sbin_PROGRAMS = fnc
fnc_SOURCES = \
    includes.h defines.h \
    pkt.h \
    buffer.c buffer.h \
    configlib.c configlib.h \
    config.c \
    tool.c tool.h \
    privsep.c privsep_fdpass.c privsep.h \
    wrr.c \
    crypto.c crypto.h \
    log.c log.h \
    reorder.h reorder.c \
    timestamp.h timestamp.c \
    tuntap_generic.c tuntap_generic.h \
    fnc.c fnc.h

# There are better ways to do that, using a "compat" library
# and using AC_REPLACE_FUNCS.
if !HAVE_STRNVIS
fnc_SOURCES += vis.c vis.h
endif
if !HAVE_CLOSEFROM
fnc_SOURCES += closefrom.c
endif
if !HAVE_SETPROCTITLE
fnc_SOURCES += setproctitle.c setproctitle.h
endif
if !HAVE_STRLCAT
fnc_SOURCES += strlcat.c
endif
if !HAVE_STRLCPY
fnc_SOURCES += strlcpy.c
endif

if LINUX
fnc_SOURCES += tuntap_linux.c systemd.c systemd.h
endif

if BSD
fnc_SOURCES += tuntap_bsd.c
endif

if DARWIN
fnc_SOURCES += tuntap_darwin.c
endif

if ENABLE_CONTROL
fnc_SOURCES += control.c control.h
endif

# Always include thoses files for distribution
EXTRA_DIST = \
    closefrom.c \
    setproctitle.c setproctitle.h \
    strlcat.c \
    strlcpy.c \
    vis.c vis.h \
    tuntap_linux.c \
    tuntap_bsd.c \
    systemd.c systemd.h \
    control.c control.h

fnc_LDADD=-lm $(libsodium_LIBS) $(libev_LIBS)
fnc_CFLAGS=$(CFLAGS) $(libsodium_CFLAGS) $(libev_CFLAGS)

if HAVE_FILTERS
fnc_SOURCES += filters.c
fnc_LDADD += $(libpcap_LIBS)
fnc_CFLAGS += $(libpcap_CFLAGS)
endif
