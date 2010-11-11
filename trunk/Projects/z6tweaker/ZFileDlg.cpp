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
#include "ZFileBrowser.h"
#include <qpixmap.h>
#include <qlabel.h>
#include <ZApplication.h>
#include <ZSoftKey.h>
#include "ZConfig.h"
#include <string.h>
#include <ZSingleCaptureDlg.h>
#include <ZScrollView.h>
#include <ZMessageDlg.h>
#include <ZNoticeDlg.h>
#include <ZOptionsMenu.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <qfile.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <string.h>
#include <qtextstream.h>
#include <qvector.h>
#include <qtextcodec.h>
#include <ezxres.h>

QString ProgDir;
QString Idioma;
RES_ICON_Reader iconReader;

QString CONFIG_FONT2, MENU_BACK, MENU_SELECT2, CONFIG_BOOTLOGO2, SHORTCUT_SEARCHING, CONFIG_VR2, CONFIG_SHORTCUT2, SHORTCUT_APPLY, VR_APPLY, CONFIG_SLIDERTONES2, NAME, DIR, CONFIG_WEBLINK2, SLIDER_ASKREMOVE;




//************************************************************************************************
//************************************* class ZSelectFont ****************************************
//************************************************************************************************
ZSelectFont::ZSelectFont()
  :MyBaseDlg()
{
  ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  QString val; 
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = "ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/en-us";

  CargarIdioma();

  this->setMainWidgetTitle(CONFIG_FONT2);
  qDebug("Create ZFileOpenDialog....");
  
  QWidget *myWidget = new QWidget ( this );
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );

  browser = new ZListBox ( QString ( "%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );
  browser->clear();

  QString name, exec, dir, ico;	

  QPixmap p1;
  ZConfig Fini(QString("%1/z6tweaker.cfg").arg(ProgDir), false);
  QString Fval = Fini.readEntry(QString("PROGRAM"), QString("Fonts"), "");
  system("rm /tmp/fontselect");
  system(QString("find %1 -name \"*.ttf\" > /tmp/fontselect").arg(Fval));
  system(QString("find %1 -name \"*.TTF\" >> /tmp/fontselect").arg(Fval));
  QString archivo = QString("/tmp/fontselect");
  QString line="";
  QFile entrada(archivo);
  QTextStream stentrada(&entrada);
  if ( entrada.open(IO_ReadOnly | IO_Translate) )
  {
	while ( !stentrada.eof() )
	{
	  line = stentrada.readLine();
	  if ( line != "" )
	  {
		ZListBoxItem* listitem = new ZListBoxItem ( browser, QString ( "%I%M" ) );
		p1=QPixmap(iconReader.getIcon("fm_font_small", false));
		if ( p1.height() > 19 ) { QImage image; image = p1.convertToImage(); 
		image = image.smoothScale(19, 19); p1.convertFromImage(image); } 
		listitem->setPixmap ( 0, p1 );
		listitem->appendSubItem ( 1, getFileName(line), false, 0 );
		browser->insertItem ( listitem, 2, true );
	  }
	}
  }
  entrada.close();
  system("rm /tmp/fontselect");

  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));

  myVBoxLayout->addWidget ( browser, 1 );
  
  setContentWidget ( browser );
  
  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  //ZSoftKey *softKey = this->getSoftKey( true );

  softKey->setText ( ZSoftKey::RIGHT, MENU_BACK, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::LEFT, MENU_SELECT2, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( brClicked() ) );
  
  this->setCSTWidget( softKey );

}


ZSelectFont::~ZSelectFont()
{
  
}

void ZSelectFont::CargarIdioma()
{
	ZConfig IDini(Idioma, false);	
	CONFIG_FONT2 = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_FONT"), "");
	MENU_BACK = IDini.readEntry(QString("COMMON"), QString("MENU_BACK"), "");
	MENU_SELECT2 = IDini.readEntry(QString("COMMON"), QString("MENU_SELECT"), "");
}

QString ZSelectFont::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { 
		QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
		return txtcodec->toUnicode(xString); }
	else { return xString; }
}  

void ZSelectFont::accept()
{
	brClicked();
}

QString ZSelectFont::getFileName(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( 0, i);
  if (str.left(1) = "/") str.remove ( 0, 1);
  return str;    
}


void ZSelectFont::selectItem(int idx)
{
  qDebug(QString("********\nIdx: aboutToShow")); 

}

void ZSelectFont::slot_cancel()
{
  qDebug(QString("**********************\ncancel"));
  reject();
}


void ZSelectFont::brClicked()
{

  qApp->processEvents();
  int index = browser->currentItem();
  if (index == -1) {return;}

  QString val, font;
  val = browser->item(index)->getSubItemText(1,0,true);

  ZConfig Fini(QString("%1/z6tweaker.cfg").arg(ProgDir), false);
  QString Fval = Fini.readEntry(QString("PROGRAM"), QString("Fonts"), "");
  system(QString("find %1 -name \"%2\" > /tmp/fontselect").arg(Fval).arg(val));
  QString archivo = QString("/tmp/fontselect");
  QString line=""; QFile entrada(archivo); QTextStream stentrada(&entrada);
  if ( entrada.open(IO_ReadOnly | IO_Translate) )
  { 
	while ( !stentrada.eof() )
	{
	  line = stentrada.readLine();
	  if ( getFileName(line) == val )
	  {
		font = line;
	  }
	}
  }
  entrada.close();
  system("rm /tmp/fontselect");

  fileName = font;
  basePath = font;
  MyBaseDlg::accept();


}

//************************************************************************************************
//************************************** class ZBootlogo *****************************************
//************************************************************************************************
ZBootlogo::ZBootlogo()
  :MyBaseDlg()
{
  ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  QString val; 
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = "ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/en-us";

  CargarIdioma();

  this->setMainWidgetTitle(CONFIG_BOOTLOGO2);
  qDebug("Create ZFileOpenDialog....");
  
  QWidget *myWidget = new QWidget ( this );
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );

  browser = new ZListBox ( QString ( "%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );
  browser->clear();

  QString name, exec, dir, ico;	

  QPixmap p1;
  ZConfig Fini(QString("%1/z6tweaker.cfg").arg(ProgDir), false);
  QString Fval = Fini.readEntry(QString("PROGRAM"), QString("Bootlogo"), "");
  system("rm /tmp/fontselect");
  system(QString("find %1 -name \"*.bmp\" > /tmp/fontselect").arg(Fval));
  system(QString("find %1 -name \"*.BMP\" >> /tmp/fontselect").arg(Fval));
  QString archivo = QString("/tmp/fontselect");
  QString line="";
  QFile entrada(archivo);
  QTextStream stentrada(&entrada);
  if ( entrada.open(IO_ReadOnly | IO_Translate) )
  {
	while ( !stentrada.eof() )
	{
	  line = stentrada.readLine();
	  if ( line != "" )
	  {
		ZListBoxItem* listitem = new ZListBoxItem ( browser, QString ( "%I%M" ) );
		p1=QPixmap(iconReader.getIcon("fm_pic_small", false));
		if ( p1.height() > 19 ) { QImage image; image = p1.convertToImage(); 
		image = image.smoothScale(19, 19); p1.convertFromImage(image); } 
		listitem->setPixmap ( 0, p1 );
		listitem->appendSubItem ( 1, getFileName(line), false, 0 );
		browser->insertItem ( listitem, 2, true );
	  }
	}
  }
  entrada.close();
  system("rm /tmp/fontselect");

  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));

  myVBoxLayout->addWidget ( browser, 1 );
  
  setContentWidget ( browser );
  
  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  //ZSoftKey *softKey = this->getSoftKey( true );

  softKey->setText ( ZSoftKey::RIGHT, MENU_BACK, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::LEFT, MENU_SELECT2, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( brClicked() ) );
 
  this->setCSTWidget( softKey );

}


ZBootlogo::~ZBootlogo()
{
  
}

void ZBootlogo::CargarIdioma()
{
	ZConfig IDini(Idioma, false);	
	CONFIG_BOOTLOGO2 = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_BOOTLOGO"), "");
	MENU_BACK = IDini.readEntry(QString("COMMON"), QString("MENU_BACK"), "");
	MENU_SELECT2 = IDini.readEntry(QString("COMMON"), QString("MENU_SELECT"), "");
}

QString ZBootlogo::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { 
		QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
		return txtcodec->toUnicode(xString); }
	else { return xString; }
}  

void ZBootlogo::accept()
{
	brClicked();
}

QString ZBootlogo::getFileName(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( 0, i);
  if (str.left(1) = "/") str.remove ( 0, 1);
  return str;    
}


void ZBootlogo::selectItem(int idx)
{
  qDebug(QString("********\nIdx: aboutToShow")); 

}

void ZBootlogo::slot_cancel()
{
  qDebug(QString("**********************\ncancel"));
  reject();
}


void ZBootlogo::brClicked()
{

  qApp->processEvents();
  int index = browser->currentItem();
  if (index == -1) {return;}

  QString val, font;
  val = browser->item(index)->getSubItemText(1,0,true);

  ZConfig Fini(QString("%1/z6tweaker.cfg").arg(ProgDir), false);
  QString Fval = Fini.readEntry(QString("PROGRAM"), QString("Bootlogo"), "");
  system(QString("find %1 -name \"%2\" > /tmp/fontselect").arg(Fval).arg(val));
  QString archivo = QString("/tmp/fontselect");
  QString line=""; QFile entrada(archivo); QTextStream stentrada(&entrada);
  if ( entrada.open(IO_ReadOnly | IO_Translate) )
  { 
	while ( !stentrada.eof() )
	{
	  line = stentrada.readLine();
	  if ( getFileName(line) == val )
	  {
		font = line;
	  }
	}
  }
  entrada.close();
  system("rm /tmp/fontselect");

  fileName = font;
  basePath = font;
  MyBaseDlg::accept();


}

//************************************************************************************************
//************************************** class ZShortcut *****************************************
//************************************************************************************************
ZShortcut::ZShortcut()
  :MyBaseDlg()
{
  ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  QString val; 
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = "ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/en-us";

  CargarIdioma();

  QString xval1 = SHORTCUT_SEARCHING;
  ZMessageDlg* msg =  new ZMessageDlg ( CONFIG_SHORTCUT2, QString("%1").arg(xval1), ZMessageDlg::NONE, 200, NULL, "ZMessageDlg", true, 0 );
  QPixmap px; px.load(ProgDir + "/images/icon.png"); msg->setTitleIcon(px);
  msg->exec();

  this->setMainWidgetTitle(CONFIG_SHORTCUT2);
  
  QWidget *myWidget = new QWidget ( this );
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );

  browser = new ZListBox ( QString ( "%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );
  browser->clear();

	QPixmap p1;	
	QString name, exec, java, ico, appid;	
	RES_ICON_Reader iconReader;

	ZConfig confCARD ( "/mmc/mmca1/.system/java/CardRegistry", false );
	QStringList grouplist;
	confCARD.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
		name = confCARD.readEntry ( *it, "Name", "" );
		exec = confCARD.readEntry ( *it, "Exec", "" );
		java = confCARD.readEntry ( *it, "JavaId", "" );
		ico = confCARD.readEntry ( *it, "BigIcon", "" );
		appid = *it;
		if ( (appid!="")&&(name!="")&&(java=="")&&(exec!="") )
		{
			ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
			QPixmap p1=QPixmap(iconReader.getIcon(ico, false)); p1.load(ico);
			if ( p1.height() > 19 ) { QImage image; image = p1.convertToImage(); 
			image = image.smoothScale(19, 19); p1.convertFromImage(image); }     
			listitem1->setPixmap ( 0, p1 );
			listitem1->appendSubItem ( 1, name, false, 0 );
			browser->insertItem ( listitem1,-1,true );
		}
	}	
	ZConfig confUSER ( "/ezxlocal/download/appwrite/am/UserRegistry", false );
	confUSER.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
		name = confUSER.readEntry ( *it, "Name", "" );
		exec = confUSER.readEntry ( *it, "Exec", "" );
		java = confUSER.readEntry ( *it, "JavaId", "" );
		ico = confUSER.readEntry ( *it, "BigIcon", "" );
		appid = *it;
		if ( (appid!="")&&(name!="")&&(java=="")&&(exec!="") )
		{
			ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
			QPixmap p1=QPixmap(iconReader.getIcon(ico, false)); p1.load(ico);
			if ( p1.height() > 19 ) { QImage image; image = p1.convertToImage(); 
			image = image.smoothScale(19, 19); p1.convertFromImage(image); }     
			listitem1->setPixmap ( 0, p1 );
			listitem1->appendSubItem ( 1, name, false, 0 );
			browser->insertItem ( listitem1,-1,true );
		}
	}	
	ZConfig confSYS ( "/usr/data_resource/preloadapps/SysRegistry", false );
	confSYS.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
		name = confSYS.readEntry ( *it, "Name", "" );
		exec = confSYS.readEntry ( *it, "Exec", "" );
		java = confSYS.readEntry ( *it, "JavaId", "" );
		ico = confSYS.readEntry ( *it, "BigIcon", "" );
		if ( (exec!="")&&(name!="")&&(java=="")&&(ico!="") )
		{
			ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
			QPixmap p1=QPixmap(iconReader.getIcon(ico, false)); p1.load(ico);
			if ( p1.height() > 19 ) { QImage image; image = p1.convertToImage(); 
			image = image.smoothScale(19, 19); p1.convertFromImage(image); }     
			listitem1->setPixmap ( 0, p1 );
			listitem1->appendSubItem ( 1, name, false, 0 );
			browser->insertItem ( listitem1,-1,true );
		}
	}	

  delete msg;
  msg = NULL;

  browser->sort(true); 
  browser->setCurrentItem(0,true);

  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));
  myVBoxLayout->addWidget ( browser, 1 );
  setContentWidget ( browser );
  
  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  softKey->setText ( ZSoftKey::RIGHT, MENU_BACK, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::LEFT, MENU_SELECT2, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( brClicked() ) );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  
  this->setCSTWidget( softKey );

}


ZShortcut::~ZShortcut()
{
  
}

void ZShortcut::CargarIdioma()
{
	ZConfig IDini(Idioma, false);
	SHORTCUT_SEARCHING = IDini.readEntry(QString("Z6TWEAKER"), QString("SHORTCUT_SEARCHING"), "");
	CONFIG_SHORTCUT2 = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_SHORTCUT"), "");
	MENU_BACK = IDini.readEntry(QString("COMMON"), QString("MENU_BACK"), "");
	MENU_SELECT2 = IDini.readEntry(QString("COMMON"), QString("MENU_SELECT"), "");
	SHORTCUT_APPLY = IDini.readEntry(QString("Z6TWEAKER"), QString("SHORTCUT_APPLY"), "");
}

QString ZShortcut::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { 
		QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
		return txtcodec->toUnicode(xString); }
	else { return xString; }
}  

void ZShortcut::accept()
{
	brClicked();
}

QString ZShortcut::getFileName(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( 0, i);
  if (str.left(1) = "/") str.remove ( 0, 1);
  return str;    
}


void ZShortcut::selectItem(int idx)
{
  qDebug(QString("********\nIdx: aboutToShow")); 

}

void ZShortcut::slot_cancel()
{
  qDebug(QString("**********************\ncancel"));
  reject();
}


void ZShortcut::brClicked()
{
  qApp->processEvents();
  int index = browser->currentItem();
  if (index == -1) {return;}
  QString app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true );

  ZConfig ini(Idioma, false);
  ZMessageDlg* msg =  new ZMessageDlg ( CONFIG_SHORTCUT2, SHORTCUT_APPLY, ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
  int i = msg->exec(); delete msg; msg = NULL;
  if (i)
  {
  	system("cat /ezxlocal/download/appwrite/am/UserRegistry /mmc/mmca1/.system/java/CardRegistry /usr/data_resource/preloadapps/SysRegistry > /tmp/shortcuttemp");
	QString name, appid;	
	ZConfig confTEMP ( "/tmp/shortcuttemp", false );
	QStringList grouplist;
	confTEMP.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
		name = confTEMP.readEntry ( *it, "Name", "" );
		appid = *it;
		if ( (appid!="")&&(name==app) )
		{
		  ZConfig Wini("/ezxlocal/download/appwrite/setup/ezx_idle.cfg", true);
		  Wini.writeEntry( QString("static"), QString("shortcut_Side"), appid );
		}
	}	
   	system("rm /tmp/shortcuttemp");
 }
  MyBaseDlg::accept();


}


//************************************************************************************************
//************************************ class ZSlidertones ****************************************
//************************************************************************************************
ZSlidertones::ZSlidertones()
  :MyBaseDlg()
{
  ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  QString val; 
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = "ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/en-us";

  CargarIdioma(); 

  this->setMainWidgetTitle(CONFIG_SLIDERTONES2);
  
  QWidget *myWidget = new QWidget ( this );
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );

  browser = new ZListBox ( QString ( "%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );
  browser->clear();

  CargarMenu();

  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));
  myVBoxLayout->addWidget ( browser, 1 );
  setContentWidget ( browser );

  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  softKey->setText ( ZSoftKey::RIGHT, MENU_BACK, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::LEFT, MENU_SELECT2, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( brClicked() ) );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  
  this->setCSTWidget( softKey );

}

ZSlidertones::~ZSlidertones()
{
  
}

void ZSlidertones::CargarIdioma()
{
	ZConfig IDini(Idioma, false);
	CONFIG_SLIDERTONES2 = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_SLIDERTONES"), "");
	MENU_BACK = IDini.readEntry(QString("COMMON"), QString("MENU_BACK"), "");
	MENU_SELECT2 = IDini.readEntry(QString("COMMON"), QString("MENU_SELECT"), "");
	NAME = IDini.readEntry(QString("Z6TWEAKER"), QString("SLIDER_NAME"), "");
	SLIDER_ASKREMOVE = IDini.readEntry(QString("Z6TWEAKER"), QString("SLIDER_ASKREMOVE"), "");

}

QString ZSlidertones::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { 
		QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
		return txtcodec->toUnicode(xString); }
	else { return xString; }
}  

void ZSlidertones::CargarMenu()
{
	QPixmap p1;
	QString nametone, opentone, closetone;
	ZConfig confCARD ("/ezxlocal/download/appwrite/setup/ezx_slidetone.cfg", false);
	QStringList grouplist;
	confCARD.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
		nametone = confCARD.readEntry ( *it, "DisplayName", "" );
		opentone = confCARD.readEntry ( *it, "OpenTone", "" );
		closetone = confCARD.readEntry ( *it, "CloseTone", "" );
		if ( nametone!="" && opentone!="" && closetone!="" )
		{
			ZListBoxItem* listitem = new ZListBoxItem ( browser, QString ( "%I%M%I" ) );
			p1=QPixmap(iconReader.getIcon("fm_audio_small", false));
			if ( p1.height() > 19 ) { QImage image; image = p1.convertToImage(); 
			image = image.smoothScale(19, 19); p1.convertFromImage(image); } 
			listitem->setPixmap ( 0, p1 );
			listitem->appendSubItem ( 1, nametone, false, 0 );
			browser->insertItem ( listitem, 2, true );
		}
	}
}

void ZSlidertones::accept()
{
	brClicked();
}

QString ZSlidertones::getFileName(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( 0, i);
  if (str.left(1) = "/") str.remove ( 0, 1);
  return str;    
}


void ZSlidertones::selectItem(int idx)
{
  qDebug(QString("********\nIdx: aboutToShow")); 

}

void ZSlidertones::slot_cancel()
{
  qDebug(QString("**********************\ncancel"));
  reject();
}


void ZSlidertones::brClicked()
{
  qApp->processEvents();
  int index = browser->currentItem();
  if (index == -1) {return;}
  int result = index+1;

  QString nombre;
  int Cambio=0;

  QString val, val2;
  val2 = browser->item(index)->getSubItemText(1,0,true);

  ZSingleCaptureDlg* zscd = new ZSingleCaptureDlg( QString(CONFIG_SLIDERTONES2), NAME, ZSingleCaptureDlg::normal, NULL, "ZSingleCaptureDlg", true, 0, 0);
  ZLineEdit* zle = (ZLineEdit*)zscd->getLineEdit();  
  zle->setText(val2);
  zle->setEdited(true);  
  if (zscd->exec() == 1)
  {
	QString valnew = zle->text();
	if ( (valnew!=val2) && (valnew!="") )
	{ 
	  ZConfig Wini("/ezxlocal/download/appwrite/setup/ezx_slidetone.cfg", true);
	  Wini.writeEntry( QString("%1").arg(result), QString("DisplayName"), valnew );
	  Wini.writeEntry( QString("%1").arg(result), QString("OpenTone"), QString("ui_%1_open.wav").arg(valnew).lower() );
	  Wini.writeEntry( QString("%1").arg(result), QString("CloseTone"), QString("ui_%1_close.wav").arg(valnew).lower() );
	  Cambio=1;
	}
  }

  if ( Cambio==1 ) { browser->clear(); CargarMenu(); browser->setCurrentItem(index,true); }

}

void ZSlidertones::Agregar()
{



}

void ZSlidertones::Borrar()
{
  QString val = browser->item(browser->currentItem())->getSubItemText(1,0,true);
  ZMessageDlg* msg =  new ZMessageDlg ( CONFIG_SLIDERTONES2, QString(SLIDER_ASKREMOVE).arg(val), ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
  int i = msg->exec(); delete msg; msg = NULL;
  if (i)
  {
	system("rm /tmp/ezx_slidetone2.cfg"); system("touch /tmp/ezx_slidetone2.cfg");
	ZConfig confCARD2 ( "/tmp/ezx_slidetone2.cfg", true );
	QString Tname, Topen, Tclose, Tnum;
	ZConfig confCARD ( "/ezxlocal/download/appwrite/setup/ezx_slidetone.cfg", true );
	QStringList grouplist; int lugar=1;
	confCARD.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
		Tname = confCARD.readEntry ( *it, "DisplayName", "" );
		Topen = confCARD.readEntry ( *it, "OpenTone", "" );
		Tclose = confCARD.readEntry ( *it, "CloseTone", "" );
		Tnum = QString("%1").arg(lugar);
		if ( Tname!="" && Tname!=val )
		{ 
			confCARD2.writeEntry ( Tnum, "DisplayName", Tname );
			confCARD2.writeEntry ( Tnum, "OpenTone", Topen );
			confCARD2.writeEntry ( Tnum, "CloseTone", Tclose );
			++lugar;
		}
	}	

	system("cp /tmp/ezx_slidetone2.cfg /ezxlocal/download/appwrite/setup/ezx_slidetone.cfg");
	system("rm /tmp/ezx_slidetone2.cfg");
	browser->clear(); CargarMenu(); 
  }
}



//************************************************************************************************
//************************************** class ZWeblink ******************************************
//************************************************************************************************
ZWeblink::ZWeblink()
  :MyBaseDlg()
{
  ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  QString val; 
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = "ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/en-us";

  CargarIdioma(); 

  this->setMainWidgetTitle(CONFIG_WEBLINK2);
  
  QWidget *myWidget = new QWidget ( this );
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );

  browser = new ZListBox ( QString ( "%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );
  browser->clear();

  CargarMenu();

  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));
  myVBoxLayout->addWidget ( browser, 1 );
  setContentWidget ( browser );
  
  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  softKey->setText ( ZSoftKey::RIGHT, MENU_BACK, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::LEFT, MENU_SELECT2, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( brClicked() ) );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  
  this->setCSTWidget( softKey );

}

ZWeblink::~ZWeblink()
{
  
}

void ZWeblink::CargarIdioma()
{
	ZConfig IDini(Idioma, false);
	CONFIG_WEBLINK2 = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_WEBLINK"), "");
	MENU_BACK = IDini.readEntry(QString("COMMON"), QString("MENU_BACK"), "");
	MENU_SELECT2 = IDini.readEntry(QString("COMMON"), QString("MENU_SELECT"), "");
	NAME = IDini.readEntry(QString("Z6TWEAKER"), QString("WEBLINK_NAME"), "");
	DIR = IDini.readEntry(QString("Z6TWEAKER"), QString("WEBLINK_DIR"), "");
}

QString ZWeblink::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { 
		QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
		return txtcodec->toUnicode(xString); }
	else { return xString; }
}  

void ZWeblink::CargarMenu()
{
  QPixmap p1; QString val;
  ZConfig Wini("/ezxlocal/download/appwrite/setup/ezx_hsa.cfg", false);

  ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  p1=QPixmap(iconReader.getIcon("fm_bkmrk_small", false));
  if ( p1.height() > 19 ) { QImage image; image = p1.convertToImage(); 
  image = image.smoothScale(19, 19); p1.convertFromImage(image); } 
  listitem1->setPixmap ( 0, p1 );
  val = Wini.readEntry(QString("Hsa"), QString("OrangeWorldItem1_NAME"), "");
  listitem1->appendSubItem ( 1, val, false, NULL );
  val = Wini.readEntry(QString("Hsa"), QString("OrangeWorldItem1_URL"), "");
  listitem1->appendSubItem ( 1, val, false, NULL );
  browser->insertItem ( listitem1,-1,true );

  ZListBoxItem* listitem2 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  listitem2->setPixmap ( 0, p1 );
  val = Wini.readEntry(QString("Hsa"), QString("OrangeWorldItem2_NAME"), "");
  listitem2->appendSubItem ( 1, val, false, NULL );
  val = Wini.readEntry(QString("Hsa"), QString("OrangeWorldItem2_URL"), "");
  listitem2->appendSubItem ( 1, val, false, NULL );
  browser->insertItem ( listitem2,-1,true );

  ZListBoxItem* listitem3 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  listitem3->setPixmap ( 0, p1 );
  val = Wini.readEntry(QString("Hsa"), QString("OrangeWorldItem3_NAME"), "");
  listitem3->appendSubItem ( 1, val, false, NULL );
  val = Wini.readEntry(QString("Hsa"), QString("OrangeWorldItem3_URL"), "");
  listitem3->appendSubItem ( 1, val, false, NULL );
  browser->insertItem ( listitem3,-1,true );

  ZListBoxItem* listitem4 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  listitem4->setPixmap ( 0, p1 );
  val = Wini.readEntry(QString("Hsa"), QString("OrangeWorldItem4_NAME"), "");
  listitem4->appendSubItem ( 1, val, false, NULL );
  val = Wini.readEntry(QString("Hsa"), QString("OrangeWorldItem4_URL"), "");
  listitem4->appendSubItem ( 1, val, false, NULL );
  browser->insertItem ( listitem4,-1,true );

  ZListBoxItem* listitem5 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  listitem5->setPixmap ( 0, p1 );
  val = Wini.readEntry(QString("Hsa"), QString("OrangeWorldItem5_NAME"), "");
  listitem5->appendSubItem ( 1, val, false, NULL );
  val = Wini.readEntry(QString("Hsa"), QString("OrangeWorldItem5_URL"), "");
  listitem5->appendSubItem ( 1, val, false, NULL );
  browser->insertItem ( listitem5,-1,true );

}

void ZWeblink::accept()
{
	brClicked();
}

QString ZWeblink::getFileName(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( 0, i);
  if (str.left(1) = "/") str.remove ( 0, 1);
  return str;    
}


void ZWeblink::selectItem(int idx)
{
  qDebug(QString("********\nIdx: aboutToShow")); 

}

void ZWeblink::slot_cancel()
{
  qDebug(QString("**********************\ncancel"));
  reject();
}


void ZWeblink::brClicked()
{
  int Cambio=0;
  qApp->processEvents();
  int index = browser->currentItem();
  if (index == -1) {return;}
  int result = index+1;

  QString nombre,link;

  if (result==1) { nombre="OrangeWorldItem1_NAME"; link="OrangeWorldItem1_URL"; }
  if (result==2) { nombre="OrangeWorldItem2_NAME"; link="OrangeWorldItem2_URL"; }
  if (result==3) { nombre="OrangeWorldItem3_NAME"; link="OrangeWorldItem3_URL"; }
  if (result==4) { nombre="OrangeWorldItem4_NAME"; link="OrangeWorldItem4_URL"; }
  if (result==5) { nombre="OrangeWorldItem5_NAME"; link="OrangeWorldItem5_URL"; }

  QString val, val2;
  val2 = browser->item(index)->getSubItemText(1,0,true);

  ZSingleCaptureDlg* zscd = new ZSingleCaptureDlg( QString(CONFIG_WEBLINK2), NAME, ZSingleCaptureDlg::normal, NULL, "ZSingleCaptureDlg", true, 0, 0);
  ZLineEdit* zle = (ZLineEdit*)zscd->getLineEdit();  
//  zle->setEchoMode(ZLineEdit::Password);
  zle->setText(val2);
  zle->setEdited(true);  
  if (zscd->exec() == 1)
  {
	QString valnew = zle->text();
	if ( (valnew!=val2) && (valnew!="") )
	{ 
	  ZConfig Wini("/ezxlocal/download/appwrite/setup/ezx_hsa.cfg", false);
	  Wini.writeEntry( QString("Hsa"), QString(nombre), valnew );
	  Cambio=1;
	}
  }

  val2 = browser->item(index)->getSubItemText(1,1,true);

  ZSingleCaptureDlg* zscd2 = new ZSingleCaptureDlg( QString(CONFIG_WEBLINK2), DIR, ZSingleCaptureDlg::normal, NULL, "ZSingleCaptureDlg", true, 0, 0);
  ZLineEdit* zle2 = (ZLineEdit*)zscd2->getLineEdit();  
//  zle2->setEchoMode(ZLineEdit::Password);
  zle2->setText(val2);
  zle2->setEdited(true);  
  if (zscd2->exec() == 1)
  {
	QString valnew = zle2->text();
	if ( (valnew!=val2) && (valnew!="") )
	{ 
	  ZConfig Wini("/ezxlocal/download/appwrite/setup/ezx_hsa.cfg", true);
	  Wini.writeEntry( QString("Hsa"), QString(link), valnew );
	  Cambio=1;
	}
  }

  if ( Cambio==1 ) { browser->clear(); CargarMenu(); browser->setCurrentItem(index,true); }

}


//************************************************************************************************
//************************************ class ZVoiceRecord ****************************************
//************************************************************************************************
ZVoiceRecord::ZVoiceRecord()
  :MyBaseDlg()
{
  ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  QString val; 
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = "ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/en-us";

  CargarIdioma();

  QString xval1 = SHORTCUT_SEARCHING;
  ZMessageDlg* msg =  new ZMessageDlg ( CONFIG_VR2, QString("%1").arg(xval1), ZMessageDlg::NONE, 200, NULL, "ZMessageDlg", true, 0 );
  QPixmap px; px.load(ProgDir + "/images/icon.png"); msg->setTitleIcon(px);
  msg->exec();

  this->setMainWidgetTitle(CONFIG_VR2);
  
  QWidget *myWidget = new QWidget ( this );
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );

  browser = new ZListBox ( QString ( "%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );
  browser->clear();

	QPixmap p1;	
	QString name, exec, java, ico, appid;	
	RES_ICON_Reader iconReader;

	ZConfig confCARD ( "/mmc/mmca1/.system/java/CardRegistry", false );
	QStringList grouplist;
	confCARD.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
		name = confCARD.readEntry ( *it, "Name", "" );
		exec = confCARD.readEntry ( *it, "Exec", "" );
		java = confCARD.readEntry ( *it, "JavaId", "" );
		ico = confCARD.readEntry ( *it, "BigIcon", "" );
		appid = *it;
		if ( (appid!="")&&(name!="")&&(java=="")&&(exec!="") )
		{
			ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
			QPixmap p1=QPixmap(iconReader.getIcon(ico, false)); p1.load(ico);
			if ( p1.height() > 19 ) { QImage image; image = p1.convertToImage(); 
			image = image.smoothScale(19, 19); p1.convertFromImage(image); }     
			listitem1->setPixmap ( 0, p1 );
			listitem1->appendSubItem ( 1, name, false, 0 );
			browser->insertItem ( listitem1,-1,true );
		}
	}	
	ZConfig confUSER ( "/ezxlocal/download/appwrite/am/UserRegistry", false );
	confUSER.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
		name = confUSER.readEntry ( *it, "Name", "" );
		exec = confUSER.readEntry ( *it, "Exec", "" );
		java = confUSER.readEntry ( *it, "JavaId", "" );
		ico = confUSER.readEntry ( *it, "BigIcon", "" );
		appid = *it;
		if ( (appid!="")&&(name!="")&&(java=="")&&(exec!="") )
		{
			ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
			QPixmap p1=QPixmap(iconReader.getIcon(ico, false)); p1.load(ico);
			if ( p1.height() > 19 ) { QImage image; image = p1.convertToImage(); 
			image = image.smoothScale(19, 19); p1.convertFromImage(image); }     
			listitem1->setPixmap ( 0, p1 );
			listitem1->appendSubItem ( 1, name, false, 0 );
			browser->insertItem ( listitem1,-1,true );
		}
	}	
	ZConfig confSYS ( "/usr/data_resource/preloadapps/SysRegistry", false );
	confSYS.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
		name = confSYS.readEntry ( *it, "Name", "" );
		exec = confSYS.readEntry ( *it, "Exec", "" );
		java = confSYS.readEntry ( *it, "JavaId", "" );
		ico = confSYS.readEntry ( *it, "BigIcon", "" );
		if ( (exec!="")&&(name!="")&&(java=="")&&(ico!="") )
		{
			ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
			QPixmap p1=QPixmap(iconReader.getIcon(ico, false)); p1.load(ico);
			if ( p1.height() > 19 ) { QImage image; image = p1.convertToImage(); 
			image = image.smoothScale(19, 19); p1.convertFromImage(image); }     
			listitem1->setPixmap ( 0, p1 );
			listitem1->appendSubItem ( 1, name, false, 0 );
			browser->insertItem ( listitem1,-1,true );
		}
	}	

  delete msg;
  msg = NULL;

  browser->sort(true); 
  browser->setCurrentItem(0,true);

  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));
  myVBoxLayout->addWidget ( browser, 1 );
  setContentWidget ( browser );
  
  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  softKey->setText ( ZSoftKey::RIGHT, MENU_BACK, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::LEFT, MENU_SELECT2, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( brClicked() ) );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  
  this->setCSTWidget( softKey );

}


ZVoiceRecord::~ZVoiceRecord()
{
  
}

void ZVoiceRecord::CargarIdioma()
{
	ZConfig IDini(Idioma, false);
	SHORTCUT_SEARCHING = IDini.readEntry(QString("Z6TWEAKER"), QString("SHORTCUT_SEARCHING"), "");
	CONFIG_VR2 = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_VR"), "");
	MENU_BACK = IDini.readEntry(QString("COMMON"), QString("MENU_BACK"), "");
	MENU_SELECT2 = IDini.readEntry(QString("COMMON"), QString("MENU_SELECT"), "");
	VR_APPLY = IDini.readEntry(QString("Z6TWEAKER"), QString("VR_APPLY"), "");
}

QString ZVoiceRecord::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { 
		QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
		return txtcodec->toUnicode(xString); }
	else { return xString; }
}  

void ZVoiceRecord::accept()
{
	brClicked();
}

QString ZVoiceRecord::getFileName(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( 0, i);
  if (str.left(1) = "/") str.remove ( 0, 1);
  return str;    
}


void ZVoiceRecord::selectItem(int idx)
{
  qDebug(QString("********\nIdx: aboutToShow")); 

}

void ZVoiceRecord::slot_cancel()
{
  qDebug(QString("**********************\ncancel"));
  reject();
}


void ZVoiceRecord::brClicked()
{
  qApp->processEvents();
  int index = browser->currentItem();
  if (index == -1) {return;}
  QString app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true );

  ZConfig ini(Idioma, false);
  ZMessageDlg* msg =  new ZMessageDlg ( CONFIG_VR2, VR_APPLY, ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
  int i = msg->exec(); delete msg; msg = NULL;
  if (i)
  {
  	system("cat /ezxlocal/download/appwrite/am/UserRegistry /mmc/mmca1/.system/java/CardRegistry /usr/data_resource/preloadapps/SysRegistry > /tmp/shortcuttemp");
	QString name, appid, dir, exe;	
	ZConfig confTEMP ( "/tmp/shortcuttemp", false );
	QStringList grouplist;
	confTEMP.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
		name = confTEMP.readEntry ( *it, "Name", "" );
		dir = confTEMP.readEntry ( *it, "Directory", "" );
		exe = confTEMP.readEntry ( *it, "Exec", "" );
		appid = *it;
		if ( (appid!="")&&(name==app) )
		{
			int encontro = dir.find( "../.." , 0 );
			if ( encontro > -1 ) { dir.remove( encontro,5); }
			if ( dir[0] != QChar('/') ) { dir="/usr/SYSqtapp/"+dir; }
			int i = dir.findRev("../.."); if ( i==0 ) { dir.remove(0,5); }
			if ( name == "VR" )	{
			system(QString("echo \"#!/bin/sh\n/usr/SYSqtapp/vui/vr\" > /ezxlocal/download/mystuff/.system/LinXtend/usr/bin/vr").arg(dir));
			} else {
			system(QString("echo \"#!/bin/sh\n%1/%2\" > /ezxlocal/download/mystuff/.system/LinXtend/usr/bin/vr").arg(dir).arg(exe));
			}

		}
	}	
   	system("rm /tmp/shortcuttemp");
 }
  MyBaseDlg::accept();


}

