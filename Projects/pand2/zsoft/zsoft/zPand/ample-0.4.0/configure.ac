dnl Process this file with autoconf to produce a configure script.

dnl Some intro checks and defines
AC_INIT(Ample,AMPLE_VERSION,david@2gen.com)
AC_REVISION ($Id: configure.ac,v 1.2 2001/12/16 02:11:57 alphix Exp $)
AC_PREREQ(2.50)
AC_CONFIG_SRCDIR(src/ample.c)
AC_CONFIG_HEADER(config.h)

PACKAGE=ample
AMPLE_MAJOR_VERSION=0
AMPLE_MINOR_VERSION=4
AMPLE_MICRO_VERSION=0

AMPLE_VERSION=\"$AMPLE_MAJOR_VERSION.$AMPLE_MINOR_VERSION.$AMPLE_MICRO_VERSION\"
AC_DEFINE_UNQUOTED(AMPLE_VERSION,$AMPLE_VERSION,[Define program version])

dnl Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_INSTALL

dnl Checks for libraries.
dnl from aclocal.m4
AC_CHECK_SOCKET_LIBS

dnl Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h errno.h fcntl.h limits.h netinet/in.h stdlib.h \
		  string.h sys/socket.h unistd.h getopt.h signal.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIGNAL
dnl from aclocal.m4
AC_TYPE_SOCKLEN_T

dnl Checks for library functions.
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([getcwd inet_ntoa memset socket strcasecmp alarm strchr strdup strerror strncasecmp strrchr strtol])
AC_FUNC_FORK
AC_FUNC_LSTAT
AC_FUNC_MALLOC

dnl Checks for conditionals
AC_ARG_WITH(libwrap, AC_HELP_STRING([--with-libwrap],[Use tcpd wrappers]))
AC_ARG_ENABLE(syslog, AC_HELP_STRING([--disable-syslog],[Disable the use of syslog]))

AC_MSG_CHECKING([configure options])
if test "x$with_libwrap" = "xyes" && test "x$enable_syslog" = "xno" ; then
	AC_MSG_ERROR([you can't enable libwrap and disable syslog])
elif test "x$enable_syslog" = "xno" ; then
	AC_MSG_RESULT([syslog disabled, libwrap disabled])
elif test "x$with_libwrap" = "xyes" ; then
	AC_MSG_RESULT([syslog enabled, libwrap enabled])
	AC_CHECK_HEADERS([syslog.h],,[AC_MSG_ERROR([syslog enabled but syslog header not found])])
	dnl from aclocal.m4	
	AC_LIB_WRAP
else
	AC_MSG_RESULT([syslog enabled, libwrap disabled])
	AC_CHECK_HEADERS([syslog.h],,AC_MSG_ERROR([syslog enabled but syslog header not found]))
fi	

dnl Done
AC_OUTPUT([Makefile src/Makefile])
