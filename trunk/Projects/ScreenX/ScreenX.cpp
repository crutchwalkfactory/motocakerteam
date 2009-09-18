#include "ScreenX.h"
#include "GUI_Define.h"
#include "BaseDlg.h"
#include "myam.h"
#include "zsms.h"

#include "stdio.h"
#include <stdlib.h>
#include <unistd.h>
#include <qpixmap.h>

#include <ZApplication.h>
#include <ZListBox.h>
#include <ZSoftKey.h>
#include <ZOptionsMenu.h>
#include <ZMessageDlg.h>

#include <qmap.h>
#include <qfile.h>
#include <qcopchannel_qws.h>

#include <dirent.h>
#include <fcntl.h>
#include "ZMsg.h"
#include "ZDefs.h"
#include <qevent.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <signal.h>
#include <sys/wait.h>
#include <ZConfig.h>
#include "exec.h"
#include <qregexp.h>
#include <qimage.h>
#include <ezxres.h>

QString Idioma;
QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
RES_ICON_Reader iconReader;
int Cambio, Pos, prePos;
int Isize = 30;
int Top = 5;
int Left1 = 21;
int Left2 = 62;
int Left3 = 103;
int Left4 = 144;
int Left5 = 185;
QString keyLocked="NO";


extern "C" int UTIL_GetSliderStatus(void);


extern ZApplication *app;


ScreenX::ScreenX ( QWidget* parent, const char* name, WFlags fl )
    : ZKbMainWidget ( ZHeader::TINY_TYPE, NULL, "ZMainWidget", WType_TopLevel )
{
	ProgDir = getProgramDir();

	QString val; ZConfig PREini(QString("%1/am2.cfg").arg(ProgDir), false);
	val = PREini.readEntry(QString("AM"), QString("Language"), "");
	if ( val == "" ) {
		ZConfig ini(QString("/ezxlocal/download/mystuff/.system/LinXtend/usr/CurrentLanguage.cfg"), false);
		val = ini.readEntry(QString("LINUXMOD"), QString("Language"), "");
		Idioma = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + val;
		toLog(QString("Current Language: %1").arg(Idioma) );
	} else {
		Idioma = ProgDir + "/languages/" + val;
	}
	
	system("cat /mmc/mmca1/.system/java/CardRegistry /ezxlocal/download/appwrite/am/UserRegistry /ezxlocal/download/appwrite/am/InstalledDB /usr/data_resource/preloadapps/SysRegistry > /tmp/AMmenu");
 
	system(QString("mount -o bind %1/vr /ezxlocal/LinXtend/usr/bin/vr").arg(ProgDir));

	CreateWindow ( parent );
	( ( ZApplication* ) qApp )->showMainWidget ( this );
}

ScreenX::~ScreenX()
{
	system("umount /ezxlocal/LinXtend/usr/bin/vr");
    delete menu;
    delete softKey;
    delete tabWidget;
}

void ScreenX::CreateWindow ( QWidget* parent )
{
	toLog(QString("CreateWindow: Start"));

	QPixmap pixmap;
	Panel = new ZScrollPanel (this , NULL , 0 ,  (ZSkinService::WidgetClsID)4);
	Panel->resize ( 240, 320 );

	CargarProgramas();

	if ( Icon1.height() > Isize ) { QImage image; image = Icon1.convertToImage(); 
	image = image.smoothScale( Isize, Isize ); Icon1.convertFromImage(image); }  
	Icono1 = new ZImage(this); Icono1->setPixmap(Icon1);
	Panel->addChild(Icono1, Left1, Top);

	if ( Icon2.height() > Isize ) { QImage image; image = Icon2.convertToImage(); 
	image = image.smoothScale( Isize, Isize ); Icon2.convertFromImage(image); }  
	Icono2 = new ZImage(this); Icono2->setPixmap(Icon2);
	Panel->addChild(Icono2, Left2, Top);

	if ( Icon3.height() > Isize ) { QImage image; image = Icon3.convertToImage(); 
	image = image.smoothScale( Isize, Isize ); Icon3.convertFromImage(image); }  
	Icono3 = new ZImage(this); Icono3->setPixmap(Icon3);
	Panel->addChild(Icono3, Left3, Top);

	if ( Icon4.height() > Isize ) { QImage image; image = Icon4.convertToImage(); 
	image = image.smoothScale( Isize, Isize ); Icon4.convertFromImage(image); }  
	Icono4 = new ZImage(this); Icono4->setPixmap(Icon4);
	Panel->addChild(Icono4, Left4, Top);

	if ( Icon5.height() > Isize ) { QImage image; image = Icon5.convertToImage(); 
	image = image.smoothScale( Isize, Isize ); Icon5.convertFromImage(image); }  
	Icono5 = new ZImage(this); Icono5->setPixmap(Icon5);
	Panel->addChild(Icono5, Left5, Top);

	connect( app, SIGNAL(sliderStatusChanged(int)), this, SLOT(Slider(int)) );
	connect( app, SIGNAL(askReturnToIdle(int)), this, SLOT(Salir(int)) );
	connect( app, SIGNAL(signalThemeChange()), this, SLOT(Salir(int)) );
	connect( app, SIGNAL(systemMessage()), this, SLOT(Noticia()) );

	setContentWidget ( Panel );
	//setFullScreenMode(true, true);

	softKey = new ZSoftKey ( NULL , this , this );
	Create_MenuApp();
	Create_SoftKey();
	setSoftKey ( softKey );
	
}


void ScreenX::Noticia()
{
  toLog( QString("Noticia!") );

}

void ScreenX::keyPressEvent(QKeyEvent* k)
{
  toLog( QString("Button Pressed: %1").arg(k->key()) );

  //ZHeader smss;
  //int xsms = smss.getStatusVal(ZHeader::NOTIFY);
  

  if ( keyLocked == "NO" )
  {

	if ( k->key() == Z6KEY_LEFT )
	{
		prePos=Pos; --Pos; if (Pos<1) { Pos=1; } Recargar();
	}
	else if ( k->key() == Z6KEY_RIGHT )
	{
		prePos=Pos; ++Pos; if (Pos>5) { Pos=5; } Recargar();
	}
	else if ( k->key() == Z6KEY_CENTER )
	{
		QString run;
		if (Pos==1) { run = Exec1; } if (Pos==2) { run = Exec2; } if (Pos==3) { run = Exec3; }
		if (Pos==4) { run = Exec4; } if (Pos==5) { run = Exec5; }
		if (run != "") { 
			QRegExp rx( "{*-*-*-*-*}" ); if ( run.find(rx)!=-1 )
			{ ZMsg msg( "/AM/Launcher", "launchApp4NoLockCheck" ); msg << (QUuid)run; msg.send(); } 
		}
	}
	else if ( k->key() == Z6KEY_RED )
	{
		Pos=0;
		Recargar();

	}
	else if ( k->key() == Z6KEY_SIDE_SELECT )
	{
		ZConfig Vini(QString(ProgDir + "/am2.cfg"), false);
		QString run = Vini.readEntry(QString("PROGRAMS"), QString("Exec6"), "");
		if (run != "") { 
			QRegExp rx( "{*-*-*-*-*}" ); if ( run.find(rx)!=-1 )
			{ ZMsg msg( "/AM/Launcher", "launchApp4NoLockCheck" ); msg << (QUuid)run; msg.send(); } 
		}
	}
	else if ( k->key() == Z6KEY_MUSIC )
	{
		printf("PUTO!\n");
		ZConfig Vini(QString(ProgDir + "/am2.cfg"), false);
		QString run = Vini.readEntry(QString("PROGRAMS"), QString("Exec7"), "");
		if (run != "") { 
			QRegExp rx( "{*-*-*-*-*}" ); if ( run.find(rx)!=-1 )
			{ ZMsg msg( "/AM/Launcher", "launchApp4NoLockCheck" ); msg << (QUuid)run; msg.send(); } 
		}
	}
	else if ( ( k->key() == Z6KEY_VOLUP ) | ( k->key() == Z6KEY_VOLDOWN ) )
	{
		/*ZConfig Vini(QString("/mmc/mmca1/bin/am2.cfg"), false);
		QString run = Vini.readEntry(QString("PROGRAMS"), QString("Exec8"), "");
		if (run != "") { 
			QRegExp rx( "{*-*-*-*-*}" ); if ( run.find(rx)!=-1 )
			{ ZMsg msg( "/AM/Launcher", "launchApp4NoLockCheck" ); msg << (QUuid)run; msg.send(); } 
		}*/
		/*SETUP_Utility pepe;
		int xi; pepe.getActiveProfile();*/

	}
	else if ( k->key() == Z6KEY_GREEN )
	{
		QString run = "{ce59717d-fb23-4b7c-8800-25ce613f4042}";
		ZMsg msg( "/AM/Launcher", "launchApp4NoLockCheck" ); msg << (QUuid)run; msg.send();

		//toLog( QString("Green key...") );
		//AM_LauncherInterface pepe = new AM_LauncherInterface(0,0);
		//pepe->autoOpenWithAnotherApp( (QUuid)run, 0,0 );
	}
	else if ( k->key() == Z6KEY_CAMERA )
	{
		ZConfig Vini(QString(ProgDir + "/am2.cfg"), false);
		QString run = Vini.readEntry(QString("PROGRAMS"), QString("Exec9"), "");
		if (run != "") { 
			QRegExp rx( "{*-*-*-*-*}" ); if ( run.find(rx)!=-1 )
			{ ZMsg msg( "/AM/Launcher", "launchApp4NoLockCheck" ); msg << (QUuid)run; msg.send(); } 
		}
	}
	else // Teclas numericas para marcar numero telefonico
	{


	}
  }

}

void ScreenX::Slider(int r)
{
	toLog( QString("Slider Status: %1").arg(UTIL_GetSliderStatus()) );
	if ( (UTIL_GetSliderStatus()==0)&&(keyLocked=="YES") ) { keyLocked="NO"; Create_SoftKey(); }
}

void ScreenX::Salir(int r)
{
	Pos=0;
	Recargar();

	toLog( QString("Slider Closed...") );
	if ( QString("%1").arg(app->getAppChannelName()) == "EZX/Application/AM" ) 
	{
		toLog( QString("Forced %1 to stay loaded").arg(app->getAppChannelName()) );
		app->exec(); 
	}

}

void ScreenX::Recargar()
{

	QPixmap pixmap;
	CargarProgramas();

	if ( Pos == 0 ) { 
		Isize=30; 
		if ( Icon1.height() > Isize ) { QImage image; image = Icon1.convertToImage(); 
		image = image.smoothScale( Isize, Isize ); Icon1.convertFromImage(image); }  
		Panel->moveChild( Icono1, Left1, Top ); 
		Icono1->setPixmap(Icon1);
		if ( Icon2.height() > Isize ) { QImage image; image = Icon2.convertToImage(); 
		image = image.smoothScale( Isize, Isize ); Icon2.convertFromImage(image); }  
		Panel->moveChild( Icono2, Left2, Top ); 
		Icono2->setPixmap(Icon2);
		if ( Icon3.height() > Isize ) { QImage image; image = Icon3.convertToImage(); 
		image = image.smoothScale( Isize, Isize ); Icon3.convertFromImage(image); }  
		Panel->moveChild( Icono3, Left3, Top ); 
		Icono3->setPixmap(Icon3);
		if ( Icon4.height() > Isize ) { QImage image; image = Icon4.convertToImage(); 
		image = image.smoothScale( Isize, Isize ); Icon4.convertFromImage(image); }  
		Panel->moveChild( Icono4, Left4, Top ); 
		Icono4->setPixmap(Icon4);
		if ( Icon5.height() > Isize ) { QImage image; image = Icon5.convertToImage(); 
		image = image.smoothScale( Isize, Isize ); Icon5.convertFromImage(image); }  
		Panel->moveChild( Icono5, Left5, Top ); 
		Icono5->setPixmap(Icon5);

	}

	if ( Pos == 1 ) { 
		Isize=40; 
		if ( Icon1.height() > Isize ) { QImage image; image = Icon1.convertToImage(); 
		image = image.smoothScale( Isize, Isize ); Icon1.convertFromImage(image); }  
		Panel->moveChild( Icono1, Left1-5, Top-5 );
		Icono1->setPixmap(Icon1);
		Isize=30; 
		if ( Icon2.height() > Isize ) { QImage image; image = Icon2.convertToImage(); 
		image = image.smoothScale( Isize, Isize ); Icon2.convertFromImage(image); }  
		Panel->moveChild( Icono2, Left2, Top ); 
		Icono2->setPixmap(Icon2);
	}	

	if ( Pos == 2 ) { 
		Isize=40; 
		if ( Icon2.height() > Isize ) { QImage image; image = Icon2.convertToImage(); 
		image = image.smoothScale( Isize, Isize ); Icon2.convertFromImage(image); }  
		Panel->moveChild( Icono2, Left2-5, Top-5 );
		Icono2->setPixmap(Icon2);
		Isize=30;
		if ( prePos == 1 )
		{
			if ( Icon1.height() > Isize ) { QImage image; image = Icon1.convertToImage(); 
			image = image.smoothScale( Isize, Isize ); Icon1.convertFromImage(image); }  
			Panel->moveChild( Icono1, Left1, Top ); 
			Icono1->setPixmap(Icon1);
		}
		if ( prePos == 3 )
		{
			if ( Icon3.height() > Isize ) { QImage image; image = Icon3.convertToImage(); 
			image = image.smoothScale( Isize, Isize ); Icon3.convertFromImage(image); }  
			Panel->moveChild( Icono3, Left3, Top ); 
			Icono3->setPixmap(Icon3);
		}
	}	

	if ( Pos == 3 ) { 
		Isize=40; 
		if ( Icon3.height() > Isize ) { QImage image; image = Icon3.convertToImage(); 
		image = image.smoothScale( Isize, Isize ); Icon3.convertFromImage(image); }  
		Panel->moveChild( Icono3, Left3-5, Top-5 );
		Icono3->setPixmap(Icon3);
		Isize=30;
		if ( prePos == 2 )
		{
			if ( Icon2.height() > Isize ) { QImage image; image = Icon2.convertToImage(); 
			image = image.smoothScale( Isize, Isize ); Icon2.convertFromImage(image); }  
			Panel->moveChild( Icono2, Left2, Top ); 
			Icono2->setPixmap(Icon2);
		}
		if ( prePos == 4 )
		{
			if ( Icon4.height() > Isize ) { QImage image; image = Icon4.convertToImage(); 
			image = image.smoothScale( Isize, Isize ); Icon4.convertFromImage(image); }  
			Panel->moveChild( Icono4, Left4, Top ); 
			Icono4->setPixmap(Icon4);
		}
	}	

	if ( Pos == 4 ) { 
		Isize=40; 
		if ( Icon4.height() > Isize ) { QImage image; image = Icon4.convertToImage(); 
		image = image.smoothScale( Isize, Isize ); Icon4.convertFromImage(image); }  
		Panel->moveChild( Icono4, Left4-5, Top-5 );
		Icono4->setPixmap(Icon4);
		Isize=30;
		if ( prePos == 3 )
		{
			if ( Icon3.height() > Isize ) { QImage image; image = Icon3.convertToImage(); 
			image = image.smoothScale( Isize, Isize ); Icon3.convertFromImage(image); }  
			Panel->moveChild( Icono3, Left3, Top ); 
			Icono3->setPixmap(Icon3);
		}
		if ( prePos == 5 )
		{
			if ( Icon5.height() > Isize ) { QImage image; image = Icon5.convertToImage(); 
			image = image.smoothScale( Isize, Isize ); Icon5.convertFromImage(image); }  
			Panel->moveChild( Icono5, Left5, Top ); 
			Icono5->setPixmap(Icon5);
		}
	}	

	if ( Pos == 5 ) { 
		Isize=40; 
		if ( Icon5.height() > Isize ) { QImage image; image = Icon5.convertToImage(); 
		image = image.smoothScale( Isize, Isize ); Icon5.convertFromImage(image); }  
		Panel->moveChild( Icono5, Left5-5, Top-5 );
		Icono5->setPixmap(Icon5);
		Isize=30; 
		if ( Icon4.height() > Isize ) { QImage image; image = Icon4.convertToImage(); 
		image = image.smoothScale( Isize, Isize ); Icon4.convertFromImage(image); }  
		Panel->moveChild( Icono4, Left4, Top ); 
		Icono4->setPixmap(Icon4);
	}	
	

}

void ScreenX::CargarProgramas()
{
	ZConfig Vini(QString(ProgDir + "/am2.cfg"), false);
	Prog1 = Vini.readEntry(QString("PROGRAMS"), QString("Program1"), "");
	Prog2 = Vini.readEntry(QString("PROGRAMS"), QString("Program2"), "");
	Prog3 = Vini.readEntry(QString("PROGRAMS"), QString("Program3"), "");
	Prog4 = Vini.readEntry(QString("PROGRAMS"), QString("Program4"), "");
	Prog5 = Vini.readEntry(QString("PROGRAMS"), QString("Program5"), "");

	Exec1 = Vini.readEntry(QString("PROGRAMS"), QString("Exec1"), "");
	Exec2 = Vini.readEntry(QString("PROGRAMS"), QString("Exec2"), "");
	Exec3 = Vini.readEntry(QString("PROGRAMS"), QString("Exec3"), "");
	Exec4 = Vini.readEntry(QString("PROGRAMS"), QString("Exec4"), "");
	Exec5 = Vini.readEntry(QString("PROGRAMS"), QString("Exec5"), "");

	QString val = Vini.readEntry(QString("PROGRAMS"), QString("Icon1"), "");
	int i = val.findRev("/"); if ( i > -1 ) {
		if ( val[0] != QChar('/') ) { val=getFilePath(Exec1)+val; } Icon1.load(val);
	} else { Icon1 = QPixmap( iconReader.getIcon(val, false) ); }
	val = Vini.readEntry(QString("PROGRAMS"), QString("Icon2"), "");
	i = val.findRev("/"); if ( i > -1 ) {
		if ( val[0] != QChar('/') ) { val=getFilePath(Exec2)+val; } Icon2.load(val);
	} else { Icon2 = QPixmap( iconReader.getIcon(val, false) ); }
	val = Vini.readEntry(QString("PROGRAMS"), QString("Icon3"), ""); 
	i = val.findRev("/"); if ( i > -1 ) {
		if ( val[0] != QChar('/') ) { val=getFilePath(Exec3)+val; } Icon3.load(val);
	} else { Icon3 = QPixmap( iconReader.getIcon(val, false) ); }
	val = Vini.readEntry(QString("PROGRAMS"), QString("Icon4"), "");
	i = val.findRev("/"); if ( i > -1 ) {
		if ( val[0] != QChar('/') ) { val=getFilePath(Exec4)+val; } Icon4.load(val);
	} else { Icon4 = QPixmap( iconReader.getIcon(val, false) ); }
	val = Vini.readEntry(QString("PROGRAMS"), QString("Icon5"), "");
	i = val.findRev("/"); if ( i > -1 ) {
		if ( val[0] != QChar('/') ) { val=getFilePath(Exec5)+val; } Icon5.load(val);
	} else { Icon5 = QPixmap( iconReader.getIcon(val, false) ); }


}

QString ScreenX::getFilePath(QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( i+1, str.length() - i );
  return str;  
}

void ScreenX::mainmenu()
{
	CargarMenu("Principal");
}

void ScreenX::CargarMenu(QString lugar)
{

  ZConfig ini(ProgDir + "/am2.cfg", true);
  int val = ini.readNumEntry(QString("AM"), QString("Menu"), 0);

  if ( val == 1 )
  {
	qApp->processEvents();
	toLog(QString("Mostrando menu..."));
	ZMainMenu *zfod = new ZMainMenu(lugar);
	zfod->exec();
  }
  else
  {
	AM_MainMenuInterface pepe;
	pepe.ShowMainMenu();
  }

}

QString ScreenX::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("SETTINGS"), QString("UNICODE"), "");
	if ( IDval == "1" ) { return txtcodec->toUnicode(xString); }
	else { return xString; }
}

void ScreenX::Create_SoftKey()
{
  QString IDval; QString IDval2; ZConfig IDini(Idioma, false);
  softKey->setOptMenu ( ZSoftKey::LEFT, menu ); 
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_MENU"), "");
  softKey->setText ( ZSoftKey::LEFT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_MENU"), "");
  softKey->setTextForOptMenuHide(IDval);
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_SELECT"), "");
  IDval2 = IDini.readEntry(QString("COMMON"), QString("MENU_CANCEL"), "");
  softKey->setTextForOptMenuShow(IDval, IDval2);
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_MAIN"), "");
  softKey->setText ( ZSoftKey::RIGHT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( mainmenu() ) );      
}

void ScreenX::Create_MenuApp()
{
  QString IDval; ZConfig IDini(Idioma, false);
  QRect rect;
  QPixmap* pm  = new QPixmap();
  int idx = 0;
  menu = new ZOptionsMenu ( rect, softKey, NULL, 0);
  menu->setItemSpacing(10);  
  pm->load(ProgDir + "images/about.png");
  IDval = IDini.readEntry(QString("MAINMENU"), QString("LOCK"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( bloquear() ) );
  ++idx;
  pm->load(ProgDir + "images/about.png");
  IDval = IDini.readEntry(QString("MAINMENU"), QString("CAPTURE"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( CamButton() ) );      
  ++idx;
  pm->load(ProgDir + "images/about.png");
  IDval = IDini.readEntry(QString("MAINMENU"), QString("CREATESMS"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( CreateSMS() ) );      
  ++idx;
  pm->load(ProgDir + "images/about.png");
  SETUP_Utility pepe; int i = pepe.isAirplaneModeOn();
  if ( i==1 ) { IDval = IDini.readEntry(QString("MAINMENU"), QString("PLANEMODEOFF"), ""); }
  else { IDval = IDini.readEntry(QString("MAINMENU"), QString("PLANEMODEON"), ""); }
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( ModoAvion() ) );      
  ++idx;
  pm->load(ProgDir + "images/about.png");
  IDval = IDini.readEntry(QString("MAINMENU"), QString("SCREEN"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( ScreenOptions() ) );      
  ++idx;
  pm->load(ProgDir + "images/about.png");
  IDval = IDini.readEntry(QString("MAINMENU"), QString("ALARM"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( Alarm() ) );      
/*  ++idx;
  pm->load(ProgDir + "images/about.png");
  IDval = IDini.readEntry(QString("MAINMENU"), QString("SETTINGS"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( Settings() ) );      
*/
}

void ScreenX::bloquear()
{
  qApp->processEvents();
  QString IDval; QString IDval2; ZConfig IDini(Idioma, false);
  softKey->setOptMenu ( ZSoftKey::LEFT, NULL ); 
  softKey->setText ( ZSoftKey::LEFT, "", ( ZSoftKey::TEXT_PRIORITY ) 0 );
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_UNLOCK"), "");
  softKey->setText ( ZSoftKey::RIGHT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( Desbloquear() ) );  
  keyLocked="YES";
}

void ScreenX::Desbloquear()
{
	qApp->processEvents();
	ZConfig IDini(Idioma, false);
	QString IDval1 = IDini.readEntry(QString("TEXT"), QString("UNLOCK_TITLE"), "");
	QString IDval2 = IDini.readEntry(QString("TEXT"), QString("UNLOCK_TEXT"), "");
	ZMessageDlg* msg =  new ZMessageDlg ( IDval1, IDval2, ZMessageDlg::just_ok, 3000, NULL, "ZMessageDlg", true, 0 );
	msg->setTitleIcon(""); int i = msg->exec(); delete msg; msg = NULL;
	if (i==1) { keyLocked="NO"; Create_SoftKey(); }
}

void ScreenX::Alarm()
{
	qApp->processEvents();
	system("/usr/SYSqtapp/alarmclock/alarmclock");
	
    /*AM_ActionData ad;
    ad.setAction("dial");
    ad.setValue("number", "");
    int r = ad.invoke(true);*/

}

void ScreenX::CamButton()
{
	qApp->processEvents();
	system("/ezxlocal/LinXtend/usr/bin/camera");
}

void ScreenX::CreateSMS()
{
	ZSMS pepe;
	ZSMS_OutgoingMsg mail(QString(""),QString(""));
	pepe.composeMsg(mail);

}

void ScreenX::ModoAvion()
{
	qApp->processEvents();
	QString IDval; ZConfig IDini(Idioma, false);
	SETUP_Utility pepe;
	int i = pepe.isAirplaneModeOn();
	if ( i==1 )
	{
		pepe.setAirplaneMode(false); 
		IDval = IDini.readEntry(QString("MAINMENU"), QString("PLANEMODEOFF"), "");
		menu->changeItem(3, IDval, NULL);
		toLog( QString("Airplane mode off") );
	}
	else
	{
		pepe.setAirplaneMode(true);
		IDval = IDini.readEntry(QString("MAINMENU"), QString("PLANEMODEOFF"), "");
		menu->changeItem(3, IDval, NULL);
		toLog( QString("Airplane mode on") );
	}


}


void ScreenX::ScreenOptions()
{
	qApp->processEvents();
	ZScreen *zfod = new ZScreen();
	zfod->exec();


}

void ScreenX::Settings()
{
	qApp->processEvents();
	ZSettings *zfod = new ZSettings();
	zfod->exec();
	CargarProgramas();
	Recargar();

}

void ScreenX::toLog(QString mes, bool r1, bool r2)
{
	if (r1)
		qDebug ( "====================================================================" );
	if (mes != NULL)
		qDebug ( mes );
	if (r2)
		qDebug ( "====================================================================" );	
}


QString ScreenX::getProgramDir()
{
	ProgDir = QString ( qApp->argv() [0] ) ;
	int i = ProgDir.findRev ( "/" );
	ProgDir.remove ( i+1, ProgDir.length() - i );
	toLog(ProgDir, true, true);
	return ProgDir;
}


