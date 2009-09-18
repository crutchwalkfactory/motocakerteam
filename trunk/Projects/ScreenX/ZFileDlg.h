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
#include "myam.h"


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

class ZMainMenu : public MyBaseDlg {
  Q_OBJECT

  public:
    ZMainMenu(QString location); 
    ~ZMainMenu();
    QString getFileName(const QString &AFileName);
	QString texto(const char*xString);    
	QString resultado() { return fileName; } 
    AM_RegistryManager* rman;
    ZListBox* zllb;
    ZIconView* illb;
 
  public slots:   
    void slot_cancel();
    void selectItem(int);
	void CrearLista();
	void appClicked();
	void VerLista();
    void CargarMenu(QString Carpeta);
    void CargarMenu2(QString Carpeta);

  
  private:
    QString fileName;
    QString basePath;      
   
  protected:
    virtual void accept(); 

};

class ZSettings : public MyBaseDlg {
  Q_OBJECT

  public:
    ZSettings(); 
    ~ZSettings();
    QString getFileName(const QString &AFileName);
	QString texto(const char*xString);    
	QString resultado() { return fileName; } 
    
  public slots:   
    void slot_cancel();
    void selectItem(int);
	void brClicked();   
 	void CargarBrowser();   
 	void CargarApps();   
  
  private:
    QString fileName;
    QString basePath;      
    ZListBox* browser;
   
  protected:
    virtual void accept(); 

};


class ZScreen : public MyBaseDlg {
  Q_OBJECT

  public:
    ZScreen(); 
    ~ZScreen();
    QString getFileName(const QString &AFileName);
	QString texto(const char*xString);    
	QString resultado() { return fileName; } 
    
  public slots:   
    void slot_cancel();
    void selectItem(int);
	void brClicked();   
    
  private:
    QString fileName;
    QString basePath;      
    ZListBox* browser;
   
  protected:
    virtual void accept(); 

};


#endif

