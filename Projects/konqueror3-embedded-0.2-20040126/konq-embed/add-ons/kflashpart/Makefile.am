SUBDIRS = flashlib
KDE_OPTIONS = qtonly

if KONQ_EMBEDDED
embedded_inc = -I$(srcdir)/../../dropin -I$(srcdir)/../../kdesrc \
	-I$(srcdir)/../../kdesrc/kdecore -I$(srcdir)/../../kdesrc/khtml
kflash_bin =
kflash_emb = libkflash_e.la
kflash_kde =
else
embedded_inc =
kflash_bin = kflashplay
kflash_emb =
kflash_kde = libkflash.la
endif

INCLUDES    = $(embedded_inc) $(all_includes)


# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

#mimedatadir = $(kde_mimedir)/application
#mimedata_DATA = x-shockwave-flash.desktop x-futuresplash.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kflashpart.pot

# *** The Player ***

bin_PROGRAMS = $(kflash_bin)
kflashplay_SOURCES = main.cpp kflashplay.cpp
kflashplay_LDFLAGS = $(KDE_RPATH)
kflashplay_LDADD   = $(LIB_KPARTS) $(all_libraries)
 
# this is where the desktop file will go
shelldesktopdir   = $(kde_appsdir)/Utilities
shelldesktop_DATA = kflashplay.desktop
 
# this is where the shell's XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/kflashplay
shellrc_DATA = kflashplay.rc

# *** The Part ***

# Full Desktop version
kde_module_LTLIBRARIES = $(kflash_kde)
libkflash_la_SOURCES = kflashpart.cpp
libkflash_la_LDFLAGS = -module $(KDE_PLUGIN)
libkflash_la_LIBADD  = $(LIB_KHTML) $(LIB_KPARTS) flashlib/libflash.la $(all_libraries)
libkflash_la_DEPENDENCIES = flashlib/libflash.la

# Embedded version
noinst_LTLIBRARIES = $(kflash_emb)
libkflash_e_la_SOURCES = kflashpart.cpp
libkflash_e_la_LIBADD = flashlib/libflash.la
libkflash_e_la_DEPENDENCIES = flashlib/libflash.la

# this is where the part's XML-GUI resource file goes
partrcdir   = $(kde_datadir)/kflashplay
partrc_DATA = kflashpart.rc

# the part's desktop file
desktopdir   = $(kde_servicesdir)
desktop_DATA = kflashpart.desktop
