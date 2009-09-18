/*  This file is part of the KDE project
    Copyright (C) 2000,2001 Simon Hausmann <hausmann@kde.org>

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

// Moved to its own file by Luciano Montanaro, because some of the frontends do
// not need it.

#ifndef __popupaction_h__
#define __popupaction_h__

// destruction and removeFrom code I deliberated left out as it's not needed
// here and just costs size :)
class PopupAction : public QAction
{
    Q_OBJECT
public:
    PopupAction( QPopupMenu *popup, const QString &text, const QIconSet &iconSet,
                 const QString &menuText, int accel, QObject *parent, const char *name = 0,
                 bool toggle = false );

    virtual bool addTo( QWidget *widget );

public slots:
    virtual void setEnabled( bool enable );

protected:
    virtual bool eventFilter( QObject *o, QEvent *e );

private:
    QPopupMenu *m_popup;
    QToolButton *m_toolButton; // ### let's save memory and assume we plug the action only once anyway
};

#endif
