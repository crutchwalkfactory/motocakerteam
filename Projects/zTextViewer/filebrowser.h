#ifndef FILEBROWSER_H
#define FILEBROWSER_H

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

#endif // FILEBROWSER_H
