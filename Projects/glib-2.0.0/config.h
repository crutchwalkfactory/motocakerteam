/* config.h.  Generated automatically by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */
/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
/* acconfig.h
   This file is in the public domain.

   Descriptive text for the C preprocessor macros that
   the distributed Autoconf macros can define.
   No software package will use all of them; autoheader copies the ones
   your configure.in uses into your configuration header file templates.

   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  Although this order
   can split up related entries, it makes it easier to check whether
   a given entry is in the file.

   Leave the following blank line there!!  Autoheader needs it.  */


/* Other stuff */

/* #undef PACKAGE */
/* #undef VERSION */


/* Leave that blank line there!!  Autoheader needs it.
   If you're adding to this file, keep in mind:
   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  */

#define ENABLE_NLS 1
#define GETTEXT_PACKAGE "glib20"
#define GLIB_LOCALE_DIR "NONE/share/locale"
#define HAVE_GETTEXT 1
#define HAVE_LC_MESSAGES 1
#define SANE_MALLOC_PROTOS 1
/* #undef USE_LIBICONV_GNU */
/* #undef USE_LIBICONV_NATIVE */

/* Define to one of `_getb67', `GETB67', `getb67' for Cray-2 and Cray-YMP
   systems. This function is required for `alloca.c' support on those systems.
   */
/* #undef CRAY_STACKSEG_END */

/* Define if using `alloca.c'. */
/* #undef C_ALLOCA */

/* Whether to disable memory pools */
/* #undef DISABLE_MEM_POOLS */

/* Whether to enable GC friendliness */
/* #undef ENABLE_GC_FRIENDLY */

/* Define to the GLIB binary age */
#define GLIB_BINARY_AGE 0

/* Byte contents of gmutex */
#define GLIB_BYTE_CONTENTS_GMUTEX 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

/* Define to the GLIB interface age */
#define GLIB_INTERFACE_AGE 0

/* Define to the GLIB major version */
#define GLIB_MAJOR_VERSION 2

/* Define to the GLIB micro version */
#define GLIB_MICRO_VERSION 0

/* Define to the GLIB minor version */
#define GLIB_MINOR_VERSION 0

/* Size of gmutex */
#define GLIB_SIZEOF_GMUTEX 24

/* Size of intmax_t */
#define GLIB_SIZEOF_INTMAX_T 8

/* Size of ptrdiff_t */
#define GLIB_SIZEOF_PTRDIFF_T 4

/* Size of size_t */
#define GLIB_SIZEOF_SIZE_T 4

/* Size of system_thread */
#define GLIB_SIZEOF_SYSTEM_THREAD 4

/* Whether glib was compiled with debugging enabled */
#define G_COMPILED_WITH_DEBUGGING "minimum"

/* Have inline keyword */
#define G_HAVE_INLINE 1

/* Have __inline keyword */
#define G_HAVE___INLINE 1

/* Have __inline__ keyword */
#define G_HAVE___INLINE__ 1

/* Source file containing theread implementation */
#define G_THREAD_SOURCE "gthread-posix.c"

/* whether to use the PID niceness surrogate for thread priorities */
/* #undef G_THREAD_USE_PID_SURROGATE */

/* A 'va_copy' style function */
#define G_VA_COPY va_copy

/* 'va_lists' cannot be copies as values */
/* #undef G_VA_COPY_AS_ARRAY */

/* Define if you have `alloca', as a function or macro. */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix). */
#define HAVE_ALLOCA_H 1

/* Define if you have the <argz.h> header file. */
#define HAVE_ARGZ_H 1

/* Define if you have the `atexit' function. */
#define HAVE_ATEXIT 1

/* Define if you have the `bind_textdomain_codeset' function. */
#define HAVE_BIND_TEXTDOMAIN_CODESET 1

/* Define if you have a version of the vsnprintf function with semantics as
   specified by the ISO C99 standard. */
#define HAVE_C99_VSNPRINTF 1

/* Have nl_langinfo (CODESET) */
#define HAVE_CODESET 1

/* Define if you have the `dcgettext' function. */
#define HAVE_DCGETTEXT 1

/* Define if you have the <dirent.h> header file. */
#define HAVE_DIRENT_H 1

/* Define if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define if you don't have `vprintf' but do have `_doprnt.' */
/* #undef HAVE_DOPRNT */

/* Define if you have the <float.h> header file. */
#define HAVE_FLOAT_H 1

/* Define if you have the `getcwd' function. */
#define HAVE_GETCWD 1

/* Define if you have the `getpagesize' function. */
#define HAVE_GETPAGESIZE 1

/* Define if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if you have <langinfo.h> and nl_langinfo(CODESET). */
#define HAVE_LANGINFO_CODESET 1

/* Define if you have the <langinfo.h> header file. */
#define HAVE_LANGINFO_H 1

/* Define if your <locale.h> file defines LC_MESSAGES. */
#define HAVE_LC_MESSAGES 1

/* Define if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define if you have the `localtime_r' function. */
#define HAVE_LOCALTIME_R 1

/* Define if you have the `lstat' function. */
#define HAVE_LSTAT 1

/* Define if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define if you have the `memmove' function. */
#define HAVE_MEMMOVE 1

/* Define if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if you have the `mkstemp' function. */
#define HAVE_MKSTEMP 1

/* Define if you have a working `mmap' system call. */
#define HAVE_MMAP 1

/* Define if you have the `munmap' function. */
#define HAVE_MUNMAP 1

/* Define if you have the `nanosleep' function. */
#define HAVE_NANOSLEEP 1

/* Define if you have the `nl_langinfo' function. */
#define HAVE_NL_LANGINFO 1

/* Define if you have the <nl_types.h> header file. */
#define HAVE_NL_TYPES_H 1

/* Have non-POSIX function getpwuid_r */
/* #undef HAVE_NONPOSIX_GETPWUID_R */

/* Define if you have the `on_exit' function. */
#define HAVE_ON_EXIT 1

/* Define if you have the `poll' function. */
#define HAVE_POLL 1

/* Have POSIX function getpwuid_r */
#define HAVE_POSIX_GETPWUID_R 1

/* Have function pthread_attr_setstacksize */
#define HAVE_PTHREAD_ATTR_SETSTACKSIZE 1

/* Define if you have the `putenv' function. */
#define HAVE_PUTENV 1

/* Define if you have the <pwd.h> header file. */
#define HAVE_PWD_H 1

/* Define if you have the <sched.h> header file. */
#define HAVE_SCHED_H 1

/* Define if you have the `setenv' function. */
#define HAVE_SETENV 1

/* Define if you have the `setlocale' function. */
#define HAVE_SETLOCALE 1

/* Define if you have the <stddef.h> header file. */
#define HAVE_STDDEF_H 1

/* Define if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define if you have the `stpcpy' function. */
#define HAVE_STPCPY 1

/* Define if you have the `strcasecmp' function. */
#define HAVE_STRCASECMP 1

/* Define if you have the `strchr' function. */
#define HAVE_STRCHR 1

/* Define if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Have functions strlcpy and strlcat */
/* #undef HAVE_STRLCPY */

/* Define if you have the `strncasecmp' function. */
#define HAVE_STRNCASECMP 1

/* Define if you have the `strsignal' function. */
#define HAVE_STRSIGNAL 1

/* Define if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define if you have the <sys/poll.h> header file. */
#define HAVE_SYS_POLL_H 1

/* found fd_set in sys/select.h */
#define HAVE_SYS_SELECT_H 1

/* Define if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <sys/times.h> header file. */
#define HAVE_SYS_TIMES_H 1

/* Define if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define if you have the `unsetenv' function. */
#define HAVE_UNSETENV 1

/* Define if you have the <values.h> header file. */
#define HAVE_VALUES_H 1

/* Define if you have the `vasprintf' function. */
#define HAVE_VASPRINTF 1

/* Define if you have the `vprintf' function. */
#define HAVE_VPRINTF 1

/* Define if you have the `vsnprintf' function. */
#define HAVE_VSNPRINTF 1

/* Have a working bcopy */
/* #undef HAVE_WORKING_BCOPY */

/* Define if you have the `__argz_count' function. */
#define HAVE___ARGZ_COUNT 1

/* Define if you have the `__argz_next' function. */
#define HAVE___ARGZ_NEXT 1

/* Define if you have the `__argz_stringify' function. */
#define HAVE___ARGZ_STRINGIFY 1

/* didn't find fd_set */
/* #undef NO_FD_SET */

/* global 'sys_errlist' not found */
/* #undef NO_SYS_ERRLIST */

/* global 'sys_siglist' not found */
/* #undef NO_SYS_SIGLIST */

/* global 'sys_siglist' not declared */
/* #undef NO_SYS_SIGLIST_DECL */

/* Name of package */
/* #undef PACKAGE */

/* Maximum POSIX RT priority */
#define POSIX_MAX_PRIORITY sched_get_priority_max(SCHED_OTHER)

/* Minimum POSIX RT priority */
#define POSIX_MIN_PRIORITY sched_get_priority_min(SCHED_OTHER)

/* The POSIX RT yield function */
#define POSIX_YIELD_FUNC sched_yield()

/* whether realloc (NULL,) works */
#define REALLOC_0_WORKS 1

/* The size of a `char', as computed by sizeof. */
#define SIZEOF_CHAR 1

/* The size of a `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of a `long', as computed by sizeof. */
#define SIZEOF_LONG 4

/* The size of a `long long', as computed by sizeof. */
#define SIZEOF_LONG_LONG 8

/* The size of a `short', as computed by sizeof. */
#define SIZEOF_SHORT 2

/* The size of a `void *', as computed by sizeof. */
#define SIZEOF_VOID_P 4

/* The size of a `__int64', as computed by sizeof. */
#define SIZEOF___INT64 0

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
        STACK_DIRECTION > 0 => grows toward higher addresses
        STACK_DIRECTION < 0 => grows toward lower addresses
        STACK_DIRECTION = 0 => direction of growth unknown */
/* #undef STACK_DIRECTION */

/* Define if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
/* #undef VERSION */

/* Define if your processor stores words with the most significant byte first
   (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */

/* Number of bits in a file offset, on hosts where this is settable. */
#define _FILE_OFFSET_BITS 64

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define as `__inline' if that's what the C compiler calls it, or to nothing
   if it is not supported. */
/* #undef inline */

/* Define to `long' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to `unsigned' if <sys/types.h> does not define. */
/* #undef size_t */
