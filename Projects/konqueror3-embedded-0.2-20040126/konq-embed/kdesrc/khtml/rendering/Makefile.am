#    This file is part of the KDE libraries
#    Copyright (C) 1997 Martin Jones (mjones@kde.org)
#              (C) 1997 Torben Weis (weis@kde.org)

#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Library General Public
#    License as published by the Free Software Foundation; either
#    version 2 of the License, or (at your option) any later version.

#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Library General Public License for more details.

#    You should have received a copy of the GNU Library General Public License
#    along with this library; see the file COPYING.LIB.  If not, write to
#    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
#    Boston, MA 02111-1307, USA.

KDE_CXXFLAGS = $(WOVERLOADED_VIRTUAL)

noinst_LTLIBRARIES = libkhtmlrender.la
libkhtmlrender_la_SOURCES = \
	bidi.cpp break_lines.cpp \
	render_style.cpp render_object.cpp render_container.cpp render_box.cpp \
	render_flow.cpp render_text.cpp \
	render_image.cpp render_table.cpp table_layout.cpp \
	render_replaced.cpp render_form.cpp render_list.cpp \
	render_root.cpp render_frames.cpp render_br.cpp render_applet.cpp \
	render_html.cpp render_body.cpp font.cpp 

libkhtmlrender_la_METASOURCES = AUTO

noinst_HEADERS = \
	bidi.h break_lines.h \
	render_style.h render_object.h render_container.h render_box.h \
	render_flow.h render_text.h render_table.h render_replaced.h \
	render_form.h render_list.h render_root.h render_frames.h \
	render_br.h render_applet.h font.h table_layout.h

INCLUDES = -I$(top_srcdir)/kimgio -I$(top_srcdir)/kio -I$(top_srcdir)/dcop \
 -I$(top_srcdir)/kfile -I$(top_srcdir)/khtml -I$(top_srcdir) -I$(srcdir)/../../../dropin -I$(srcdir)/../../kdecore -I$(srcdir)/../.. -I$(srcdir)/.. -I$(srcdir)/../misc -I$(srcdir)/../misc -I$(srcdir)/../css -I$(srcdir)/../dom -I$(srcdir)/../xml -I$(srcdir)/../html -I$(srcdir)/../../../dropin/kio -I$(srcdir)/../../kjs -I$(srcdir)/../../../dropin/kssl -I$(srcdir)/../../../dropin/khtml -I$(srcdir)/../../../dropin/khtml/java $(QT_INCLUDES) $(X_INCLUDES) $(all_includes)

SRCDOC_DEST=$(kde_htmldir)/en/kdelibs/khtml

## generate lib documentation
srcdoc:
	$(mkinstalldirs) $(SRCDOC_DEST)
	kdoc -H -d $(SRCDOC_DEST) kdecore -lqt


KDE_OPTIONS = qtonly
