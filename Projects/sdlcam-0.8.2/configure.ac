AC_INIT(SDLcam, 0.8.2, [raph@raphnet.net])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADER(config.h)

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX

AC_LIBTOOL_DLOPEN
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

dnl
dnl tweak compiler flags
dnl

if test "$GCC" = "yes" ; then
	CFLAGS="$CFLAGS -Wshadow -Wall -Wstrict-prototypes"
	CXXFLAGS="$CXXFLAGS -Wshadow -Wall"
fi

CPPFLAGS="$CPPFLAGS -DMMX -DTIMER"

CPPFLAGS="$CPPFLAGS -DPKGDATADIR=\\\"\$(pkgdatadir)\\\" -DLIBDIR=\\\"\$(libdir)\\\""

dnl
dnl control capture modules which are known to be flaky
dnl

AC_CHECK_HEADER([encore2.h], [BUILD_DIVX=true], [BUILD_DIVX=false])
AM_CONDITIONAL([BUILD_DIVX], test ${BUILD_DIVX} = true)

AC_CHECK_HEADER([avifile/avifile.h], [BUILD_DIVX2=true], [BUILD_DIVX2=false])
AM_CONDITIONAL([BUILD_DIVX2], test ${BUILD_DIVX2} = true)

AC_CHECK_HEADER([fame.h], [BUILD_MPEG=true], [BUILD_MPEG=false])
AM_CONDITIONAL([BUILD_MPEG], test ${BUILD_MPEG} = true)

dnl
dnl check for libraries
dnl

PKG_CHECK_MODULES(SDL, sdl)
PKG_CHECK_MODULES(XML2, libxml-2.0)
PKG_CHECK_MODULES(PNG, libpng)

AC_CHECK_LIB(SDL_ttf, TTF_Init,
	[SDL_LIBS="$SDL_LIBS -lSDL_ttf"],
	[AC_MSG_ERROR([SDL_ttf not found])])
AC_CHECK_LIB(SDL_image, IMG_Load,
	[SDL_LIBS="$SDL_LIBS -lSDL_image"],
	[AC_MSG_ERROR([SDL_image not found])])
AC_CHECK_LIB(jpeg, jpeg_CreateCompress,
	[JPEG_LIBS="-ljpeg"],
	[AC_MSG_ERROR([libjpeg not found])])
AC_SUBST(JPEG_LIBS)

AC_CHECK_LIB(dl, dlopen, , [AC_MSG_ERROR([libdl not found])])

dnl we'll manage libs on a per-binary basis
LIBS=""

AC_OUTPUT([
	SDLcam.xml
	Makefile
	capture/Makefile
	Documentation/Makefile
	filter/Makefile
	sources/Makefile
])
