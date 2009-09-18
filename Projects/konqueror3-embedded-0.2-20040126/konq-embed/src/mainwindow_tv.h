/* This file is part of the KDE project
   $Id: mainwindow_tv.h,v 1.1 2003/11/25 15:24:05 montanaro Exp $
*/

#ifndef __mainwindow_tv_h__
#define __mainwindow_tv_h__

#include <config.h>

#if defined(KONQ_GUI_TV)

#include "mainwindowbase.h"

class QDialog;
class QHBox;
class QLabel;

class StatusDialog : public QDialog
{
    Q_OBJECT
public:
    StatusDialog(QWidget *parent = 0);
    ~StatusDialog();
    
    void setMessage(const QString &msg);
    void enterEvent(QEvent *);
private:
    QHBox *box;
    QLabel *label;
};

class MainWindowTv : public MainWindowBase
{
    Q_OBJECT
public:
    MainWindowTv();

protected slots:
    virtual void popupMenu( const QString &url, const QPoint & );

    virtual void statusJustText( const QString &msg, bool visible );

    virtual void statusMessage( const QString &msg, bool visible );

protected:
    virtual BrowserView *createNewViewInternal( const KURL &url, 
		    const KParts::URLArgs &args, bool withStatus = true );

    virtual void initGUI();
private:
    StatusDialog *statusDialog;
};

#endif

#endif // __mainwindow_tv_h__
