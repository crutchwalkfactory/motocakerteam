/*  This file is part of the KDE project
    Copyright (C) 2001 Simon Hausmann <hausmann@kde.org>

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

   $Id: mainwindow_qpe.h,v 1.5 2003/03/17 20:49:02 mueller Exp $
*/

#ifndef __mainwindow_qpe_h__
#define __mainwindow_qpe_h__

#include <config.h>

#if defined(KONQ_GUI_QPE)

#include "mainwindowbase.h"
#include "mainwindow_x11.h"

class MainWindowQPE : public MainWindowX11
{
    Q_OBJECT
public:
    MainWindowQPE();

public slots:
    void setDocument( const QString &s );

protected:
    virtual QIconSet loadPixmap( const char * const xpm[] );
};

#endif

#endif // __mainwindow_qpe_h__
