#include "ZDefs.h"
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
#include <ZNumPickerDlg.h>
#include "ZMsg.h"


//class AM_RegistryManager;


QString ProgDir;
QString Idioma2;
RES_ICON_Reader iconReader2;
QString ventana;
int CurShortcut, CurShortcut2;
QString ubic;
QString seteo;
int setval=0;
int CTP;


//************************************************************************************************
//************************************** class ZSettings *****************************************
//************************************************************************************************
ZSettings::ZSettings()
  :MyBaseDlg()
{
  ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  QString val; ZConfig PREini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", false);
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma2 = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma2).isFile() ) Idioma2 = ProgDir + "/languages/en-us";
  ZConfig lang (Idioma2,false); 

  ventana = "browser";

  if ( seteo == "Teclas" ) { this->setMainWidgetTitle(lang.readEntry(QString("SCREEN"), "SHORTCUT", "")); }
  if ( seteo == "Botones" ) { this->setMainWidgetTitle(lang.readEntry(QString("SCREEN"), "BUTTONS", "")); }

  QWidget *myWidget = new QWidget ( this );
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );

  browser = new myListBox ( QString ( "%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );
  browser->sort(false);

  CargarBrowser();
  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));

  myVBoxLayout->addWidget ( browser, 1 );
  
  setContentWidget ( browser );
  
  softKey = new ZSoftKey ( "CST_2A", this, this );
  //ZSoftKey *softKey = this->getSoftKey( true );

  CargarSoftKeys();

  this->setCSTWidget( softKey );

}


ZSettings::~ZSettings()
{
  
}

void ZSettings::CargarSoftKeys()
{
  QString IDval, IDval2; ZConfig IDini(Idioma2, false);
  if ( ( seteo == "Teclas" ) && ( ventana == "browser" ) )
  {
	QRect rect;
	int idx = 0;
	menu = new ZOptionsMenu ( rect, softKey, NULL, 0);
	menu->setItemSpacing(10);  
	IDval = IDini.readEntry(QString("SETTINGS"), QString("ADD"), "");
	menu->insertItem ( IDval, NULL, NULL, true, idx, idx );
	menu->connectItem ( idx, this, SLOT ( AgregarApp() ) );
	++idx;
	IDval = IDini.readEntry(QString("SETTINGS"), QString("DELETE"), "");
	menu->insertItem ( IDval, NULL, NULL, true, idx, idx );
	menu->connectItem ( idx, this, SLOT ( BorrarApp() ) );
	++idx;
	IDval = IDini.readEntry(QString("SETTINGS"), QString("REORDER"), "Reorder");
	menu->insertItem ( IDval, NULL, NULL, true, idx, idx );
	menu->connectItem ( idx, this, SLOT ( Reordenar() ) );

	IDval = IDini.readEntry(QString("COMMON"), QString("MENU_MENU"), "");
	softKey->setText ( ZSoftKey::LEFT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
	IDval = IDini.readEntry(QString("COMMON"), QString("MENU_MENU"), "");
	softKey->setTextForOptMenuHide( IDval);
	IDval = IDini.readEntry(QString("COMMON"), QString("MENU_SELECT"), "");
	IDval2 = IDini.readEntry(QString("COMMON"), QString("MENU_CANCEL"), "");
	softKey->setTextForOptMenuShow( IDval, IDval2);
	IDval = IDini.readEntry(QString("COMMON"), QString("MENU_BACK"), "");
	softKey->setText ( ZSoftKey::RIGHT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
	softKey->setOptMenu ( ZSoftKey::LEFT, menu );
	softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  }
  else if ( ventana == "Reorder" )
  {
	softKey->setOptMenu ( ZSoftKey::LEFT, NULL );
	softKey->setText ( ZSoftKey::LEFT, IDini.readEntry(QString("COMMON"), "MENU_SELECT", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
	softKey->setText ( ZSoftKey::RIGHT, IDini.readEntry(QString("SETTINGS"), "DONE", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
	softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( brClicked() ) );
	softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );

  }
  else
  {
	softKey->setOptMenu ( ZSoftKey::LEFT, NULL );
	softKey->setText ( ZSoftKey::LEFT, IDini.readEntry(QString("COMMON"), "MENU_SELECT", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
	softKey->setText ( ZSoftKey::RIGHT, IDini.readEntry(QString("COMMON"), "MENU_BACK", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
	softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( brClicked() ) );
	softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );

  }
}

void ZSettings::AgregarApp()
{
	ventana = "AddApp"; CargarSoftKeys(); CargarApps();
}

void ZSettings::Reordenar()
{
	ordenando = -1;
	ventana = "Reorder"; CargarSoftKeys();
}

void ZSettings::BorrarApp()
{
  ZConfig Wini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true); 
  int TP = Wini.readNumEntry(QString("AM"), QString("Icons"), 0);  
  if ( TP > 5 )
  {
 	int index = browser->currentItem() + 1;
	system("cp /ezxlocal/download/appwrite/setup/ezx_motodesk.cfg /tmp/ezx_motodesk.cfg");
	ZConfig Wini2("/tmp/ezx_motodesk.cfg", true);
	int pepe = 1;
	for ( int k=0; k<TP; ++k )
	{
	  if ( k == index-1 ) { ++pepe; }
		Wini.writeEntry ( QString("PROGRAMS"), QString("Exec%1").arg(k+1), Wini2.readEntry( QString("PROGRAMS"), QString("Exec%1").arg(pepe), "" ) );
		Wini.writeEntry ( QString("PROGRAMS"), QString("Program%1").arg(k+1), Wini2.readEntry( QString("PROGRAMS"), QString("Program%1").arg(pepe), "" ) );
		Wini.writeEntry ( QString("PROGRAMS"), QString("Icon%1").arg(k+1), Wini2.readEntry( QString("PROGRAMS"), QString("Icon%1").arg(pepe), "" ) );
		++pepe;
	}
	Wini.removeEntry ( QString("PROGRAMS"), QString("Exec%1").arg(TP) );
	Wini.removeEntry ( QString("PROGRAMS"), QString("Program%1").arg(TP) );
	Wini.removeEntry ( QString("PROGRAMS"), QString("Icon%1").arg(TP) );
	Wini.writeEntry ( QString("AM"), QString("Icons"), TP-1 );
	system("rm /tmp/ezx_motodesk.cfg");
	CargarBrowser();
  } else {
	ZConfig IDini(Idioma2, false);	
	QString	IDval = IDini.readEntry(QString("SETTINGS"), QString("ERROR1"), "");
	QString	IDval2 = IDini.readEntry(QString("SETTINGS"), QString("ERROR2"), "");
 	ZMessageDlg* msg =  new ZMessageDlg ( IDval, IDval2, ZMessageDlg::just_ok, 0, NULL, "ZMessageDlg", true, 0 );
	msg->exec();
	delete msg;
	msg = NULL;
  }
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

void ZSettings::slot_cancel()
{
	if ( ventana == "Reorder" )
	{
	  ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true); 
	  int cpr = ini.readNumEntry(QString("AM"), QString("Icons"), 0);  
	  if ( cpr > 0 )
	  {
		for ( int j=0; j < cpr; ++j )
		{
			QString val = browser->item(j)->getSubItemText(1,0,true);
			ini.writeEntry( QString("PROGRAMS"), QString("Program%1").arg(j+1), QString(val) );
			val = browser->item(j)->getSubItemText(2,0,true);
			ini.writeEntry( QString("PROGRAMS"), QString("Exec%1").arg(j+1), QString(val) );
			val = browser->item(j)->getSubItemText(3,0,true);
			ini.writeEntry( QString("PROGRAMS"), QString("Icon%1").arg(j+1), QString(val) );
		}
	  }
	  ventana = "browser"; CargarSoftKeys();
	  CargarBrowser();
	}
	else if ( ventana == "browser" ) { reject(); }
	else { ventana="browser"; CargarSoftKeys(); CargarBrowser(); }
}


void ZSettings::brClicked()
{
	CargarSoftKeys();

	if ( ventana == "AddApp" )
	{
	 	int index = browser->currentItem();
	  	QString val = browser->item(index)->getSubItemText(1,0,true);
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
			ZConfig lang (Idioma2,false); 
			QString Tname = lang.readEntry( QString("TRANSLATE"), name, name ); 
			if ( (appid!="")&&(Tname==val) )
			{
			  ZConfig Wini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true); 
			  int NProg = CTP; NProg++;
			  Wini.writeEntry( QString("PROGRAMS"), QString("Exec%1").arg(NProg), "{"+appid+"}" );
			  Wini.writeEntry( QString("PROGRAMS"), QString("Program%1").arg(NProg), name );
			  int i = dir.findRev("../.."); if ( i==0 ) { dir.remove(0,5); }
			  i = icon.findRev("/"); if ( i > -1 ) { if ( icon[0] != QChar('/') ) { icon=dir+"/"+icon; } }
			  Wini.writeEntry( QString("PROGRAMS"), QString("Icon%1").arg(NProg), icon );
			  Wini.writeEntry( QString("AM"), QString("Icons"), NProg );
			}
		}
		ventana="browser"; CargarSoftKeys(); CargarBrowser();

	}
	else if ( ventana == "Reorder" )
	{
		ZConfig IDini(Idioma2, false);
		if ( ordenando == -1 )
		{
			qApp->processEvents();
			int index = browser->currentItem();
			if (index == -1) {return;}
			QString val;
			val = browser->item(index)->getSubItemText(1,0,true);
			QPixmap pixmap;
			pixmap = QPixmap( iconReader2.getIcon("gen_reorder_arrw_small", false) );
			if ( pixmap.height() > 19 ) { QImage image; image = pixmap.convertToImage(); 
			image = image.smoothScale(19, 19); pixmap.convertFromImage(image); } 
			tPix = browser->item(index)->getPixmap(0);
			browser->item(index)->setPixmap ( 0, pixmap );
			browser->item(index)->setSubItem(4, 0, "Yes", false, NULL);
			softKey->setText ( ZSoftKey::LEFT, IDini.readEntry(QString("SETTINGS"), "RELEASE", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
			ordenando=index;
		} else {
			QPixmap pixmap;
			RES_ICON_Reader iconReader; 
			int index = browser->currentItem();
			QString val = browser->item(index)->getSubItemText(3,0,true);
			QString valexec = browser->item(index)->getSubItemText(2,0,true);
			int i = val.findRev("/"); if ( i > -1 ) {
				if ( val[0] != QChar('/') ) { val=valexec+val; } pixmap.load(val);
			} else { pixmap = QPixmap( iconReader2.getIcon(val, false) ); }
			if ( pixmap.height() > 19 ) { QImage image; image = pixmap.convertToImage(); 
			image = image.smoothScale(19, 19); pixmap.convertFromImage(image); }     
			browser->item(index)->setPixmap ( 0, pixmap );
			browser->item(index)->setSubItem(4, 0, "No", false, NULL);
			softKey->setText ( ZSoftKey::LEFT, IDini.readEntry(QString("COMMON"), "MENU_SELECT", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
			ordenando=-1;
		}
	}
	else if ( ventana == "browser" )
	{
	  if ( seteo == "Botones" ) {
		qApp->processEvents();
		int index = browser->currentItem();
		if (index == -1) {return;}
		QString val;
		val = browser->item(index)->getSubItemText(1,0,true);
		CargarApps(); ventana="apps"; CurShortcut=index+1;
	  }
	}
	else
	{
	 	int index = browser->currentItem();
	  	QString val = browser->item(index)->getSubItemText(1,0,true);
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
			ZConfig lang (Idioma2,false); 
			QString Tname = lang.readEntry( QString("TRANSLATE"), name, name ); 
			if ( (appid!="")&&(Tname==val) )
			{
			  ZConfig Wini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true); 
			  if ( seteo == "Botones" ) {
				  Wini.writeEntry( QString("BUTTONS"), QString("Exec%1").arg(CurShortcut), "{"+appid+"}" );
				  Wini.writeEntry( QString("BUTTONS"), QString("Program%1").arg(CurShortcut), name );
				  int i = dir.findRev("../.."); if ( i==0 ) { dir.remove(0,5); }
				  i = icon.findRev("/"); if ( i > -1 ) { if ( icon[0] != QChar('/') ) { icon=dir+"/"+icon; } }
				  Wini.writeEntry( QString("BUTTONS"), QString("Icon%1").arg(CurShortcut), icon );
			  }
			}
		}	
		ventana="browser"; CargarSoftKeys(); CargarBrowser(); browser->setCurrentItem(CurShortcut-1);
		
	}

}

void ZSettings::CargarBrowser()
{
  QString val; ZConfig PREini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma2 = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  ZConfig lang (Idioma2,false); 

  browser->clear();
  browser->sort(false);

  if ( (seteo=="Teclas") || (ventana=="Reorder") )
  {
	CTP = PREini.readNumEntry(QString("AM"), QString("Icons"), 0);  
	if ( CTP > 0 )
	{
		for ( int j=0; j < CTP; ++j )
		{
			QPixmap pixmap;
			ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M240%M20%M10%M10" ) );
			QString val = PREini.readEntry(QString("PROGRAMS"), QString("Icon%1").arg(j+1), "");
			QString valexec = PREini.readEntry(QString("PROGRAMS"), QString("Exec%1").arg(j+1), "");
			int i = valexec.findRev ( "/" ); valexec.remove ( i+1, valexec.length() - i );

			i = val.findRev("/"); if ( i > -1 ) {
				if ( val[0] != QChar('/') ) { val=valexec+val; } pixmap.load(val);
			} else { pixmap = QPixmap( iconReader2.getIcon(val, false) ); }
			if ( pixmap.height() > 19 ) { QImage image; image = pixmap.convertToImage(); 
			image = image.smoothScale(19, 19); pixmap.convertFromImage(image); }     

			listitem1->setPixmap ( 0, pixmap );
			QString IDval = PREini.readEntry(QString("PROGRAMS"), QString("Program%1").arg(j+1), ""); 
			QString Nval = lang.readEntry( QString("TRANSLATE"), IDval, IDval ); 
			listitem1->appendSubItem ( 1, Nval, false, 0 );
			valexec = PREini.readEntry(QString("PROGRAMS"), QString("Exec%1").arg(j+1), "");
			listitem1->appendSubItem ( 2, valexec, false, 0 );
			val = PREini.readEntry(QString("PROGRAMS"), QString("Icon%1").arg(j+1), "");
			listitem1->appendSubItem ( 3, val, false, 0 );
			listitem1->appendSubItem ( 4, "No", false, 0 );
			browser->insertItem ( listitem1, j, true );
		}
	}


  }
  if ( seteo == "Botones" )
  {
	QString t1 = lang.readEntry(QString("SETTINGS"), QString("SIDEBUTTON"), "");
	QString t2 = lang.readEntry(QString("SETTINGS"), QString("MUSIC"), "");
	QString t3 = lang.readEntry(QString("SETTINGS"), QString("VOICE"), "");
	QString t4 = lang.readEntry(QString("SETTINGS"), QString("CAMERA"), "");
	QString t5 = lang.readEntry(QString("SETTINGS"), QString("BUTTONUP"), "");
	QString t6 = lang.readEntry(QString("SETTINGS"), QString("BUTTONDOWN"), "");
	QString t7 = lang.readEntry(QString("SETTINGS"), QString("BUTTONLEFT"), "");
	QString t8 = lang.readEntry(QString("SETTINGS"), QString("BUTTONRIGHT"), "");

	for ( int j=0; j < 8; ++j )
	{
		QPixmap pixmap;
		ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
		QString val = PREini.readEntry(QString("BUTTONS"), QString("Icon%1").arg(j+1), "");
		QString valexec = PREini.readEntry(QString("BUTTONS"), QString("Exec%1").arg(j+1), "");
		int i = valexec.findRev ( "/" ); valexec.remove ( i+1, valexec.length() - i );

		i = val.findRev("/"); if ( i > -1 ) {
			if ( val[0] != QChar('/') ) { val=valexec+val; } pixmap.load(val);
		} else { pixmap = QPixmap( iconReader2.getIcon(val, false) ); }
		if ( pixmap.height() > 19 ) { QImage image; image = pixmap.convertToImage(); 
		image = image.smoothScale(19, 19); pixmap.convertFromImage(image); }     

		QString t1;
		if (j==0) t1 = lang.readEntry(QString("SETTINGS"), QString("SIDEBUTTON"), "");
		else if ( j==1) t1 = lang.readEntry(QString("SETTINGS"), QString("MUSIC"), "");
		else if ( j==2) t1 = lang.readEntry(QString("SETTINGS"), QString("VOICE"), "");
		else if ( j==3) t1 = lang.readEntry(QString("SETTINGS"), QString("CAMERA"), "");
		else if ( j==4) t1 = lang.readEntry(QString("SETTINGS"), QString("BUTTONUP"), "");
		else if ( j==5) t1 = lang.readEntry(QString("SETTINGS"), QString("BUTTONDOWN"), "");
		else if ( j==6) t1 = lang.readEntry(QString("SETTINGS"), QString("BUTTONLEFT"), "");
		else if ( j==7) t1 = lang.readEntry(QString("SETTINGS"), QString("BUTTONRIGHT"), "");

		listitem1->appendSubItem ( 1, t1, false, 0 );
		QPixmap* pm2 = new QPixmap(pixmap);
		QString IDval = PREini.readEntry(QString("BUTTONS"), QString("Program%1").arg(j+1), "");
		QString Nval = lang.readEntry( QString("TRANSLATE"), IDval, IDval );  
		listitem1->appendSubItem ( 1, Nval, false, pm2 );
		browser->insertItem ( listitem1, j, true );
	}

  }

}


void ZSettings::CargarApps()
{

	browser->clear();

	QPixmap p1;	
	QString name, exec, java, ico, appid, dir, type;
	RES_ICON_Reader iconReader;
	
	ZConfig lang( Idioma2, false );
	ZConfig confSYS ( "/tmp/AMmenu", false );
	QStringList grouplist;
	confSYS.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
		name = confSYS.readEntry ( *it, "Name", "" );
		QString Nval = lang.readEntry( QString("TRANSLATE"), name, name ); 
		exec = confSYS.readEntry ( *it, "Exec", "" );
		dir = confSYS.readEntry ( *it, "Directory", "" );
		java = confSYS.readEntry ( *it, "JavaId", "" );
		ico = confSYS.readEntry ( *it, "BigIcon", "" );
		type = confSYS.readEntry ( *it, "Type", "" );
		int agregar=0;
		if ( (name!="")&&(ico!="")&&(type=="2") ) agregar=1; 
		else if ( (name!="")&&(ico!="")&&(type=="4") ) agregar=1;
		if ( agregar==1 )
		{
			ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
			int i = dir.findRev("../.."); if ( i==0 ) { dir.remove(0,5); }
			i = ico.findRev("/"); if ( i > -1 ) { if ( ico[0] != QChar('/') ) { ico=dir+"/"+ico; } }
			QPixmap pixmap=QPixmap(iconReader.getIcon(ico, false));	pixmap.load(ico);		
			if ( pixmap.height() > 19 ) { QImage image; image = pixmap.convertToImage(); 
			image = image.smoothScale(19, 19); pixmap.convertFromImage(image); }     
			listitem1->setPixmap ( 0, pixmap );
			listitem1->appendSubItem ( 1, Nval, false, 0 );
			browser->insertItem ( listitem1, -1, true );
		}
	}	
	browser->sort(true);
	browser->setCurrentItem(0,true);

}

void ZSettings::keyPressEvent(QKeyEvent* k)
{
	if ( ( k->key() == Z6KEY_RED ) || ( k->key() == SLIDER_CLOSE ) )
	{
		reject();
	}

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

  QString val; ZConfig PREini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", false);
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma2 = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma2).isFile() ) Idioma2 = ProgDir + "/languages/en-us";

  ZConfig lang (Idioma2,false); 
  QString IDval1 = lang.readEntry(QString("SCREEN"), QString("SHOW"), "");
  QString IDval2 = lang.readEntry(QString("SCREEN"), QString("HIDE"), "");

  this->setMainWidgetTitle(lang.readEntry(QString("SCREEN"), "SCREEN", ""));

  setval = 0;

  QWidget *myWidget = new QWidget ( this );
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );

  browser = new ZListBox ( QString ( "%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );

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


ZScreen::~ZScreen()
{
  
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

void ZScreen::slot_cancel()
{
	if ( setval == 1 ) { setval = 0; CargarBrowser(); } 
	else reject();
}

void ZScreen::CargarBrowser()
{
  browser->clear();
  ZConfig lang (Idioma2,false); 
  if ( setval == 0 )
  {

	ZListBoxItem* listitem = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	QString IDval = lang.readEntry(QString("SCREEN"), QString("SKIN"), "");
	listitem->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem, 0, true );

	ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("SCREEN"), QString("WALL"), "");
	listitem1->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem1, 1, true );

	ZListBoxItem* listitem2c = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("SCREEN"), QString("ICONBAR"), "");
	listitem2c->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem2c, 2, true );

	ZListBoxItem* listitem2b = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("SCREEN"), QString("BUTTONS"), "");
	listitem2b->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem2b, 3, true );

	ZListBoxItem* listitem3b = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("SCREEN"), QString("MENU"), "");
	listitem3b->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem3b, 4, true );

	ZListBoxItem* listitem4 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("SCREEN"), QString("SYSINFO"), "");
	listitem4->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem4, 5, true );

	ZListBoxItem* listitem4b = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("SCREEN"), QString("NOTES"), "");
	listitem4b->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem4b, 6, true );

	ZListBoxItem* listitem3d = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("SCREEN"), QString("DATE"), "");
	listitem3d->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem3d, 7, true );

	ZListBoxItem* listitem3c = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("SCREEN"), QString("CLOCK"), "");
	listitem3c->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem3c, 8, true );

  }
  else
  {

	ZListBoxItem* listitem2 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	QString IDval = lang.readEntry(QString("SCREEN"), QString("SHORTCUT"), "");
	listitem2->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem2, 0, true );

	ZListBoxItem* listitem2d = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("SCREEN"), QString("BARSTYLE"), "");
	listitem2d->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem2d, 1, true );

	ZListBoxItem* listitem2c = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("SCREEN"), QString("BARPOSITION"), "");
	listitem2c->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem2c, 2, true );

	ZListBoxItem* listitem3a = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("SCREEN"), QString("ICONTEXT"), "");
	listitem3a->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem3a, 3, true );

	ZListBoxItem* listitem3b = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("SCREEN"), QString("CENTERKEY"), "");
	listitem3b->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem3b, 4, true );

  }
}


void ZScreen::brClicked()
{
  qApp->processEvents();
  int index = browser->currentItem();
  if (index == -1) {return;}
  QString val, font;
  val = browser->item(index)->getSubItemText(1,0,true);

  if ( setval==0 )
  {

	if ( index == 0 )
	{
		system("/usr/SYSqtapp/systemsetup/themesetup"); 
		qApp->processEvents();
	}

	if ( index == 1 )
	{
		qApp->processEvents();
		seteo = "Teclas";
		ZWall *zfoda = new ZWall();
		zfoda->exec();
	}

	if ( index == 2 )
	{
		qApp->processEvents();
		setval = 1;
		CargarBrowser();
	}

	if ( index == 3 )
	{
		qApp->processEvents();
		seteo = "Botones";
		ZSettings *zfod = new ZSettings();
		zfod->exec();
	}

	if ( index == 4 )
	{
		ZConfig IDini(Idioma2, false);
		int val; ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
		val = ini.readNumEntry(QString("AM"), QString("Menu"), 0);
		ZSingleSelectDlg* eq =  new ZSingleSelectDlg( IDini.readEntry(QString("SCREEN"), QString("MENU"), ""),
				 QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
		QStringList list2;
		list2 = IDini.readEntry(QString("SCREEN"), QString("MENU_CLASSIC"), "");
		list2 += IDini.readEntry(QString("SCREEN"), QString("MENU_PLUS"), "");
		eq->addItemsList(list2);
		ZListBox* listboxx2 = eq->getListBox();
		if ( val == 0 ) listboxx2->checkItem(0, true);
		else if ( val == 1 ) listboxx2->checkItem(1, true);
		else if ( val == 2 ) listboxx2->checkItem(2, true);
		eq->exec();
		int result = eq->result();
		if (result != 1) {
			result = 0;
		} else {
		  if (listboxx2->itemChecked(0)) ini.writeEntry(QString("AM"), QString("Menu"), 0);
		  else if (listboxx2->itemChecked(1)) ini.writeEntry(QString("AM"), QString("Menu"), 1);
		}
	}
	if ( index == 5 )
	{
		ZConfig IDini(Idioma2, false);
		QString val; ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
		val = ini.readEntry(QString("AM"), QString("SysInfo"), "No");
		ZSingleSelectDlg* eq =  new ZSingleSelectDlg( IDini.readEntry(QString("SCREEN"), QString("SYSINFO"), ""),
				 QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
		QStringList list2;
		list2 = IDini.readEntry(QString("SCREEN"), QString("SHOW"), "");
		list2 += IDini.readEntry(QString("SCREEN"), QString("HIDE"), "");
		eq->addItemsList(list2);
		ZListBox* listboxx2 = eq->getListBox();
		if ( val == "Yes" ) listboxx2->checkItem(0, true);
		else listboxx2->checkItem(1, true);
		eq->exec();
		int result = eq->result();
		if (result != 1) {
			result = 0;
		} else {
		  if (listboxx2->itemChecked(0)) ini.writeEntry(QString("AM"), QString("SysInfo"), "Yes");
		  else if (listboxx2->itemChecked(1)) ini.writeEntry(QString("AM"), QString("SysInfo"), "No");
		}
	}
	if ( index == 6 )
	{
		ZConfig IDini(Idioma2, false);
		QString val; ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
		val = ini.readEntry(QString("AM"), QString("Notes"), "No");
		ZSingleSelectDlg* eq =  new ZSingleSelectDlg( IDini.readEntry(QString("SCREEN"), QString("NOTES"), ""),
				 QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
		QStringList list2;
		list2 = IDini.readEntry(QString("SCREEN"), QString("SHOW"), "");
		list2 += IDini.readEntry(QString("SCREEN"), QString("HIDE"), "");
		eq->addItemsList(list2);
		ZListBox* listboxx2 = eq->getListBox();
		if ( val == "Yes" ) listboxx2->checkItem(0, true);
		else listboxx2->checkItem(1, true);
		eq->exec();
		int result = eq->result();
		if (result != 1) {
			result = 0;
		} else {
		  if (listboxx2->itemChecked(0)) ini.writeEntry(QString("AM"), QString("Notes"), "Yes");
		  else if (listboxx2->itemChecked(1)) ini.writeEntry(QString("AM"), QString("Notes"), "No");
		}
	}
	if ( index == 7 )
	{
		ZConfig IDini(Idioma2, false);
		QString val; ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
		val = ini.readEntry(QString("AM"), QString("Date"), "No");
		ZSingleSelectDlg* eq =  new ZSingleSelectDlg( IDini.readEntry(QString("SCREEN"), QString("DATE"), ""),
				 QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
		QStringList list2;
		list2 = IDini.readEntry(QString("SCREEN"), QString("SHOW"), "");
		list2 += IDini.readEntry(QString("SCREEN"), QString("HIDE"), "");
		eq->addItemsList(list2);
		ZListBox* listboxx2 = eq->getListBox();
		if ( val == "Yes" ) listboxx2->checkItem(0, true);
		else listboxx2->checkItem(1, true);
		eq->exec();
		int result = eq->result();
		if (result != 1) {
			result = 0;
		} else {
		  if (listboxx2->itemChecked(0)) ini.writeEntry(QString("AM"), QString("Date"), "Yes");
		  else if (listboxx2->itemChecked(1)) ini.writeEntry(QString("AM"), QString("Date"), "No");
		}
	}
	if ( index == 8 )
	{
		ZConfig IDini(Idioma2, false);
		QString val; ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
		val = ini.readEntry(QString("AM"), QString("Clock"), "No");
		ZSingleSelectDlg* eq =  new ZSingleSelectDlg( IDini.readEntry(QString("SCREEN"), QString("CLOCK"), ""),
				 QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
		QStringList list2;
		list2 = IDini.readEntry(QString("SCREEN"), QString("ANALOG"), "");
		list2 += IDini.readEntry(QString("SCREEN"), QString("DIGITAL"), "");
		list2 += IDini.readEntry(QString("SCREEN"), QString("HIDE"), "");
		eq->addItemsList(list2);
		ZListBox* listboxx2 = eq->getListBox();
		if ( val == "Analog" ) listboxx2->checkItem(0, true);
		else if ( val == "Digital" ) listboxx2->checkItem(1, true);
		else if ( val == "No" ) listboxx2->checkItem(2, true);
		eq->exec();
		int result = eq->result();
		if (result != 1) {
			result = 0;
		} else {
		  if (listboxx2->itemChecked(0)) ini.writeEntry(QString("AM"), QString("Clock"), "Analog");
		  else if (listboxx2->itemChecked(1)) ini.writeEntry(QString("AM"), QString("Clock"), "Digital");
		  else if (listboxx2->itemChecked(2)) ini.writeEntry(QString("AM"), QString("Clock"), "No");
		}
	}


  }
  else
  {
	if ( index == 0 )
	{
		qApp->processEvents();
		seteo = "Teclas";
		ZSettings *zfod = new ZSettings();
		zfod->exec();
	}
	
 	if ( index == 1 )
	{
		ZConfig IDini(Idioma2, false);
		QString val; ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
		val = ini.readEntry(QString("AM"), QString("BarStyle"), "0");
		ZSingleSelectDlg* eq =  new ZSingleSelectDlg( IDini.readEntry(QString("SCREEN"), QString("BARPSTYLE"), ""),
				 QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
		QStringList list2;
		list2 = IDini.readEntry(QString("SCREEN"), QString("BARNORMAL"), "");
		list2 += IDini.readEntry(QString("SCREEN"), QString("BARCONTINUED"), "");
		list2 += IDini.readEntry(QString("SCREEN"), QString("BARFIXED"), "");
		eq->addItemsList(list2);
		ZListBox* listboxx2 = eq->getListBox();
		if ( val == "0" ) listboxx2->checkItem(0, true);
		else if ( val == "1" ) listboxx2->checkItem(1, true);
		else if ( val == "2" ) listboxx2->checkItem(2, true);
		eq->exec();
		int result = eq->result();
		if (result != 1) {
			result = 0;
		} else {
		  if (listboxx2->itemChecked(0)) { ini.writeEntry(QString("AM"), QString("BarStyle"), "0"); }
		  else if (listboxx2->itemChecked(1)) { ini.writeEntry(QString("AM"), QString("BarStyle"), "1"); }
		  else if (listboxx2->itemChecked(2)) { ini.writeEntry(QString("AM"), QString("BarStyle"), "2"); }
		}
	}
	if ( index == 2 )
	{
		ZConfig IDini(Idioma2, false);
		QString val; ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
		val = ini.readEntry(QString("AM"), QString("BarPosition"), "Up");
		ZSingleSelectDlg* eq =  new ZSingleSelectDlg( IDini.readEntry(QString("SCREEN"), QString("BARPOSITION"), ""),
				 QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
		QStringList list2;
		list2 = IDini.readEntry(QString("SCREEN"), QString("UP"), "");
		list2 += IDini.readEntry(QString("SCREEN"), QString("DOWN"), "");
		list2 += IDini.readEntry(QString("SCREEN"), QString("LEFT"), "Left");
		list2 += IDini.readEntry(QString("SCREEN"), QString("RIGHT"), "Right");
		eq->addItemsList(list2);
		ZListBox* listboxx2 = eq->getListBox();
		if ( val == "Up" ) listboxx2->checkItem(0, true);
		else if ( val == "Down" ) listboxx2->checkItem(1, true);
		else if ( val == "Left" ) listboxx2->checkItem(2, true);
		else if ( val == "Right" ) listboxx2->checkItem(3, true);
		eq->exec();
		int result = eq->result();
		if (result != 1) {
			result = 0;
		} else {
		  if (listboxx2->itemChecked(0)) ini.writeEntry(QString("AM"), QString("BarPosition"), "Up");
		  else if (listboxx2->itemChecked(1)) ini.writeEntry(QString("AM"), QString("BarPosition"), "Down");
		  else if (listboxx2->itemChecked(2)) ini.writeEntry(QString("AM"), QString("BarPosition"), "Left");
		  else if (listboxx2->itemChecked(3)) ini.writeEntry(QString("AM"), QString("BarPosition"), "Right");
		}
	}
	if ( index == 3 )
	{
		ZConfig IDini(Idioma2, false);
		QString val; ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
		val = ini.readEntry(QString("AM"), QString("IconText"), "No");
		ZSingleSelectDlg* eq =  new ZSingleSelectDlg( IDini.readEntry(QString("SCREEN"), QString("ICONTEXT"), ""),
				 QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
		QStringList list2;
		list2 = IDini.readEntry(QString("SCREEN"), QString("SHOW"), "");
		list2 += IDini.readEntry(QString("SCREEN"), QString("HIDE"), "");
		eq->addItemsList(list2);
		ZListBox* listboxx2 = eq->getListBox();
		if ( val == "Yes" ) listboxx2->checkItem(0, true);
		else listboxx2->checkItem(1, true);
		eq->exec();
		int result = eq->result();
		if (result != 1) {
			result = 0;
		} else {
		  if (listboxx2->itemChecked(0)) ini.writeEntry(QString("AM"), QString("IconText"), "Yes");
		  else if (listboxx2->itemChecked(1)) ini.writeEntry(QString("AM"), QString("IconText"), "No");
		}
	}
	if ( index == 4 )
	{
		ZConfig IDini(Idioma2, false);
		QString val; ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
		val = ini.readEntry(QString("AM"), QString("CenterKey"), "No");
		ZSingleSelectDlg* eq =  new ZSingleSelectDlg( IDini.readEntry(QString("SCREEN"), QString("CENTERKEY"), ""),
				 QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
		QStringList list2;
		list2 = IDini.readEntry(QString("SCREEN"), QString("SHOWHIDE"), "");
		list2 += IDini.readEntry(QString("SCREEN"), QString("ENABLE"), "");
		eq->addItemsList(list2);
		ZListBox* listboxx2 = eq->getListBox();
		if ( val == "0" ) listboxx2->checkItem(0, true);
		else listboxx2->checkItem(1, true);
		eq->exec();
		int result = eq->result();
		if (result != 1) {
			result = 0;
		} else {
		  if (listboxx2->itemChecked(0)) ini.writeEntry(QString("AM"), QString("CenterKey"), "0");
		  else if (listboxx2->itemChecked(1)) ini.writeEntry(QString("AM"), QString("CenterKey"), "1");
		}
	}
 }  

}

void ZScreen::keyPressEvent(QKeyEvent* k)
{
	if ( ( k->key() == Z6KEY_RED ) || ( k->key() == SLIDER_CLOSE ) )
	{
		if ( setval == 1 ) { setval = 0; CargarBrowser(); } 
		else reject();
	}

}




//************************************************************************************************
//**************************************** class ZWall *******************************************
//************************************************************************************************
ZWall::ZWall()
  :MyBaseDlg()
{
  ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  QString val; ZConfig PREini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", false);
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma2 = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma2).isFile() ) Idioma2 = ProgDir + "/languages/en-us";

  ZConfig lang (Idioma2,false); 
  QString IDval1 = lang.readEntry(QString("SCREEN"), QString("SHOW"), "");
  QString IDval2 = lang.readEntry(QString("SCREEN"), QString("HIDE"), "");

  this->setMainWidgetTitle(lang.readEntry(QString("SCREEN"), "WALL", ""));
  
  QWidget *myWidget = new QWidget ( this );
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );

  browser = new ZListBox ( QString ( "%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );
  browser->sort(false);

  CargarItems();

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


ZWall::~ZWall()
{
  
}

void ZWall::CargarItems()
{
  ZConfig lang (Idioma2,false); 
  ZConfig PREini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
  QString activo = PREini.readEntry(QString("AM"), QString("Walltype"), "Default");
  browser->clear();
  QString IDval;
	
  ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  IDval = lang.readEntry(QString("WALL"), QString("WALLTYPE"), "");
  listitem1->appendSubItem ( 1, IDval, false, 0 );
  IDval = lang.readEntry(QString("WALL"), QString("%1").arg(activo).upper(), "");
  listitem1->appendSubItem ( 1, IDval, false, 0 );
  browser->insertItem ( listitem1, 0, true );

  if ( activo=="Wallchanger" )
  {
	ZListBoxItem* listitem3 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("WALL"), QString("WALLPATH"), "");
	listitem3->appendSubItem ( 1, IDval, false, 0 );
	IDval = PREini.readEntry(QString("AM"), QString("Wallpath"), "/mmc/mmca1/Images");
	listitem3->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem3, 1, true );

	ZListBoxItem* listitem4 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("WALL"), QString("TIMETEXT"), "");
	listitem4->appendSubItem ( 1, IDval, false, 0 );
	IDval = PREini.readEntry(QString("AM"), QString("Wallinterval"), "5");
	listitem4->appendSubItem ( 1, QString(IDval + " " + lang.readEntry(QString("WALL"), QString("MINUTES"), "")), false, 0 );
	browser->insertItem ( listitem4, 2, true );
  }
  else if ( activo=="Animate" )
  {
	ZListBoxItem* listitem4 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("WALL"), QString("IMAGE"), "");
	listitem4->appendSubItem ( 1, IDval, false, 0 );
	IDval = getFileName( PREini.readEntry(QString("AM"), QString("AnimationImage"), "-") );
	listitem4->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem4, 1, true );
  }
  else if ( activo=="Wallclock" )
  {
	ZListBoxItem* listitem4 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
	IDval = lang.readEntry(QString("WALL"), QString("THEME"), "");
	listitem4->appendSubItem ( 1, IDval, false, 0 );
	IDval = PREini.readEntry(QString("AM"), QString("WallclockSelected"), "");
	listitem4->appendSubItem ( 1, IDval, false, 0 );
	browser->insertItem ( listitem4, 1, true );

	QString WCS = PREini.readEntry(QString("AM"), QString("WallclockSelected"), "01");
	ZConfig WCini(QString(ProgDir + "/screen/" + WCS + "/settings.cfg"), true);
	if ( WCini.readEntry(QString("SCREEN"), QString("Wallchanger"), "No") == "Yes" )
	{
		ZListBoxItem* listitem3 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
		IDval = lang.readEntry(QString("WALL"), QString("WALLPATH"), "");
		listitem3->appendSubItem ( 1, IDval, false, 0 );
		IDval = PREini.readEntry(QString("AM"), QString("Wallpath"), "/mmc/mmca1/Images");
		listitem3->appendSubItem ( 1, IDval, false, 0 );
		browser->insertItem ( listitem3, 2, true );

		ZListBoxItem* listitem4 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
		IDval = lang.readEntry(QString("WALL"), QString("TIMETEXT"), "");
		listitem4->appendSubItem ( 1, IDval, false, 0 );
		IDval = PREini.readEntry(QString("AM"), QString("Wallinterval"), "5");
		listitem4->appendSubItem ( 1, QString(IDval + " " + lang.readEntry(QString("WALL"), QString("MINUTES"), "")), false, 0 );
		browser->insertItem ( listitem4, 3, true );
	}
  }


}

void ZWall::accept()
{
	brClicked();
}

QString ZWall::getFileName(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( 0, i);
  if (str.left(1) = "/") str.remove ( 0, 1);
  return str;    
}

void ZWall::slot_cancel()
{
  reject();
}


void ZWall::brClicked()
{
  qApp->processEvents();
  int index = browser->currentItem();
  if (index == -1) {return;}
  QString val;
  val = browser->item(index)->getSubItemText(1,0,true);

  ZConfig PREini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
  QString activo = PREini.readEntry(QString("AM"), QString("Walltype"), "");

  if  ( index == 0 ) 
  {
    ZConfig IDini(Idioma2, false);
	QString val;
	val = PREini.readEntry(QString("AM"), QString("Walltype"), "Default");
	ZSingleSelectDlg* eq =  new ZSingleSelectDlg( IDini.readEntry(QString("WALL"), QString("WALLTYPE"), ""),
			 QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list2;
	list2 = IDini.readEntry(QString("WALL"), QString("DEFAULT"), "");
	//list2 += IDini.readEntry(QString("WALL"), QString("ANIMATE"), "");
	list2 += IDini.readEntry(QString("WALL"), QString("WALLCHANGER"), "");
	list2 += IDini.readEntry(QString("WALL"), QString("WALLCLOCK"), "");
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
		PREini.writeEntry(QString("AM"), QString("Walltype"), "Default");
	  }
	  /*else if (listboxx2->itemChecked(1))
	  {
		PREini.writeEntry(QString("AM"), QString("Walltype"), "Animate");
	  }*/
	  else if (listboxx2->itemChecked(1))
	  {
		PREini.writeEntry(QString("AM"), QString("Walltype"), "Wallchanger");
	  }
	  else if (listboxx2->itemChecked(2))
	  {
		PREini.writeEntry(QString("AM"), QString("Walltype"), "Wallclock");
	  }
	  CargarItems();
	  browser->setCurrentItem(0,true);
	}
  }

  else if ( activo=="Wallchanger" )
  {
	  if  ( index == 1 ) 
	  {
		  ZBrowser2 *zb = new ZBrowser2("/");
		  zb->exec();
		  delete zb;
		  zb = NULL;
		  CargarItems();
		  browser->setCurrentItem(1,true);
	  }
	  if  ( index == 2 ) 
	  {
		ZConfig IDini(Idioma2, false);
		int val; ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
		val = ini.readNumEntry(QString("AM"), QString("Wallinterval"), 5);
		ZNumPickerDlg* numero = new ZNumPickerDlg(10, this, "ZNumPickerDlg", 0);
		ZNumModule* module = numero->getNumModule();
		module->setMaxValue(300);
		module->setMinValue(1);
		module->setValue(val);
	
		QString IDval = IDini.readEntry(QString("WALL"), QString("MINUTES"), "");
		ZLabel* label = new ZLabel(IDval, module, NULL,0, (ZSkinService::WidgetClsID)4);
		module->setLabel(label, (ZNumModule::RelativeAlignment)0);
		IDval = IDini.readEntry(QString("WALL"), QString("TIMETEXT"), "");
		numero->setTitle(IDval);
		if (numero->exec())
		{	
			int r = numero->getNumValue();
			ini.writeEntry(QString("AM"), QString("Wallinterval"), r);
			qApp->processEvents();
		}
		CargarItems();
		browser->setCurrentItem(2,true);
	  }
  }
  else if ( activo=="Animate" )
  {
	  ZBrowser3 *zb = new ZBrowser3("/");
	  zb->exec();
	  CargarItems();
	  browser->setCurrentItem(1,true);
	  delete zb;
	  zb = NULL;
  }
  else if ( activo=="Wallclock" )
  {
	if  ( index == 1 ) 
	{

	  ZConfig IDini(Idioma2, false);
	  QString val; ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
	  //val = ini.readEntry(QString("AM"), QString("Animation"), "No");
	  ZSingleSelectDlg* eq =  new ZSingleSelectDlg( IDini.readEntry(QString("WALL"), QString("THEME"), ""),
				 QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	  QStringList list2;


	  QDir dir ( ProgDir + "/screen/", "*" );
	  dir.setMatchAllDirs ( true );
	  dir.setFilter ( QDir::Dirs | QDir::Hidden );
	  if ( !dir.isReadable() )
		return;
	  QStringList entries = dir.entryList();
	  entries.sort();
	  QStringList::ConstIterator it = entries.begin();

	  while ( it != entries.end() ) {
		if ( (*it != ".") && (*it != "..") )
		{
			QString str = *it;
			list2 += str;
		}
		++it;
	  }

		eq->addItemsList(list2);
		ZListBox* listboxx2 = eq->getListBox();
		eq->exec();
		int result = eq->result();
		if (result != 1) {
			result = 0;
		} else {
		  ini.writeEntry(QString("AM"), QString("WallclockSelected"), list2[eq->getCheckedItemIndex()] );
		}
	    CargarItems();
		browser->setCurrentItem(1,true);
	}

	if  ( index == 2 ) 
	{
	  ZBrowser2 *zb = new ZBrowser2("/");
	  zb->exec();
	  delete zb;
	  zb = NULL;
	  CargarItems();
	  browser->setCurrentItem(2,true);
	}
	if  ( index == 3 ) 
	{
		ZConfig IDini(Idioma2, false);
		int val; ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
		val = ini.readNumEntry(QString("AM"), QString("Wallinterval"), 5);
		ZNumPickerDlg* numero = new ZNumPickerDlg(10, this, "ZNumPickerDlg", 0);
		ZNumModule* module = numero->getNumModule();
		module->setMaxValue(300);
		module->setMinValue(1);
		module->setValue(val);

		QString IDval = IDini.readEntry(QString("WALL"), QString("MINUTES"), "");
		ZLabel* label = new ZLabel(IDval, module, NULL,0, (ZSkinService::WidgetClsID)4);
		module->setLabel(label, (ZNumModule::RelativeAlignment)0);
		IDval = IDini.readEntry(QString("WALL"), QString("TIMETEXT"), "");
		numero->setTitle(IDval);
		if (numero->exec())
		{	
			int r = numero->getNumValue();
			ini.writeEntry(QString("AM"), QString("Wallinterval"), r);
			qApp->processEvents();
		}
			CargarItems();
			browser->setCurrentItem(3,true);
		}

  }




}

void ZWall::keyPressEvent(QKeyEvent* k)
{
	if ( ( k->key() == Z6KEY_RED ) || ( k->key() == SLIDER_CLOSE ) )
	{
		reject();
	}

	

}






//************************************************************************************************
//************************************** class ZBrowser ******************************************
//************************************************************************************************
ZBrowser::ZBrowser(QString location)
  :MyBaseDlg()
{

  ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  QString val; ZConfig PREini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", false);
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma2 = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma2).isFile() ) Idioma2 = ProgDir + "/languages/en-us";

  ZConfig lang (Idioma2,false); 
  QString IDval1 = lang.readEntry(QString("SCREEN"), QString("SHOW"), "");
  QString IDval2 = lang.readEntry(QString("SCREEN"), QString("HIDE"), "");

  this->setMainWidgetTitle(lang.readEntry(QString("WALL"), "SELECT", ""));
  
  QWidget *myWidget = new QWidget ( this );
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );

  browser = new ZListBox ( QString ( "%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );

  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));

  CargarBrowser(location);

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

ZBrowser::~ZBrowser()
{}

void ZBrowser::brClicked()
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
	i = fName.findRev ( "//" ); fName.remove ( i, 1 );
	if ( QFileInfo(fName).isDir() ) { CargarBrowser( fName ); }
	else { 
		QString extension = fName;
		int i = extension.findRev ( "." ); extension.remove ( 0, i +1 );
		extension =  QString("%1").arg(extension).lower();
		if ( (extension=="jpg") || (extension=="png") || (extension=="jpeg") || (extension=="gif") )
		{
			ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
			ini.writeEntry(QString("AM"), "Wall", fName);
			reject();
		}
	}
  }


}

void ZBrowser::CargarBrowser(QString direccion)
{
  if ( direccion == "" ) direccion="/";
  RES_ICON_Reader iconReader;

  browser->clear();
  QDir dir ( direccion, "*" );
  sPath = direccion;

  system(QString("echo \"CARPETA = %1\n\"").arg(sPath));

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
		pixmap=QPixmap( iconReader.getIcon("fm_folder_small", false) );
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

	pixmap=QPixmap( iconReader.getIcon("msg_receipt_request_small", false) );
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

void ZBrowser::slot_cancel()
{
  reject();
}

void ZBrowser::accept()
{
	brClicked();
}

void ZBrowser::keyPressEvent(QKeyEvent* k)
{
	if ( ( k->key() == Z6KEY_RED ) || ( k->key() == SLIDER_CLOSE ) )
	{
		reject();
	}

	

}


//************************************************************************************************
//************************************** class ZBrowser2 *****************************************
//************************************************************************************************
ZBrowser2::ZBrowser2(QString location)
  :MyBaseDlg()
{

  ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  QString val; ZConfig PREini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", false);
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma2 = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma2).isFile() ) Idioma2 = ProgDir + "/languages/en-us";

  ZConfig lang (Idioma2,false); 
  QString IDval1 = lang.readEntry(QString("SCREEN"), QString("SHOW"), "");
  QString IDval2 = lang.readEntry(QString("SCREEN"), QString("HIDE"), "");

  this->setMainWidgetTitle(lang.readEntry(QString("WALL"), "SELECT", ""));
  
  QWidget *myWidget = new QWidget ( this );
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );

  browser = new ZListBox ( QString ( "%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );

  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));

  CargarBrowser(location);

  myVBoxLayout->addWidget ( browser, 1 );
  
  setContentWidget ( browser );
  
  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  //ZSoftKey *softKey = this->getSoftKey( true );

  softKey->setText ( ZSoftKey::RIGHT, lang.readEntry(QString("COMMON"), "MENU_BACK", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::LEFT, lang.readEntry(QString("COMMON"), "MENU_SELECT", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( selected() ) );
  
  this->setCSTWidget( softKey );

}

ZBrowser2::~ZBrowser2()
{}

void ZBrowser2::brClicked()
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
	i = fName.findRev ( "//" ); fName.remove ( i, 1 );
	CargarBrowser( fName );
  }


}

void ZBrowser2::selected()
{
	QString app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true );

    int i = sPath.findRev ( "//" ); sPath.remove ( i, 1 );
	QString fName = sPath + "/" + app;
	i = fName.findRev ( "//" ); fName.remove ( i, 1 );
	ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
	ini.writeEntry(QString("AM"), "Wallpath", fName);
	reject();

}

void ZBrowser2::CargarBrowser(QString direccion)
{
  if ( direccion == "" ) direccion="/";
  RES_ICON_Reader iconReader;

  browser->clear();
  QDir dir ( direccion, "*" );
  sPath = direccion;

  system(QString("echo \"CARPETA = %1\n\"").arg(sPath));

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
		pixmap=QPixmap( iconReader.getIcon("fm_folder_small", false) );
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

	pixmap=QPixmap( iconReader.getIcon("msg_receipt_request_small", false) );
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

void ZBrowser2::slot_cancel()
{
  reject();
}

void ZBrowser2::accept()
{
	brClicked();
}

void ZBrowser2::keyPressEvent(QKeyEvent* k)
{
	if ( ( k->key() == Z6KEY_RED ) || ( k->key() == SLIDER_CLOSE ) )
	{
		reject();
	}

	

}



//************************************************************************************************
//************************************** class ZBrowser3 *****************************************
//************************************************************************************************
ZBrowser3::ZBrowser3(QString location)
  :MyBaseDlg()
{

  ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  QString val; ZConfig PREini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", false);
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma2 = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma2).isFile() ) Idioma2 = ProgDir + "/languages/en-us";

  ZConfig lang (Idioma2,false); 
  QString IDval1 = lang.readEntry(QString("SCREEN"), QString("SHOW"), "");
  QString IDval2 = lang.readEntry(QString("SCREEN"), QString("HIDE"), "");

  this->setMainWidgetTitle(lang.readEntry(QString("WALL"), "SELECT", ""));
  
  QWidget *myWidget = new QWidget ( this );
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );

  browser = new ZListBox ( QString ( "%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );

  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));

  CargarBrowser(location);

  myVBoxLayout->addWidget ( browser, 1 );
  
  setContentWidget ( browser );
  
  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  //ZSoftKey *softKey = this->getSoftKey( true );

  softKey->setText ( ZSoftKey::RIGHT, lang.readEntry(QString("COMMON"), "MENU_BACK", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::LEFT, lang.readEntry(QString("COMMON"), "MENU_SELECT", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( selected() ) );
  
  this->setCSTWidget( softKey );

}

ZBrowser3::~ZBrowser3()
{}

void ZBrowser3::brClicked()
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
	QString fName = sPath + "/" + app;
	if ( QFileInfo(fName).isFile() ) 
	{
		int i = sPath.findRev ( "//" ); sPath.remove ( i, 1 );
		i = fName.findRev ( "//" ); fName.remove ( i, 1 );
		ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
		ini.writeEntry(QString("AM"), "AnimationImage", fName);
		reject();
	}
	else
	{
	    int i = sPath.findRev ( "//" ); sPath.remove ( i, 1 );
		QString fName = sPath + "/" + app;
		i = fName.findRev ( "//" ); fName.remove ( i, 1 );
		CargarBrowser( fName );
	}
  }


}

void ZBrowser3::selected()
{
	brClicked();

}

void ZBrowser3::CargarBrowser(QString direccion)
{
  if ( direccion == "" ) direccion="/";
  RES_ICON_Reader iconReader;

  browser->clear();
  QDir dir ( direccion, "*" );
  sPath = direccion;

  system(QString("echo \"CARPETA = %1\n\"").arg(sPath));

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
		pixmap=QPixmap( iconReader.getIcon("fm_folder_small", false) );
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

	pixmap=QPixmap( iconReader.getIcon("msg_receipt_request_small", false) );
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

void ZBrowser3::slot_cancel()
{
  reject();
}

void ZBrowser3::accept()
{
	brClicked();
}

void ZBrowser3::keyPressEvent(QKeyEvent* k)
{
	if ( ( k->key() == Z6KEY_RED ) || ( k->key() == SLIDER_CLOSE ) )
	{
		reject();
	}

	

}


