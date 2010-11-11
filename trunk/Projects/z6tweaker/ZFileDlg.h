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
#include "ZFileBrowser.h"
#include <ZMultiLineEdit.h>
#include <ZMessageDlg.h>


#define Z6KEY_LEFT              0x1012
#define Z6KEY_UP                0x1013
#define Z6KEY_RIGHT             0x1014
#define Z6KEY_DOWN              0x1015
#define Z6KEY_CENTER            0x1004

class ZSelectFont : public MyBaseDlg {
  Q_OBJECT

  public:
    ZSelectFont(); 
    ~ZSelectFont();
    QString getFileName(const QString &AFileName);
	QString texto(const char*xString);    
	QString resultado() { return fileName; } 
    
  public slots:   
    void slot_cancel();
    void selectItem(int);
	void brClicked();   
    void CargarIdioma();

  private:
    QString fileName;
    QString basePath;      
    ZListBox* browser;
   
  protected:
    virtual void accept(); 
//     virtual void keyPressEvent(QKeyEvent* e);
};

class ZBootlogo : public MyBaseDlg {
  Q_OBJECT

  public:
    ZBootlogo(); 
    ~ZBootlogo();
    QString getFileName(const QString &AFileName);
	QString texto(const char*xString);    
	QString resultado() { return fileName; } 
    
  public slots:   
    void slot_cancel();
    void selectItem(int);
	void brClicked();   
    void CargarIdioma();
    
  private:
    QString fileName;
    QString basePath;      
    ZListBox* browser;
   
  protected:
    virtual void accept(); 
//     virtual void keyPressEvent(QKeyEvent* e);
};

class ZShortcut : public MyBaseDlg {
  Q_OBJECT

  public:
    ZShortcut(); 
    ~ZShortcut();
    QString getFileName(const QString &AFileName);
	QString texto(const char*xString);    
	QString resultado() { return fileName; } 
    
  public slots:   
    void slot_cancel();
    void selectItem(int);
	void brClicked();   
    void CargarIdioma();
    
  private:
    QString fileName;
    QString basePath;      
    ZListBox* browser;
   
  protected:
    virtual void accept(); 
//     virtual void keyPressEvent(QKeyEvent* e);
};

class ZVoiceRecord : public MyBaseDlg {
  Q_OBJECT

  public:
    ZVoiceRecord(); 
    ~ZVoiceRecord();
    QString getFileName(const QString &AFileName);
	QString texto(const char*xString);    
	QString resultado() { return fileName; } 
    
  public slots:   
    void slot_cancel();
    void selectItem(int);
	void brClicked();   
    void CargarIdioma();
    
  private:
    QString fileName;
    QString basePath;      
    ZListBox* browser;
   
  protected:
    virtual void accept(); 
//     virtual void keyPressEvent(QKeyEvent* e);
};

class ZSlidertones : public MyBaseDlg {
  Q_OBJECT

  public:
    ZSlidertones(); 
    ~ZSlidertones();
    QString getFileName(const QString &AFileName);
	QString texto(const char*xString);    
	QString resultado() { return fileName; } 
    
  public slots:   
    void slot_cancel();
    void selectItem(int);
	void brClicked();   
 	void CargarMenu();   
 	void Agregar();   
 	void Borrar();   
    void CargarIdioma();
   
  private:
    QString fileName;
    QString basePath;      
    ZListBox* browser;
   
  protected:
    virtual void accept(); 
//     virtual void keyPressEvent(QKeyEvent* e);
};

class ZWeblink : public MyBaseDlg {
  Q_OBJECT

  public:
    ZWeblink(); 
    ~ZWeblink();
    QString getFileName(const QString &AFileName);
	QString texto(const char*xString);    
	QString resultado() { return fileName; } 
    
  public slots:   
    void slot_cancel();
    void selectItem(int);
	void brClicked();   
 	void CargarMenu();   
    void CargarIdioma();
   
  private:
    QString fileName;
    QString basePath;      
    ZListBox* browser;
   
  protected:
    virtual void accept(); 
//     virtual void keyPressEvent(QKeyEvent* e);
};

#endif
