/*  This file is part of the KDE project
    Copyright (C) 2000 David Faure <faure@kde.org>
    Copyright (C) 2000 Simon Hausmann <hausmann@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    As a special exception this program may be linked with Qt non-commercial 
    edition, the resulting executable be distributed, without including the 
    source code for the Qt non-commercial edition in the source distribution.

*/

#include "kglobalsettings.h"

#include "kglobal.h"
#include "klocale.h"
#include <kcharsets.h>

QFont *KGlobalSettings::s_generalFont = 0;
QFont *KGlobalSettings::s_fixedFont = 0;

QFont KGlobalSettings::generalFont()
{
    if ( s_generalFont )
        return *s_generalFont;

    s_generalFont = new QFont( "helvetica", 12 );
    s_generalFont->setPixelSize( 12 );
    s_generalFont->setStyleHint( QFont::SansSerif );

#if 0 // QT_VERSION < 300 && defined(Q_WS_X11)
    KGlobal::charsets()->setQFont( *s_generalFont, KGlobal::locale()->charset() );
#endif

    return *s_generalFont;
}

QFont KGlobalSettings::fixedFont()
{
    if ( s_fixedFont )
        return *s_fixedFont;

    s_fixedFont = new QFont( "courier", 12 );
    s_fixedFont->setPixelSize( 12 );
    s_fixedFont->setStyleHint( QFont::TypeWriter );

#if 0 && QT_VERSION < 300 && defined(Q_WS_X11)
    KGlobal::charsets()->setQFont( *s_fixedFont, KGlobal::locale()->charset() );
#endif

    return *s_fixedFont;
}
