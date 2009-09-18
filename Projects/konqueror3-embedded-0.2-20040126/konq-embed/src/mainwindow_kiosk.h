/* This file is part of the KDE project
   $Id: mainwindow_kiosk.h,v 1.4 2003/05/13 14:49:19 montanaro Exp $
*/

#ifndef __mainwindow_kiosk_h__
#define __mainwindow_kiosk_h__

#include <config.h>

#if !defined(KONQ_GUI_QPE) && !defined(KONQ_GUI_X11)

#include "mainwindowbase.h"

class MainWindowKiosk : public MainWindowBase
{
    Q_OBJECT
public:
    MainWindowKiosk();

protected slots:
    virtual void popupMenu( const QString &url, const QPoint & );

protected:
    virtual BrowserView *createNewViewInternal( const KURL &url, const KParts::URLArgs &args, bool withStatus = true );

    virtual void initGUI();
};

#endif

#endif // __mainwindow_kiosk_h__
