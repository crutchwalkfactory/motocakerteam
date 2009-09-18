if BUILD_DIVX
divx_la_SOURCES = divx.c
divx_la_LDFLAGS = -module -avoid-version
LA_DIVX = divx.la
endif
if BUILD_DIVX2
divx2_la_SOURCES = divx2.cpp
divx2_la_LDFLAGS = -module -avoid-version
LA_DIVX2 = divx2.la
endif
if BUILD_MPEG
mpeg_la_SOURCES = mpeg.c
mpeg_la_LDFLAGS = -module -avoid-version
LA_MPEG = mpeg.la
endif

libdir = @libdir@/capture
lib_LTLIBRARIES = $(LA_DIVX) $(LA_DIVX2) $(LA_MPEG)
