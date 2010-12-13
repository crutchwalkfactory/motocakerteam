#include "z6manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ZSoftKey.h>
#include <ezxres.h>
#include <ZListBox.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <qpixmap.h>
#include <ZApplication.h>
#include <ZNoticeDlg.h>
#include <ZMessageDlg.h>
#include <ZOptionsMenu.h>



using namespace std;

QString sPath, sPath2;
QString Idioma;
QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
RES_ICON_Reader iconReader;


Z6Man::Z6Man ( int argc, char **argv, QWidget* parent, const char* name, WFlags fl )
    : ZKbMainWidget ( ZHeader::FULL_TYPE, NULL, "ZMainWidget", 0 )
{

  QString val;
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = "ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/en-us";

  CreateWindow ( argc, argv, parent );
  ( ( ZApplication* ) qApp )->showMainWidget ( this );

}


void Z6Man::CreateWindow ( int argc, char **argv, QWidget* parent )
{
  setMainWidgetTitle ( "Hide" );

  tabWidget = new ZNavTabWidget(ZNavTabWidget::SEPARATE, false, false, this);

  browser = new ZListBox ( QString ( "%I%M%I" ), this, 0);
  browser->setFixedWidth ( 240 );
  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));

  browser2 = new ZListBox ( QString ( "%I%M%I" ), this, 0);
  browser2->setFixedWidth ( 240 );
  connect(browser2, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked2()));

  CargarBrowser( "/ezxlocal/download/mystuff" );
  CargarBrowser2( "/mmc/mmca1" );

  QPixmap p1 = QPixmap( iconReader.getIcon("fm_phone_mem_small", false) );
  tabWidget->addTab(browser, QIconSet(p1), "");
  p1 = QPixmap( iconReader.getIcon("fm_memory_card_small", false) );
  tabWidget->addTab(browser2, QIconSet(p1), "");


  setContentWidget ( tabWidget );

  system(QString("echo \"PROBANDO\""));
  softKey = new ZSoftKey ( NULL , this , this );
  system(QString("echo \"PROBANDO\""));
  Create_SoftKey();
  setSoftKey ( softKey );

}

Z6Man::~Z6Man()
{
}

QString Z6Man::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("SETTINGS"), QString("UNICODE"), "");
	if ( IDval == "1" ) { return txtcodec->toUnicode(xString); }
	else { return xString; }
}

void Z6Man::brClicked()
{
  QString app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true );
  
  if ( app == ".." ) { 
	QString nPath = sPath;
	int i = nPath.findRev ( "/" );
	nPath.remove ( i, nPath.length() - i );
	CargarBrowser( nPath );
  }
  else 
  {
    int i = sPath.findRev ( "//" ); sPath.remove ( i, 1 );
	QString fName = sPath + "/" + app;
	if ( QFileInfo(fName).isDir() ) { CargarBrowser( fName ); return; }
  }
}

void Z6Man::brClicked2()
{
  QString app = browser2->item ( browser2->currentItem() )->getSubItemText ( 1, 0, true );
  
  if ( app == ".." ) { 
	QString nPath2 = sPath2;
	int i = nPath2.findRev ( "/" );
	nPath2.remove ( i, nPath2.length() - i );
	CargarBrowser2( nPath2 );
  }
  else 
  {
    int i = sPath2.findRev ( "//" ); sPath2.remove ( i, 1 );
	QString fName = sPath2 + "/" + app;
	if ( QFileInfo(fName).isDir() ) { CargarBrowser2( fName ); return; }
  }
}


void Z6Man::selected()
{
	int tabx = tabWidget->currentPageIndex();
	if ( tabx == 0 ) {
	  int index = browser->currentItem();
	  QString app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true );
	  int i = sPath.findRev ( "//" ); sPath.remove ( i, 1 );
	  QString fName = sPath + "/" + app;
	  QString newName = app;
	  if ( newName[0] == QChar('.') ) { newName.remove(0,1); }
	  else { newName = QChar('.') + app; }
	  QString fName2 = sPath + "/" + newName;
	  system(QString("echo \"%1  -->  %2\"").arg(fName).arg(fName2));
	  //QDir d; d.rename(fName, newName, true);
	  system(QString("mv -f \"%1\" \"%2\"").arg(fName).arg(fName2));
	  CargarBrowser(sPath);
	  //browser->setCurrentItem(index,true);
	}
	if ( tabx == 1 ) {
	  int index = browser2->currentItem();
	  QString app = browser2->item ( browser2->currentItem() )->getSubItemText ( 1, 0, true );
	  int i = sPath2.findRev ( "//" ); sPath2.remove ( i, 1 );
	  QString fName = sPath2 + "/" + app;
	  QString newName = app;
	  if ( newName[0] == QChar('.') ) { newName.remove(0,1); }
	  else { newName = QChar('.') + app; }
	  QString fName2 = sPath2 + "/" + newName;
	  system(QString("echo \"%1  -->  %2\"").arg(fName).arg(fName2));
	  //QDir d; d.rename(fName, newName, true);
	  system(QString("mv -f \"%1\" \"%2\"").arg(fName).arg(fName2));
	  CargarBrowser2(sPath2);
	  //browser2->setCurrentItem(index,true);
	}


}

void Z6Man::Create_SoftKey()
{
  QString IDval; QString IDval2; ZConfig IDini(Idioma, false);
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_SELECT"), "");
  softKey->setText ( ZSoftKey::LEFT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_EXIT"), "");
  softKey->setText ( ZSoftKey::RIGHT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( selected() ) );      
  softKey->setClickedSlot ( ZSoftKey::RIGHT, qApp, SLOT ( quit() ) );      
}

void Z6Man::CargarBrowser(QString direccion)
{

  browser->clear();
  QDir dir ( direccion, "*" );
  sPath = direccion;
  dir.setMatchAllDirs ( true );
  dir.setFilter ( QDir::Dirs | QDir::Hidden );
  if ( !dir.isReadable() )
	return;
  QStringList entries = dir.entryList();
  entries.sort();
  QStringList::ConstIterator it = entries.begin();

  QPixmap pixmap;
  QPixmap* p1;
  if ( (sPath != "") && (sPath != "/") && (sPath != "//") && (sPath != "/ezxlocal/download/mystuff") )
  {
	pixmap=QPixmap( iconReader.getIcon("gen_back_to_arrw_thb", false) );
	if ( pixmap.height() > 19 ) { QImage image; image = pixmap.convertToImage(); 
	image = image.smoothScale( 19, 19 ); pixmap.convertFromImage(image); }  
	ZListBoxItem* Item;
	Item = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	Item->setPixmap ( 0, pixmap ); Item->appendSubItem ( 1, "..", false, NULL );
	browser->insertItem ( Item,-1,true );
  }

  while ( it != entries.end() ) {
	if ( ( *it != "." ) && ( *it != ".." ) )
	{
		pixmap = QPixmap( iconReader.getIcon("fm_folder_small", false) );
		if ( pixmap.height() > 19 ) { QImage image; image = pixmap.convertToImage(); 
		image = image.smoothScale( 19, 19 ); pixmap.convertFromImage(image); }  

		ZListBoxItem* Item;
		Item = new ZListBoxItem ( browser, QString ( "%I%M%I" ) );
	    Item->setPixmap ( 0, pixmap ); QString temp = *it;
		if ( temp[0] == QChar('.') ) { 
			p1 = new QPixmap("/ezxlocal/LinXtend/usr/bin/z6manager/images/locked.png");
			Item->appendSubItem ( 1, *it, false, p1 ); browser->insertItem ( Item,-1,true );
		} else {
			p1 = new QPixmap("/ezxlocal/LinXtend/usr/bin/z6manager/images/null.png");
			Item->appendSubItem ( 1, *it, false, NULL ); browser->insertItem ( Item,-1,true );
		}
	}
	++it;
  }

}

void Z6Man::CargarBrowser2(QString direccion)
{

  browser2->clear();
  QDir dir ( direccion, "*" );
  sPath2 = direccion;
  dir.setMatchAllDirs ( true );
  dir.setFilter ( QDir::Dirs | QDir::Hidden );
  if ( !dir.isReadable() )
	return;
  QStringList entries = dir.entryList();
  entries.sort();
  QStringList::ConstIterator it = entries.begin();

  QPixmap pixmap;
  QPixmap* p1;
  if ( (sPath2 != "") && (sPath2 != "/") && (sPath2 != "//") && (sPath2 != "/mmc/mmca1") )
  {
	pixmap=QPixmap( iconReader.getIcon("gen_back_to_arrw_thb", false) );
	if ( pixmap.height() > 19 ) { QImage image; image = pixmap.convertToImage(); 
	image = image.smoothScale( 19, 19 ); pixmap.convertFromImage(image); }  
	ZListBoxItem* Item;
	Item = new ZListBoxItem ( browser2, QString ( "%I%M" ) );
	Item->setPixmap ( 0, pixmap ); Item->appendSubItem ( 1, "..", false, NULL );
	browser2->insertItem ( Item,-1,true );
  }

  while ( it != entries.end() ) {
	if ( ( *it != "." ) && ( *it != ".." ) )
	{
		pixmap=QPixmap( iconReader.getIcon("fm_folder_small", false) );
		if ( pixmap.height() > 19 ) { QImage image; image = pixmap.convertToImage(); 
		image = image.smoothScale( 19, 19 ); pixmap.convertFromImage(image); }  

		ZListBoxItem* Item;
		Item = new ZListBoxItem ( browser2, QString ( "%I%M%I" ) );
	    Item->setPixmap ( 0, pixmap ); QString temp = *it;
		if ( temp[0] == QChar('.') ) { 
			p1 = new QPixmap("/ezxlocal/LinXtend/usr/bin/z6manager/images/locked.png");
			Item->appendSubItem ( 1, *it, false, p1 ); browser2->insertItem ( Item,-1,true );
		} else {
			p1 = new QPixmap("/ezxlocal/LinXtend/usr/bin/z6manager/images/null.png");
			Item->appendSubItem ( 1, *it, false, NULL ); browser2->insertItem ( Item,-1,true );
		}
	}
	++it;
  }

}

