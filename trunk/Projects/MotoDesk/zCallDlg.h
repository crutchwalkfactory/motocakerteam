//
// C++ Interface: ZFileDlg
//
// Description: 
//
//
// Author: root <root@andLinux>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ZCALLDLG_H
#define ZCALLDLG_H

#include "GUI_Define.h"
#include <ZMultiLineEdit.h>
#include <ZMessageDlg.h>
#include <ZListBox.h>
#include <ZIconView.h>


#include <qtextstream.h>
#include <qobject.h>
#include <stdio.h>
#include <stdlib.h>
#include <qfile.h>
#include <qdir.h>
#include <ZConfig.h>
#include <ZApplication.h>
#include <ZMessageDlg.h>
#include <ZListBox.h>
#include <ZNoticeDlg.h>
#include <ZPopup.h>
#include <ZSingleSelectDlg.h>

#include <qmap.h>
#include <qlist.h>
#include <qstringlist.h>
#include <qdir.h>
#include <qfile.h>
#include <sys/vfs.h>


class zCalls : public ZPopup {
  Q_OBJECT

  public:
    zCalls(QString idioma); 
    ~zCalls();
	void insertText(QString numero);
	void CreateDialog(QString Idioma);
 
  public slots:   
    virtual void accept();
    virtual void reject();


  protected:
	virtual void keyPressEvent(QKeyEvent* k);

};


class zProfile : public ZPopup {
  Q_OBJECT

  public:
    zProfile(QString Idioma); 
    ~zProfile();
	void CreateDialog(QString Idioma);

  public slots:	
    virtual void accept();
    virtual void reject();
	void brClicked();   

  private:   
	ZListBox* listboxx2;

  protected:
	virtual void keyPressEvent(QKeyEvent* k);

};


class zLock : public ZPopup {
  Q_OBJECT

  public:
    zLock(QString Idioma); 
    ~zLock();

  public slots:	
	void unlock();   

  protected:
	virtual void keyPressEvent(QKeyEvent* k);

};


class myListBox : public ZListBox {
  Q_OBJECT

  public:
      myListBox( QString type, QWidget* parent=0, WFlags f = 0,
                  ZSkinService::WidgetClsID clsId = ZSkinService::clsZListBox1 );
	  virtual ~myListBox();

  protected:
	virtual void keyPressEvent(QKeyEvent* k);

};


class zMissed : public ZPopup {
  Q_OBJECT

  public:
    zMissed(QString Idioma, int misscall, int missmsg); 
    ~zMissed();

  public slots:	
    virtual void accept();
    virtual void reject();
	void CreateDialog();
	void brClicked();   
	void NotiNueva(const QCString &mensaje, const QByteArray &datos);

  private:   
	ZListBox* listboxx2;

  protected:
	virtual void keyPressEvent(QKeyEvent* k);

};

#endif


