#
# Makefile for e2fsck
#

srcdir = .
top_srcdir = ..

top_builddir = ..
my_dir = e2fsck
INSTALL = /usr/bin/install -c

# Beginning of file MCONFIG

all::

check::

SHELL = /bin/sh

COMPRESS_EXT = gz bz2 bz Z

prefix = /usr
root_prefix = 
exec_prefix = ${prefix}
root_bindir = ${root_prefix}/bin
root_sbindir = ${root_prefix}/sbin
root_libdir = ${root_prefix}/lib
datarootdir = ${prefix}/share
bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libdir = ${exec_prefix}/lib
datadir= ${datarootdir}
localedir = $(datadir)/locale
root_sysconfdir= ${root_prefix}/etc
includedir = ${prefix}/include
mandir = ${datarootdir}/man
man1dir = $(mandir)/man1
man3dir = $(mandir)/man3
man5dir = $(mandir)/man5
man8dir = $(mandir)/man8
infodir = ${datarootdir}/info
datadir = ${datarootdir}



CC = arm-linux-gcc
BUILD_CC = gcc
DEFS = -DLOCALEDIR=\"$(localedir)\" -DROOT_SYSCONFDIR=\"$(root_sysconfdir)\" -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DENABLE_HTREE=1 -DCONFIG_TESTIO_DEBUG=1 -DUSE_UUIDD=1 -DPACKAGE=\"e2fsprogs\" -DVERSION=\"0.14.1\" -DHAVE_LONG_LONG=1 -DHAVE_LONG_DOUBLE=1 -DHAVE_WCHAR_T=1 -DHAVE_WINT_T=1 -DHAVE_INTTYPES_H_WITH_UINTMAX=1 -DHAVE_STDINT_H_WITH_UINTMAX=1 -DHAVE_INTMAX_T=1 -DHAVE_POSIX_PRINTF=1 -DHAVE_ALLOCA_H=1 -DHAVE_ALLOCA=1 -DHAVE_STDLIB_H=1 -DHAVE_UNISTD_H=1 -DHAVE_GETPAGESIZE=1 -DINTDIV0_RAISES_SIGFPE=0 -DHAVE_UNSIGNED_LONG_LONG=1 -DHAVE_UINTMAX_T=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_STDINT_H=1 -DHAVE_ARGZ_H=1 -DHAVE_LIMITS_H=1 -DHAVE_LOCALE_H=1 -DHAVE_NL_TYPES_H=1 -DHAVE_MALLOC_H=1 -DHAVE_STDDEF_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_UNISTD_H=1 -DHAVE_SYS_PARAM_H=1 -DHAVE_ASPRINTF=1 -DHAVE_FWPRINTF=1 -DHAVE_GETCWD=1 -DHAVE_GETEGID=1 -DHAVE_GETEUID=1 -DHAVE_GETGID=1 -DHAVE_GETUID=1 -DHAVE_MEMPCPY=1 -DHAVE_MUNMAP=1 -DHAVE_PUTENV=1 -DHAVE_SETENV=1 -DHAVE_SETLOCALE=1 -DHAVE_SNPRINTF=1 -DHAVE_STPCPY=1 -DHAVE_STRCASECMP=1 -DHAVE_STRDUP=1 -DHAVE_STRTOUL=1 -DHAVE_TSEARCH=1 -DHAVE_WCSLEN=1 -DHAVE___ARGZ_COUNT=1 -DHAVE___ARGZ_STRINGIFY=1 -DHAVE___ARGZ_NEXT=1 -DHAVE___FSETLOCKING=1 -DHAVE_DECL__SNPRINTF=0 -DHAVE_DECL__SNWPRINTF=0 -DHAVE_DECL_FEOF_UNLOCKED=1 -DHAVE_DECL_FGETS_UNLOCKED=0 -DHAVE_DECL_GETC_UNLOCKED=1 -DHAVE_ICONV=1 -DICONV_CONST= -DHAVE_LANGINFO_CODESET=1 -DHAVE_LC_MESSAGES=1 -DENABLE_NLS=1 -DHAVE_GETTEXT=1 -DHAVE_DCGETTEXT=1 -DHAVE_DIRENT_H=1 -DHAVE_ERRNO_H=1 -DHAVE_GETOPT_H=1 -DHAVE_MALLOC_H=1 -DHAVE_MNTENT_H=1 -DHAVE_PATHS_H=1 -DHAVE_SEMAPHORE_H=1 -DHAVE_SETJMP_H=1 -DHAVE_SIGNAL_H=1 -DHAVE_STDARG_H=1 -DHAVE_STDINT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_TERMIOS_H=1 -DHAVE_TERMIO_H=1 -DHAVE_UNISTD_H=1 -DHAVE_UTIME_H=1 -DHAVE_LINUX_FD_H=1 -DHAVE_LINUX_MAJOR_H=1 -DHAVE_NETINET_IN_H=1 -DHAVE_SYS_FILE_H=1 -DHAVE_SYS_IOCTL_H=1 -DHAVE_SYS_MMAN_H=1 -DHAVE_SYS_PRCTL_H=1 -DHAVE_SYS_QUEUE_H=1 -DHAVE_SYS_RESOURCE_H=1 -DHAVE_SYS_SELECT_H=1 -DHAVE_SYS_SOCKET_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_SYS_SYSCALL_H=1 -DHAVE_SYS_SYSMACROS_H=1 -DHAVE_SYS_TIME_H=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_UN_H=1 -DHAVE_SYS_WAIT_H=1 -DHAVE_SYS_MOUNT_H=1 -DHAVE_NET_IF_H=1 -DHAVE_VPRINTF=1 -DHAVE_RECLEN_DIRENT=1 -DHAVE_TYPE_SSIZE_T=1 -DHAVE_LSEEK64_PROTOTYPE=1 -DSIZEOF_SHORT=2 -DSIZEOF_INT=4 -DSIZEOF_LONG=4 -DSIZEOF_LONG_LONG=8 -DHAVE_INTTYPES_H=1 -DHAVE_INTPTR_T=1 -DHAVE_CHFLAGS=1 -DHAVE_GETRUSAGE=1 -DHAVE_LLSEEK=1 -DHAVE_LSEEK64=1 -DHAVE_OPEN64=1 -DHAVE_FSTAT64=1 -DHAVE_FTRUNCATE64=1 -DHAVE_STRTOULL=1 -DHAVE_STRCASECMP=1 -DHAVE_SRANDOM=1 -DHAVE_JRAND48=1 -DHAVE_FCHOWN=1 -DHAVE_MALLINFO=1 -DHAVE_FDATASYNC=1 -DHAVE_STRNLEN=1 -DHAVE_STRPTIME=1 -DHAVE_STRDUP=1 -DHAVE_SYSCONF=1 -DHAVE_PATHCONF=1 -DHAVE_POSIX_MEMALIGN=1 -DHAVE_MEMALIGN=1 -DHAVE_VALLOC=1 -DHAVE___SECURE_GETENV=1 -DHAVE_PRCTL=1 -DHAVE_MMAP=1 -DHAVE_UTIME=1 -DHAVE_SETRESUID=1 -DHAVE_SETRESGID=1 -DHAVE_USLEEP=1 -DHAVE_NANOSLEEP=1 -DHAVE_DLOPEN=1 -DHAVE_SEM_INIT=1 -DHAVE_EXT2_IOCTLS=1
CFLAGS = -g -O2
CPPFLAGS =  -I$(top_builddir)/lib -I$(top_srcdir)/lib 
INTL_FLAGS = 
ALL_CFLAGS = $(CPPFLAGS) $(DEFS) $(USE_WFLAGS) $(CFLAGS) $(XTRA_CFLAGS) \
	$(INTL_FLAGS) $(LINUX_INCLUDE) 
LDFLAGS = 
ALL_LDFLAGS = $(LDFLAGS) -Wl,-rpath-link,$(top_builddir)/lib
LDFLAGS_STATIC = $(LDFLAGS) -static
BUILD_CFLAGS = 
BUILD_LDFLAGS = 
RM = /bin/rm
LN = /bin/ln
LN_S = ln -s
MV = /bin/mv
CP = /bin/cp
CHMOD = /bin/chmod
AR = arm-linux-ar
AWK = mawk
SED = /bin/sed
PERL = /usr/bin/perl
RANLIB = arm-linux-ranlib
STRIP = arm-linux-strip
LD = $(PURE) arm-linux-gcc
ARUPD = $(AR) r
ARGEN = $(AR) rc
LDCONFIG = /sbin/ldconfig
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL}
MKINSTALLDIRS = $(top_builddir)/config/mkinstalldirs

#
# Library definitions
#
LIB = $(top_builddir)/lib
LIBSS = $(LIB)/libss.so # -ldl
LIBCOM_ERR = $(LIB)/libcom_err.so # -lpthread
LIBE2P = $(LIB)/libe2p.so
LIBEXT2FS = $(LIB)/libext2fs.so
LIBUUID = $(LIB)/libuuid.so 
LIBBLKID = $(LIB)/libblkid.so # $(LIBUUID)
LIBINTL = 
DEPLIBSS = $(LIB)/libss.so
DEPLIBUUID = $(LIB)/libuuid.so
DEPLIBBLKID = $(LIB)/libblkid.so # $(LIBUUID)

STATIC_LIBSS = $(LIB)/libss.a -ldl
STATIC_LIBCOM_ERR = $(LIB)/libcom_err.a -lpthread
STATIC_LIBE2P = $(LIB)/libe2p.a
STATIC_LIBEXT2FS = $(LIB)/libext2fs.a
STATIC_LIBUUID = $(LIB)/libuuid.a  
STATIC_LIBBLKID = $(LIB)/libblkid.a
DEPSTATIC_LIBSS = $(LIB)/libss.a
DEPSTATIC_LIBUUID = $(LIB)/libuuid.a
DEPSTATIC_LIBBLKID = $(LIB)/libblkid.a

PROFILED_LIBSS = $(LIB)/libss -ldl
PROFILED_LIBCOM_ERR = $(LIB)/libcom_err
PROFILED_LIBE2P = $(LIB)/libe2p
PROFILED_LIBEXT2FS = $(LIB)/libext2fs
PROFILED_LIBUUID = $(LIB)/libuuid  
PROFILED_LIBBLKID = $(LIB)/libblkid
DEPPROFILED_LIBUUID = $(LIB)/libuuid
DEPPROFILED_LIBBLKID = $(LIB)/libblkid

# An include directive pointing to a directory holding enough linux-like
# include files to satisfy some programs here
LINUX_INCLUDE=

#
# A fast substitution command for fixing up man pages, shell scripts, etc.
#
SUBST_CONF=$(top_builddir)/util/subst.conf
SUBSTITUTE= $(top_builddir)/util/subst -f $(SUBST_CONF)
SUBSTITUTE_UPTIME= $(top_builddir)/util/subst -t -f $(SUBST_CONF)
DEP_SUBSTITUTE= $(top_builddir)/util/subst $(SUBST_CONF)

$(top_builddir)/util/subst:
	cd $(top_builddir)/util ; $(MAKE) subst

#
# Warning flags
#
# Run make gcc-wall to do a build with warning messages.
#
#
WFLAGS=		-std=c99 -D_XOPEN_SOURCE=600 -D_GNU_SOURCE \
			-pedantic $(WFLAGS_EXTRA) \
			-Wall -W -Wwrite-strings -Wpointer-arith \
			-Wcast-qual -Wcast-align -Wno-variadic-macros \
			-Wstrict-prototypes -Wmissing-prototypes \
			-Wformat-security  -Wformat-nonliteral \
			-Wmissing-format-attribute -O2 -Wstrict-aliasing \
			-Wnested-externs -Winline -DNO_INLINE_FUNCS -Wshadow \
			-UENABLE_NLS

gcc-wall-new:
	(make USE_WFLAGS="$(WFLAGS)" > /dev/null) 2>&1 | sed -f $(top_srcdir)/util/gcc-wall-cleanup 

gcc-wall:
	make clean > /dev/null
	make gcc-wall-new

#
# Installation user and groups
#
BINGRP=		bin
BINOWN=		bin
BINMODE=	555
INCGRP=		bin
INCOWN=		bin
INCMODE=	444
LIBOWN=		bin
LIBGRP=		bin
LIBMODE=	444
MANGRP=		bin
MANOWN=		bin
MANMODE=	444

#
# Autoconf magic...
#

DEP_LIB_MAKEFILES = $(top_srcdir)/lib/Makefile.elf-lib  \
	$(top_srcdir)/lib/Makefile.bsd-lib $(top_srcdir)/lib/Makefile.darwin-lib \
	$(top_srcdir)/lib/Makefile.solaris-lib $(top_srcdir)/lib/Makefile.checker \
	$(top_srcdir)/lib/Makefile.profile

$(top_builddir)/config.status: $(top_srcdir)/configure
	cd $(top_builddir); ./config.status --recheck

$(top_builddir)/MCONFIG: $(top_srcdir)/MCONFIG.in $(top_builddir)/config.status
	cd $(top_builddir); CONFIG_FILES=MCONFIG ./config.status

$(top_builddir)/lib/substitute_sh: $(top_srcdir)/lib/substitute_sh.in \
		$(top_builddir)/config.status
	cd $(top_builddir); CONFIG_FILES=lib/substitute_sh ./config.status

$(top_builddir)/util/subst.conf: $(top_srcdir)/util/subst.conf.in \
		$(top_builddir)/config.status
	cd $(top_builddir); CONFIG_FILES=util/subst.conf ./config.status

Makefile: $(srcdir)/Makefile.in $(top_builddir)/MCONFIG \
		$(DEP_MAKEFILE) $(top_builddir)/config.status
	cd $(top_builddir); CONFIG_FILES=$(my_dir)/Makefile ./config.status

#$(top_srcdir)/configure: $(top_srcdir)/configure.in
#	cd $(top_srcdir) && autoconf

#
# Make depend magic...
#

.depend: Makefile $(SRCS) $(top_srcdir)/depfix.sed $(top_srcdir)/wordwrap.pl
	if test -n "$(SRCS)" ; then \
		$(CC) -M $(ALL_CFLAGS) $(SRCS) | \
			$(SED) -f $(top_srcdir)/depfix.sed \
			    -e 's; $(srcdir)/; $$(srcdir)/;g' \
			    -e 's; $(top_srcdir)/; $$(top_srcdir)/;g' \
			    -e 's; $(top_builddir)/; $$(top_builddir)/;g' \
			    -e 's; \./; ;g' \
			    -e '/^#/d' \
			    -e '/^ *\\$$/d' | \
			$(PERL) $(top_srcdir)/wordwrap.pl > .depend; \
	else :; fi

depend:: .depend
	if test -n "$(SRCS)" ; then \
		sed -e '/^# +++ Dependency line eater +++/,$$d' \
			< $(srcdir)/Makefile.in | cat - .depend \
			> $(srcdir)/Makefile.in.new; \
	if cmp -s $(srcdir)/Makefile.in $(srcdir)/Makefile.in.new ; then \
		$(RM) $(srcdir)/Makefile.in.new ; \
	else \
		$(MV) $(srcdir)/Makefile.in $(srcdir)/Makefile.in.old; \
		$(MV) $(srcdir)/Makefile.in.new $(srcdir)/Makefile.in; \
	fi ; else :; fi

# End of file MCONFIG

PROGS=		e2fsck
MANPAGES=	e2fsck.8
FMANPAGES=	e2fsck.conf.5
XTRA_CFLAGS=	-DRESOURCE_TRACK -I.

LIBS= $(LIBEXT2FS) $(LIBCOM_ERR) $(LIBBLKID) $(LIBUUID) $(LIBINTL) $(LIBE2P)
DEPLIBS= $(LIBEXT2FS) $(LIBCOM_ERR) $(DEPLIBBLKID) $(DEPLIBUUID) $(DEPLIBE2P)

STATIC_LIBS= $(STATIC_LIBEXT2FS) $(STATIC_LIBCOM_ERR) $(STATIC_LIBBLKID) \
	$(STATIC_LIBUUID) $(LIBINTL) $(STATIC_LIBE2P)
STATIC_DEPLIBS= $(STATIC_LIBEXT2FS) $(STATIC_LIBCOM_ERR) $(DEPSTATIC_LIBBLKID) \
	$(DEPSTATIC_LIBUUID) $(DEPSTATIC_LIBE2P)

PROFILED_LIBS= $(PROFILED_LIBEXT2FS) $(PROFILED_LIBCOM_ERR) \
	$(PROFILED_LIBBLKID) $(PROFILED_LIBUUID) $(PROFILED_LIBE2P) $(LIBINTL)
PROFILED_DEPLIBS= $(PROFILED_LIBEXT2FS) $(PROFILED_LIBCOM_ERR) \
	$(DEPPROFILED_LIBBLKID) $(DEPPROFILED_LIBUUID) $(DEPPROFILED_LIBE2P)

COMPILE_ET=$(top_builddir)/lib/et/compile_et --build-tree

.c.o:
	@echo "	CC $<"
	@$(CC) -c $(ALL_CFLAGS) $< -o $@
#	@$(CC) $(ALL_CFLAGS) -g -pg -o profiled/$*.o -c $<

#
# Flags for using Checker
#	Note: The optimization flags must include -g
#
#MCHECK=	-checker
#LIBS= $(LIBCOM_ERR) $(LIB_EXT2FS) $(CHECKLIB)
#DEPLIBS= $(LIBCOM_ERR) $(LIB_EXT2FS) 
#CHECKLIB= /usr/lib/libchecker.o

#
# Flags for doing mtrace --- uncomment to produce mtracing e2fsck
# 	Note:  The optimization flags must include -g
#
#MTRACE=	-DMTRACE
#MTRACE_OBJ= mtrace.o
#MTRACE_SRC= $(srcdir)/mtrace.c
#OPT= -g

#
# Flags for doing mcheck --- uncomment to produce mchecking e2fsck
# 	Note:  The optimization flags must include -g
#
#MCHECK= -DMCHECK

OBJS= crc32.o dict.o unix.o e2fsck.o super.o pass1.o pass1b.o pass2.o \
	pass3.o pass4.o pass5.o journal.o badblocks.o util.o dirinfo.o \
	dx_dirinfo.o ehandler.o problem.o message.o recovery.o region.o \
	revoke.o ea_refcount.o rehash.o profile.o prof_err.o $(MTRACE_OBJ)

PROFILED_OBJS= profiled/dict.o profiled/unix.o profiled/e2fsck.o \
	profiled/super.o profiled/pass1.o profiled/pass1b.o \
	profiled/pass2.o profiled/pass3.o profiled/pass4.o profiled/pass5.o \
	profiled/journal.o profiled/badblocks.o profiled/util.o \
	profiled/dirinfo.o profiled/dx_dirinfo.o profiled/ehandler.o \
	profiled/message.o profiled/problem.o \
	profiled/recovery.o profiled/region.o profiled/revoke.o \
	profiled/ea_refcount.o profiled/rehash.o profiled/profile.o \
	profiled/prof_err.o

SRCS= $(srcdir)/e2fsck.c \
	$(srcdir)/crc32.c \
	$(srcdir)/gen_crc32table.c \
	$(srcdir)/dict.c \
	$(srcdir)/super.c \
	$(srcdir)/pass1.c \
	$(srcdir)/pass1b.c \
	$(srcdir)/pass2.c \
	$(srcdir)/pass3.c \
	$(srcdir)/pass4.c \
	$(srcdir)/pass5.c \
	$(srcdir)/journal.c \
	$(srcdir)/recovery.c \
	$(srcdir)/revoke.c \
	$(srcdir)/badblocks.c \
	$(srcdir)/util.c \
	$(srcdir)/unix.c \
	$(srcdir)/dirinfo.c \
	$(srcdir)/dx_dirinfo.c \
	$(srcdir)/ehandler.c \
	$(srcdir)/problem.c \
	$(srcdir)/message.c \
	$(srcdir)/ea_refcount.c \
	$(srcdir)/rehash.c \
	$(srcdir)/region.c \
	$(srcdir)/profile.c \
	prof_err.c \
	$(MTRACE_SRC)

all:: profiled $(PROGS) e2fsck $(MANPAGES) $(FMANPAGES)

#all:: e2fsck.profiled

prof_err.c prof_err.h: prof_err.et
	@echo "	COMPILE_ET prof_err.et"
	@$(COMPILE_ET) $(srcdir)/prof_err.et

e2fsck: $(OBJS)  $(DEPLIBS)
	@echo "	LD $@"
	@$(LD) $(ALL_LDFLAGS) -o e2fsck $(OBJS) $(LIBS) 

e2fsck.static: $(OBJS) $(STATIC_DEPLIBS)
	@echo "	LD $@"
	@$(LD) $(LDFLAGS_STATIC) -o e2fsck.static $(OBJS) $(STATIC_LIBS) 

e2fsck.profiled: $(PROFILED_OBJS)  $(PROFILED_DEPLIBS)
	@echo "	LD $@"
	@$(LD) $(ALL_LDFLAGS) -g -pg -o e2fsck.profiled $(PROFILED_OBJS) \
		$(PROFILED_LIBS) 

gen_crc32table: $(srcdir)/gen_crc32table.c
	@echo "	CC $@"
	@$(BUILD_CC) $(BUILD_CFLAGS) -o gen_crc32table \
		$(srcdir)/gen_crc32table.c

crc32table.h: gen_crc32table
	@echo "	GEN32TABLE $@"
	@./gen_crc32table > crc32table.h

tst_crc32: $(srcdir)/crc32.c $(LIBEXT2FS)
	@$(CC) $(BUILD_LDFLAGS) $(ALL_CFLAGS) -o tst_crc32 $(srcdir)/crc32.c \
		-DUNITTEST $(LIBEXT2FS) $(LIBCOM_ERR)

tst_refcount: ea_refcount.c
	@echo "	LD $@"
	@$(CC) -o tst_refcount $(srcdir)/ea_refcount.c \
		$(ALL_CFLAGS) -DTEST_PROGRAM $(LIBCOM_ERR) $(LIBEXT2FS) 

tst_region: region.c
	@echo "	LD $@"
	@$(CC) -o tst_region $(srcdir)/region.c \
		$(ALL_CFLAGS) -DTEST_PROGRAM $(LIBCOM_ERR)

check:: tst_refcount tst_region tst_crc32
	LD_LIBRARY_PATH=$(LIB) DYLD_LIBRARY_PATH=$(LIB) ./tst_refcount
	LD_LIBRARY_PATH=$(LIB) DYLD_LIBRARY_PATH=$(LIB) ./tst_region
	LD_LIBRARY_PATH=$(LIB) DYLD_LIBRARY_PATH=$(LIB) ./tst_crc32

extend: extend.o
	@echo "	LD $@"
	@$(LD) $(ALL_LDFLAGS) -o extend extend.o $(CHECKLIB)

flushb: flushb.o
	@echo "	LD $@"
	@$(LD) $(ALL_LDFLAGS) -o flushb flushb.o $(CHECKLIB)

iscan: iscan.o util.o ehandler.o $(DEPLIBS)
	@echo "	LD $@"
	@$(LD) $(ALL_LDFLAGS) -o iscan iscan.o util.o ehandler.o $(LIBS)

test_profile: $(srcdir)/profile.c profile_helpers.o argv_parse.o \
		prof_err.o profile.h $(STATIC_LIBCOM_ERR)
	@echo "	LD $@"
	@$(CC) -o test_profile -DDEBUG_PROGRAM $(srcdir)/profile.c prof_err.o \
		profile_helpers.o argv_parse.o $(STATIC_LIBCOM_ERR) \
		$(ALL_CFLAGS)

profiled:
#	@echo "	MKDIR $@"
#	@mkdir profiled

e2fsck.8: $(DEP_SUBSTITUTE) $(srcdir)/e2fsck.8.in
	@echo "	SUBST $@"
	@$(SUBSTITUTE_UPTIME) $(srcdir)/e2fsck.8.in e2fsck.8

e2fsck.conf.5: $(DEP_SUBSTITUTE) $(srcdir)/e2fsck.conf.5.in
	@echo "	SUBST $@"
	@$(SUBSTITUTE_UPTIME) $(srcdir)/e2fsck.conf.5.in e2fsck.conf.5

installdirs:
	@echo "	MKINSTALLDIRS $(root_sbindir) $(man8dir)"
	@$(MKINSTALLDIRS) $(DESTDIR)$(root_sbindir) \
		$(DESTDIR)$(man8dir) $(DESTDIR)$(man5dir)

install: $(PROGS) $(MANPAGES) $(FMANPAGES) installdirs
	@for i in $(PROGS); do \
		echo "	INSTALL $(root_sbindir)/$$i"; \
		$(INSTALL_PROGRAM) $$i $(DESTDIR)$(root_sbindir)/$$i; \
	done
	@echo "	LINK $(root_sbindir)/fsck.ext2"
	@$(LN) -f $(DESTDIR)$(root_sbindir)/e2fsck \
			$(DESTDIR)$(root_sbindir)/fsck.ext2
	@echo "	LINK $(root_sbindir)/fsck.ext3"
	@$(LN) -f $(DESTDIR)$(root_sbindir)/e2fsck \
			$(DESTDIR)$(root_sbindir)/fsck.ext3
	@echo "	LINK $(root_sbindir)/fsck.ext4"
	@$(LN) -f $(DESTDIR)$(root_sbindir)/e2fsck \
			$(DESTDIR)$(root_sbindir)/fsck.ext4
	@echo "	LINK $(root_sbindir)/fsck.ext4dev"
	@$(LN) -f $(DESTDIR)$(root_sbindir)/e2fsck \
			$(DESTDIR)$(root_sbindir)/fsck.ext4dev
	@for i in $(MANPAGES); do \
		for j in $(COMPRESS_EXT); do \
			$(RM) -f $(DESTDIR)$(man8dir)/$$i.$$j; \
		done; \
		echo "	INSTALL_DATA $(man8dir)/$$i"; \
		$(INSTALL_DATA) $$i $(DESTDIR)$(man8dir)/$$i; \
	done
	@for i in $(FMANPAGES); do \
		for j in $(COMPRESS_EXT); do \
			$(RM) -f $(DESTDIR)$(man5dir)/$$i.$$j; \
		done; \
		echo "	INSTALL_DATA $(man5dir)/$$i"; \
		$(INSTALL_DATA) $$i $(DESTDIR)$(man5dir)/$$i; \
	done
	@echo "	LINK $(man8dir)/fsck.ext2.8"
	@$(LN) -f $(DESTDIR)$(man8dir)/e2fsck.8 \
		$(DESTDIR)$(man8dir)/fsck.ext2.8
	@echo "	LINK $(man8dir)/fsck.ext3.8"
	@$(LN) -f $(DESTDIR)$(man8dir)/e2fsck.8 \
		$(DESTDIR)$(man8dir)/fsck.ext3.8
	@echo "	LINK $(man8dir)/fsck.ext4.8"
	@$(LN) -f $(DESTDIR)$(man8dir)/e2fsck.8 \
		$(DESTDIR)$(man8dir)/fsck.ext4.8
	@echo "	LINK $(man8dir)/fsck.ext4.8"
	@$(LN) -f $(DESTDIR)$(man8dir)/e2fsck.8 \
		$(DESTDIR)$(man8dir)/fsck.ext4dev.8

install-strip: install
	@for i in $(PROGS); do \
		echo "	STRIP $(root_sbindir)/$$i"; \
		$(STRIP) $(DESTDIR)$(root_sbindir)/$$i; \
	done

uninstall:
	for i in $(PROGS); do \
		$(RM) -f $(DESTDIR)$(root_sbindir)/$$i; \
	done
	$(RM) -f $(DESTDIR)$(root_sbindir)/fsck.ext2 \
		$(DESTDIR)$(root_sbindir)/fsck.ext3 \
		$(DESTDIR)$(root_sbindir)/fsck.ext4 \
		$(DESTDIR)$(root_sbindir)/fsck.ext4dev
	for i in $(MANPAGES); do \
		$(RM) -f $(DESTDIR)$(man8dir)/$$i; \
	done
	for i in $(FMANPAGES); do \
		$(RM) -f $(DESTDIR)$(man5dir)/$$i; \
	done
	$(RM) -f $(DESTDIR)$(root_sbindir)/fsck.ext2 \
			$(DESTDIR)$(root_sbindir)/fsck.ext3 \
			$(DESTDIR)$(root_sbindir)/fsck.ext4 \
			$(DESTDIR)$(root_sbindir)/fsck.ext4dev

clean:
	$(RM) -f $(PROGS) \#* *\# *.s *.o *.a *~ core e2fsck.static \
		e2fsck.shared e2fsck.profiled flushb e2fsck.8 \
		tst_crc32 tst_region tst_refcount gen_crc32table \
		crc32table.h e2fsck.conf.5 prof_err.c prof_err.h \
		test_profile
	$(RM) -rf profiled

mostlyclean: clean
distclean: clean
	$(RM) -f .depend Makefile $(srcdir)/TAGS $(srcdir)/Makefile.in.old

# +++ Dependency line eater +++
# 
# Makefile dependencies follow.  This must be the last section in
# the Makefile.in file
#
e2fsck.o: $(srcdir)/e2fsck.c $(srcdir)/e2fsck.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_builddir)/lib/ext2fs/ext2_types.h \
 $(top_srcdir)/lib/ext2fs/ext2fs.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_srcdir)/lib/ext2fs/ext3_extents.h $(top_srcdir)/lib/et/com_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h $(srcdir)/problem.h
crc32.o: $(srcdir)/crc32.c $(srcdir)/e2fsck.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_builddir)/lib/ext2fs/ext2_types.h \
 $(top_srcdir)/lib/ext2fs/ext2fs.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_srcdir)/lib/ext2fs/ext3_extents.h $(top_srcdir)/lib/et/com_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h $(srcdir)/crc32defs.h crc32table.h
gen_crc32table.o: $(srcdir)/gen_crc32table.c $(srcdir)/crc32defs.h
dict.o: $(srcdir)/dict.c $(srcdir)/dict.h
super.o: $(srcdir)/super.c $(top_srcdir)/lib/uuid/uuid.h $(srcdir)/e2fsck.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_builddir)/lib/ext2fs/ext2_types.h \
 $(top_srcdir)/lib/ext2fs/ext2fs.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_srcdir)/lib/ext2fs/ext3_extents.h $(top_srcdir)/lib/et/com_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h $(srcdir)/problem.h
pass1.o: $(srcdir)/pass1.c $(srcdir)/e2fsck.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_builddir)/lib/ext2fs/ext2_types.h \
 $(top_srcdir)/lib/ext2fs/ext2fs.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_srcdir)/lib/ext2fs/ext3_extents.h $(top_srcdir)/lib/et/com_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h $(srcdir)/problem.h
pass1b.o: $(srcdir)/pass1b.c $(top_srcdir)/lib/et/com_err.h \
 $(srcdir)/e2fsck.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_builddir)/lib/ext2fs/ext2_types.h $(top_srcdir)/lib/ext2fs/ext2fs.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_srcdir)/lib/ext2fs/ext3_extents.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h $(srcdir)/problem.h $(srcdir)/dict.h
pass2.o: $(srcdir)/pass2.c $(srcdir)/e2fsck.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_builddir)/lib/ext2fs/ext2_types.h \
 $(top_srcdir)/lib/ext2fs/ext2fs.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_srcdir)/lib/ext2fs/ext3_extents.h $(top_srcdir)/lib/et/com_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h $(srcdir)/problem.h $(srcdir)/dict.h
pass3.o: $(srcdir)/pass3.c $(srcdir)/e2fsck.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_builddir)/lib/ext2fs/ext2_types.h \
 $(top_srcdir)/lib/ext2fs/ext2fs.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_srcdir)/lib/ext2fs/ext3_extents.h $(top_srcdir)/lib/et/com_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h $(srcdir)/problem.h
pass4.o: $(srcdir)/pass4.c $(srcdir)/e2fsck.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_builddir)/lib/ext2fs/ext2_types.h \
 $(top_srcdir)/lib/ext2fs/ext2fs.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_srcdir)/lib/ext2fs/ext3_extents.h $(top_srcdir)/lib/et/com_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h $(srcdir)/problem.h
pass5.o: $(srcdir)/pass5.c $(srcdir)/e2fsck.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_builddir)/lib/ext2fs/ext2_types.h \
 $(top_srcdir)/lib/ext2fs/ext2fs.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_srcdir)/lib/ext2fs/ext3_extents.h $(top_srcdir)/lib/et/com_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h $(srcdir)/problem.h
journal.o: $(srcdir)/journal.c $(srcdir)/jfs_user.h $(srcdir)/e2fsck.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_builddir)/lib/ext2fs/ext2_types.h \
 $(top_srcdir)/lib/ext2fs/ext2fs.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_srcdir)/lib/ext2fs/ext3_extents.h $(top_srcdir)/lib/et/com_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h $(top_srcdir)/lib/ext2fs/kernel-jbd.h \
 $(top_srcdir)/lib/ext2fs/jfs_compat.h $(top_srcdir)/lib/ext2fs/kernel-list.h \
 $(srcdir)/problem.h $(top_srcdir)/lib/uuid/uuid.h
recovery.o: $(srcdir)/recovery.c $(srcdir)/jfs_user.h $(srcdir)/e2fsck.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_builddir)/lib/ext2fs/ext2_types.h \
 $(top_srcdir)/lib/ext2fs/ext2fs.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_srcdir)/lib/ext2fs/ext3_extents.h $(top_srcdir)/lib/et/com_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h $(top_srcdir)/lib/ext2fs/kernel-jbd.h \
 $(top_srcdir)/lib/ext2fs/jfs_compat.h $(top_srcdir)/lib/ext2fs/kernel-list.h
revoke.o: $(srcdir)/revoke.c $(srcdir)/jfs_user.h $(srcdir)/e2fsck.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_builddir)/lib/ext2fs/ext2_types.h \
 $(top_srcdir)/lib/ext2fs/ext2fs.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_srcdir)/lib/ext2fs/ext3_extents.h $(top_srcdir)/lib/et/com_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h $(top_srcdir)/lib/ext2fs/kernel-jbd.h \
 $(top_srcdir)/lib/ext2fs/jfs_compat.h $(top_srcdir)/lib/ext2fs/kernel-list.h
badblocks.o: $(srcdir)/badblocks.c $(top_srcdir)/lib/et/com_err.h \
 $(srcdir)/e2fsck.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_builddir)/lib/ext2fs/ext2_types.h $(top_srcdir)/lib/ext2fs/ext2fs.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_srcdir)/lib/ext2fs/ext3_extents.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h
util.o: $(srcdir)/util.c $(srcdir)/e2fsck.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_builddir)/lib/ext2fs/ext2_types.h \
 $(top_srcdir)/lib/ext2fs/ext2fs.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_srcdir)/lib/ext2fs/ext3_extents.h $(top_srcdir)/lib/et/com_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h
unix.o: $(srcdir)/unix.c $(top_srcdir)/lib/et/com_err.h \
 $(top_srcdir)/lib/e2p/e2p.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_builddir)/lib/ext2fs/ext2_types.h $(srcdir)/e2fsck.h \
 $(top_srcdir)/lib/ext2fs/ext2fs.h $(top_srcdir)/lib/ext2fs/ext3_extents.h \
 $(top_srcdir)/lib/et/com_err.h $(top_srcdir)/lib/ext2fs/ext2_io.h \
 $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h $(srcdir)/problem.h $(top_srcdir)/version.h
dirinfo.o: $(srcdir)/dirinfo.c $(srcdir)/e2fsck.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_builddir)/lib/ext2fs/ext2_types.h \
 $(top_srcdir)/lib/ext2fs/ext2fs.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_srcdir)/lib/ext2fs/ext3_extents.h $(top_srcdir)/lib/et/com_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h $(top_srcdir)/lib/uuid/uuid.h \
 $(top_srcdir)/lib/ext2fs/tdb.h
dx_dirinfo.o: $(srcdir)/dx_dirinfo.c $(srcdir)/e2fsck.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_builddir)/lib/ext2fs/ext2_types.h \
 $(top_srcdir)/lib/ext2fs/ext2fs.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_srcdir)/lib/ext2fs/ext3_extents.h $(top_srcdir)/lib/et/com_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h
ehandler.o: $(srcdir)/ehandler.c $(srcdir)/e2fsck.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_builddir)/lib/ext2fs/ext2_types.h \
 $(top_srcdir)/lib/ext2fs/ext2fs.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_srcdir)/lib/ext2fs/ext3_extents.h $(top_srcdir)/lib/et/com_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h
problem.o: $(srcdir)/problem.c $(srcdir)/e2fsck.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_builddir)/lib/ext2fs/ext2_types.h \
 $(top_srcdir)/lib/ext2fs/ext2fs.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_srcdir)/lib/ext2fs/ext3_extents.h $(top_srcdir)/lib/et/com_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h $(srcdir)/problem.h $(srcdir)/problemP.h
message.o: $(srcdir)/message.c $(srcdir)/e2fsck.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_builddir)/lib/ext2fs/ext2_types.h \
 $(top_srcdir)/lib/ext2fs/ext2fs.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_srcdir)/lib/ext2fs/ext3_extents.h $(top_srcdir)/lib/et/com_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h $(srcdir)/problem.h
ea_refcount.o: $(srcdir)/ea_refcount.c $(srcdir)/e2fsck.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_builddir)/lib/ext2fs/ext2_types.h \
 $(top_srcdir)/lib/ext2fs/ext2fs.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_srcdir)/lib/ext2fs/ext3_extents.h $(top_srcdir)/lib/et/com_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h
rehash.o: $(srcdir)/rehash.c $(srcdir)/e2fsck.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_builddir)/lib/ext2fs/ext2_types.h \
 $(top_srcdir)/lib/ext2fs/ext2fs.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_srcdir)/lib/ext2fs/ext3_extents.h $(top_srcdir)/lib/et/com_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h $(srcdir)/problem.h
region.o: $(srcdir)/region.c $(srcdir)/e2fsck.h \
 $(top_srcdir)/lib/ext2fs/ext2_fs.h $(top_builddir)/lib/ext2fs/ext2_types.h \
 $(top_srcdir)/lib/ext2fs/ext2fs.h $(top_srcdir)/lib/ext2fs/ext2_fs.h \
 $(top_srcdir)/lib/ext2fs/ext3_extents.h $(top_srcdir)/lib/et/com_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_io.h $(top_builddir)/lib/ext2fs/ext2_err.h \
 $(top_srcdir)/lib/ext2fs/ext2_ext_attr.h $(top_srcdir)/lib/ext2fs/bitops.h \
 $(top_srcdir)/lib/blkid/blkid.h $(top_builddir)/lib/blkid/blkid_types.h \
 $(srcdir)/profile.h prof_err.h
profile.o: $(srcdir)/profile.c $(top_srcdir)/lib/et/com_err.h \
 $(srcdir)/profile.h prof_err.h
prof_err.o: prof_err.c
