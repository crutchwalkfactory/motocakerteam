# Makefile for directory with message catalog handling library of GNU gettext
# Copyright (C) 1995-1998, 2000-2003 Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU Library General Public License as published
# by the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public
# License along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
# USA.

PACKAGE = e2fsprogs
VERSION = 0.14.1

SHELL = /bin/sh

srcdir = .
top_srcdir = ..
top_builddir = ..


prefix = /usr
exec_prefix = ${prefix}
datarootdir = ${prefix}/share
transform = s,x,x,
libdir = ${exec_prefix}/lib
includedir = ${prefix}/include
datadir = ${datarootdir}
localedir = $(datadir)/locale
gettextsrcdir = $(datadir)/gettext/intl
aliaspath = $(localedir)
subdir = intl

INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
MKINSTALLDIRS = $(top_builddir)/config/mkinstalldirs
mkinstalldirs = $(SHELL) $(MKINSTALLDIRS)

l = 

AR = ar
CC = arm-linux-gcc
LIBTOOL = @LIBTOOL@
RANLIB = arm-linux-ranlib
YACC = : -y -d
YFLAGS = --name-prefix=__gettext

DEFS = -DLOCALEDIR=\"$(localedir)\" -DLOCALE_ALIAS_PATH=\"$(aliaspath)\" \
-DLIBDIR=\"$(libdir)\" -DIN_LIBINTL \
-DENABLE_RELOCATABLE=1 -DIN_LIBRARY -DINSTALLDIR=\"$(libdir)\" -DNO_XMALLOC \
-Dset_relocation_prefix=libintl_set_relocation_prefix \
-Drelocate=libintl_relocate \
-DDEPENDS_ON_LIBICONV=1 -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DENABLE_HTREE=1 -DCONFIG_TESTIO_DEBUG=1 -DUSE_UUIDD=1 -DPACKAGE=\"e2fsprogs\" -DVERSION=\"0.14.1\" -DHAVE_LONG_LONG=1 -DHAVE_LONG_DOUBLE=1 -DHAVE_WCHAR_T=1 -DHAVE_WINT_T=1 -DHAVE_INTTYPES_H_WITH_UINTMAX=1 -DHAVE_STDINT_H_WITH_UINTMAX=1 -DHAVE_INTMAX_T=1 -DHAVE_POSIX_PRINTF=1 -DHAVE_ALLOCA_H=1 -DHAVE_ALLOCA=1 -DHAVE_STDLIB_H=1 -DHAVE_UNISTD_H=1 -DHAVE_GETPAGESIZE=1 -DINTDIV0_RAISES_SIGFPE=0 -DHAVE_UNSIGNED_LONG_LONG=1 -DHAVE_UINTMAX_T=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_STDINT_H=1 -DHAVE_ARGZ_H=1 -DHAVE_LIMITS_H=1 -DHAVE_LOCALE_H=1 -DHAVE_NL_TYPES_H=1 -DHAVE_MALLOC_H=1 -DHAVE_STDDEF_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_UNISTD_H=1 -DHAVE_SYS_PARAM_H=1 -DHAVE_ASPRINTF=1 -DHAVE_FWPRINTF=1 -DHAVE_GETCWD=1 -DHAVE_GETEGID=1 -DHAVE_GETEUID=1 -DHAVE_GETGID=1 -DHAVE_GETUID=1 -DHAVE_MEMPCPY=1 -DHAVE_MUNMAP=1 -DHAVE_PUTENV=1 -DHAVE_SETENV=1 -DHAVE_SETLOCALE=1 -DHAVE_SNPRINTF=1 -DHAVE_STPCPY=1 -DHAVE_STRCASECMP=1 -DHAVE_STRDUP=1 -DHAVE_STRTOUL=1 -DHAVE_TSEARCH=1 -DHAVE_WCSLEN=1 -DHAVE___ARGZ_COUNT=1 -DHAVE___ARGZ_STRINGIFY=1 -DHAVE___ARGZ_NEXT=1 -DHAVE___FSETLOCKING=1 -DHAVE_DECL__SNPRINTF=0 -DHAVE_DECL__SNWPRINTF=0 -DHAVE_DECL_FEOF_UNLOCKED=1 -DHAVE_DECL_FGETS_UNLOCKED=0 -DHAVE_DECL_GETC_UNLOCKED=1 -DHAVE_ICONV=1 -DICONV_CONST= -DHAVE_LANGINFO_CODESET=1 -DHAVE_LC_MESSAGES=1 -DENABLE_NLS=1 -DHAVE_GETTEXT=1 -DHAVE_DCGETTEXT=1 -DHAVE_DIRENT_H=1 -DHAVE_ERRNO_H=1 -DHAVE_GETOPT_H=1 -DHAVE_MALLOC_H=1 -DHAVE_MNTENT_H=1 -DHAVE_PATHS_H=1 -DHAVE_SEMAPHORE_H=1 -DHAVE_SETJMP_H=1 -DHAVE_SIGNAL_H=1 -DHAVE_STDARG_H=1 -DHAVE_STDINT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_TERMIOS_H=1 -DHAVE_TERMIO_H=1 -DHAVE_UNISTD_H=1 -DHAVE_UTIME_H=1 -DHAVE_LINUX_FD_H=1 -DHAVE_LINUX_MAJOR_H=1 -DHAVE_NETINET_IN_H=1 -DHAVE_SYS_FILE_H=1 -DHAVE_SYS_IOCTL_H=1 -DHAVE_SYS_MMAN_H=1 -DHAVE_SYS_PRCTL_H=1 -DHAVE_SYS_QUEUE_H=1 -DHAVE_SYS_RESOURCE_H=1 -DHAVE_SYS_SELECT_H=1 -DHAVE_SYS_SOCKET_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_SYS_SYSCALL_H=1 -DHAVE_SYS_SYSMACROS_H=1 -DHAVE_SYS_TIME_H=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_UN_H=1 -DHAVE_SYS_WAIT_H=1 -DHAVE_SYS_MOUNT_H=1 -DHAVE_NET_IF_H=1 -DHAVE_VPRINTF=1 -DHAVE_RECLEN_DIRENT=1 -DHAVE_TYPE_SSIZE_T=1 -DHAVE_LSEEK64_PROTOTYPE=1 -DSIZEOF_SHORT=2 -DSIZEOF_INT=4 -DSIZEOF_LONG=4 -DSIZEOF_LONG_LONG=8 -DHAVE_INTTYPES_H=1 -DHAVE_INTPTR_T=1 -DHAVE_CHFLAGS=1 -DHAVE_GETRUSAGE=1 -DHAVE_LLSEEK=1 -DHAVE_LSEEK64=1 -DHAVE_OPEN64=1 -DHAVE_FSTAT64=1 -DHAVE_FTRUNCATE64=1 -DHAVE_STRTOULL=1 -DHAVE_STRCASECMP=1 -DHAVE_SRANDOM=1 -DHAVE_JRAND48=1 -DHAVE_FCHOWN=1 -DHAVE_MALLINFO=1 -DHAVE_FDATASYNC=1 -DHAVE_STRNLEN=1 -DHAVE_STRPTIME=1 -DHAVE_STRDUP=1 -DHAVE_SYSCONF=1 -DHAVE_PATHCONF=1 -DHAVE_POSIX_MEMALIGN=1 -DHAVE_MEMALIGN=1 -DHAVE_VALLOC=1 -DHAVE___SECURE_GETENV=1 -DHAVE_PRCTL=1 -DHAVE_MMAP=1 -DHAVE_UTIME=1 -DHAVE_SETRESUID=1 -DHAVE_SETRESGID=1 -DHAVE_USLEEP=1 -DHAVE_NANOSLEEP=1 -DHAVE_DLOPEN=1 -DHAVE_SEM_INIT=1 -DHAVE_EXT2_IOCTLS=1
CPPFLAGS = 
CFLAGS = -g -O2
LDFLAGS = 
LIBS = 

COMPILE = $(CC) -c $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) $(XCFLAGS)

HEADERS = \
  gmo.h \
  gettextP.h \
  hash-string.h \
  loadinfo.h \
  plural-exp.h \
  eval-plural.h \
  localcharset.h \
  relocatable.h \
  xsize.h \
  printf-args.h printf-args.c \
  printf-parse.h wprintf-parse.h printf-parse.c \
  vasnprintf.h vasnwprintf.h vasnprintf.c \
  os2compat.h \
  libgnuintl.h.in
SOURCES = \
  bindtextdom.c \
  dcgettext.c \
  dgettext.c \
  gettext.c \
  finddomain.c \
  loadmsgcat.c \
  localealias.c \
  textdomain.c \
  l10nflist.c \
  explodename.c \
  dcigettext.c \
  dcngettext.c \
  dngettext.c \
  ngettext.c \
  plural.y \
  plural-exp.c \
  localcharset.c \
  relocatable.c \
  localename.c \
  log.c \
  printf.c \
  osdep.c \
  os2compat.c \
  intl-compat.c
OBJECTS = \
  bindtextdom.$lo \
  dcgettext.$lo \
  dgettext.$lo \
  gettext.$lo \
  finddomain.$lo \
  loadmsgcat.$lo \
  localealias.$lo \
  textdomain.$lo \
  l10nflist.$lo \
  explodename.$lo \
  dcigettext.$lo \
  dcngettext.$lo \
  dngettext.$lo \
  ngettext.$lo \
  plural.$lo \
  plural-exp.$lo \
  localcharset.$lo \
  relocatable.$lo \
  localename.$lo \
  log.$lo \
  printf.$lo \
  osdep.$lo \
  intl-compat.$lo
DISTFILES.common = Makefile.in \
config.charset locale.alias ref-add.sin ref-del.sin $(HEADERS) $(SOURCES)
DISTFILES.generated = plural.c
DISTFILES.normal = VERSION
DISTFILES.gettext = COPYING.LIB-2.0 COPYING.LIB-2.1 libintl.glibc \
libgnuintl.h_vms Makefile.vms \
libgnuintl.h.msvc-static libgnuintl.h.msvc-shared README.woe32 Makefile.msvc
DISTFILES.obsolete = xopen-msg.sed linux-msg.sed po2tbl.sed.in cat-compat.c \
COPYING.LIB-2 gettext.h libgettext.h plural-eval.c libgnuintl.h

all: all-no
all-yes: libintl.$la libintl.h charset.alias ref-add.sed ref-del.sed
all-no: all-no-no
all-no-yes: libgnuintl.$la
all-no-no:

libintl.a libgnuintl.a: $(OBJECTS)
	@rm -f $@
	@echo "	AR $@"
	@$(AR) cru $@ $(OBJECTS)
	@echo "	RANLIB $@"
	@$(RANLIB) $@

libintl.la libgnuintl.la: $(OBJECTS)
	$(LIBTOOL) --mode=link \
	  $(CC) $(CPPFLAGS) $(CFLAGS) $(XCFLAGS) $(LDFLAGS) -o $@ \
	  $(OBJECTS)  $(LIBS) -lc \
	  -version-info $(LTV_CURRENT):$(LTV_REVISION):$(LTV_AGE) \
	  -rpath $(libdir) \
	  -no-undefined

# Libtool's library version information for libintl.
# Before making a gettext release, the gettext maintainer must change this
# according to the libtool documentation, section "Library interface versions".
# Maintainers of other packages that include the intl directory must *not*
# change these values.
LTV_CURRENT=7
LTV_REVISION=0
LTV_AGE=4

.SUFFIXES:
.SUFFIXES: .c .y .o .lo .sin .sed

.c.o:
	@echo "	CC $<"
	@$(COMPILE) $<

.y.c:
	$(YACC) $(YFLAGS) --output $@ $<
	rm -f $*.h

bindtextdom.lo: $(srcdir)/bindtextdom.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/bindtextdom.c
dcgettext.lo: $(srcdir)/dcgettext.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/dcgettext.c
dgettext.lo: $(srcdir)/dgettext.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/dgettext.c
gettext.lo: $(srcdir)/gettext.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/gettext.c
finddomain.lo: $(srcdir)/finddomain.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/finddomain.c
loadmsgcat.lo: $(srcdir)/loadmsgcat.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/loadmsgcat.c
localealias.lo: $(srcdir)/localealias.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/localealias.c
textdomain.lo: $(srcdir)/textdomain.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/textdomain.c
l10nflist.lo: $(srcdir)/l10nflist.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/l10nflist.c
explodename.lo: $(srcdir)/explodename.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/explodename.c
dcigettext.lo: $(srcdir)/dcigettext.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/dcigettext.c
dcngettext.lo: $(srcdir)/dcngettext.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/dcngettext.c
dngettext.lo: $(srcdir)/dngettext.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/dngettext.c
ngettext.lo: $(srcdir)/ngettext.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/ngettext.c
plural.lo: $(srcdir)/plural.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/plural.c
plural-exp.lo: $(srcdir)/plural-exp.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/plural-exp.c
localcharset.lo: $(srcdir)/localcharset.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/localcharset.c
relocatable.lo: $(srcdir)/relocatable.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/relocatable.c
localename.lo: $(srcdir)/localename.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/localename.c
log.lo: $(srcdir)/log.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/log.c
printf.lo: $(srcdir)/printf.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/printf.c
osdep.lo: $(srcdir)/osdep.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/osdep.c
intl-compat.lo: $(srcdir)/intl-compat.c
	$(LIBTOOL) --mode=compile $(COMPILE) $(srcdir)/intl-compat.c

ref-add.sed: $(srcdir)/ref-add.sin
	@echo "	SED ref-add.sed"
	@sed -e '/^#/d' -e 's/@''PACKAGE''@/e2fsprogs/g' $(srcdir)/ref-add.sin > t-ref-add.sed
	@mv t-ref-add.sed ref-add.sed
ref-del.sed: $(srcdir)/ref-del.sin
	@echo "	SED ref-del.sed"
	@sed -e '/^#/d' -e 's/@''PACKAGE''@/e2fsprogs/g' $(srcdir)/ref-del.sin > t-ref-del.sed
	@mv t-ref-del.sed ref-del.sed

INCLUDES = -I. -I$(srcdir) -I..

libgnuintl.h: $(srcdir)/libgnuintl.h.in
	@echo "	SED libgnuintl.h "
	@sed -e 's,@''HAVE_POSIX_PRINTF''@,1,g' \
	    -e 's,@''HAVE_ASPRINTF''@,1,g' \
	    -e 's,@''HAVE_SNPRINTF''@,1,g' \
	    -e 's,@''HAVE_WPRINTF''@,0,g' \
	  < $(srcdir)/libgnuintl.h.in > libgnuintl.h 

libintl.h: libgnuintl.h
	@echo "	CP libintl.h"
	@cp libgnuintl.h libintl.h

charset.alias: $(srcdir)/config.charset
	@echo "	CONFIG.CHARSET $@"
	@$(SHELL) $(srcdir)/config.charset 'arm-unknown-linux-gnu' > t-$@
	@mv t-$@ $@

check: all

# We must not install the libintl.h/libintl.a files if we are on a
# system which has the GNU gettext() function in its C library or in a
# separate library.
# If you want to use the one which comes with this version of the
# package, you have to use `configure --with-included-gettext'.
install: install-exec install-data
install-exec: all
	@if { test "$(PACKAGE)" = "gettext-runtime" || test "$(PACKAGE)" = "gettext-tools"; } \
	   && test 'no' = yes; then \
	  $(mkinstalldirs) $(DESTDIR)$(libdir) $(DESTDIR)$(includedir); \
	  $(INSTALL_DATA) libintl.h $(DESTDIR)$(includedir)/libintl.h; \
	  $(LIBTOOL) --mode=install \
	    $(INSTALL_DATA) libintl.$la $(DESTDIR)$(libdir)/libintl.$la; \
	  if test "@RELOCATABLE@" = yes; then \
	    dependencies=`sed -n -e 's,^dependency_libs=\(.*\),\1,p' < $(DESTDIR)$(libdir)/libintl.la | sed -e "s,^',," -e "s,'\$$,,"`; \
	    if test -n "$$dependencies"; then \
	      rm -f $(DESTDIR)$(libdir)/libintl.la; \
	    fi; \
	  fi; \
	else \
	  : ; \
	fi
	@if test "$(PACKAGE)" = "gettext-tools" \
	   && test 'no' = no; then \
	  $(mkinstalldirs) $(DESTDIR)$(libdir); \
	  $(LIBTOOL) --mode=install \
	    $(INSTALL_DATA) libgnuintl.$la $(DESTDIR)$(libdir)/libgnuintl.$la; \
	  rm -f $(DESTDIR)$(libdir)/preloadable_libintl.so; \
	  $(INSTALL_DATA) $(DESTDIR)$(libdir)/libgnuintl.so $(DESTDIR)$(libdir)/preloadable_libintl.so; \
	  $(LIBTOOL) --mode=uninstall \
	    rm -f $(DESTDIR)$(libdir)/libgnuintl.$la; \
	else \
	  : ; \
	fi
	if test 'no' = yes; then \
	  test yes != no || $(mkinstalldirs) $(DESTDIR)$(libdir); \
	  temp=$(DESTDIR)$(libdir)/t-charset.alias; \
	  dest=$(DESTDIR)$(libdir)/charset.alias; \
	  if test -f $(DESTDIR)$(libdir)/charset.alias; then \
	    orig=$(DESTDIR)$(libdir)/charset.alias; \
	    sed -f ref-add.sed $$orig > $$temp; \
	    $(INSTALL_DATA) $$temp $$dest; \
	    rm -f $$temp; \
	  else \
	    if test yes = no; then \
	      orig=charset.alias; \
	      sed -f ref-add.sed $$orig > $$temp; \
	      $(INSTALL_DATA) $$temp $$dest; \
	      rm -f $$temp; \
	    fi; \
	  fi; \
	  $(mkinstalldirs) $(DESTDIR)$(localedir); \
	  test -f $(DESTDIR)$(localedir)/locale.alias \
	    && orig=$(DESTDIR)$(localedir)/locale.alias \
	    || orig=$(srcdir)/locale.alias; \
	  temp=$(DESTDIR)$(localedir)/t-locale.alias; \
	  dest=$(DESTDIR)$(localedir)/locale.alias; \
	  sed -f ref-add.sed $$orig > $$temp; \
	  $(INSTALL_DATA) $$temp $$dest; \
	  rm -f $$temp; \
	else \
	  : ; \
	fi
install-data: all
	@if test "$(PACKAGE)" = "gettext-tools"; then \
	  $(mkinstalldirs) $(DESTDIR)$(gettextsrcdir); \
	  $(INSTALL_DATA) VERSION $(DESTDIR)$(gettextsrcdir)/VERSION; \
	  $(INSTALL_DATA) ChangeLog.inst $(DESTDIR)$(gettextsrcdir)/ChangeLog; \
	  dists="COPYING.LIB-2.0 COPYING.LIB-2.1 $(DISTFILES.common)"; \
	  for file in $$dists; do \
	    $(INSTALL_DATA) $(srcdir)/$$file \
			    $(DESTDIR)$(gettextsrcdir)/$$file; \
	  done; \
	  chmod a+x $(DESTDIR)$(gettextsrcdir)/config.charset; \
	  dists="$(DISTFILES.generated)"; \
	  for file in $$dists; do \
	    if test -f $$file; then dir=.; else dir=$(srcdir); fi; \
	    $(INSTALL_DATA) $$dir/$$file \
			    $(DESTDIR)$(gettextsrcdir)/$$file; \
	  done; \
	  dists="$(DISTFILES.obsolete)"; \
	  for file in $$dists; do \
	    rm -f $(DESTDIR)$(gettextsrcdir)/$$file; \
	  done; \
	else \
	  : ; \
	fi

install-strip: install

installdirs:
	if { test "$(PACKAGE)" = "gettext-runtime" || test "$(PACKAGE)" = "gettext-tools"; } \
	   && test 'no' = yes; then \
	  $(mkinstalldirs) $(DESTDIR)$(libdir) $(DESTDIR)$(includedir); \
	else \
	  : ; \
	fi
	if test "$(PACKAGE)" = "gettext-tools" \
	   && test 'no' = no; then \
	  $(mkinstalldirs) $(DESTDIR)$(libdir); \
	else \
	  : ; \
	fi
	if test 'no' = yes; then \
	  test yes != no || $(mkinstalldirs) $(DESTDIR)$(libdir); \
	  $(mkinstalldirs) $(DESTDIR)$(localedir); \
	else \
	  : ; \
	fi
	if test "$(PACKAGE)" = "gettext-tools"; then \
	  $(mkinstalldirs) $(DESTDIR)$(gettextsrcdir); \
	else \
	  : ; \
	fi

# Define this as empty until I found a useful application.
installcheck:

install-shlibs:

uninstall-shlibs:

install-shlibs-strip:

uninstall:
	if { test "$(PACKAGE)" = "gettext-runtime" || test "$(PACKAGE)" = "gettext-tools"; } \
	   && test 'no' = yes; then \
	  rm -f $(DESTDIR)$(includedir)/libintl.h; \
	  $(LIBTOOL) --mode=uninstall \
	    rm -f $(DESTDIR)$(libdir)/libintl.$la; \
	else \
	  : ; \
	fi
	if test "$(PACKAGE)" = "gettext-tools" \
	   && test 'no' = no; then \
	  rm -f $(DESTDIR)$(libdir)/preloadable_libintl.so; \
	else \
	  : ; \
	fi
	if test 'no' = yes; then \
	  if test -f $(DESTDIR)$(libdir)/charset.alias; then \
	    temp=$(DESTDIR)$(libdir)/t-charset.alias; \
	    dest=$(DESTDIR)$(libdir)/charset.alias; \
	    sed -f ref-del.sed $$dest > $$temp; \
	    if grep '^# Packages using this file: $$' $$temp > /dev/null; then \
	      rm -f $$dest; \
	    else \
	      $(INSTALL_DATA) $$temp $$dest; \
	    fi; \
	    rm -f $$temp; \
	  fi; \
	  if test -f $(DESTDIR)$(localedir)/locale.alias; then \
	    temp=$(DESTDIR)$(localedir)/t-locale.alias; \
	    dest=$(DESTDIR)$(localedir)/locale.alias; \
	    sed -f ref-del.sed $$dest > $$temp; \
	    if grep '^# Packages using this file: $$' $$temp > /dev/null; then \
	      rm -f $$dest; \
	    else \
	      $(INSTALL_DATA) $$temp $$dest; \
	    fi; \
	    rm -f $$temp; \
	  fi; \
	else \
	  : ; \
	fi
	if test "$(PACKAGE)" = "gettext-tools"; then \
	  for file in VERSION ChangeLog COPYING.LIB-2.0 COPYING.LIB-2.1 $(DISTFILES.common) $(DISTFILES.generated); do \
	    rm -f $(DESTDIR)$(gettextsrcdir)/$$file; \
	  done; \
	else \
	  : ; \
	fi

info dvi ps pdf html:

$(OBJECTS): libgnuintl.h
bindtextdom.$lo dcgettext.$lo dcigettext.$lo dcngettext.$lo dgettext.$lo dngettext.$lo finddomain.$lo gettext.$lo intl-compat.$lo loadmsgcat.$lo localealias.$lo ngettext.$lo textdomain.$lo: $(srcdir)/gettextP.h $(srcdir)/gmo.h $(srcdir)/loadinfo.h
dcigettext.$lo loadmsgcat.$lo: $(srcdir)/hash-string.h
explodename.$lo l10nflist.$lo: $(srcdir)/loadinfo.h
dcigettext.$lo loadmsgcat.$lo plural.$lo plural-exp.$lo: $(srcdir)/plural-exp.h
dcigettext.$lo: $(srcdir)/eval-plural.h
localcharset.$lo: $(srcdir)/localcharset.h
localealias.$lo localcharset.$lo relocatable.$lo: $(srcdir)/relocatable.h
printf.$lo: $(srcdir)/printf-args.h $(srcdir)/printf-args.c $(srcdir)/printf-parse.h $(srcdir)/wprintf-parse.h $(srcdir)/xsize.h $(srcdir)/printf-parse.c $(srcdir)/vasnprintf.h $(srcdir)/vasnwprintf.h $(srcdir)/vasnprintf.c

tags: TAGS

TAGS: $(HEADERS) $(SOURCES)
	here=`pwd`; cd $(srcdir) && etags -o $$here/TAGS $(HEADERS) $(SOURCES)

ctags: CTAGS

CTAGS: $(HEADERS) $(SOURCES)
	here=`pwd`; cd $(srcdir) && ctags -o $$here/CTAGS $(HEADERS) $(SOURCES)

id: ID

ID: $(HEADERS) $(SOURCES)
	here=`pwd`; cd $(srcdir) && mkid -f$$here/ID $(HEADERS) $(SOURCES)


mostlyclean:
	rm -f *.a *.la *.o *.obj *.lo core core.*
	rm -f libgnuintl.h libintl.h charset.alias ref-add.sed ref-del.sed
	rm -f -r .libs _libs

clean: mostlyclean

distclean: clean
	rm -f Makefile ID TAGS
	if test "$(PACKAGE)" = "gettext-runtime" || test "$(PACKAGE)" = "gettext-tools"; then \
	  rm -f ChangeLog.inst $(DISTFILES.normal); \
	else \
	  : ; \
	fi

maintainer-clean: distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."


# GNU gettext needs not contain the file `VERSION' but contains some
# other files which should not be distributed in other packages.
distdir = ../$(PACKAGE)-$(VERSION)/$(subdir)
dist distdir: Makefile
	if test "$(PACKAGE)" = "gettext-tools"; then \
	  : ; \
	else \
	  if test "$(PACKAGE)" = "gettext-runtime"; then \
	    additional="$(DISTFILES.gettext)"; \
	  else \
	    additional="$(DISTFILES.normal)"; \
	  fi; \
	  $(MAKE) $(DISTFILES.common) $(DISTFILES.generated) $$additional; \
	  for file in ChangeLog $(DISTFILES.common) $(DISTFILES.generated) $$additional; do \
	    if test -f $$file; then dir=.; else dir=$(srcdir); fi; \
	    cp -p $$dir/$$file $(distdir); \
	  done; \
	fi

Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status
#	cd $(top_builddir) && $(SHELL) ./config.status
# This would be more efficient, but doesn't work any more with autoconf-2.57,
# when AC_CONFIG_FILES([intl/Makefile:somedir/Makefile.in]) is used.
	cd $(top_builddir) && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
