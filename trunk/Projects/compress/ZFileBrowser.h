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

#include <ZListBox.h>

class FileBrowser : public ZListBox
{
  Q_OBJECT
  public:
    FileBrowser(const QString &filter, QString arg1, QWidget* arg2, unsigned int arg3, ZSkinService::WidgetClsID arg4);
    virtual ~FileBrowser();
    
    void setDir(const QString &path);
    QString getFileName() { return fileName; }
    QString getFilePath() { return basePath; }    
    
  signals:
    void picked(const QString &fileName);
    void isFilePicked(bool);
        
  private slots:
    void itemHighlighted(int index);
    void itemSelected(int index);    
    void itemClicked(ZListBoxItem *);
        
  private:
    QString nameFilter;
    QString basePath;
    QString fileName;    
};

#endif
