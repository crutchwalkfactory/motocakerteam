//
// C++ Implementation: ZFileBrowser
//
// Description:
//
//
// Author: root <root@andLinux>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ZFileBrowser.h"
#include <qdir.h>
#include <ezxres.h>

QString ProgDir;

FileBrowser::FileBrowser ( const QString &filter, QString arg1, QWidget* arg2, unsigned int arg3, ZSkinService::WidgetClsID arg4 )        : ZListBox ( arg1, arg2, arg3, arg4 )
{
  fileName = "";
  nameFilter = filter;
  setDir ( QDir::currentDirPath() );
  connect ( this, SIGNAL ( highlighted ( int ) ), this, SLOT ( itemHighlighted ( int ) ) );
  connect ( this, SIGNAL ( selected ( int ) ), this, SLOT ( itemSelected ( int ) ) );
  connect ( this, SIGNAL ( clicked ( ZListBoxItem * ) ), this, SLOT ( itemClicked ( ZListBoxItem * ) ) );

  ProgDir = QString ( qApp->argv() [0] ) ;
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );


}

FileBrowser::~FileBrowser()
{
}

void FileBrowser::setDir ( const QString &path )
{
  RES_ICON_Reader iconReader;
  
  bool isBack = true;
  
  QString iconName = "";
  //iconName = te->ProgramDir + "img/folder.png";
  this->clear();
  QDir dir ( path, nameFilter );
  dir.setMatchAllDirs ( true );
  dir.setFilter ( QDir::Dirs | QDir::Hidden );
  if ( !dir.isReadable() )
    return;
  QStringList entries = dir.entryList();
  entries.sort();
  QStringList::ConstIterator it = entries.begin();
  while ( it != entries.end() )
  {
    if ( *it != "." )
    {
      if (*it != "..") {isBack = isBack && true;} else {isBack = isBack && false;}
      ZListBoxItem* Item = new ZListBoxItem ( this, QString ( "%I%M" ) );
	  QPixmap pixmap; pixmap.load(ProgDir + "/images/folder.png");
      Item->setPixmap ( 0, pixmap  );
      Item->appendSubItem ( 1, *it, false, NULL );
      this->insertItem ( Item,-1,true );
    }
    ++it;
  }
  dir.setFilter ( QDir::Files | QDir::Hidden );
  entries = dir.entryList();
  entries.sort();
  it = entries.begin();
  while ( it != entries.end() )
  {
    if ( *it != "." )
    {
      if (*it != "..") {isBack = isBack && true;} else {isBack = isBack && false;}
      ZListBoxItem* Item = new ZListBoxItem ( this, QString ( "%I%M" ) );
	  QPixmap pixmap; pixmap.load(ProgDir + "/images/file.png");
      Item->setPixmap ( 0, pixmap);
      Item->appendSubItem ( 1, *it, false, NULL );
      this->insertItem ( Item,-1,true );
    }
    ++it;
  }
  basePath = dir.canonicalPath();
  if ((isBack) && (basePath != "/") && (basePath.length() > 1)) 
  {
    ZListBoxItem* Item = new ZListBoxItem ( this, QString ( "%I%M" ) );
	QPixmap pixmap; pixmap.load(ProgDir + "/images/back.png");
    Item->setPixmap ( 0, pixmap  );
    Item->appendSubItem ( 1, QString(".."), false, NULL );
    this->insertItem ( Item, 0,true );
    isBack = true;
  }  
}

void FileBrowser::itemHighlighted(int index)
{
  QString a = "";
  ZListBoxItem* listitem = this->item(index);
  a = listitem->getSubItemText(1, 0, true);
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

void FileBrowser::itemSelected(int index)
{
  QString a = "";
  ZListBoxItem* listitem = this->item(index);
  a = listitem->getSubItemText(1, 0, false);
  QString path = basePath + "/" + a;  
  if (QFileInfo(path).isDir())
  {
    this->clear();
    setDir(path);
  }     
  else 
  {
    if (QFileInfo(path).isFile()) 
    {
      //te->tryLoadFile(path);
     fileName = path; 
    }
  }      
}

void FileBrowser::itemClicked(ZListBoxItem *item)
{
  QString a = "";
  a = item->getSubItemText(1, 0, false);
  QString path = basePath + "/" + a;    
  if (QFileInfo(path).isDir())
    setDir(path);
}
