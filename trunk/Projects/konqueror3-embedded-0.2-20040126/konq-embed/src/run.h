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

#ifndef __run_h__
#define __run_h__

#include <khtml_run.h>

class View;

class Run : public KHTMLRun
{
public:
    Run( View *view, const KURL &url, const KParts::URLArgs &args );

    virtual void foundMimeType( const QString &mimeType );
protected:
    virtual void handleError( KIO::Job *job );

private:
    View *m_view;
};

#endif
