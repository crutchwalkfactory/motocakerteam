//
// C++ Interface: ZFileBrowser
//
// Description: 
//
//
// Author: root <root@andLinux>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ZFILEBROWSER_H
#define ZFILEBROWSER_H
#include <ZApplication.h>
#include <ZListBox.h>
#include <ZAppInfoArea.h>
#include <ZSingleCaptureDlg.h>
#include <qdir.h>

class FileBrowser : public ZListBox
{
  Q_OBJECT
  public:
    FileBrowser(const QString &sFilter, bool onlyDirs, QString arg1, QWidget* arg2, unsigned int arg3, ZSkinService::WidgetClsID arg4 = ZSkinService::clsZListBox1);
    virtual ~FileBrowser();
    
    void setDir(const QString &sPath);
    QString getFileName() { return sFileName; }
    QString getFilePath() { return sBasePath; }    
    QString getProgramDir(const QString &AFileName);
    QString ProgramDir; 
    bool makedir(const QString &ASource, const QString &AName);
	QString RealName(const QString &AFileName);
    
  signals:
    void picked(const QString &sFileName);
    void isFilePicked(bool);
       
    
  private slots:
    void itemHighlighted(int index);
    void itemSelected(int index);    
        
  private:
    QString nameFilter;
    QString sBasePath;
    QString sFileName;  
    
    bool OnlyDirs;

  protected:
    virtual void keyPressEvent(QKeyEvent* e);     
};

#endif
