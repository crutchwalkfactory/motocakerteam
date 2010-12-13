//
// C++ Implementation: ZFileDlg
//
// Description: 
//
//
// Author: root <root@andLinux>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "ZFileDlg.h"

#include <qpixmap.h>
#include <qlabel.h>
#include <ZApplication.h>
#include <ZSoftKey.h>
#include "ZConfig.h"
#include <string.h>
#include <ZScrollView.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <ezxres.h>

RES_ICON_Reader iconReader2;
QString Idioma2;
QString sPath;

//************************************************************************************************
//************************************** class ZFileOpenDialog ***********************************
//************************************************************************************************
ZFileOpenDialog::ZFileOpenDialog()
  :MyBaseDlg()
{
  setMainWidgetTitle ( "Cron" );

  browser = new ZListBox ( QString ( "%C18%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );
  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));

  sPath = "/";
  CargarBrowser( sPath );

  setContentWidget ( browser );
  
  ZSoftKey *softKey = new ZSoftKey ( NULL, this, this );

  QString ProgDir = QString ( qApp->argv() [0] ) ;
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  QString val; 
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma2 = "ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma2).isFile() ) Idioma2 = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/en-us";

  ZConfig IDini(Idioma2, false);
  QString IDval = IDini.readEntry(QString("COMMON"), QString("MENU_SELECT"), "");
  softKey->setText ( ZSoftKey::LEFT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_CANCEL"), "");
  softKey->setText ( ZSoftKey::RIGHT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( reject() ) );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( accept() ) );
  setCSTWidget ( softKey );
}

ZFileOpenDialog::~ZFileOpenDialog()
{

}

void ZFileOpenDialog::accept()
{
  fileName = sPath + "/" + browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true );
  basePath = sPath + "/" + browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true );
  MyBaseDlg::accept();
}

void ZFileOpenDialog::brClicked()
{
  QString app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true );
  
  if ( app == ".." ) { 
	QString nPath = sPath;
	int i = nPath.findRev ( "/" );
	nPath.remove ( i, nPath.length() - i );
	i = nPath.findRev ( "//" );
	nPath.remove ( i, 1 );
	CargarBrowser( nPath );
  }
  else
  {
	QString fName = sPath + "/" + app;
	int i = fName.findRev ( "//" );
	fName.remove ( i, 1 );
	if ( QFileInfo(fName).isDir() ) { CargarBrowser( fName ); }
	else { accept(); }
  }
}

void ZFileOpenDialog::CargarBrowser(QString direccion)
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
  if ( (sPath != "") && (sPath != "/") && (sPath != "//") )
  {
	pixmap=QPixmap( iconReader2.getIcon("gen_back_to_arrw_thb", false) );
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
		pixmap=QPixmap( iconReader2.getIcon("fm_folder_small", false) );
		if ( pixmap.height() > 19 ) { QImage image; image = pixmap.convertToImage(); 
		image = image.smoothScale( 19, 19 ); pixmap.convertFromImage(image); }  

		ZListBoxItem* Item;
		Item = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	    
		Item->setPixmap ( 0, pixmap );
		Item->appendSubItem ( 1, *it, false, NULL );
		browser->insertItem ( Item,-1,true );
	}
	++it;
  }

	  dir.setFilter ( QDir::Files | QDir::Hidden );

	  entries = dir.entryList();
	  entries.sort();
	  it = entries.begin();
	  while ( it != entries.end() ) {

		pixmap=QPixmap( iconReader2.getIcon("msg_receipt_request_small", false) );
		if ( pixmap.height() > 19 ) { QImage image; image = pixmap.convertToImage(); 
		image = image.smoothScale( 19, 19 ); pixmap.convertFromImage(image); }  
		ZListBoxItem* Item;
		Item = new ZListBoxItem ( browser, QString ( "%I%M" ) );
		Item->setPixmap ( 0, pixmap );
		Item->appendSubItem ( 1, *it, false, NULL );
		browser->insertItem ( Item,-1,true ); 
		++it;
	  }
}

