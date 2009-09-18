lib_LTLIBRARIES = libkonqe.la

if include_VERSION_SCRIPT
#VSCRIPT = -Wl,--version-script=$(srcdir)/libkonqe.map
VSCRIPT =
endif

libkonqe_la_SOURCES = dummy.cc
#libkonqe_la_LIBADD = ../kdesrc/kio/http/kcookiejar/libkcookiejar.la \
#	     ../kdesrc/kio/http/libkiohttp.la \
#	     ../kdesrc/kio/libkio.la 
#     ../kdesrc/kparts/libkparts.la \
#     ../kdesrc/khtml/libkhtml.la \
#     ../kdesrc/kio/http/libkiohttp.la \
#     ../dropin/kparts/libkpartsdropin.la \
#     $(LIB_KJSHTML)
libkonqe_la_LIBADD = ../kdesrc/kdecore/libkdecore.la \
	             ../kdesrc/kio/libkio.la \
	             ../kdesrc/kio/http/libkiohttp.la \
	             ../kdesrc/kio/http/kcookiejar/libkcookiejar.la \
	             ../kdesrc/kssl/libkssl.la \
                     ../kdesrc/kparts/libkparts.la \
                     ../dropin/kssl/libkssldropin.la \
		     ../dropin/kparts/libkpartsdropin.la \
		     ../dropin/kio/libkiodropin.la 

libkonqe_la_LDFLAGS =  $(QT_LDFLAGS) -ldl $(VSCRIPT)

KDE_OPTIONS = qtonly

dummy.cc:
	echo > dummy.cc
