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
#include "resources.h"
#include <qdir.h>
#include <ezxres.h>

extern TextEditor *pTextEditor;

class ZListBox;

FileBrowser2::FileBrowser2 ( const QString &sFilter, bool onlyDirs, QString arg1, QWidget* arg2, unsigned int arg3, ZSkinService::WidgetClsID arg4 )        : ZListBox ( arg1, arg2, arg3, arg4 )
{
  sFileName = "";
  nameFilter = sFilter;
  setDir ( QDir::currentDirPath() );
  connect ( this, SIGNAL ( highlighted ( int ) ), this, SLOT ( itemHighlighted ( int ) ) );
  connect ( this, SIGNAL ( selected ( int ) ), this, SLOT ( itemSelected ( int ) ) );

  ProgramDir = getProgramDir ( QString ( qApp->argv() [0] ) );
  //CreateSoftKeys_FileDlg();
  OnlyDirs = onlyDirs;

}

FileBrowser2::~FileBrowser2()
{
}

void FileBrowser2::setDir ( const QString &sPath )
{
  RES_ICON_Reader iconReader;

  bool isBack = true;

  ZConfig ini(ProgramDir + "/settings.cfg", true);
  QString Hval = ini.readEntry(QString("FM"), QString("ShowHidden"), "");

  QString iconName = "";
  //iconName = te->ProgramDir + "img/folder.png";
  this->clear();
  QString nPath = RealName(sPath);
  int encontro = nPath.find( "//" , 0 );
  if ( encontro > -1 ) { nPath.remove( encontro,1 ); }

  qDebug ( "CARPETA: " + nPath );

  ZListBoxItem* Item = new ZListBoxItem ( this, QString ( "%I%M" ) );
  Item->appendSubItem ( 1, QString ( ".." ), false, NULL );
  this->insertItem ( Item, 0,true );
  int ItemSize = this->itemHeight(0)-2 ; if ( ItemSize>20 ) ItemSize=20;
  this->clear();

  QDir dir ( nPath, nameFilter );
  dir.setMatchAllDirs ( true );
  dir.setFilter ( QDir::Dirs | QDir::Hidden );
  if ( !dir.isReadable() )
    return;
  QStringList entries = dir.entryList();
  entries.sort();
  QStringList::ConstIterator it = entries.begin();
  QPixmap pixmap;
  while ( it != entries.end() ) {
    if ( *it != "." ) {

	  ZConfig fsini(ProgramDir + "/sfoldericons.cfg", true);	
	  ZConfig cini(ProgramDir + "/foldername.cfg", true);	

      if ( *it != ".." ) {
        isBack = isBack && true;
        QFileInfo *fInfo = new QFileInfo ( QString ( nPath + "/" + *it ) );
		
		QString MYSTUFF = QString ( nPath + "/" + *it );

		int i = MYSTUFF.findRev ( "/../" );
		if ( i>0 ) {
			QString mystuff2 = MYSTUFF; mystuff2.remove ( i, mystuff2.length() - i );
			int j = mystuff2.findRev ( "/" ); mystuff2.remove ( j, mystuff2.length() - j );
			mystuff2 = QString( mystuff2 + "/" + *it);
			MYSTUFF = mystuff2;
		}
		QString	mval;
		mval = fsini.readEntry(QString("FOLDERICONS"), QString(MYSTUFF), "");
		if ( mval == "" ) { mval="fm_folder_thb"; }
		mval.remove( mval.length() -4 ,4 ); mval = mval + "_small";
		pixmap=QPixmap( iconReader.getIcon(mval, false) );

      } else {
        isBack = isBack && false;
		pixmap=QPixmap( iconReader.getIcon("gen_back_to_arrw_thb", false) );
      }

	  if ( pixmap.height() > ItemSize ) { QImage image; image = pixmap.convertToImage(); 
	  image = image.smoothScale( ItemSize, ItemSize ); pixmap.convertFromImage(image); }  
      ZListBoxItem* Item = new ZListBoxItem ( this, QString ( "%I%M" ) );
        
      QFont f = this->font();
      f.setPixelSize(f.pixelSize() - 7);
      this->setFont(f);
      
      Item->setPixmap ( 0, pixmap );

	  QString MYSTUFF = QString ( nPath + "/" + *it );

	  QString mval = cini.readEntry(QString("FOLDERNAME"), MYSTUFF, "");
	  if ( mval != "" ) { Item->appendSubItem ( 1, mval, false, NULL ); }
	  else { Item->appendSubItem ( 1, *it, false, NULL ); }

	  if ( Hval == "1" ) { this->insertItem ( Item,-1,true ); }
	  else
	  {
		QString hidden = QString ( *it );
		if ( hidden[0] != QChar('.') ) { this->insertItem ( Item,-1,true ); }
		else { if ( hidden[1] == QChar('.') ) { this->insertItem ( Item,-1,true ); } }
	  }
    }
    ++it;
  }

  if ( ! OnlyDirs )  {

    dir.setFilter ( QDir::Files | QDir::Hidden );

    entries = dir.entryList();
    entries.sort();
    it = entries.begin();
    while ( it != entries.end() ) {
      if ( *it != "." ) {
		ZListBoxItem* Item = new ZListBoxItem ( this, QString ( "%I%M" ) );
        if ( *it != ".." ) {
			isBack = isBack && true;

			QFileInfo *fInfo = new QFileInfo ( nPath + "/" + *it );
			QString Eval = QString("%1").arg(*it);
			int i = Eval.findRev ( "." ); Eval.remove ( 0, i +1 );
			ZConfig ini(ProgramDir + "/fileicons.cfg", false);	
			ZConfig sini(ProgramDir + "/sfileicons.cfg", false);	

			QString	mval;
			mval = sini.readEntry(QString("FILES"), QString("%1").arg(Eval).lower(), "");
			if ( mval == "" ) { mval="msg_unsupport_type_thb"; }
			mval.remove( mval.length() -4 ,4 ); mval = mval + "_small";
			if ( mval=="msg_unsupport_type_small" ) mval="msg_unsupport_file_type_small";
			pixmap=QPixmap( iconReader.getIcon(mval, false) );

			if ( pixmap.height() > ItemSize ) { QImage image; image = pixmap.convertToImage(); 
			image = image.smoothScale( ItemSize, ItemSize ); pixmap.convertFromImage(image); }  

	        Item->setPixmap ( 0, pixmap );
	        Item->appendSubItem ( 1, *it, false, NULL );

			if ( Hval == "1" ) { this->insertItem ( Item,-1,true ); }
		}
		else
		{
		  QString hidden = QString ( *it );
		  if ( hidden[0] != QChar('.') ) { this->insertItem ( Item,-1,true ); }
		  else { if ( hidden[1] == QChar('.') ) { this->insertItem ( Item,-1,true ); } }
		}

      }
      ++it;
    }
  }
  sBasePath = dir.canonicalPath();
  if ( ( isBack ) && ( sBasePath != "/" ) && ( sBasePath.length() > 1 ) ) {
    ZListBoxItem* Item = new ZListBoxItem ( this, QString ( "%I%M" ) );
    QPixmap pixmap=QPixmap ( ProgramDir + IDB_IMAGE_BACK );
    Item->setPixmap ( 0, pixmap );
    Item->appendSubItem ( 1, QString ( ".." ), false, NULL );
    this->insertItem ( Item, 0,true );
    isBack = true;
  }
  this->enableMarquee(ZListBox::LISTITEM_REGION_C, true);
  setCurrentItem ( 0 );
}

void FileBrowser2::itemHighlighted ( int index )
{
  QString a = "";
  ZListBoxItem* listitem = this->item ( index );
  a = listitem->getSubItemText ( 1, 0, false );
  sBasePath = RealName(sBasePath);
  QString sPath = sBasePath + "/" + a;
  sPath = RealName(sPath);
//  qDebug ( "**** Highlighted...: " + a );
  if ( QFileInfo ( sPath ).isFile() ) {
    sFileName = sPath;
    //emit picked ( sPath );
    //emit isFilePicked ( TRUE );
    // laucnch app
    
  } else {
    //sFileName = "";
    sFileName = sPath;
	emit picked ( sPath );
    emit isFilePicked ( TRUE );
  }
}

// событие возникает при нажатии на центральную кнопку на элементе
void FileBrowser2::itemSelected ( int index )
{
  QString a = "";
  ZListBoxItem* listitem = this->item ( index );
  a = listitem->getSubItemText ( 1, 0, false );
  sBasePath = RealName(sBasePath);
  QString sPath = sBasePath + "/" + a;
  sPath = RealName(sPath);
  qDebug ( "**** Selected...: " + a );
 
  if ( a == ".." ) {
	int i = sPath.findRev ( "/.." );
	sPath.remove ( i, 3 );
	i = sPath.findRev ( "/" );
	sPath.remove ( i, sPath.length() - i );
	setDir ( sPath );
	emit isFilePicked ( FALSE );
	emit picked ( sPath );
  }
  else
  {
	  if ( QFileInfo ( sPath ).isDir() ) {
		this->clear();
		setDir ( sPath );
		emit isFilePicked ( FALSE );
		emit picked ( sPath );
		    
	  } else {
		// загрузить файл и скрыть окно выбора файлов
		if ( QFileInfo ( sPath ).isFile() ) {
		  sFileName = sPath;
		  emit isFilePicked ( TRUE );
		// laucnch app      
		}
	  }
  }
}

QString FileBrowser2::getProgramDir ( const QString &AFileName )
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( i+1, str.length() - i );
  return str;
}

bool FileBrowser2::makedir(const QString &ASource, const QString &AName)
{
  if (ASource.isEmpty()) return false;
  if (AName.isEmpty()) return false;
  QDir dir;
  dir.setPath(ASource);
  return dir.mkdir(AName, false)  ;

} 

void FileBrowser2::keyPressEvent ( QKeyEvent* e )
{
  // get visble lines
  int vLines = -1;
  for (int y=0; y<(count()-1); ++y){
    if (itemVisible(y)) { ++vLines; }
  }
  
  switch ( e->key() ) 
  {
    
    case Z6KEY_SIDE_UP:
    { // set top item
      int t = currentItem();
      if (t > (vLines - 1))   {
        this->setCurrentItem( t - vLines);
      } else { this->setCurrentItem( 0 ); }   
    break; }
    
    case Z6KEY_SIDE_DOWN:
    { //set bottom item
      int t = currentItem();
      if (t < ( count() - vLines - 1))   {
        this->setBottomItem( t + vLines);
        this->setCurrentItem( t + vLines);
      } else { this->setCurrentItem( count() - 1 ); }   

    break; }
    
    default:
      ZListBox::keyPressEvent ( e );
  }       
}

QString FileBrowser2::RealName(const QString &AFileName)
{
  QString str = AFileName;
  ZConfig cini(ProgramDir + "/foldername.cfg", true);	
  QString mval = cini.readEntry(QString("CUSTOMNAME"), str, "");
  if ( mval == "" ) { mval = str; }
  return mval;  

}




FileBrowser::FileBrowser ( const QString &sFilter, QString arg1, QWidget* arg2, unsigned int arg3, ZSkinService::WidgetClsID arg4)        : ZListBox ( arg1, arg2, arg3, arg4 )
{
  sFileName = "";
  nameFilter = sFilter;
  setDir ( QDir::currentDirPath() );
  connect ( this, SIGNAL ( highlighted ( int ) ), this, SLOT ( itemHighlighted ( int ) ) );
  connect ( this, SIGNAL ( selected ( int ) ), this, SLOT ( itemSelected ( int ) ) );
  
  //CreateSoftKeys_FileDlg();
}

FileBrowser::~FileBrowser()
{
}

void FileBrowser::setDir ( const QString &sPath )
{
  RES_ICON_Reader iconReader;
  
  bool isBack = true;
  
  QString iconName = "";
  //iconName = te->ProgramDir + "img/folder.png";
  this->clear();
  QDir dir ( sPath, nameFilter );
  dir.setMatchAllDirs ( true );
  dir.setFilter ( QDir::Dirs | QDir::Hidden );
  if ( !dir.isReadable() )
    return;
  QStringList entries = dir.entryList();
  entries.sort();
  QStringList::ConstIterator it = entries.begin();
  QPixmap pixmap;
  while ( it != entries.end() )
  {
    if ( *it != "." )
    {
      
      if (*it != "..")  
      { 
        isBack = isBack && true; 
        pixmap=QPixmap( pTextEditor->getProgramDir() + IDB_IMAGE_FOLDER) ;
      }  
      else  
      { 
        isBack = isBack && false;  
        pixmap=QPixmap( pTextEditor->getProgramDir() + IDB_IMAGE_BACK) ;
      }
      ZListBoxItem* Item = new ZListBoxItem ( this, QString ( "%I%M" ) );
      //QPixmap pixmap=QPixmap( pTextEditor->getProgramDir() + IDB_IMAGE_FOLDER) ;
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
      if (*it != "..")  
      { 
        isBack = isBack && true; 
        pixmap=QPixmap( pTextEditor->getProgramDir() + IDB_IMAGE_FILE) ;
      }  
      else  
      { 
        isBack = isBack && false;  
        pixmap=QPixmap( pTextEditor->getProgramDir() + IDB_IMAGE_BACK) ;
      }
      ZListBoxItem* Item = new ZListBoxItem ( this, QString ( "%I%M" ) );
      //QPixmap pixmap=QPixmap ( pTextEditor->getProgramDir() + IDB_IMAGE_FILE); 
      Item->setPixmap ( 0, pixmap);
      Item->appendSubItem ( 1, *it, false, NULL );
      this->insertItem ( Item,-1,true );
    }
    ++it;
  }
  sBasePath = dir.canonicalPath();
  if ((isBack) && (sBasePath != "/") && (sBasePath.length() > 1)) 
  {
    ZListBoxItem* Item = new ZListBoxItem ( this, QString ( "%I%M" ) );
    QPixmap pixmap=QPixmap ( pTextEditor->getProgramDir() + IDB_IMAGE_BACK);
    Item->setPixmap ( 0, pixmap  );
    Item->appendSubItem ( 1, QString(".."), false, NULL );
    this->insertItem ( Item, 0,true );
    isBack = true;
  }  
  setCurrentItem(0);
}

void FileBrowser::itemHighlighted(int index)
{
  qDebug("**** Highlighted...");
  
  QString a = "";
  ZListBoxItem* listitem = this->item(index);
  a = listitem->getSubItemText(1, 0, true);
  QString sPath = sBasePath + "/" + a;
  if (QFileInfo(sPath).isFile()) {
    sFileName = sPath;
    emit picked(sPath);
    emit isFilePicked(TRUE);
  } else {
    sFileName = "";
    emit isFilePicked(FALSE);
  }
}

// событие возникает при нажатии на центральную кнопку на элементе
void FileBrowser::itemSelected(int index)
{
  qDebug("**** Selected...");
  QString a = "";
  ZListBoxItem* listitem = this->item(index);
  a = listitem->getSubItemText(1, 0, false);
  QString sPath = sBasePath + "/" + a;  
  if (QFileInfo(sPath).isDir())
  {
    this->clear();
    setDir(sPath);
  }     
  else 
  {
   // загрузить файл и скрыть окно выбора файлов
    if (QFileInfo(sPath).isFile()) 
    {
      sFileName = sPath; 
    }
  }      
}

