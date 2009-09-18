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
#include <ZSingleCaptureDlg.h>
#include <ZScrollView.h>
#include <ZMessageDlg.h>
#include <ZNoticeDlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <qfile.h>
#include <qdir.h>
#include <qlist.h>
#include <string.h>
#include <qtextstream.h>
#include <qvector.h>
#include <qtextcodec.h>
#include <ezxres.h>

#include "ZMsg.h"

//class AM_RegistryManager;


QString ProgDir;
QString Idioma2;
RES_ICON_Reader iconReader2;
QString ventana;
int CurShortcut;
QString ubic;
QString currentMenu;
QString lastMenu = "2a58c0d6-05c6-45aa-8938-63059d41a4e9";
QStringList apoyo;
QString seteo;


//************************************************************************************************
//************************************** class ZMainMenu *****************************************
//************************************************************************************************
ZMainMenu::ZMainMenu(QString location)
  :MyBaseDlg()
{
  ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  QString val; ZConfig PREini(QString("%1/am2.cfg").arg(ProgDir), false);
  val = PREini.readEntry(QString("AM"), QString("Language"), "");
  if ( val == "" ) {
	ZConfig ini(QString("/ezxlocal/download/mystuff/.system/LinXtend/usr/CurrentLanguage.cfg"), false);
	val = ini.readEntry(QString("LINUXMOD"), QString("Language"), "");
	Idioma2 = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + val;
	system(QString("echo \"CURRENT LANGUAGE = %1\"").arg(Idioma2));
  } else {
	Idioma2 = ProgDir + "/languages/" + val;
  }
  ZConfig lang (Idioma2,false); 
  QString IDval1 = lang.readEntry(QString("SCREEN"), QString("SHOW"), "");
  QString IDval2 = lang.readEntry(QString("SCREEN"), QString("HIDE"), "");

  this->setMainWidgetTitle(lang.readEntry(QString("TRANSLATE"), "MAIN", ""));
  
  ubic=location;
  CrearLista();
  VerLista();

  
  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  //ZSoftKey *softKey = this->getSoftKey( true );

  softKey->setText ( ZSoftKey::RIGHT, lang.readEntry(QString("COMMON"), "MENU_BACK", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::LEFT, lang.readEntry(QString("COMMON"), "MENU_SELECT", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( appClicked() ) );
  
  this->setCSTWidget( softKey );

}


ZMainMenu::~ZMainMenu()
{
  
}

void ZMainMenu::CrearLista()
{
  QString val; ZConfig Vini(QString("%1/am2.cfg").arg(ProgDir), true);
  val = Vini.readEntry(QString("AM"), QString("IconStyle"), "");

  if ( val == "small" )
  {
	zllb = new ZListBox ( QString ( "%I%M%I" ), this, 0);
  	zllb->setFixedWidth ( 240 );
  }
  else
  {
	illb = new ZIconView ( this );
	illb->setFixedWidth ( 260 );
  }

}

void ZMainMenu::VerLista()
{
  QString val; ZConfig Vini(QString("%1/am2.cfg").arg(ProgDir), true);
  val = Vini.readEntry(QString("AM"), QString("IconStyle"), "");

  if ( val == "small" )
  { 
    CargarMenu(ubic);
    connect(zllb, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(appClicked()));
	setContentWidget ( zllb );
	zllb->setFocus();
  }
  else
  {
    CargarMenu2(ubic);
	connect(illb, SIGNAL(returnPressed(ZIconViewItem *)), SLOT(appClicked()));
	setContentWidget ( illb );
	illb->setFocus();
  }

}


void ZMainMenu::CargarMenu(QString Carpeta)
{
  RES_ICON_Reader iconReader;

	zllb->clear();
	QString carp;
	if ( Carpeta == "Principal" ) { Carpeta = "2a58c0d6-05c6-45aa-8938-63059d41a4e9"; }
	currentMenu = Carpeta;
	ZConfig userMenuTree ( "/ezxlocal/download/appwrite/am/UserMenuTree", false );
    QStringList list = userMenuTree.readListEntry ( Carpeta, QString("Items"), QChar(';') );
	QString IDval, IDval2, Tipo;
	ZConfig IDini( "/tmp/AMmenu", true );
	ZConfig lang ( Idioma2, false );

	QString titulo;
	if ( Carpeta == "2a58c0d6-05c6-45aa-8938-63059d41a4e9") 
	{ titulo = lang.readEntry( QString("TRANSLATE"), "MAIN", "Main Menu" ); }
	else
	{
		QString temp = IDini.readEntry( Carpeta, QString("Name"), "" );
		titulo = lang.readEntry( QString("TRANSLATE"), temp, temp );
	}

	this->setMainWidgetTitle(titulo);
	
	for (int j=0;j<list.count();++j)
	{
		ZListBoxItem* listitem1 = new ZListBoxItem ( zllb, QString ( "%I%M" ) );
		IDval = IDini.readEntry( QString("%1").arg(list[j]), QString("Name"), "" );
		Tipo = IDini.readEntry( QString("%1").arg(list[j]), QString("Type"), "" );
		if ( (IDval != "") && (Tipo != "4" ) ) 
		{
		  QString temp = IDval;
	  	  QString Nval = lang.readEntry( QString("TRANSLATE"), IDval, IDval );
		  QString icono;
		  QString IDval3 = IDini.readEntry( QString("%1").arg(list[j]), QString("BigIcon"), "" );
		  if ( IDval3 == "Files/suiteicon.png" )
		  {
			QString IDval4 = IDini.readEntry( QString("%1").arg(list[j]), QString("Dir"), "" );
			icono = QString("%1/%2").arg(IDval4).arg(IDval3);
		  }
		  else { icono = IDval3; }
	      QPixmap pixmap=QPixmap(iconReader.getIcon(icono, false)); pixmap.load(icono);
		  if ( pixmap.height() > 19 ) { QImage image; image = pixmap.convertToImage(); 
		  image = image.smoothScale(19, 19); pixmap.convertFromImage(image); }     
	      listitem1->setPixmap ( 0, pixmap  );
		  listitem1->appendSubItem ( 1, Nval, true, NULL );
		  zllb->insertItem ( listitem1,-1,true );
		  
		}
	}
}

void ZMainMenu::CargarMenu2(QString Carpeta)
{
  RES_ICON_Reader iconReader;

	illb->clear();
	QString carp;
	if ( Carpeta == "Principal" ) { Carpeta = "2a58c0d6-05c6-45aa-8938-63059d41a4e9"; }
	currentMenu = Carpeta;
	ZConfig userMenuTree ( "/ezxlocal/download/appwrite/am/UserMenuTree", false );
    QStringList list = userMenuTree.readListEntry ( Carpeta, QString("Items"), QChar(';') );
	QString IDval, IDval2, Tipo;
	ZConfig IDini( "/tmp/AMmenu", false );
	ZConfig lang ( Idioma2, false );

	for (int j=0;j<list.count();++j)
	{
		IDval = IDini.readEntry( QString("%1").arg(list[j]), QString("Name"), "" );
		Tipo = IDini.readEntry( QString("%1").arg(list[j]), QString("Type"), "" );
		apoyo.clear();
		if ( (IDval != "") && (Tipo != "4" ) ) 
		{
		  QString temp = IDval;
	  	  QString Nval = lang.readEntry( QString("TRANSLATE"), IDval, IDval );
		  QString icono;
		  QString IDval3 = IDini.readEntry( QString("%1").arg(list[j]), QString("BigIcon"), "" );
		  if ( IDval3 == "Files/suiteicon.png" )
		  {
			QString IDval4 = IDini.readEntry( QString("%1").arg(list[j]), QString("Dir"), "" );
			icono = QString("%1/%2").arg(IDval4).arg(IDval3);
		  } 
		  else { icono = IDval3; }
	      QPixmap pixmap2=QPixmap(iconReader.getIcon(icono, false));
		  pixmap2.load(icono);
		  if ( pixmap2.height() > 48 ) { QImage image2; image2 = pixmap2.convertToImage(); 
		  image2 = image2.smoothScale(48, 48); pixmap2.convertFromImage(image2); }     
		  ZIconViewItem* listitem2 = new ZIconViewItem ( illb, Nval, pixmap2 );
		  if ( j==0 ) { illb->setSelected( listitem2, true, true); }
		  illb->setZItemStyle( ZSkinService::clsZIconView2 );
		}
	}

}

void ZMainMenu::appClicked()
{
  
  QString app; int lugar;
  QString val; ZConfig Vini(QString("%1/am2.cfg").arg(ProgDir), true);
  val = Vini.readEntry(QString("AM"), QString("IconStyle"), "");
  if ( val == "small" ) { app = zllb->item ( zllb->currentItem() )->getSubItemText ( 1, 0, true ); }
  else { app = illb->currentItem()->text(); }
  qApp->processEvents();

  ZConfig lang( Idioma2, false );
  ZConfig IDini( QString("/tmp/AMmenu"), false );
  ZConfig userMenuTree ( "/ezxlocal/download/appwrite/am/UserMenuTree", false );

  QStringList list = userMenuTree.readListEntry ( currentMenu, QString("Items"), QChar(';') );
  for (int j=0;j<list.count();++j)
  {
	QString IDval = IDini.readEntry( QString("%1").arg(list[j]), QString("Name"), "" );
	QString Tval = lang.readEntry( QString("TRANSLATE"), IDval, IDval );
    if ( Tval == app  )
	{
	  QString Tipo = IDini.readEntry( QString("%1").arg(list[j]), QString("Type"), "" );
	  if ( Tipo == "1" )
	  {
		if ( val == "small" ) { CargarMenu ( list[j] ); }
		else { CargarMenu2 ( list[j] ); }
		lastMenu = lastMenu + ";" + list[j];
	  }
	  else
	  {
		QString run = "{"+list[j]+"}";
		qDebug(QString("**********************\nEJECUTANDO %1").arg(IDval));
		ZMsg msg( "/AM/Launcher", "launchApp4NoLockCheck" ); msg << (QUuid)run; msg.send();
	  }
	}
  }
}


QString ZMainMenu::texto(const char*xString)
{
	ZConfig IDini(Idioma2, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { 
		QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
		return txtcodec->toUnicode(xString); }
	else { return xString; }
}  

void ZMainMenu::accept()
{
	//brClicked();
}

QString ZMainMenu::getFileName(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( 0, i);
  if (str.left(1) = "/") str.remove ( 0, 1);
  return str;    
}


void ZMainMenu::selectItem(int idx)
{
  qDebug(QString("********\nIdx: aboutToShow")); 

}

void ZMainMenu::slot_cancel()
{
  if ( currentMenu == "2a58c0d6-05c6-45aa-8938-63059d41a4e9" )
  {
	reject();
  }
  else
  {
	int i = lastMenu.findRev ( ";" ); lastMenu.remove ( i, lastMenu.length() - i );
	QString nMenu = lastMenu;
	i = nMenu.findRev ( ";" ); if ( i > 0 ) { nMenu.remove ( 0, i+1 ); }
	qDebug(QString("********\nMENU: %1").arg(nMenu));
	CargarMenu(nMenu);
	
  }

}







//************************************************************************************************
//************************************** class ZSettings *****************************************
//************************************************************************************************
ZSettings::ZSettings()
  :MyBaseDlg()
{
  ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  QString val; ZConfig PREini(QString("%1/am2.cfg").arg(ProgDir), false);
  val = PREini.readEntry(QString("AM"), QString("Language"), "");
  if ( val == "" ) {
	ZConfig ini(QString("/ezxlocal/download/mystuff/.system/LinXtend/usr/CurrentLanguage.cfg"), false);
	val = ini.readEntry(QString("LINUXMOD"), QString("Language"), "");
	Idioma2 = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + val;
	system(QString("echo \"CURRENT LANGUAGE = %1\"").arg(Idioma2));
  } else {
	Idioma2 = ProgDir + "/languages/" + val;
  }
  ZConfig lang (Idioma2,false); 

  ventana = "browser";

  if ( seteo == "Teclas" ) { this->setMainWidgetTitle(lang.readEntry(QString("SCREEN"), "SHORTCUT", "")); }
  if ( seteo == "Botones" ) { this->setMainWidgetTitle(lang.readEntry(QString("SCREEN"), "BUTTONS", "")); }

  QWidget *myWidget = new QWidget ( this );
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );

  browser = new ZListBox ( QString ( "%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );
  browser->sort(false);

  CargarBrowser();
  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));

  myVBoxLayout->addWidget ( browser, 1 );
  
  setContentWidget ( browser );
  
  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  //ZSoftKey *softKey = this->getSoftKey( true );

  softKey->setText ( ZSoftKey::RIGHT, lang.readEntry(QString("COMMON"), "MENU_BACK", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::LEFT, lang.readEntry(QString("COMMON"), "MENU_SELECT", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( brClicked() ) );
  
  this->setCSTWidget( softKey );

}


ZSettings::~ZSettings()
{
  
}

QString ZSettings::texto(const char*xString)
{
	ZConfig IDini(Idioma2, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { 
		QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
		return txtcodec->toUnicode(xString); }
	else { return xString; }
}  

void ZSettings::accept()
{
	brClicked();
}

QString ZSettings::getFileName(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( 0, i);
  if (str.left(1) = "/") str.remove ( 0, 1);
  return str;    
}


void ZSettings::selectItem(int idx)
{
  qDebug(QString("********\nIdx: aboutToShow")); 

}

void ZSettings::slot_cancel()
{
	if ( ventana == "browser" ) { reject(); }
	else { ventana="browser"; CargarBrowser(); }

}


void ZSettings::brClicked()
{
	if ( ventana == "browser" )
	{
	  qApp->processEvents();
	  int index = browser->currentItem();
	  if (index == -1) {return;}
	  QString val;
	  val = browser->item(index)->getSubItemText(1,0,true);

	  if ( seteo == "Teclas" ) {
		if ( index==0 ) { CargarApps(); ventana="apps"; CurShortcut=1; }
		if ( index==1 ) { CargarApps(); ventana="apps"; CurShortcut=2; }
		if ( index==2 ) { CargarApps(); ventana="apps"; CurShortcut=3; }
		if ( index==3 ) { CargarApps(); ventana="apps"; CurShortcut=4; }
		if ( index==4 ) { CargarApps(); ventana="apps"; CurShortcut=5; }
	  } else if ( seteo == "Botones" ) {
		if ( index==0 ) { CargarApps(); ventana="apps"; CurShortcut=6; }
		if ( index==1 ) { CargarApps(); ventana="apps"; CurShortcut=7; }
		if ( index==2 ) { CargarApps(); ventana="apps"; CurShortcut=8; }
		if ( index==3 ) { CargarApps(); ventana="apps"; CurShortcut=9; }
	  }

	}
	else
	{
	 	int index = browser->currentItem();
	  	QString val = browser->item(index)->getSubItemText(1,0,true);
	  	//system("cat /ezxlocal/download/appwrite/am/UserRegistry /mmc/mmca1/.system/java/CardRegistry /ezxlocal/download/appwrite/am/InstalledDB /usr/data_resource/preloadapps/SysRegistry > /tmp/shortcuttemp");
		QString name, appid, icon, dir, exe;	
		ZConfig confTEMP ( "/tmp/AMmenu", false );
		QStringList grouplist;
		confTEMP.getGroupsKeyList ( grouplist );
		for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
			name = confTEMP.readEntry ( *it, "Name", "" );
			icon = confTEMP.readEntry ( *it, "BigIcon", "" );
			dir = confTEMP.readEntry ( *it, "Directory", "" );
			exe = confTEMP.readEntry ( *it, "Exec", "" );
			appid = *it;
			int encontro = dir.find( "../.." , 0 );
			if ( encontro > -1 ) { dir.remove( encontro,5); }
			if ( dir[0] != QChar('/') ) { dir="/usr/SYSqtapp/"+dir; }
			if ( (appid!="")&&(name==val) )
			{
			  ZConfig Wini(ProgDir + "/am2.cfg", true); 
			  Wini.writeEntry( QString("PROGRAMS"), QString("Exec%1").arg(CurShortcut), "{"+appid+"}" );
			  Wini.writeEntry( QString("PROGRAMS"), QString("Program%1").arg(CurShortcut), name );
			  int i = dir.findRev("../.."); if ( i==0 ) { dir.remove(0,5); }
			  i = icon.findRev("/"); if ( i > -1 ) { if ( icon[0] != QChar('/') ) { icon=dir+"/"+icon; } }
			  Wini.writeEntry( QString("PROGRAMS"), QString("Icon%1").arg(CurShortcut), icon );
			  if ( CurShortcut == 8 )
			  {
				system(QString("echo \"#!/bin/sh\n%1/%2\" > %3/vr").arg(dir).arg(exe).arg(ProgDir));

			  }
			}
		}	
	   	//system("rm /tmp/AMmenu");
		ventana="browser"; CargarBrowser();
		
	}

}

void ZSettings::CargarBrowser()
{
  QString val; ZConfig PREini(QString("%1/am2.cfg").arg(ProgDir), true);
  val = PREini.readEntry(QString("AM"), QString("Language"), "");
  if ( val == "" ) {
	ZConfig ini(QString("/ezxlocal/download/mystuff/.system/LinXtend/usr/CurrentLanguage.cfg"), false);
	val = ini.readEntry(QString("LINUXMOD"), QString("Language"), "");
	Idioma2 = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + val;
	system(QString("echo \"CURRENT LANGUAGE = %1\"").arg(Idioma2));
  } else {
	Idioma2 = ProgDir + "/languages/" + val;
  }
  ZConfig lang (Idioma2,false); 

  browser->clear();

  if ( seteo == "Teclas" )
  {
	ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	QString IDval = lang.readEntry(QString("SETTINGS"), QString("ICON1"), "");
	listitem1->appendSubItem ( 1, IDval, false, 0 );
	IDval = PREini.readEntry(QString("PROGRAMS"), QString("Program1"), "");  
	listitem1->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem1, 0, true );

	ZListBoxItem* listitem2 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("SETTINGS"), QString("ICON2"), "");
	listitem2->appendSubItem ( 1, IDval, false, 0 );
	IDval = PREini.readEntry(QString("PROGRAMS"), QString("Program2"), "");  
	listitem2->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem2, 1, true );

	ZListBoxItem* listitem3 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("SETTINGS"), QString("ICON3"), "");
	listitem3->appendSubItem ( 1, IDval, false, 0 );
	IDval = PREini.readEntry(QString("PROGRAMS"), QString("Program3"), "");  
	listitem3->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem3, 2, true );

	ZListBoxItem* listitem4 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("SETTINGS"), QString("ICON4"), "");
	listitem4->appendSubItem ( 1, IDval, false, 0 );
	IDval = PREini.readEntry(QString("PROGRAMS"), QString("Program4"), "");  
	listitem4->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem4, 3, true );

	ZListBoxItem* listitem5 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("SETTINGS"), QString("ICON5"), "");
	listitem5->appendSubItem ( 1, IDval, false, 0 );
	IDval = PREini.readEntry(QString("PROGRAMS"), QString("Program5"), "");  
	listitem5->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem5, 4, true );

  }
  if ( seteo == "Botones" )
  {

	ZListBoxItem* listitem6 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	QString IDval = lang.readEntry(QString("SETTINGS"), QString("SIDEBUTTON"), "");
	listitem6->appendSubItem ( 1, IDval, false, 0 );
	IDval = PREini.readEntry(QString("PROGRAMS"), QString("Program6"), "");  
	listitem6->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem6, 5, true );

	ZListBoxItem* listitem9 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("SETTINGS"), QString("MUSIC"), "");
	listitem9->appendSubItem ( 1, IDval, false, 0 );
	IDval = PREini.readEntry(QString("PROGRAMS"), QString("Program7"), "");  
	listitem9->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem9, 6, true );

	ZListBoxItem* listitem7 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("SETTINGS"), QString("VOICE"), "");
	listitem7->appendSubItem ( 1, IDval, false, 0 );
	IDval = PREini.readEntry(QString("PROGRAMS"), QString("Program8"), "");  
	listitem7->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem7, 7, true );

	ZListBoxItem* listitem8 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("SETTINGS"), QString("CAMERA"), "");
	listitem8->appendSubItem ( 1, IDval, false, 0 );
	IDval = PREini.readEntry(QString("PROGRAMS"), QString("Program9"), "");  
	listitem8->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem8, 8, true );

  }

}


void ZSettings::CargarApps()
{

  browser->clear();

	QPixmap p1;	
	QString name, exec, java, ico, appid, dir, type;
	RES_ICON_Reader iconReader;

	ZConfig confSYS ( "/tmp/AMmenu", false );
	QStringList grouplist;
	confSYS.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
		name = confSYS.readEntry ( *it, "Name", "" );
		exec = confSYS.readEntry ( *it, "Exec", "" );
		dir = confSYS.readEntry ( *it, "Directory", "" );
		java = confSYS.readEntry ( *it, "JavaId", "" );
		ico = confSYS.readEntry ( *it, "BigIcon", "" );
		type = confSYS.readEntry ( *it, "Type", "" );
		if ( (name!="")&&(ico!="")&&(type=="2") )
		{
			ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
			int i = dir.findRev("../.."); if ( i==0 ) { dir.remove(0,5); }
			i = ico.findRev("/"); if ( i > -1 ) { if ( ico[0] != QChar('/') ) { ico=dir+"/"+ico; } }
			QPixmap pixmap=QPixmap(iconReader.getIcon(ico, false));	pixmap.load(ico);		
			if ( pixmap.height() > 19 ) { QImage image; image = pixmap.convertToImage(); 
			image = image.smoothScale(19, 19); pixmap.convertFromImage(image); }     
			listitem1->setPixmap ( 0, pixmap );
			listitem1->appendSubItem ( 1, name, false, 0 );
			browser->insertItem ( listitem1, -1, true );
		}
	}	
	//system("rm /tmp/AMmenu");
	browser->sort(true);
	browser->setCurrentItem(0,true);

}




//************************************************************************************************
//*************************************** class ZScreen ******************************************
//************************************************************************************************
ZScreen::ZScreen()
  :MyBaseDlg()
{
  ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  QString val; ZConfig PREini(QString("%1/am2.cfg").arg(ProgDir), false);
  val = PREini.readEntry(QString("AM"), QString("Language"), "");
  if ( val == "" ) {
	ZConfig ini(QString("/ezxlocal/download/mystuff/.system/LinXtend/usr/CurrentLanguage.cfg"), false);
	val = ini.readEntry(QString("LINUXMOD"), QString("Language"), "");
	Idioma2 = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + val;
	system(QString("echo \"CURRENT LANGUAGE = %1\"").arg(Idioma2));
  } else {
	Idioma2 = ProgDir + "/languages/" + val;
  }
  ZConfig lang (Idioma2,false); 
  QString IDval1 = lang.readEntry(QString("SCREEN"), QString("SHOW"), "");
  QString IDval2 = lang.readEntry(QString("SCREEN"), QString("HIDE"), "");

  this->setMainWidgetTitle(lang.readEntry(QString("SCREEN"), "SCREEN", ""));
  
  QWidget *myWidget = new QWidget ( this );
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );

  browser = new ZListBox ( QString ( "%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );
  browser->clear();

  ZListBoxItem* listitem = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  QString IDval = lang.readEntry(QString("SCREEN"), QString("SKIN"), "");
  listitem->appendSubItem ( 1, IDval, false, 0 );
  browser->insertItem ( listitem, 0, true );

  ZListBoxItem* listitem2 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  IDval = lang.readEntry(QString("SCREEN"), QString("SHORTCUT"), "");
  listitem2->appendSubItem ( 1, IDval, false, 0 );
  browser->insertItem ( listitem2, 1, true );

  ZListBoxItem* listitem2b = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  IDval = lang.readEntry(QString("SCREEN"), QString("BUTTONS"), "");
  listitem2b->appendSubItem ( 1, IDval, false, 0 );
  browser->insertItem ( listitem2b, 2, true );

  ZListBoxItem* listitem3b = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  IDval = lang.readEntry(QString("SCREEN"), QString("MENU"), "");
  listitem3b->appendSubItem ( 1, IDval, false, 0 );
  browser->insertItem ( listitem3b, 3, true );


/*  ZListBoxItem* listitem3 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  IDval = lang.readEntry(QString("SCREEN"), QString("DATE"), "");
  listitem3->appendSubItem ( 1, IDval, false, 0 );
  IDval = PREini.readEntry(QString("AM"), QString("Date"), "");
  if ( IDval=="YES" ) { listitem3->appendSubItem ( 1, IDval1, false, 0 ); }
  else  { listitem3->appendSubItem ( 1, IDval2, false, 0 ); }
  browser->insertItem ( listitem3, 3, true );

  ZListBoxItem* listitem4 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  IDval = lang.readEntry(QString("SCREEN"), QString("CLOCK"), "");
  listitem4->appendSubItem ( 1, IDval, false, 0 );
  IDval = PREini.readEntry(QString("AM"), QString("Clock"), "");
  if ( IDval=="YES" ) { listitem4->appendSubItem ( 1, IDval1, false, 0 ); }
  else  { listitem4->appendSubItem ( 1, IDval2, false, 0 ); }
  browser->insertItem ( listitem4, 4, true );
*/

  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));

  myVBoxLayout->addWidget ( browser, 1 );
  
  setContentWidget ( browser );
  
  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  //ZSoftKey *softKey = this->getSoftKey( true );

  softKey->setText ( ZSoftKey::RIGHT, lang.readEntry(QString("COMMON"), "MENU_BACK", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::LEFT, lang.readEntry(QString("COMMON"), "MENU_SELECT", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( brClicked() ) );
  
  this->setCSTWidget( softKey );

}


ZScreen::~ZScreen()
{
  
}

QString ZScreen::texto(const char*xString)
{
	ZConfig IDini(Idioma2, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { 
		QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
		return txtcodec->toUnicode(xString); }
	else { return xString; }
}  

void ZScreen::accept()
{
	brClicked();
}

QString ZScreen::getFileName(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( 0, i);
  if (str.left(1) = "/") str.remove ( 0, 1);
  return str;    
}


void ZScreen::selectItem(int idx)
{
  qDebug(QString("********\nIdx: aboutToShow")); 

}

void ZScreen::slot_cancel()
{
  qDebug(QString("**********************\ncancel"));
  reject();
}


void ZScreen::brClicked()
{
  qApp->processEvents();
  int index = browser->currentItem();
  if (index == -1) {return;}
  QString val, font;
  val = browser->item(index)->getSubItemText(1,0,true);

  if ( index == 0 ) { system("/usr/SYSqtapp/systemsetup/themesetup"); }

  if ( index == 1 )
  {
	qApp->processEvents();
	seteo = "Teclas";
	ZSettings *zfod = new ZSettings();
	zfod->exec();
  }

  if ( index == 2 )
  {
	qApp->processEvents();
	seteo = "Botones";
	ZSettings *zfod = new ZSettings();
	zfod->exec();
  }

  if ( index == 3 )
  {
    ZConfig IDini(Idioma2, false);
	QString s1 = QString("%1/am2.cfg").arg(ProgDir);
	int val; ZConfig ini(s1, true);
	val = ini.readNumEntry(QString("AM"), QString("Menu"), 0);
	ZSingleSelectDlg* eq =  new ZSingleSelectDlg( IDini.readEntry(QString("SCREEN"), QString("MENU"), ""),
			 QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list2;
	list2 = IDini.readEntry(QString("SCREEN"), QString("MENU_CLASSIC"), "");
	list2 += IDini.readEntry(QString("SCREEN"), QString("MENU_PLUS"), "");
	eq->addItemsList(list2);
	ZListBox* listboxx2 = eq->getListBox();
	eq->exec();
	int result = eq->result();
	if (result != 1) {
		result = 0;
	} else {
	  result = eq->getCheckedItemIndex();
	  if (listboxx2->itemChecked(0))
	  {
		ini.writeEntry(QString("AM"), QString("Menu"), 0);
	  }
	  if (listboxx2->itemChecked(1))
	  {
		ini.writeEntry(QString("AM"), QString("Menu"), 1);
	  }
	}
  }
  

}















