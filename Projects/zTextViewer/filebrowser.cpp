#include "filebrowser.h"
#include "texteditor.h"

#include <ZMessageDlg.h>

//#include <zglobal.h>
//#include <ezxres.h>

#include <qdir.h>
#include <qpixmap.h>

extern TextEditor* te;

FileBrowser::FileBrowser(const QString &filter, QString arg1, QWidget* arg2, unsigned int arg3, ZSkinService::WidgetClsID arg4)        : ZListBox(arg1, arg2, arg3, arg4)
{
  fileName = "";
  nameFilter = filter;
  setDir(QDir::currentDirPath());
  connect(this, SIGNAL(highlighted(int)), this, SLOT(itemHighlighted(int)));
  connect(this, SIGNAL(selected(int)), this, SLOT(itemSelected(int)));
  connect(this, SIGNAL(clicked(ZListBoxItem *)), this, SLOT(itemClicked(ZListBoxItem *)));
}
    
FileBrowser::~FileBrowser()
{
}

void FileBrowser::setDir(const QString &path)
{
  //RES_ICON_Reader ir;
  this->clear();
    
  QDir dir(path, nameFilter);    
  dir.setMatchAllDirs(true);
    
  dir.setFilter(QDir::Dirs | QDir::Hidden);
    
  if (!dir.isReadable())
    return;
  
  
  QStringList entries = dir.entryList();
  QStringList::ConstIterator it = entries.begin();
  while (it != entries.end()) {
    if (*it != ".") {
      ZListBoxItem* Item = new ZListBoxItem ( this, QString ( "%I%M" ) );
      Item->setPixmap ( 0, QPixmap ( te->ProgramDir + "img/folder.png") );
      Item->appendSubItem ( 1, *it, false, NULL);
      this->insertItem ( Item,-1,true );
    }
    ++it;
  }

  dir.setFilter(QDir::Files | QDir::Hidden);    
  entries = dir.entryList();
  it = entries.begin();
  while (it != entries.end()) {
    if (*it != ".") {
      ZListBoxItem* Item = new ZListBoxItem ( this, QString ( "%I%M" ) );
      Item->setPixmap ( 0, QPixmap ( te->ProgramDir + "img/file.png") );
      Item->appendSubItem ( 1, *it, false, NULL);
      this->insertItem ( Item,-1,true );
    }
    ++it;
  }

  basePath = dir.canonicalPath();
}

void FileBrowser::itemHighlighted(int index)
{
  QString a = "";
  //ZListItem* listitem = this->item(index);
  ZListBoxItem* listitem = this->item(index);
  a = listitem->getSubItemText(1, 0, true);
  //a = listitem->current();
  QString path = basePath + "/" + a;
  if (QFileInfo(path).isFile()) {
    fileName = path;
    emit picked(path);
    emit isFilePicked(TRUE);
  } else {
    fileName = "";
    emit isFilePicked(FALSE);
  }
 
}

// событие возникает при нажатии на центральную кнопку на элементе
void FileBrowser::itemSelected(int index)
{
  QString a = "";
  ZListBoxItem* listitem = this->item(index);
  a = listitem->getSubItemText(1, 0, false);
  QString path = basePath + "/" + a;  
 // QString path = basePath + "/" + text(index);
  if (QFileInfo(path).isDir())
  {
    this->clear();
    setDir(path);
  }     
  else 
  {
   // загрузить файл и скрыть окно выбора файлов
    if (QFileInfo(path).isFile()) 
    {
      te->tryLoadFile(path);
      
      
    }
  }      

}
    
    
void FileBrowser::itemClicked(ZListBoxItem *item)
{
//  QString path = basePath + "/" + text(index(item));
  QString a = "";
  a = item->getSubItemText(1, 0, false);
  QString path = basePath + "/" + a;    
  if (QFileInfo(path).isDir())
    setDir(path);
  
}


