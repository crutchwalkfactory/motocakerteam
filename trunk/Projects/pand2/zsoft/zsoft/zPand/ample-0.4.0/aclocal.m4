dnl Renamed to AC_TYPE_SOCKLEN_T and some tweaks - David H - 2001-12-12
dnl
dnl @synopsis TYPE_SOCKLEN_T
dnl
dnl Check whether sys/socket.h defines type socklen_t. Please note
dnl that some systems require sys/types.h to be included before
dnl sys/socket.h can be compiled.
dnl
dnl @version $Id: aclocal.m4,v 1.3 2001/12/16 01:35:44 alphix Exp $
dnl @author Lars Brinkhoff <lars@nocrew.org>
dnl
AC_DEFUN([AC_TYPE_SOCKLEN_T],
[
AC_CACHE_CHECK([for socklen_t], 
ac_cv_type_socklen_t, [
        AC_TRY_COMPILE([
                      #include <sys/types.h>
                      #include <sys/socket.h>
              ],
              [
                      socklen_t len = 42; return 0;
              ],
              ac_cv_type_socklen_t="yes", ac_cv_type_socklen_t="no")
        ])

        if test "x$ac_cv_type_socklen_t" = "xno"; then
              AC_DEFINE(socklen_t, int, [the type of the last argument to getsockopt etc])
        fi
])
dnl end of AC_TYPE_SOCKLEN_T


dnl Renamed to AC_CHECK_SOCKET_LIBS and partially rewritten - David H - 2001-12-12
dnl
dnl @synopsis ETR_SOCKET_NSL
dnl
dnl This macro figures out what libraries are required on this platform
dnl to link sockets programs.  It's usually -lsocket and/or -lnsl or
dnl neither.  We test for all three combinations.
dnl
dnl @version $Id: aclocal.m4,v 1.3 2001/12/16 01:35:44 alphix Exp $
dnl @author Warren Young <warren@etr-usa.com>
dnl
AC_DEFUN([AC_CHECK_SOCKET_LIBS],
[
AC_CACHE_CHECK([for extra libraries needed for socket functions],
ac_cv_socket_libs, [
        oLIBS=$LIBS

        AC_TRY_LINK([
                        #include <sys/types.h>
                        #include <sys/socket.h>
                        #include <netinet/in.h>
                        #include <arpa/inet.h>
                ],
                [
                        struct in_addr add;
                        int sd = socket(AF_INET, SOCK_STREAM, 0);
                        inet_ntoa(add);
                ],
                ac_cv_socket_libs="none", ac_cv_socket_libs=no)

        if test "x$ac_cv_socket_libs" = "xno"
        then
                LIBS="$oLIBS -lsocket"
                AC_TRY_LINK([
                                #include <sys/types.h>
                                #include <sys/socket.h>
                                #include <netinet/in.h>
                                #include <arpa/inet.h>
                        ],
                        [
                                struct in_addr add;
                                int sd = socket(AF_INET, SOCK_STREAM, 0);
                                inet_ntoa(add);
                        ],
                        ac_cv_socket_libs=-lsocket, ac_cv_socket_libs=no)
        fi

        if test "x$ac_cv_socket_libs" = "xno"
        then
                LIBS="$oLIBS -lsocket -lnsl"
                AC_TRY_LINK([
                                #include <sys/types.h>
                                #include <sys/socket.h>
                                #include <netinet/in.h>
                                #include <arpa/inet.h>
                        ],
                        [
                                struct in_addr add;
                                int sd = socket(AF_INET, SOCK_STREAM, 0);
                                inet_ntoa(add);
                        ],
                        ac_cv_socket_libs="-lsocket -lnsl", ac_cv_socket_libs=no)
        fi

])

        if test "x$ac_cv_socket_libs" = "xno"
        then
                AC_MSG_ERROR([cannot find socket libraries])
	fi
]) dnl AC_CHECK_SOCKET_LIBS


dnl Written by me - David H - 2001-12-12
dnl
dnl @synopsis AC_LIB_WRAP
dnl
dnl This macro makes sure that libwrap is found and also checks if
dnl libnsl is needed for libwrap to function correctly.
dnl
dnl @author David Härdeman <david@2gen.com>
dnl
AC_DEFUN([AC_LIB_WRAP],
[
AC_CACHE_VAL(ac_cv_libwrap_libs, 
[

        oLIBS=$LIBS
	AC_CHECK_HEADER([tcpd.h],,AC_MSG_ERROR([cannot find tcpd.h]),)
	AC_MSG_CHECKING([for libwrap libraries])

	LIBS="$oLIBS -lwrap"
        AC_TRY_LINK([
			#include <syslog.h>
                        #include <tcpd.h>
       			int allow_severity = LOG_INFO;
			int deny_severity = LOG_WARNING;
         	],
                [
			struct request_info request;
			hosts_access(&request);
                ],
                ac_cv_libwrap_libs=-lwrap, ac_cv_libwrap_libs=no)

        if test "x$ac_cv_libwrap_libs" = "xno"
        then
                LIBS="$oLIBS -lwrap -lnsl"
                AC_TRY_LINK([
				#include <syslog.h>
                                #include <tcpd.h>
				int allow_severity = LOG_INFO;
				int deny_severity = LOG_WARNING;
                        ],
                        [
				struct request_info request;
				hosts_access(&request);
                        ],
                        ac_cv_libwrap_libs="-lwrap -lnsl", ac_cv_libwrap_libs=no)
        fi
])

        if test "x$ac_cv_libwrap_libs" = "xno"
        then
                AC_MSG_ERROR([cannot find libraries])
	else
		AC_MSG_RESULT($ac_cv_libwrap_libs)
	fi
]) dnl AC_LIB_WRAP
