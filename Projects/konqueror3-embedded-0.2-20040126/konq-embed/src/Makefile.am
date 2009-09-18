# the implementations of preferences.ui are deliberately in
# the CVS repository, to avoid a uic dependency (for now)
# so when making changes to preferences.ui don't forget to
# 'make uifiles'

SUBDIRS = actions

UIC = uic

if LARGEICONS
isearch = pics/big
else
isearch = pics
endif

INCLUDES = -I$(srcdir)/$(isearch) \
	   -I$(srcdir)/../dropin -I$(srcdir)/../kdesrc/kdecore \
	   -I$(srcdir)/../kdesrc/kio/http/kcookiejar \
	   -I$(srcdir)/../kdesrc/kssl \
	   -I$(srcdir)/../kdesrc/khtml -I$(srcdir)/../kdesrc \
	   -I$(srcdir)/../kdesrc/khtml/dom \
	   -I$(srcdir)/../kdesrc/khtml/misc \
	   -I$(srcdir)/../dropin/kio \
	   -I$(srcdir)/../dropin/khtml \
	   $(QTOPIA_INCLUDES) $(all_includes)

bin_PROGRAMS = konqueror

konqueror_SOURCES = main.cc mainwindowbase.cc mainwindebug.cc \
	       htmlview.cc popuphandler.cc \
	       view.cc run.cc browseriface.cc \
	       bookmarks.cc preferences.cc preferencesimpl.cc \
               mainwindow_qpe.cc mainwindow_x11.cc \
               mainwindow_kiosk.cc mainwindow_tv.cc \
               xmltree.cc bookmarkeditor.cc bookmarkeditorimpl.cc \
               bookmarkedit.cc bookmarkeditimpl.cc

konqueror_LDADD = $(LIB_ADDONS) \
		$(srcdir)/../dropin/kio/libkiodropin.la \
		$(LIB_KJSHTML) \
		../lib/libkonqe.la \
		$(srcdir)/../kdesrc/kparts/libkparts.la \
		$(srcdir)/../dropin/libdropin.la \
		$(srcdir)/../dropin/kparts/libkpartsdropin.la \
		$(LIB_KHTML) \
		../dropin/kssl/libkssldropin.la \
		../kdesrc/kssl/libkssl.la 


konqueror_LDFLAGS = $(KONQ_SSL_LDFLAGS) $(all_libraries) -export-dynamic
konqueror_METASOURCES = AUTO
konqueror_DEPENDENCIES = $(LIB_KJSHTML) $(LIB_KHTML) $(LIB_ADDONS)
BUILT_SOURCES = init.inc

icon_DATA = konqueror.png
icondir = $(kde_icondir)

dotdesktop_DATA = konqueror.desktop
dotdesktopdir = $(kde_appsdir)/Applications

KDE_OPTIONS = qtonly

init.inc: $(LIB_ADDONS)
	echo '/* This file is generated automatically, do not edit */' > $@
	-test -n "$(INIT_ADDONS)" && (for i in $(INIT_ADDONS); do echo "KONQE_INIT($$i);"; done >> $@)

uifiles:
	$(UIC) -o $(srcdir)/preferences.h $(srcdir)/preferences.ui
	echo '#include <klocale.h>' > $(srcdir)/preferences.cc
	$(UIC) -tr i18n -i preferences.h $(srcdir)/preferences.ui | sed -e "s,i18n( \"\" ),QString::null,g" >> $(srcdir)/preferences.cc
	echo '#include "preferences.moc"' >> $(srcdir)/preferences.cc

	echo "#include <config.h>" > $(srcdir)/bookmarkeditor.h
	echo "#if defined(ENABLE_BOOKMARKS)" >> $(srcdir)/bookmarkeditor.h
	$(UIC) $(srcdir)/bookmarkeditor.ui >> $(srcdir)/bookmarkeditor.h
	echo "#endif" >> $(srcdir)/bookmarkeditor.h

	echo "#include <config.h>" > $(srcdir)/bookmarkeditor.cc
	echo "#if defined(ENABLE_BOOKMARKS)" >> $(srcdir)/bookmarkeditor.cc
	echo '#include <klocale.h>' >> $(srcdir)/bookmarkeditor.cc
	$(UIC) -tr i18n -i bookmarkeditor.h $(srcdir)/bookmarkeditor.ui | sed -e "s,i18n( \"\" ),QString::null,g" >> $(srcdir)/bookmarkeditor.cc
	echo '#include "bookmarkeditor.moc"' >> $(srcdir)/bookmarkeditor.cc
	echo "#endif" >> $(srcdir)/bookmarkeditor.cc

	echo "#include <config.h>" > $(srcdir)/bookmarkedit.h
	echo "#if defined(ENABLE_BOOKMARKS)" >> $(srcdir)/bookmarkedit.h
	$(UIC) $(srcdir)/bookmarkedit.ui >> $(srcdir)/bookmarkedit.h
	echo "#endif" >> $(srcdir)/bookmarkedit.h

	echo "#include <config.h>" > $(srcdir)/bookmarkedit.cc
	echo "#if defined(ENABLE_BOOKMARKS)" >> $(srcdir)/bookmarkedit.cc
	echo '#include <klocale.h>' >> $(srcdir)/bookmarkedit.cc
	$(UIC) -tr i18n -i bookmarkedit.h $(srcdir)/bookmarkedit.ui | sed -e "s,i18n( \"\" ),QString::null,g" >> $(srcdir)/bookmarkedit.cc
	echo '#include "bookmarkedit.moc"' >> $(srcdir)/bookmarkedit.cc
	echo "#endif" >> $(srcdir)/bookmarkedit.cc

fixup_xpms:
	@cd $(srcdir) ;\
	for xpm in `find pics -type f -name *.xpm`; do \
		if ! grep "static const char \\* const" $$xpm 2>/dev/null >/dev/null; then \
			echo "fixing $$xpm" ;\
			perl -pi -e "s,static const char \\*(.*),static const char * const \\1," $$xpm ;\
		fi ;\
	done
