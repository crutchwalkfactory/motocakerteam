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
#ifndef ZFILEDLG_H
#define ZFILEDLG_H

#include "GUI_Define.h"
#include "BaseDlg.h"
#include <ZMultiLineEdit.h>
#include <ZMessageDlg.h>
#include <ZListBox.h>
#include <ZIconView.h>
#include <ZImage.h>
#include "myam.h"
#include "zCallDlg.h"

#include <qtextstream.h>
#include <qobject.h>
#include <stdio.h>
#include <stdlib.h>
#include <qfile.h>
#include <qdir.h>
#include <ZConfig.h>
#include <ZApplication.h>
#include <ZMessageDlg.h>
#include <ZNoticeDlg.h>
#include <ZSingleSelectDlg.h>
#include <ZOptionsMenu.h>

#include <qmap.h>
#include <qlist.h>
#include <qstringlist.h>
#include <qdir.h>
#include <qfile.h>
#include <sys/vfs.h>


#define Z6KEY_LEFT              0x1012
#define Z6KEY_UP                0x1013
#define Z6KEY_RIGHT             0x1014
#define Z6KEY_DOWN              0x1015
#define Z6KEY_CENTER            0x1004


class ZSettings : public MyBaseDlg {
  Q_OBJECT

  public:
    ZSettings(); 
    ~ZSettings();
    QString getFileName(const QString &AFileName);
	QString resultado() { return fileName; } 
	int ordenando;   

  public slots:   
    void slot_cancel();
	void brClicked();   
 	void CargarBrowser();   
 	void CargarApps();
	void CargarSoftKeys();  
	void AgregarApp();  
	void BorrarApp();  
	void Reordenar();

  private:
    QString fileName;
    QString basePath;      
    myListBox* browser;
    ZSoftKey *softKey;
    ZOptionsMenu * menu;
	QPixmap tPix;
   
  protected:
	virtual void keyPressEvent(QKeyEvent* k);
    virtual void accept(); 

};


class ZScreen : public MyBaseDlg {
  Q_OBJECT

  public:
    ZScreen(); 
    ~ZScreen();
    QString getFileName(const QString &AFileName);
	QString resultado() { return fileName; } 
    
  public slots:   
 	void CargarBrowser();   
    void slot_cancel();
	void brClicked();   
    
  private:
    QString fileName;
    QString basePath;      
    ZListBox* browser;
   
  protected:
	virtual void keyPressEvent(QKeyEvent* k);
    virtual void accept(); 

};

class ZWall : public MyBaseDlg {
  Q_OBJECT

  public:
    ZWall(); 
    ~ZWall();
    QString getFileName(const QString &AFileName);

  public slots:   
    void slot_cancel();
	void brClicked();   
	void CargarItems();
    
  private:
    QString fileName;
    QString basePath;
    ZListBox* browser;
   
  protected:
	virtual void keyPressEvent(QKeyEvent* k);
    virtual void accept(); 

};

class ZBrowser : public MyBaseDlg
{
    Q_OBJECT
  public:
    ZBrowser(QString location);
    ~ZBrowser();
	QString sPath, onlydirs;

public slots:
	void CargarBrowser(QString direccion);
	void brClicked();   
    void slot_cancel();

  private:
	ZListBox* browser;

  protected:
	virtual void keyPressEvent(QKeyEvent* k);
    virtual void accept(); 

};


class ZBrowser2 : public MyBaseDlg
{
    Q_OBJECT
  public:
    ZBrowser2(QString location);
    ~ZBrowser2();
	QString sPath, onlydirs;

public slots:
	void CargarBrowser(QString direccion);
	void brClicked();   
    void slot_cancel();
    void selected();

  private:
	ZListBox* browser;

  protected:
	virtual void keyPressEvent(QKeyEvent* k);
    virtual void accept(); 

};

class ZBrowser3 : public MyBaseDlg
{
    Q_OBJECT
  public:
    ZBrowser3(QString location);
    ~ZBrowser3();
	QString sPath, onlydirs;

public slots:
	void CargarBrowser(QString direccion);
	void brClicked();   
    void slot_cancel();
    void selected();

  private:
	ZListBox* browser;

  protected:
	virtual void keyPressEvent(QKeyEvent* k);
    virtual void accept(); 

};

#endif

