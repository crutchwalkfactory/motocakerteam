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
};

class ZFileSaveDialog : public MyBaseDlg {
  Q_OBJECT

  public:
    ZFileSaveDialog(); 
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

#endif