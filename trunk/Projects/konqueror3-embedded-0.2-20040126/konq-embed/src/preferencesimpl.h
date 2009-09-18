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

*/


#ifndef __preferencesimpl_h__
#define __preferencesimpl_h__

#include "preferences.h"

#include <qstringlist.h>

class MainWindowBase;

class Preferences : public PreferencesBase
{
    Q_OBJECT
public:
    Preferences( MainWindowBase *parent, const char *name = 0 );
    virtual ~Preferences();

    void save();

public slots:
    virtual void defaults();
    virtual void makeCurrentSiteHomePage();

private:
    void setupFontCombo( QComboBox *combo, const QStringList &items, const QString &current );
    void setFontComboEntry( QComboBox *combo, const QString &item );

    MainWindowBase *m_mainWindow;
};

/*
 * vim:et
 */

#endif

