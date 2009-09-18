
SUBDIRS = malloc
INCLUDES = -I$(srcdir)/../../dropin $(QTOPIA_INCLUDES) $(all_includes)

noinst_LTLIBRARIES = libkdecore.la

libkdecore_la_SOURCES = kurl.cpp krfcdate.cpp ksock.cpp \
			kentities.c kstandarddirs.cpp ktempfile.cpp \
			ksavefile.cpp kmdcodec.cpp ksockaddr.cpp \
			netsupp.cpp kextsock.cpp kbufferedio.cpp \
	                kmultipledrag.cpp kasyncio.cpp

libkdecore_la_LIBADD = ../../dropin/libdropin.la
libkdecore_la_METASOURCES = AUTO

KDE_OPTIONS = qtonly
