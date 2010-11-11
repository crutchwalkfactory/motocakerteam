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

#include "BaseDlg.h"
#include "ZFileBrowser.h"
#include <ZMultiLineEdit.h>
#include <ZMessageDlg.h>
#include <ZPressButton.h>
#include "resources.h"

class FileBrowser;
class FileBrowser2;

class ZFileOpenDialog : public MyBaseDlg {
  Q_OBJECT

  public:
    ZFileOpenDialog(); 
    ~ZFileOpenDialog();
    //int exec();
    //int result() const ;  
    QString getFileName() { return fileName; } 
    QString getFilePath() { return basePath; } 
  private:
    QString fileName;
    QString basePath;      
    FileBrowser* fb;
    
  protected:
    virtual void accept(); 
    virtual void keyPressEvent(QKeyEvent* e);
};

class ZFileSaveDialog : public MyBaseDlg {
  Q_OBJECT

  public:
    ZFileSaveDialog(const QString &curPath = "/", const QString &originName = ""); 
    ~ZFileSaveDialog();
    //int exec();
    //int result() const ; 
    QString getFileName() { return fileName; } 
    QString getFilePath() { return basePath; }
  private:
    ZMultiLineEdit *zmle;
    QString fileName;  
    QString basePath;   
    FileBrowser* fb;
           
  protected:
    virtual void keyPressEvent( QKeyEvent* e);    
    virtual void accept();  

};

class myButton : public ZPressButton {
  Q_OBJECT

  public:
    myButton(QWidget*, char const*, ZSkinService::WidgetClsID = ZSkinService::clsZPressButton);
    virtual ~myButton();

  protected:
	virtual void keyPressEvent(QKeyEvent* k);

};

class ZSearch : public MyBaseDlg {
  Q_OBJECT

  public:
    ZSearch(const QString &curPath = "/", const QString &originName = ""); 
    ~ZSearch();
    QString getFileName() { return fileName; } 
    QString getFilePath() { return basePath; } 
    
  public slots:   
    void slot_cancel();
    void slot_search();
    
  private:
    ZListBox* browser;
	myButton* buttonDir; 
	ZLineEdit* lineEdit;  
    QString fileName;  
    QString basePath;   

  protected:
	//virtual void keyPressEvent(QKeyEvent* e);
   
};


class ZFolderSelect2 : public MyBaseDlg {
  Q_OBJECT

  public:
    ZFolderSelect2(bool onlyDirs, QString path = "/"); 
    ~ZFolderSelect2();
    //int exec();
    //int result() const ;  
    QString getFileName() { return fileName; } 
    QString getFilePath() { return basePath; } 
    
  public slots:   
    void slot_cancel();
    void slot_paste();
    void selectItem(int);
    
  private:
    QString fileName;
    QString basePath;      
    FileBrowser2* fb;
    
  protected:
    virtual void accept(); 
//     virtual void keyPressEvent(QKeyEvent* e);
};




#endif
