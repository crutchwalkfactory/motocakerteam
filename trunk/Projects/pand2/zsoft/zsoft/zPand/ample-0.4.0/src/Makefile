# Copyright (C) 1994, 1995-8, 1999 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


SHELL = /bin/sh

srcdir = .
top_srcdir = ..

prefix = /usr/local
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
datadir = ${prefix}/share
exec_prefix = ${prefix}
includedir = ${prefix}/include
infodir = ${prefix}/info
libdir = ${exec_prefix}/lib
libexecdir = ${exec_prefix}/libexec
localstatedir = ${prefix}/var
mandir = ${prefix}/man
oldincludedir = /usr/include
sbindir = ${exec_prefix}/sbin
sharedstatedir = ${prefix}/com
sysconfdir = ${prefix}/etc


CC = gcc
CFLAGS = -g -O2
CPPFLAGS = 
EXTRACPPFLAGS = -I$(top_srcdir)
DEFS = -DHAVE_CONFIG_H -DDEF_LOGFILE=\"$(logfile)\" -DDEF_CONFFILE=\"$(conffile)\" -DDEF_PATH=\"$(mp3path)\"
LIBS = 
LDFLAGS = 
LIBTOOL = $(CC)
INSTALL = /usr/bin/install -c

conffile = $(sysconfdir)/ample.conf
logdir  = $(localstatedir)/log
logfile = $(logdir)/ample.log
mp3path = $(datadir)/mp3

PROGRAMS = ample
SOURCES = ample.c client.c entries.c configuration.c helper.c
OBJECTS = ample.o client.o entries.o configuration.o helper.o
COMPILE = $(CC) $(CFLAGS) $(CPPFLAGS) $(EXTRACPPFLAGS) $(DEFS)
LINK = $(LIBTOOL) $(LDFLAGS)

all: ample

ample: $(OBJECTS)
	$(LINK) $(OBJECTS) $(LIBS) -o $@

.c.o: 
	$(COMPILE) -c $<

install:
	@for i in $(PROGRAMS); do \
	  echo "Installing $$i ..."; \
	  $(INSTALL) -s $$i $(bindir); \
	done
	$(INSTALL) -d $(logdir)

clean:
	rm -f *.o ample

maint-clean:
	rm -f *.o *~ Makefile ample

dist-clean:
	rm -f *.o *~ Makefile ample
	rm -rf CVS

.PHONY: all install clean maint-clean dist-clean

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
