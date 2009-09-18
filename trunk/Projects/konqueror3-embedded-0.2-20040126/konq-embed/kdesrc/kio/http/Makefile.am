
AM_CXXFLAGS = -DMAX_CACHE_AGE=300

INCLUDES = -I$(srcdir)/../../../dropin -I$(srcdir)/../../kdecore -I$(srcdir)/../.. \
	   -I$(srcdir)/../../../dropin/kio -I$(srcdir)/../../kssl \
	   -I$(srcdir)/../../interfaces \
	   -I$(srcdir)/../ $(QTOPIA_INCLUDES) $(all_includes)

SUBDIRS = kcookiejar

noinst_LTLIBRARIES = libkiohttp.la

libkiohttp_la_SOURCES = http.cc http_cache_cleaner.cpp httpfilter.cc
libkiohttp_la_METASOURCES = AUTO
libkiohttp_la_LIBADD = 
libkiohttp_la_LDFLAGS = $(all_libraries) 

KDE_OPTIONS = qtonly
