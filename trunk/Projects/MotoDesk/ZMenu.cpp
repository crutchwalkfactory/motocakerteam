#include <SETUP_THEME_ThemeAPI.h>
#include "ZDefs.h"
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
#include "ZMenu.h"
#include <fcntl.h>
#include <signal.h>


//class AM_RegistryManager;


QString ProgDir2;
QString Idioma3;
RES_ICON_Reader iconReader3;
QString currentMenu2;
QString lastMenu2 = "2a58c0d6-05c6-45aa-8938-63059d41a4e9";
QString lastMenuX = "";
int lastCM1; int lastCM2; int lastCM3; int lastCM4; int lastCM5;
int lastPos1; int lastPos2; int lastPos3; int lastPos4; int lastPos5;
QStringList lista, lProg, lIcon, lExec, lAccion;
int Level=0; 
int iTop, iCM, iout;
int iTextColor, iPos, iIsize, iProgCount;

int timerStop2;
int start;
int side, preside;
int lugar;
int Arriba;
QString iconoA;
int iconSize;

extern ZApplication * app;


//************************************************************************************************
//************************************** class ZoomMenu ******************************************
//************************************************************************************************
MyMenu::MyMenu(QString location)
  :MyBaseDlg()
{

/*  ZConfig FiniA(QString("/ezxlocal/download/appwrite/setup/ezx_theme.cfg"), true);
  QString currentTheme = FiniA.readEntry(QString("THEME_GROUP_LOG"), QString("CurrentSkin"), "");
  ZConfig FiniB(currentTheme, true);
  int opacidad = FiniB.readNumEntry(QString("ZFsba_1"), QString("Bg_Opacity_Std"), 100);

  QPixmap f1; 
  QString SWall = SETUP_THEME::getThemeValue(SETUP_THEME::SLIDE_TONE);
  f1.load( SWall );
  setScreenFSBA( f1 , 100, (PIXMAP_STRETCH_POLICY_E)5 );
  f1 = QPixmap( iconReader3.getIcon("fsba01_bg", false) );
  setScreenFSBA( f1 , opacidad, (PIXMAP_STRETCH_POLICY_E)5 );
*/
  start=1; side=0; preside=0;
  lastCM1=0; lastCM2=0; lastCM3=0; lastCM4=0; lastCM5=0;
  lastPos1=1; lastPos2=1; lastPos3=1; lastPos4=1; lastPos5=1;

  iTop=50; iIsize = 30; iout=1;

  ProgDir2 =  QString ( qApp->argv() [0] );
  int i = ProgDir2.findRev ( "/" );
  ProgDir2.remove ( i+1, ProgDir2.length() - i );

  system("cat /mmc/mmca1/.system/java/CardRegistry /ezxlocal/download/appwrite/am/UserRegistry /ezxlocal/download/appwrite/am/InstalledDB /usr/data_resource/preloadapps/SysRegistry > /tmp/AMmenu");

  QString val; ZConfig PREini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", false);

  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma3 = ProgDir2 + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma3).isFile() ) Idioma3 = ProgDir2 + "/languages/en-us";

  ZConfig lang (Idioma3,true); 

  this->setMainWidgetTitle(lang.readEntry(QString("COMMON"), "MENU_MAIN", ""));

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(timerTick()));
  timerStop2=1;

	QString Fval1; ZConfig Fini1(QString("/ezxlocal/download/appwrite/setup/ezx_theme.cfg"), true);
	Fval1 = Fini1.readEntry(QString("THEME_GROUP_LOG"), QString("CurrentSkin"), "");
	ZConfig Fini3(Fval1 + "/common.ini", false);
	iTextColor = Fini3.readNumEntry(QString("Font_ListitemAStd"), QString("Color"), 0);
  
	Panel = new ZScrollPanel (this , NULL , 0 ,  (ZSkinService::WidgetClsID)4);
	Panel->resize ( 240, 320 );

	Icono1 = new ZImage(this);
	Panel->addChild(Icono1, 20, iTop);
	Icono2 = new ZImage(this);
	Panel->addChild(Icono2, 45, iTop);
	Icono3 = new ZImage(this);
	Panel->addChild(Icono3, 70, iTop);
	Icono4 = new ZImage(this);
	Panel->addChild(Icono4, 95, iTop);
	Icono5 = new ZImage(this);
	Panel->addChild(Icono5, 120, iTop);
	Icono6 = new ZImage(this);
	Panel->addChild(Icono6, 145, iTop);
	Icono7 = new ZImage(this);
	Panel->addChild(Icono7, 170, iTop);
	Icono8 = new ZImage(this);
	Panel->addChild(Icono8, 195, iTop);

	fizq = new ZImage(this); Panel->addChild(fizq, 6, iTop+10);
	fder = new ZImage(this); Panel->addChild(fder, 227, iTop+10);

	eti1 = new ZLabel(this);
	eti1->setGeometry(0,0,230,40);
	eti1->setText("");
	eti1->setAlignment(ZLabel::AlignCenter);
	eti1->setFontColor(QRgb(iTextColor));

	QFont f = eti1->font();
	f.setPixelSize( 18 );
	f.setBold(true);
	eti1->setFont(f);
	

	connect( app, SIGNAL(signalLanguageChange()), this, SLOT(ChangeLang()) );

  this->setContentWidget ( Panel );
  
  softKey = new ZSoftKey ( "CST_2A", this, this );
  //ZSoftKey *softKey = this->getSoftKey( true );

  softKey->setText ( ZSoftKey::RIGHT, lang.readEntry(QString("COMMON"), "MENU_BACK", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::LEFT, lang.readEntry(QString("COMMON"), "MENU_SELECT", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( mClicked() ) );
  
  this->setCSTWidget( softKey );

  int y = this->headerSize().height();
  y = y + softKey->height();
  iTop = 320 - y - 30;

  IconoG = new ZImage(this);
  int xy = 320-y; xy = xy/2;
  Arriba = xy-65;
  Panel->addChild(IconoG, 80, Arriba );

  preIcono = new ZImage(this);
  Panel->addChild(preIcono, 200, Arriba );

  Panel->addChild(eti1, 5, xy + 30 );

  CargarData("2a58c0d6-05c6-45aa-8938-63059d41a4e9");
  RecargarIconos();



}


MyMenu::~MyMenu()
{

}

void MyMenu::ChangeLang()
{
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma3 = ProgDir2 + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma3).isFile() ) Idioma3 = ProgDir2 + "/languages/en-us";
  CargarData(lastMenu2);
  RecargarIconos();
}

void MyMenu::ChangeIcon()
{
  if (side==0)
  {
	int xxi; 
	xxi = iCM+iPos-1; if (xxi<0) xxi=iProgCount-1;
	QString newicon = lIcon[xxi];

	if (newicon[0]==QChar('/')) { newicon.remove( newicon.length() -8 ,8 ); newicon = newicon + "_big.png"; }
	else { newicon.remove( newicon.length() -6 ,6 ); newicon = newicon + "_big"; }

	QPixmap pm = QPixmap( iconReader3.getIcon( newicon, false) ); 
	if ( pm.isNull() ) pm.load(newicon);
	if ( pm.isNull() ) pm = QPixmap( iconReader3.getIcon( lIcon[xxi], false) );
	if ( pm.isNull() ) pm.load(lIcon[xxi]);

	QImage image = pm.convertToImage(); image = image.smoothScale( 80, 80 ); pm.convertFromImage(image);
	IconoG->setPixmap( pm );
	Panel->moveChild(IconoG, 80, Arriba );
	Panel->moveChild(preIcono, 240, Arriba );
	//qApp->processEvents();
  }
  else
  {
	int xxi; 
	if ( side==1 ) { xxi = iCM+iPos; if (xxi<0) xxi=iProgCount-1; if (xxi>iProgCount-1) xxi=0; }
	if ( side==2 ) { xxi = iCM+iPos-2; if (xxi<0) xxi=iProgCount-1; if (xxi>iProgCount-1) xxi=0; }

	QString newicon = lIcon[xxi];
	if (newicon[0]==QChar('/')) { newicon.remove( newicon.length() -8 ,8 ); newicon = newicon + "_big.png"; }
	else { newicon.remove( newicon.length() -6 ,6 ); newicon = newicon + "_big"; }
	QPixmap pm = QPixmap( iconReader3.getIcon( newicon, false) ); 
	if ( pm.isNull() ) pm.load(newicon);
	if ( pm.isNull() ) pm = QPixmap( iconReader3.getIcon( lIcon[xxi], false) );
	if ( pm.isNull() ) pm.load(lIcon[xxi]);
	QImage image = pm.convertToImage(); image = image.smoothScale( 80, 80 ); pm.convertFromImage(image);
	IconoG->setPixmap( pm );
	Panel->moveChild(IconoG, 80, Arriba );
	Panel->moveChild(preIcono, 240, Arriba );

	newicon = lIcon[iCM+iPos-1];
	if (newicon[0]==QChar('/')) { newicon.remove( newicon.length() -8 ,8 ); newicon = newicon + "_big.png"; }
	else { newicon.remove( newicon.length() -6 ,6 ); newicon = newicon + "_big"; }
	pm = QPixmap( iconReader3.getIcon( newicon, false) ); 
	if ( pm.isNull() ) pm.load(newicon);
	if ( pm.isNull() ) pm = QPixmap( iconReader3.getIcon( lIcon[iCM+iPos-1], false) );
	if ( pm.isNull() ) pm.load(lIcon[iCM+iPos-1]);
	image = pm.convertToImage(); image = image.smoothScale( 80, 80 ); pm.convertFromImage(image);
	preIcono->setPixmap( pm );

	if ( side==1 ) lugar=-80;
	if ( side==2 ) lugar=240; 
	timer->stop();
	timer->start(5,false);
  }

}

void MyMenu::timerTick()
{
	//qApp->processEvents();
	if ( ( lugar != 80 ) && ( side == 1 ) )
	{
		Panel->moveChild(preIcono, lugar+10, Arriba );
		Panel->moveChild(IconoG, lugar+10+160, Arriba );
		lugar=lugar+10;
	}
	else if ( ( lugar != 80 ) && ( side == 2 ) )
	{
		Panel->moveChild(preIcono, lugar-10, Arriba );
		Panel->moveChild(IconoG, lugar-10-160, Arriba );
		lugar=lugar-10;
	}
	if ( lugar == 80 )
	{
		timer->stop();
	}

}

void MyMenu::RecargarIconos()
{
	QPixmap pixmap;
	CargarProgramas();
	QString textox="";
	QImage image;

	int Left;
	if ( iProgCount < 8 )
	{
		int ix = iProgCount*25;
		Left = 120 - ix/2;
	}
	else
	{
		Left = 23;
	}

	if (iPos==1) { iIsize=24; textox=Prog1; } else { iIsize=20; }
	if ( Icon1.height() > iIsize ) { image = Icon1.convertToImage(); 
	image = image.smoothScale( iIsize, iIsize ); Icon1.convertFromImage(image); }  
	if (iPos==1) { Panel->moveChild( Icono1, Left-2, iTop-4 ); }
	else { Panel->moveChild( Icono1, Left, iTop ); }
	Icono1->setPixmap(Icon1); Icono1->setAutoMask(true);

	if (iPos==2) { iIsize=24; textox=Prog2; } else { iIsize=20; }
	if ( Icon2.height() > iIsize ) { image = Icon2.convertToImage(); 
	image = image.smoothScale( iIsize, iIsize ); Icon2.convertFromImage(image); }  
	if (iPos==2) { Panel->moveChild( Icono2, Left+23, iTop-4 ); }
	else { Panel->moveChild( Icono2, Left+25, iTop ); }
	Icono2->setPixmap(Icon2);

	if (iPos==3) { iIsize=24; textox=Prog3; } else { iIsize=20; }
	if ( Icon3.height() > iIsize ) { image = Icon3.convertToImage(); 
	image = image.smoothScale( iIsize, iIsize ); Icon3.convertFromImage(image); }  
	if (iPos==3) { Panel->moveChild( Icono3, Left+48, iTop-4 ); }
	else { Panel->moveChild( Icono3, Left+50, iTop ); }
	Icono3->setPixmap(Icon3);

	if (iPos==4) { iIsize=24; textox=Prog4; } else { iIsize=20; }
	if ( Icon4.height() > iIsize ) { image = Icon4.convertToImage(); 
	image = image.smoothScale( iIsize, iIsize ); Icon4.convertFromImage(image); }  
	if (iPos==4) { Panel->moveChild( Icono4, Left+73, iTop-4 ); }
	else { Panel->moveChild( Icono4, Left+75, iTop ); }
	Icono4->setPixmap(Icon4);

	if (iPos==5) { iIsize=24; textox=Prog5; } else { iIsize=20; }
	if ( Icon5.height() > iIsize ) { image = Icon5.convertToImage(); 
	image = image.smoothScale( iIsize, iIsize ); Icon5.convertFromImage(image); }  
	if (iPos==5) { Panel->moveChild( Icono5, Left+98, iTop-4 ); }
	else { Panel->moveChild( Icono5, Left+100, iTop ); }
	Icono5->setPixmap(Icon5);

	if (iPos==6) { iIsize=24; textox=Prog6; } else { iIsize=20; }
	if ( Icon6.height() > iIsize ) { image = Icon6.convertToImage(); 
	image = image.smoothScale( iIsize, iIsize ); Icon6.convertFromImage(image); }  
	if (iPos==6) { Panel->moveChild( Icono6, Left+123, iTop-4 ); }
	else { Panel->moveChild( Icono6, Left+125, iTop ); }
	Icono6->setPixmap(Icon6);

	if (iPos==7) { iIsize=24; textox=Prog7; } else { iIsize=20; }
	if ( Icon7.height() > iIsize ) { image = Icon7.convertToImage(); 
	image = image.smoothScale( iIsize, iIsize ); Icon7.convertFromImage(image); }  
	if (iPos==7) { Panel->moveChild( Icono7, Left+148, iTop-4 ); }
	else { Panel->moveChild( Icono7, Left+150, iTop ); }
	Icono7->setPixmap(Icon7);

	if (iPos==8) { iIsize=24; textox=Prog8; } else { iIsize=20; }
	if ( Icon8.height() > iIsize ) { image = Icon8.convertToImage(); 
	image = image.smoothScale( iIsize, iIsize ); Icon8.convertFromImage(image); }  
	if (iPos==8) { Panel->moveChild( Icono8, Left+173, iTop-4 ); }
	else { Panel->moveChild( Icono8, Left+175, iTop ); }
	Icono8->setPixmap(Icon8);


	image = fizqi.convertToImage(); 
	image = image.smoothScale( 7, 7 ); fizqi.convertFromImage(image);
	Panel->moveChild( fizq, 6, iTop+6 );
	image = fderi.convertToImage(); 
	image = image.smoothScale( 7, 7 ); fderi.convertFromImage(image);
	Panel->moveChild( fder, 227, iTop+6 );
	fizq->setPixmap(fizqi);
	fder->setPixmap(fderi);

	int y = this->headerSize().height();
	y = y + softKey->height();
	
	eti1->setText(textox);
	ChangeIcon();

}


void MyMenu::CargarProgramas()
{
	ZConfig Vini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
    
	if ( iProgCount >= 0+iCM ) Prog1 = lProg[0+iCM]; else Prog1 = "";
	if ( iProgCount >= 1+iCM ) Prog2 = lProg[1+iCM]; else Prog2 = "";
	if ( iProgCount >= 2+iCM ) Prog3 = lProg[2+iCM]; else Prog3 = "";
	if ( iProgCount >= 3+iCM ) Prog4 = lProg[3+iCM]; else Prog4 = "";
	if ( iProgCount >= 4+iCM ) Prog5 = lProg[4+iCM]; else Prog5 = "";
	if ( iProgCount >= 5+iCM ) Prog6 = lProg[5+iCM]; else Prog6 = "";
	if ( iProgCount >= 6+iCM ) Prog7 = lProg[6+iCM]; else Prog7 = "";
	if ( iProgCount >= 7+iCM ) Prog8 = lProg[7+iCM]; else Prog8 = "";

	if ( iProgCount >= 0+iCM ) Exec1 = lExec[0+iCM]; else Exec1 = "";
	if ( iProgCount >= 1+iCM ) Exec2 = lExec[1+iCM]; else Exec2 = "";
	if ( iProgCount >= 2+iCM ) Exec3 = lExec[2+iCM]; else Exec3 = "";
	if ( iProgCount >= 3+iCM ) Exec4 = lExec[3+iCM]; else Exec4 = "";
	if ( iProgCount >= 4+iCM ) Exec5 = lExec[4+iCM]; else Exec5 = "";
	if ( iProgCount >= 5+iCM ) Exec6 = lExec[5+iCM]; else Exec6 = "";
	if ( iProgCount >= 6+iCM ) Exec7 = lExec[6+iCM]; else Exec7 = "";
	if ( iProgCount >= 7+iCM ) Exec8 = lExec[7+iCM]; else Exec8 = "";

	if ( iProgCount >= 0+iCM ) Acc1 = lAccion[0+iCM]; else Acc1 = "";
	if ( iProgCount >= 1+iCM ) Acc2 = lAccion[1+iCM]; else Acc2 = "";
	if ( iProgCount >= 2+iCM ) Acc3 = lAccion[2+iCM]; else Acc3 = "";
	if ( iProgCount >= 3+iCM ) Acc4 = lAccion[3+iCM]; else Acc4 = "";
	if ( iProgCount >= 4+iCM ) Acc5 = lAccion[4+iCM]; else Acc5 = "";
	if ( iProgCount >= 5+iCM ) Acc6 = lAccion[5+iCM]; else Acc6 = "";
	if ( iProgCount >= 6+iCM ) Acc7 = lAccion[6+iCM]; else Acc7 = "";
	if ( iProgCount >= 7+iCM ) Acc8 = lAccion[7+iCM]; else Acc8 = "";

	QString val; int i;
	if ( iProgCount >= 0+iCM ) {
		val = lIcon[0+iCM]; i = val.findRev("/"); if ( i > -1 ) { if ( val[0] != QChar('/') ) { val=getFilePath(Exec1)+val; } 
		Icon1.load(val); } else { Icon1 = QPixmap( iconReader3.getIcon(val, false) ); }
	} else Icon1.load(ProgDir2 + "/null.png");

	if ( iProgCount >= 1+iCM ) {
		val = lIcon[1+iCM]; i = val.findRev("/"); if ( i > -1 ) { if ( val[0] != QChar('/') ) { val=getFilePath(Exec2)+val; } 
		Icon2.load(val); } else { Icon2 = QPixmap( iconReader3.getIcon(val, false) ); }
	} else Icon2.load(ProgDir2 + "/null.png");

	if ( iProgCount >= 2+iCM ) {
		val = lIcon[2+iCM]; i = val.findRev("/"); if ( i > -1 ) { if ( val[0] != QChar('/') ) { val=getFilePath(Exec3)+val; } 
		Icon3.load(val); } else { Icon3 = QPixmap( iconReader3.getIcon(val, false) ); }
	}  else Icon3.load(ProgDir2 + "/null.png");

	if ( iProgCount >= 3+iCM ) {
		val = lIcon[3+iCM]; i = val.findRev("/"); if ( i > -1 ) { if ( val[0] != QChar('/') ) { val=getFilePath(Exec4)+val; } 
		Icon4.load(val); } else { Icon4 = QPixmap( iconReader3.getIcon(val, false) ); }
	} else Icon4.load(ProgDir2 + "/null.png");

	if ( iProgCount >= 4+iCM ) {
		val = lIcon[4+iCM]; 
		i = val.findRev("/"); if ( i > -1 ) { if ( val[0] != QChar('/') ) { val=getFilePath(Exec5)+val; } 
		Icon5.load(val); } else { Icon5 = QPixmap( iconReader3.getIcon(val, false) ); }
	} else Icon5.load(ProgDir2 + "/null.png");

	if ( iProgCount >= 5+iCM ) {
		val = lIcon[5+iCM]; i = val.findRev("/"); if ( i > -1 ) { if ( val[0] != QChar('/') ) { val=getFilePath(Exec6)+val; } 
		Icon6.load(val); } else { Icon6 = QPixmap( iconReader3.getIcon(val, false) ); }
	} else Icon6.load(ProgDir2 + "/null.png");

	if ( iProgCount >= 6+iCM ) {
		val = lIcon[6+iCM]; i = val.findRev("/"); if ( i > -1 ) { if ( val[0] != QChar('/') ) { val=getFilePath(Exec7)+val; } 
		Icon7.load(val); } else { Icon7 = QPixmap( iconReader3.getIcon(val, false) ); }
	} else Icon7.load(ProgDir2 + "/null.png");

	if ( iProgCount >= 7+iCM ) {
		val = lIcon[7+iCM]; i = val.findRev("/"); if ( i > -1 ) { if ( val[0] != QChar('/') ) { val=getFilePath(Exec8)+val; } 
		Icon8.load(val); } else { Icon8 = QPixmap( iconReader3.getIcon(val, false) ); }
	} else Icon8.load(ProgDir2 + "/null.png");


	if ( iCM > 0 ) fizqi = QPixmap( iconReader3.getIcon("left_nav_icon_std", false) );
	else fizqi.load( "/ezxlocal/LinXtend/usr/bin/z6manager/images/null.png" );

	if ( iCM < iProgCount-8 ) fderi = QPixmap( iconReader3.getIcon("right_nav_icon_std", false) );
	else fderi.load( "/ezxlocal/LinXtend/usr/bin/z6manager/images/null.png" );


}


void MyMenu::CargarData(QString Carpeta)
{
	iCM=0; iPos=1;
	currentMenu2 = Carpeta;
	lProg.clear();
	lIcon.clear();
	lExec.clear();
	lAccion.clear();
	lista.clear();
	int basura=0;

	ZConfig userMenuTree ( "/ezxlocal/download/appwrite/am/UserMenuTree", true );
    lista = userMenuTree.readListEntry ( Carpeta, QString("Items"), QChar(';') );
	ZConfig IDini( "/tmp/AMmenu", true );
	ZConfig lang ( Idioma3, true );

	QString titulo;
	if ( Carpeta == "2a58c0d6-05c6-45aa-8938-63059d41a4e9")
	{ this->setMainWidgetTitle( lang.readEntry( QString("TRANSLATE"), "MAIN", "Main Menu" ) ); }
	else
	{
		QString temp = IDini.readEntry( Carpeta, QString("Name"), "" );
		this->setMainWidgetTitle( lang.readEntry( QString("TRANSLATE"), temp, temp ) );
	}

	for (int j=0;j<lista.count();++j)
	{
		QString name = IDini.readEntry( QString("%1").arg(lista[j]), QString("Name"), "" );
		QString Nval = lang.readEntry( QString("TRANSLATE"), name, name );
		QString dir = IDini.readEntry( QString("%1").arg(lista[j]), QString("Directory"), "" );
		QString ico = IDini.readEntry( QString("%1").arg(lista[j]), QString("BigIcon"), "" );
		QString type = IDini.readEntry( QString("%1").arg(lista[j]), QString("Type"), "" );
		QString accion = IDini.readEntry( QString("%1").arg(lista[j]), QString("Action"), "-" );
		if ( (name!="") )
		{
			int i = dir.findRev("../.."); if ( i==0 ) { dir.remove(0,5); }
			i = ico.findRev("/"); if ( i > -1 ) { if ( ico[0] != QChar('/') ) { ico=dir+"/"+ico; } }
			lProg += Nval;
			lIcon += ico;
			lAccion += accion;
			lExec += lista[j];
			if (Level==0) { iPos=lastPos1; iCM=lastCM1; }
			else if (Level==1) { iPos=lastPos2; iCM=lastCM2; }
			else if (Level==2) { iPos=lastPos3; iCM=lastCM3; }
			else if (Level==3) { iPos=lastPos4; iCM=lastCM4; }
			else if (Level==4) { iPos=lastPos5; iCM=lastCM5; }
		}
		else
		{ 
			++basura;
		}
	}

	iProgCount = lista.count() - basura;


}


void MyMenu::accept()
{
	//brClicked();
}

QString MyMenu::getFileName(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( 0, i);
  if (str.left(1) = "/") str.remove ( 0, 1);
  return str;    
}

QString MyMenu::getFilePath(QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( i+1, str.length() - i );
  return str;  
}

void MyMenu::slot_cancel()
{
  if ( currentMenu2 == "2a58c0d6-05c6-45aa-8938-63059d41a4e9" )
  {
	reject();
  }
  else
  {
	int i = lastMenu2.findRev ( ";" ); lastMenu2.remove ( i, lastMenu2.length() - i );
	QString nMenu = lastMenu2;
	i = nMenu.findRev ( ";" ); if ( i > 0 ) { nMenu.remove ( 0, i+1 ); }
	Level=Level-1;
	side=0;
	CargarData(nMenu);
	RecargarIconos();
  }

}

void MyMenu::keyPressEvent(QKeyEvent* k)
{
	if ( k->key() == Z6KEY_LEFT )
	{
		--iPos; side=1;
		if ( (iPos<1 ) && (iProgCount<=8) ) { iPos=iProgCount; }
		else if (iPos<1) { iCM--; if ( iCM<0 ) { iCM=iProgCount-8; iPos=8; } else iPos=1; }
		RecargarIconos();
	}
	else if ( k->key() == Z6KEY_RIGHT )
	{
		++iPos; side=2;
		if ( (iPos>iProgCount ) && (iProgCount<=8) ) { iPos=1; }
		else if (iPos>8) { iCM++; if ( iCM>iProgCount-8 ) { iCM=0; iPos=1; } else iPos=8; }
		RecargarIconos();
	}
	else if ( k->key() == Z6KEY_CENTER )
	{
		side=0; ChangeIcon();
		mClicked();
	}
	else if ( ( k->key() == Z6KEY_RED ) || ( k->key() == SLIDER_CLOSE ) )
	{
		reject();
	}

}

void MyMenu::mClicked()
{
  QString run;
  if (iPos==1) { run = Exec1; } if (iPos==2) { run = Exec2; } if (iPos==3) { run = Exec3; }
  if (iPos==4) { run = Exec4; } if (iPos==5) { run = Exec5; } if (iPos==6) { run = Exec6; }
  if (iPos==7) { run = Exec7; } if (iPos==8) { run = Exec8; }
  if (run != "") { 
	ZConfig IDini( "/tmp/AMmenu", true );
	QString Tipo = IDini.readEntry( QString(run), QString("Type"), "" );
	if ( Tipo == "1" )
	{
		if (Level==0) { Level=1; lastPos1=iPos; lastCM1=iCM;
		  lastCM2=0; lastCM3=0; lastCM4=0; lastCM5=0; lastPos2=1; lastPos3=1; lastPos4=1; lastPos5=1;
		}
		else if (Level==1) { Level=2; lastPos2=iPos; lastCM2=iCM;
		  lastCM3=0; lastCM4=0; lastCM5=0; lastPos3=1; lastPos4=1; lastPos5=1;
		}
		else if (Level==2) { Level=3; lastPos3=iPos; lastCM3=iCM;
		  lastCM4=0; lastCM5=0; lastPos4=1; lastPos5=1;
		}
		else if (Level==3) { Level=4; lastPos4=iPos; lastCM4=iCM;
		  lastCM5=0; lastPos5=1;
		}
		else if (Level==4) { Level=5; lastPos5=iPos; lastCM5=iCM; }

		else if (Level==5) { Level=6; }

		lastMenu2 = lastMenu2 + ";" + run;
		lastMenuX = currentMenu2;
		CargarData(run);
		RecargarIconos();
	}
	else if ( Tipo == "4" )
	{
		QString act;
		if (iPos==1) { act = Acc1; } if (iPos==2) { act = Acc2; } if (iPos==3) { act = Acc3; }
		if (iPos==4) { act = Acc4; } if (iPos==5) { act = Acc5; } if (iPos==6) { act = Acc6; }
		if (iPos==7) { act = Acc7; } if (iPos==8) { act = Acc8; }
		AM_ActionData ad;
		ad.setAction(act);
		ad.invoke();    
	}
	else
	{
		QString run2 = "{" + run + "}";
		QRegExp rx( "{*-*-*-*-*}" ); if ( run2.find(rx)!=-1 )
		{ ZMsg msg( "/AM/Launcher", "launchApp" ); msg << (QUuid)run2; msg.send(); } 
	}
  }
}



