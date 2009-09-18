/*  This file is part of the KDE project
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

#include "kiconloader.h"

KIconLoader *KIconLoader::s_self = 0;

KIconLoader *KIconLoader::self()
{
    if ( !s_self )
        s_self = new KIconLoader;
    return s_self;
}

QPixmap KIconLoader::loadIcon( const QString &name, int, int, int )
{
    return m_pixmaps[ name ];
}

void KIconLoader::registerIcon( const QString &name, const QPixmap &pix )
{
    m_pixmaps[ name ] = pix;
}
