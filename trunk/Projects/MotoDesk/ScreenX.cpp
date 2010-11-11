#include <MMVolumeControl.h>

#include <TAPI_ACCE_Client.h>
#include <TAPI_CLIENT_Func.h>
#include <TAPI_SIM_Client.h>
#include <TAPI_Result.h>
#include <TAPI_AppMsg.h>
#include <TAPI_AppMsgId.h>
#include <TAPI_General.h>

#include <SETUP_THEME_ThemeAPI.h>
#include "ScreenX.h"
#include "GUI_Define.h"
#include "BaseDlg.h"
#include "myam.h"
#include "zsms.h"
#include "ZMenu.h"
#include "ZMsg.h"
#include "ZDefs.h"
#include "exec.h"

#include <ZHeader.h>
#include <ZPressButton.h>
#include <ZApplication.h>
#include <ZListBox.h>
#include <ZSoftKey.h>
#include <ZOptionsMenu.h>
#include <ZMessageDlg.h>
#include <ZSingleCaptureDlg.h>
#include <ZConfig.h>

#include <qmap.h>
#include <qfile.h>
#include <qcopchannel_qws.h>
#include <qregexp.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qwmatrix.h>
#include <ezxres.h>
#include <qevent.h>
#include <qtextstream.h>
#include <qtextcodec.h>

#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/vfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

QCopChannel * MDchannel;
QCopChannel * ctecla;
QCopChannel * notificaciones;
QCopChannel * lcdstatus;
QCopChannel * alarmstatus;

int misscall, missmsg;

QString Idioma;
QString unlocktext;
QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
RES_ICON_Reader iconReader;
int btoothon=0;
int mpvol=3;
int centerkey=0;
int Pos=1;
int Cambio;
int prePos=1;
int Isize = 30;
int Top = 10;
int ClockPos;
int ClockFormat=0;
int Left, Left1, Left2, Left3, Left4, Left5;
QString keyLocked="NO";
int CM = 0;
int ProgCount=5;
int out=1;
QString SWall="";
int TextColor, TextColorO, TextSize;
int animado=0;
int tiempoespera=0;
QStringList fondos;
int wallTime=0;
int timerStop=0;
QDateTime tiempo, tiempo2;
int llamando=0;
QString WCS;
int ht1, ht2, dt1, dt2, dt3;
int intervalo=5000;
QString refrescar="No";
QString currentTheme;
int BarStyle=0;
int barHidden=0;
int TODO=1;
QString sysval="No";
QString shownotes="No";
int notenumber=0;
int tpress=0;
int AClockColor=0;
int Top1,Top2,Top3,Top4,Top5;
QString BarPosition;
int TopSI, LeftSI;
QString single;
int IconFixed=3; int iconOn=40; int ic1=30; int ic2=30; int ic3=30; int ic4=30; int ic5=30;
QString showSItext="Yes";
QString IconText="Yes";
int CheckOn=0;
int timerCount;
int isMediaPlayer=0;
int mchannel=0;
int showdate, showtime;
int analogclock=0;
int analogtop, analogleft, analogwidth, analogheight;
int wallchangeron;
int AAon=1;
int AAtop, AAleft, AAheight, AAwidth;

extern "C" int UTIL_GetSliderStatus(void);
extern "C" int UTIL_GetMMCSDStatus();
extern "C" int UTIL_GetPhoneInCall(void);
extern "C" int BT_PowerOn();
extern "C" int MBTAPI_CTRL_PowerOn();
extern "C" int MBTAPI_CTRL_PowerOff();
extern "C" int UTIL_GetKeypadLockStatus();




extern ZApplication *app;


ScreenX::ScreenX ( QWidget* parent, const char* name, WFlags fl )
    : ZKbMainWidget ( ZHeader::TINY_TYPE, NULL, "ZMainWidget", WType_TopLevel )
{

	ProgDir = getProgramDir();
	ZConfig PREini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
	ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
	Idioma = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
	if ( ! QFileInfo(Idioma).isFile() ) Idioma = ProgDir + "/languages/en-us";
	
	system("cat /mmc/mmca1/.system/java/CardRegistry /ezxlocal/download/appwrite/am/UserRegistry /ezxlocal/download/appwrite/am/InstalledDB /usr/data_resource/preloadapps/SysRegistry > /tmp/AMmenu");
 
	if ( ! QFileInfo("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg").isFile() ) 
	{
		system( QString("cp %1/ezxmotobackup.cfg /ezxlocal/download/appwrite/setup/ezx_motodesk.cfg").arg(ProgDir) );
	}

	QFile entrada("/usr/data_resource/ezx_resource_version.txt"); QTextStream stentrada(&entrada); QString sistema;
	if ( entrada.open(IO_ReadOnly | IO_Translate) ) { sistema = stentrada.readLine(); } entrada.close();
	int j1 = sistema.find("Linuxmod",0); if ( j1==-1 ) { printf("LINUXMOD ONLY !!!\nGOODBYE MOTHERFUCKER !!!\n"); Salir(); }

	ZConfig Fini1(QString("/ezxlocal/download/appwrite/setup/ezx_theme.cfg"), true);
	currentTheme = Fini1.readEntry(QString("THEME_GROUP_LOG"), QString("CurrentSkin"), "");

	reloj = new QTimer(this);
	
	llamada = new zCalls(Idioma);
	alerta = new zProfile(Idioma);
	
	CreateWindow ( parent );
	( ( ZApplication* ) qApp )->showMainWidget ( this );

	int i = UTIL_GetKeypadLockStatus();
	if ( i==1 )
	{ 
		UTIL_KeyboardLockClient pepe;
		pepe.connectToServer();
		pepe.unlockKeyboard();
		pepe.disConnect();
		//system(QString("%1/lock.sh").arg(ProgDir)); 
	}

	if ( QCopChannel::isRegistered("/hardkey/bc") )
	{
		ctecla = new QCopChannel("/hardkey/bc", this); 
		connect( ctecla, SIGNAL(received(const QCString &,const QByteArray &)), this, SLOT(Presionado(const QCString &,const QByteArray &)) ); 
	}

	if ( QCopChannel::isRegistered("UNS/Server") )
	{
		notificaciones = new QCopChannel("UNS/Server", this); 
		connect( notificaciones, SIGNAL(received(const QCString &,const QByteArray &)), this, SLOT(NotiNueva(const QCString &,const QByteArray &)) ); 
	}
	misscall=0; missmsg=0;

	if ( QCopChannel::isRegistered("EZX/System") )
	{
		lcdstatus = new QCopChannel("EZX/System", this); 
		connect( lcdstatus, SIGNAL(received(const QCString &,const QByteArray &)), this, SLOT(nLCD(const QCString &,const QByteArray &)) ); 
	}

	if ( QCopChannel::isRegistered("EZX/Application/MotoDesk") )
	{
		MDchannel = new QCopChannel("EZX/Application/MotoDesk", this); 
		connect( MDchannel, SIGNAL(received(const QCString &,const QByteArray &)), this, SLOT(MDstatus(const QCString &,const QByteArray &)) );
	}

	/*if ( QCopChannel::isRegistered("EZX/Alarmserver") )
	{
		alarmstatus = new QCopChannel("EZX/Alarmserver", this); 
		connect( alarmstatus, SIGNAL(received(const QCString &,const QByteArray &)), this, SLOT(aStatus(const QCString &,const QByteArray &)) );
		
	}*/

//	/Phone/Idle/Mediaplayer/Channel	ALARM/IDLE/CHANNEL EZX/Alarmserver


	if ( QFileInfo("/ezxlocal/motodesk.qws").isFile() ) 
	{
		system("/usr/SYSqtapp/windowsserver/windowsserver&");
		//system("/usr/SYSqtapp/phone/phone&");
		//system("/usr/SYSqtapp/am/bin/am&");
	}

}

ScreenX::~ScreenX()
{
    delete softKey;
    delete menu;
}

void ScreenX::Presionado(const QCString &mensaje, const QByteArray &datos)
{
	if ( sysval=="Yes" ) SystemInfo();
	ShowClock(); tiempoespera=0;

	if (keyLocked=="YES") return;

	QDataStream stream(datos, IO_ReadOnly);
	if( mensaje == "keyMsg(int,int)")
	{
		int key, type;
		stream >> key >> type; 
		if( (key==4149) && (type==1) )
		{
			ZConfig Vini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
			QString run = Vini.readEntry(QString("BUTTONS"), QString("Exec3"), "");
			if (run != "") { 
				QRegExp rx( "{*-*-*-*-*}" ); if ( run.find(rx)!=-1 )
				{ ZMsg msg( "/AM/Launcher", "launchApp" ); msg << (QUuid)run; msg.send(); } 
			}
		} 
	}
}

void ScreenX::NotiNueva(const QCString &mensaje, const QByteArray &datos)
{
	if ( sysval=="Yes" ) SystemInfo();
	ShowClock();
	QDataStream stream(datos, IO_ReadOnly);
	int key, type;
	stream >> key >> type; 
	//toLog( QString("NOTIFICACION: %1").arg(mensaje) );
	//toLog( QString("MENSAJE: %1 - %2").arg(key).arg(type) );

	if( mensaje == "addItem(int,...)")
	{
		if (key==0) misscall=1;
		if (key==3) missmsg=1;
		//toLog( QString("MENSAJES SIN LEER: %1").arg(missmsg) );
		//toLog( QString("LLAMADAS PERDIDAS: %1").arg(misscall) );
	}
	if( mensaje == "removeItem(int,int)")
	{
		if (key==0) misscall=0;
		if (key==3) missmsg=0;
		//toLog( QString("MENSAJES SIN LEER: %1").arg(missmsg) );
		//toLog( QString("LLAMADAS PERDIDAS: %1").arg(misscall) );
	}
	if ( (misscall==1) || (missmsg==1) ) 
	{
	  if ( this->hasFocus() )
	  {
		zMissed * misseds = new zMissed(Idioma, misscall, missmsg);
		misseds->exec();
		delete misseds;
		misseds = NULL;
	  }
	}

}

void ScreenX::clearNoti()
{
	misscall=0; missmsg=0;
}

void ScreenX::nLCD(const QCString &mensaje, const QByteArray &datos)
{
	if ( mensaje=="LCDOn" )
	{
		if ( timerStop==1 ) { 
			timerStop=0; 
			reloj->start(5000, false); 
			if ( sysval=="Yes" ) SystemInfo();
			ShowClock();
			if ( analogclock==1 ) AnalogClock();
			if ( isMediaPlayer==1 ) { getTrackInfo(); getAlbumArt(); }
		}
	}
	else if ( mensaje=="LCDOff" )
	{
		timerStop=1; reloj->stop(); 
		clock->setText(""); date->setText(""); 
		systemInfo1->setText(""); systemInfo2->setText(""); systemInfo3->setText(""); 
	}

}

void ScreenX::MDstatus(const QCString &mensaje, const QByteArray &datos)
{
  if ( mensaje=="UPDATE_MUSIC" )
  {
	ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
	QString walltype = ini.readEntry(QString("AM"), QString("Walltype"), "Default");
	if ( walltype=="Wallclock" )
	{
		WCS = ini.readEntry(QString("AM"), QString("WallclockSelected"), "01");
		ZConfig WCini(QString(ProgDir + "/screen/" + WCS + "/settings.cfg"), true);
		if ( WCini.readEntry(QString("SCREEN"), QString("AlbumArt"), "No") == "Yes" )
		{
			AAon=1;
			AAleft = WCini.readNumEntry(QString("ALBUMART"), QString("Left"), 321);
			AAtop = WCini.readNumEntry(QString("ALBUMART"), QString("Top"), 321);
			AAheight = WCini.readNumEntry(QString("ALBUMART"), QString("Height"), 30);
			AAwidth = WCini.readNumEntry(QString("ALBUMART"), QString("Width"), 30);
			Panel->moveChild( AlbumArt, AAleft, AAtop );
			getAlbumArt(); 
		}
		else { Panel->moveChild( AlbumArt, 321, 321 ); }
	}
	else { Panel->moveChild( AlbumArt, 321, 321 ); }

	if (isMediaPlayer==1) { getTrackInfo(); }


  }
  if ( mensaje=="UPDATE_SCREEN" )
  {
	system("busybox killall rockboxui.portrait");
	system("busybox killall rockboxui.landscape");
	isMediaPlayer=0; RecargarIconos(); Panel->moveChild( AlbumArt, 321, 321 );
  }
}

void ScreenX::aStatus(const QCString &mensaje, const QByteArray &datos)
{
	QDataStream stream(datos, IO_ReadOnly);
	QDateTime pepe; QCString s1, s2;
	int key, type;
	stream >> pepe >> s1 >> s2 >> key;
	
	toLog( QString("ALARMA: %1").arg(mensaje) );
	toLog( QString("MENSAJE: %1 - %2 - %3 - %4").arg(pepe.toString()).arg(s1).arg(s2).arg(key) );


	//if ( mensaje == "addAlarm(QDateTime,QCString,QCString,int)" )
	//{
		eti1->setText( QString("%1").arg(pepe.toString()) );		

	//}



}

void ScreenX::CreateWindow ( QWidget* parent )
{

	Panel = new ZScrollPanel (this , NULL , 0 ,  (ZSkinService::WidgetClsID)4);
	Panel->resize ( 240, 320 );

	fondo = new ZImage(this);
	Panel->addChild(fondo, 0, 0-this->headerSize().height() );

toLog( QString("HOLA PELOTUDO") );

	LeerDatos();

	Icono1 = new ZImage(this);
	Panel->addChild(Icono1, 321, 321);
	Icono2 = new ZImage(this);
	Panel->addChild(Icono2, 321, 321);
	Icono3 = new ZImage(this);
	Panel->addChild(Icono3, 321, 321);
	Icono4 = new ZImage(this);
	Panel->addChild(Icono4, 321, 321);
	Icono5 = new ZImage(this);
	Panel->addChild(Icono5, 321, 321);
	fizq = new ZImage(this); Panel->addChild(fizq, 321, 321);
	fder = new ZImage(this); Panel->addChild(fder, 321, 321);


	eti1 = new ZLabel(this);
	eti1->setGeometry(321,321,200,40);
	eti1->setText("");
	eti1->setAlignment(ZLabel::AlignCenter);
	eti1->setOutline(true,QRgb(TextColorO),QRgb(TextColor));
	Panel->addChild(eti1, 321, 321);
	QFont f = eti1->font();
	f.setPixelSize(TextSize);
	eti1->setFont(f);

	date = new ZLabel(this);
	date->setText("");
	date->setOutline(true,QRgb(TextColorO),QRgb(TextColor));
	date->setAlignment(ZLabel::AlignLeft);
	date->setGeometry(100,40, 100, 40);
	date->setFont(f);

	lNotes = new ZLabel(this);
	lNotes->setText("");
	lNotes->setOutline(true,QRgb(TextColorO),QRgb(TextColor));
	lNotes->setAlignment(ZLabel::AlignLeft);
	lNotes->setGeometry(240,30, 321, 321);
	lNotes->setFont(f);

	clock = new ZLabel(this);
	clock->setText("");
	clock->setOutline(true,QRgb(TextColorO),QRgb(TextColor));
	clock->setAlignment(ZLabel::AlignRight);
	clock->setGeometry(100,40, 100, 40);
	clock->setFont(f);

	Panel->addChild(date, 321, 321 );
	Panel->addChild(clock, 321, 321 );


	systemInfo1 = new ZLabel(this);
	systemInfo1->setOutline(true,QRgb(TextColorO),QRgb(TextColor));
	systemInfo1->setAlignment(ZLabel::AlignLeft);

	systemInfo2 = new ZLabel(this);
	systemInfo2->setOutline(true,QRgb(TextColorO),QRgb(TextColor));
	systemInfo2->setAlignment(ZLabel::AlignLeft);

	systemInfo3 = new ZLabel(this);
	systemInfo3->setOutline(true,QRgb(TextColorO),QRgb(TextColor));
	systemInfo3->setAlignment(ZLabel::AlignLeft);

	trackinfo = new ZLabel(this);
	trackinfo->setGeometry(321,321,146,30);
	f.setPixelSize(13);
	trackinfo->setFont(f);
	trackinfo->setOutline(true,QRgb(TextColorO),QRgb(TextColor));
	Panel->addChild(trackinfo, 321, 321 );


	Panel->addChild(systemInfo1, 321, 321 );
	Panel->addChild(systemInfo2, 321, 321 );
	Panel->addChild(systemInfo3, 321, 321 );

	connect( app, SIGNAL(sliderStatusChanged(int)), this, SLOT(Slider()) );
	connect( app, SIGNAL(askReturnToIdle(int)), this, SLOT(Salir()) );
	connect( app, SIGNAL(signalThemeChange()), this, SLOT(Salir()) );
	connect( app, SIGNAL(signalLanguageChange()), this, SLOT(chLanguage()) );

	setContentWidget ( Panel );

	softKey = new ZSoftKey ( NULL , this , 0 );
	Create_MenuApp();
	Create_SoftKey();
	setSoftKey ( softKey );


	analog = new ZImage(this); Panel->addChild(analog, 321, 321);
	agujas = new ZImage(this); Panel->addChild(agujas, 321, 321);

	hora1 = new ZImage(this); Panel->addChild(hora1, 321, 321);
	hora2 = new ZImage(this); Panel->addChild(hora2, 321, 321);
	hora3 = new ZImage(this); Panel->addChild(hora3, 321, 321);
	hora4 = new ZImage(this); Panel->addChild(hora4, 321, 321);
	dia1 = new ZImage(this); Panel->addChild(dia1, 321, 321);
	dia2 = new ZImage(this); Panel->addChild(dia2, 321, 321);
	dia3 = new ZImage(this); Panel->addChild(dia3, 321, 321);
	dia4 = new ZImage(this); Panel->addChild(dia4, 321, 321);

	AlbumArt = new ZImage(this); Panel->addChild(AlbumArt, 321, 321);

	connect(reloj, SIGNAL(timeout()), SLOT(Clocktick()));
	reloj->start(5000, false);

	refrescar="Yes";

	tiempo  = QDateTime::currentDateTime();
	tiempo2 = tiempo;

	CheckBT();

	setFocusPolicy(QWidget::StrongFocus);

	showbar = new QTimer(this);
	connect(showbar, SIGNAL(timeout()), this, SLOT(timerTick()));

	qApp->processEvents();
	CheckPositions();
	Recargar();
	RecargarIconos();
	Clocktick();

}

void ScreenX::timerTick()
{
  if ( this->hasFocus() )
  {
	TODO=0;
	if ( (BarPosition=="Up") || (BarPosition=="Down") || (BarPosition=="Right") )
	{
		if ( barHidden==0 ) {
			Left1=Left1-10; Left2=Left2-10; Left3=Left3-10; Left4=Left4-10; Left5=Left5-10; 
			Panel->moveChild( Icono1, Left1, Top1 );
			Panel->moveChild( Icono2, Left2, Top2 );
			Panel->moveChild( Icono3, Left3, Top3 );
			Panel->moveChild( Icono4, Left4, Top4 );
			Panel->moveChild( Icono5, Left5, Top5 );
			if (timerCount==24) { TODO=1; showbar->stop(); timerCount=0; RecargarIconos(); }
		} else if ( barHidden==1 ) { 
			Left1=Left1+10; Left2=Left2+10; Left3=Left3+10; Left4=Left4+10; Left5=Left5+10;
			Panel->moveChild( Icono1, Left1, Top1 );
			Panel->moveChild( Icono2, Left2, Top2 );
			Panel->moveChild( Icono3, Left3, Top3 );
			Panel->moveChild( Icono4, Left4, Top4 );
			Panel->moveChild( Icono5, Left5, Top5 );
			if (timerCount==24)	{ TODO=0; showbar->stop(); timerCount=0; RecargarIconos(); 
			fderi.load( ProgDir + "/null.png" ); fder->setPixmap(fderi); fizq->setPixmap(fderi); }
		}
	}
	else if (BarPosition=="Left") 
	{
		if ( barHidden==0 ) {  
			Left1=Left1+10; Left2=Left2+10; Left3=Left3+10; Left4=Left4+10; Left5=Left5+10; 
			Panel->moveChild( Icono1, Left1, Top1 );
			Panel->moveChild( Icono2, Left2, Top2 );
			Panel->moveChild( Icono3, Left3, Top3 );
			Panel->moveChild( Icono4, Left4, Top4 );
			Panel->moveChild( Icono5, Left5, Top5 );
			if (timerCount==24) { TODO=1; showbar->stop(); timerCount=0; RecargarIconos(); }

		} else if ( barHidden==1 ) { 
			Left1=Left1-10; Left2=Left2-10; Left3=Left3-10; Left4=Left4-10; Left5=Left5-10;
			Panel->moveChild( Icono1, Left1, Top1 );
			Panel->moveChild( Icono2, Left2, Top2 );
			Panel->moveChild( Icono3, Left3, Top3 );
			Panel->moveChild( Icono4, Left4, Top4 );
			Panel->moveChild( Icono5, Left5, Top5 );
			if (timerCount==24) { TODO=0; showbar->stop(); timerCount=0; RecargarIconos(); 
			fderi.load( ProgDir + "/null.png" ); fder->setPixmap(fderi); fizq->setPixmap(fderi); }

		}
	}
	timerCount = timerCount + 1;
  }
}

void ScreenX::LeerDatos()
{
	QString Fval1; ZConfig Fini1(QString("/ezxlocal/download/appwrite/setup/ezx_theme.cfg"), true);
	Fval1 = Fini1.readEntry(QString("THEME_GROUP_LOG"), QString("CurrentSkin"), "");
	SWall = SETUP_THEME::getThemeValue(SETUP_THEME::SLIDE_TONE);
	ZConfig Fini3(Fval1 + "/phone_p.ini", false);
	TextColor = Fini3.readNumEntry(QString("Font_DigitalClockStd"), QString("Color"), 0);
	TextColorO = Fini3.readNumEntry(QString("Font_DigitalClockStd"), QString("OutlineColor"), 0);
	TextSize = Fini3.readNumEntry(QString("Font_IdleStatusStd"), QString("Size"), 14);
	if (TextSize>16) TextSize=16;
	AClockColor = Fini3.readNumEntry(QString("Idle"), QString("AnalogClockHands_Color"), 0);

}


void ScreenX::Walltimertick()
{
	//system(QString("echo \"CHANGING WALLPAPER\""));
	ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
	QString val = ini.readEntry(QString("AM"), QString("Wallpath"), "/mmc/mmca1/Images");
	QString cval = ini.readEntry(QString("AM"), QString("Wall"), "No");
	Top = ini.readNumEntry(QString("AM"), QString("Top"), 5);
	if (val=="No") return;

	QDir dir ( val, "*" );
	dir.setMatchAllDirs ( true );
	dir.setFilter ( QDir::Files | QDir::Hidden );
	if ( !dir.isReadable() )
	return;
	QStringList entries = dir.entryList();
	QStringList::ConstIterator it = entries.begin();
	fondos.clear();
	while ( it != entries.end() ) {
	if ( ( *it != "." ) && ( *it != ".." ) )
	{
	  QString Eval = QString("%1").arg(*it).lower();
	  int i = Eval.findRev("."); Eval.remove ( 0, i +1 );
	  if ( (Eval=="png") || (Eval=="jpg") || (Eval=="gif") || (Eval=="bmp") )
	  {
		fondos += val+"/"+*it;
	  }
	}
	++it;
	}

	QString line, proximo="-"; int next=0;
	for ( int j=0; j<fondos.count(); ++j )
	{
	  line = fondos[j];
	  if ( next == 1 ) { proximo = fondos[j]; }
	  if ( line == cval ) next=1; else next=0;
	}
	if ( proximo == "-" ) proximo = QString("%1").arg(fondos[0]);
	ini.writeEntry(QString("AM"), QString("Wall"), proximo );
	//system(QString("echo \"NUEVO FONDO %1\"").arg(proximo));

	Recargar();
	if (analogclock==1) AnalogClock();

}

void ScreenX::Clocktick()
{
  if ( sysval=="Yes" ) SystemInfo();
  if ( isMediaPlayer==1 ) { getTrackInfo(); }
  if (AAon==1) getAlbumArt();
  ShowClock();

  qApp->processEvents();

  if ( this->hasFocus() )
  {
	int ix;
	if ( intervalo < 1000 ) ix = 1; else ix = intervalo / 1000;
	if ( UTIL_GetSliderStatus()==1 ) tiempoespera = tiempoespera + ix;
	toLog( QString("TIEMPO ESPERA: %1").arg(tiempoespera) );
	if ( tiempoespera > 5 )
	{ 
		if (keyLocked=="NO")
		{
			qApp->processEvents();
			softKey->setText ( ZSoftKey::LEFT, "", ( ZSoftKey::TEXT_PRIORITY ) 0 );
			softKey->setText ( ZSoftKey::RIGHT, unlocktext, ( ZSoftKey::TEXT_PRIORITY ) 0 );
			keyLocked="YES"; 
			system("/ezxlocal/LinXtend/usr/bin/keylock &");
		}
	}
  }
}

void ScreenX::ShowClock()
{

  ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
  QString walltype = ini.readEntry(QString("AM"), QString("Walltype"), "Default");
  int wallt = ini.readNumEntry(QString("AM"), QString("Wallinterval"), 5) * 60;
  QString SWallx = SETUP_THEME::getThemeValue(SETUP_THEME::SLIDE_TONE);

  ZConfig Fini1(QString("/ezxlocal/download/appwrite/setup/ezx_theme.cfg"), true);
  if ( currentTheme != Fini1.readEntry(QString("THEME_GROUP_LOG"), QString("CurrentSkin"), "") ) RecargarMenu();

  if ( (walltype=="Default") && (SWallx!=SWall) ) {
	Recargar();
  }

  if (analogclock==1) AnalogClock();

  if (showtime=1)
  {
	ZConfig Fini4(QString("/ezxlocal/download/appwrite/setup/ezx_system.cfg"), false);
	ClockFormat = Fini4.readNumEntry(QString("SYS_SYSTEM_SET"), QString("TimeFormat"), 0);
	QTime tiempo = QTime::currentTime();
	QString hora;
	if ( ClockFormat==0 ) {
		int h = tiempo.hour();
		int m = tiempo.minute();
		if ( h>11 ) { 
			if ( h>12 ) { h=h-12; }
			if ( m>9 ) hora = QString("%1:%2pm").arg(h).arg(m);
			else hora = QString("%1:0%2pm").arg(h).arg(m);
		} else {
			if ( h==0 ) { h=12; }
			if ( m>9 ) hora = QString("%1:%2am").arg(h).arg(m);
			else hora = QString("%1:0%2am").arg(h).arg(m);
		}
	} else {
		hora = tiempo.toString();
		hora.remove(5,3);
	}
	clock->setText(hora);
  } 
  else { clock->setText(""); }


  if (showdate=1)
  {
	ZConfig Fini4(QString("/ezxlocal/download/appwrite/setup/ezx_system.cfg"), false);
	int DateFormat = Fini4.readNumEntry(QString("SYS_SYSTEM_SET"), QString("DateFormat"), 0);
	QDate fecha = QDate::currentDate();
	int d = fecha.day(); int m = fecha.month(); int y = fecha.year();
	QString d2; if (d<10) d2=QString("0%1").arg(d); else d2=QString("%1").arg(d);
	QString m2; if (m<10) m2=QString("0%1").arg(m); else m2=QString("%1").arg(m);

	QString tf;
	if ( DateFormat==0) { tf = QString("%1/%2/%3").arg(m2).arg(d2).arg(y); }
	else if ( DateFormat==1) { tf = QString("%1/%2/%3").arg(y).arg(m2).arg(d2); }
	else if ( DateFormat==2) { tf = QString("%1/%2/%3").arg(d2).arg(m2).arg(y); }
	date->setText(tf);
  }
  else { date->setText(""); }


  if ( shownotes=="Yes" )
  {
	ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_notepad.cfg", true);
	int nxi = ini.readNumEntry(QString("NOTES"), QString("Count"), 0);
	if ( nxi!=0 ) 
	{
		int notesok=0;
		for ( int j=0; j<nxi; ++j )
		{
		  if ( ini.readEntry(QString("NOTES"), QString("Note%1flag").arg(j+1), "Yes") == "Yes") notesok=1;
		}
		if ( notesok!=0 )// { lNotes->setText(""); return; }
		{
			int ok=0;
			while ( ok == 0 )
			{
				notenumber=notenumber+1;
				if ( notenumber > nxi ) notenumber=1;
				if ( ini.readEntry(QString("NOTES"), QString("Note%1flag").arg(notenumber), "Yes") == "Yes")
				{
					ok=1; lNotes->setText( ini.readEntry(QString("NOTES"), QString("Note%1text").arg(notenumber), "") );
				}
			}
		} else lNotes->setText("");
	}
  }

  wallchangeron=0;
  if ( walltype=="Wallclock" )
  {
	readWallClock(); 
	WCS = ini.readEntry(QString("AM"), QString("WallclockSelected"), "01");
	ZConfig WCini(QString(ProgDir + "/screen/" + WCS + "/settings.cfg"), true);
	if ( WCini.readEntry(QString("SCREEN"), QString("Wallchanger"), "No") == "Yes" ) wallchangeron=1;
  }
  toLog( QString("WALLCHANGERON: %1").arg(wallchangeron) );

  if ( walltype=="Wallchanger" ) wallchangeron=1;

  if ( wallchangeron==1 )
  {
	tiempo = QDateTime::currentDateTime();
	wallTime = tiempo2.secsTo( tiempo );
	if ( wallTime >= wallt ) { Walltimertick(); tiempo2=tiempo; }
  }

  CheckAvion();
  CheckBT();

}


void ScreenX::paintEvent(QPaintEvent * pe)
{
  if ( this->hasFocus() )
  {
	qApp->processEvents();
	//tiempoespera=0; 
	int i = UTIL_GetKeypadLockStatus();
	if ( (i==0) && (keyLocked=="YES") ) { keyLocked="NO"; tiempoespera=0; Create_SoftKey(); DesbloquearAhora(); }

/*	ShowClock(); 
	if ( sysval=="Yes" ) SystemInfo();

	mchannel=0;
	if ( QCopChannel::isRegistered("EZX/Application/mediaplayer") ) mchannel=1;
	else if ( QCopChannel::isRegistered("EZX/Application/zPlayer") ) mchannel=1;
	else if ( QCopChannel::isRegistered("EZX/Application/rockboxui.portrait") ) mchannel=2;
	else if ( QCopChannel::isRegistered("EZX/Application/rockboxui.landscape") ) mchannel=2;
	if (mchannel==0) isMediaPlayer=0; RecargarIconos(); 

	qApp->processEvents();
*/
	if ( btoothon==1 ) { btoothon=0; system("busybox killall bluetooth"); }
	if ( QCopChannel::isRegistered("EZX/Application/addrbk") ) system("busybox killall addrbk");

  }
}


void ScreenX::AnalogClock()
{
	
  ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
  QString walltype = ini.readEntry(QString("AM"), QString("Walltype"), "Default");
  WCS = ini.readEntry(QString("AM"), QString("WallclockSelected"), "01");
  ZConfig WCini( ProgDir + "/screen/" + WCS + "/settings.cfg", true);
  QString wcanalog =  WCini.readEntry(QString("SCREEN"), QString("Analog"), "No");

	QPixmap himg; QPixmap mimg;

	if ( walltype=="Wallclock" )
	{
		himg.load(ProgDir + "/screen/" + WCS + "/" + "hhand.png");
		mimg.load(ProgDir + "/screen/" + WCS + "/" + "mhand.png");
	}
	else
	{
		himg.load(ProgDir + "hhand.png");
		mimg.load(ProgDir + "mhand.png");
	}

	QPixmap rfondo = preClock;
	QPainter p0(&rfondo);
	QTime tiempo = QTime::currentTime();
	int h = tiempo.hour();
	int m = tiempo.minute();
	if ( h>12 ) h=h-12;

	int clk_center_x0 = analogwidth/2;
	int clk_center_y0 = analogheight/2;

	int clk_center_x = analogleft + clk_center_x0;
	int clk_center_y = analogtop + clk_center_y0;

	double valx = 360/12 * h; double valy = 30 * m / 60;
	QWMatrix him; him.rotate( valx+valy ); himg = himg.xForm( him );
	p0.drawPixmap(clk_center_x - himg.width()/2 , clk_center_y - himg.height()/2, himg);

	QWMatrix mim; mim.rotate( 360/60 * m); mimg = mimg.xForm( mim );
	p0.drawPixmap(clk_center_x - mimg.width()/2 , clk_center_y - mimg.height()/2, mimg);

	setScreenFSBA( rfondo , 100, (PIXMAP_STRETCH_POLICY_E)5 );
	setFullScreenMode(true, true);
	setFullScreenMode(false, false);

}

void ScreenX::Izquierda()
{
	if ( isMediaPlayer==1 )
	{
		if (mchannel==1) { IDLE_MediaPlayer_Client musica; musica.sendPrevCmd(); }
		if (mchannel==2) { QCopChannel::send("EZX/Application/SDL", "LEFT", 0); getTrackInfo(); getAlbumArt(); }
		return;
	}

	ZConfig Vini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
	ProgCount = Vini.readNumEntry(QString("AM"), QString("Icons"), 0);
	if ( (out==0) && (BarStyle==2) ) --prePos;
	if ( out==0 ) --Pos; 
	if (Pos<1) 
	{ 
		CM--; if ( CM<0 )
		{ 
			if (BarStyle==1) { CM=ProgCount-5; Pos=5; } 
			else if (BarStyle==0) { CM=0; Pos=1; } 
		}
		else Pos=1;
	}
	out=0;
	RecargarIconos();
}

void ScreenX::Derecha()
{
	if ( isMediaPlayer==1 )
	{
		if (mchannel==1) { IDLE_MediaPlayer_Client musica; musica.sendNextCmd(); }
		if (mchannel==2) { QCopChannel::send("EZX/Application/SDL", "RIGHT", 0); getTrackInfo(); getAlbumArt(); }
		return;
	}

	ZConfig Vini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
	ProgCount = Vini.readNumEntry(QString("AM"), QString("Icons"), 0);
	if ( (out==0) && (BarStyle==2) ) ++prePos; 
	if ( out==0 ) ++Pos; 
	if (Pos>5) 
	{ 
		CM++; if ( CM>ProgCount-5 )
		{
			if (BarStyle==1) { CM=0; Pos=1; } 
			else if (BarStyle==0) { CM=ProgCount-5; Pos=5; }
		}
		else Pos=5;
	}
	out=0;
	RecargarIconos();
}

void ScreenX::RunProg(int numero)
{
	out=1;
	ZConfig Vini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
	QString run = Vini.readEntry(QString("BUTTONS"), QString("Exec%1").arg(numero), "");
	if (run != "") { 
		QRegExp rx( "{*-*-*-*-*}" ); if ( run.find(rx)!=-1 )
		{ ZMsg msg( "/AM/Launcher", "launchApp" ); msg << (QUuid)run; msg.send(); } 
	}
}

void ScreenX::keyReleaseEvent(QKeyEvent* k)
{

  tiempoespera=0; 
  if ( keyLocked == "NO" )
  {
	if ( k->key() == Z6KEY_C )
	{ 
	  if (tpress>5)
	  {
		ZConfig IDini(Idioma, true);
		QString val = IDini.readEntry(QString("TEXT"), QString("EXIT_TEXT"), "");
		ZMessageDlg* msg =  new ZMessageDlg ( "MotoDesk", QString("%1").arg(val), ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
		int i = msg->exec(); delete msg; msg = NULL;
		if (i) { qApp->quit(); }
	  }
	  else
	  {


	  }
	}
	else if ( k->key() == Z6KEY_CAMERA )
	{
	  if (tpress>5)
	  {
		system("/ezxlocal/LinXtend/usr/bin/cameraflash &");
	  }
	  else
	  {
		ZConfig Vini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
		QString run = Vini.readEntry(QString("BUTTONS"), QString("Exec4"), "");
		if (run != "") { 
			QRegExp rx( "{*-*-*-*-*}" ); if ( run.find(rx)!=-1 )
			{ ZMsg msg( "/AM/Launcher", "launchApp" ); msg << (QUuid)run; msg.send(); } 
		}
	  }
	}
	else if ( k->key() == Z6KEY_MUSIC )
	{
		mchannel=0;
		if ( QCopChannel::isRegistered("EZX/Application/mediaplayer") ) mchannel=1;
		if ( QCopChannel::isRegistered("EZX/Application/zPlayer") ) mchannel=1;
		if ( QCopChannel::isRegistered("EZX/Application/rockboxui.portrait") ) mchannel=2;
		if ( QCopChannel::isRegistered("EZX/Application/rockboxui.landscape") ) mchannel=2;

		if ( tpress>5 )
		{
			ZConfig Vini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
			QString run;
			if (mchannel==1) run = "{ba63e2ca-089c-4a95-8e13-81522eed47a0}";
			else run = Vini.readEntry(QString("BUTTONS"), QString("Exec2"), "");
			if (run != "") { 
				QRegExp rx( "{*-*-*-*-*}" ); if ( run.find(rx)!=-1 )
				{ ZMsg msg( "/AM/Launcher", "launchApp" ); msg << (QUuid)run; msg.send(); } 
			}
		}
		else
		{
			if (isMediaPlayer==1) { isMediaPlayer=0; RecargarIconos(); return; }

			if ( mchannel==0 )
			{
				ZConfig Vini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
				QString run = Vini.readEntry(QString("BUTTONS"), QString("Exec2"), "");
				if (run != "") { 
					QRegExp rx( "{*-*-*-*-*}" ); if ( run.find(rx)!=-1 )
					{ ZMsg msg( "/AM/Launcher", "launchApp" ); msg << (QUuid)run; msg.send(); } 
				}
			}
			else
			{
				//zMusic* msg = new zMusic(Idioma);
				//int i = msg->exec(); delete msg; msg = NULL;
				if (isMediaPlayer==0) isMediaPlayer=1;
				else isMediaPlayer=0;
				RecargarIconos(); 
			}
		}
	}

	tpress=0;
  }

}


void ScreenX::keyPressEvent(QKeyEvent* k)
{

  //toLog( QString("Button Pressed: %1").arg(k->key()) );
  if ( sysval=="Yes" ) SystemInfo();

  tiempoespera=0; 
  //if ( timerStop==1 ) { Clocktick(); timerStop=0; reloj->start(5000, false); }


  if ( keyLocked == "NO" )
  {
	if (k->state()==0) { tpress=tpress+1; } 

	if ( k->key() == Z6KEY_LEFT )
	{
		if (barHidden==0)
		{
			if ( (BarPosition=="Up") || (BarPosition=="Down") ) { if (centerkey==0) Izquierda(); else if (out==0) Izquierda(); else RunProg(7); }
			if ( (BarPosition=="Left") || (BarPosition=="Right") ) RunProg(7);
		}
		else RunProg(7);
	}
	else if ( k->key() == Z6KEY_RIGHT )
	{
		if (barHidden==0)
		{
			if ( (BarPosition=="Up") || (BarPosition=="Down") ) { if (centerkey==0) Derecha(); else if (out==0) Derecha(); else RunProg(8); }
			if ( (BarPosition=="Left") || (BarPosition=="Right") ) RunProg(8);
		}
		else RunProg(8);
	}
	else if ( k->key() == Z6KEY_UP )
	{
		if (barHidden==0)
		{
			if ( (BarPosition=="Left") || (BarPosition=="Right") ) { if (centerkey==0) Izquierda(); else if (out==0) Izquierda(); else RunProg(5); }
			if ( (BarPosition=="Up") || (BarPosition=="Down") ) RunProg(5);
		}
		else RunProg(5);
	}
	else if ( k->key() == Z6KEY_DOWN )
	{
		if (barHidden==0)
		{
			if ( (BarPosition=="Left") || (BarPosition=="Right") ) { if (centerkey==0) Derecha(); else if (out==0) Derecha(); else RunProg(6); }
			if ( (BarPosition=="Up") || (BarPosition=="Down") ) RunProg(6);
		}
		else RunProg(6);
	}
	else if ( k->key() == Z6KEY_CENTER )
	{
		if ( isMediaPlayer==1 )
		{
			if (mchannel==1) { IDLE_MediaPlayer_Client musica; musica.sendPauseCmd(); }
			if (mchannel==2) { QCopChannel::send("EZX/Application/SDL", "LSOFTKEY", 0); getTrackInfo(); getAlbumArt(); }
			return;
		}

	  if ( showbar->isActive() ) return;
	  if ( out==1 )
	  {
		if ( centerkey == 0 )
		{
			if (barHidden==0)
			{
				barHidden=1;
			}
			else
			{
				barHidden=0;
			}
			Recargar();
			if ( analogclock==1 ) AnalogClock();
			timerCount=0;
			showbar->start(5,false);
		}
		else
		{
			out=0;
			RecargarIconos();
		}
	  }
	  else if ( out==0 ) 
	  {
		QString run;
		if (Pos==1) { run = Exec1; } else if (Pos==2) { run = Exec2; } else if (Pos==3) { run = Exec3; }
		else if (Pos==4) { run = Exec4; } else if (Pos==5) { run = Exec5; }
		if (run == "") return;
		ZConfig Registro ( "/tmp/AMmenu", false );
		QString ayuda = run; ayuda.remove(0,1); int pi=ayuda.findRev("}"); ayuda.remove(pi,1);
		int Tipo = Registro.readNumEntry(ayuda, QString("Type"), 0);
		if ( Tipo == 4 )
		{
			QString Accion = Registro.readEntry(QString(ayuda), QString("Action"), "");
			AM_ActionData ad;
			ad.setAction(Accion);
			ad.invoke();    
		}
		else
		{
			QRegExp rx( "{*-*-*-*-*}" ); if ( run.find(rx)!=-1 )
			{ ZMsg msg( "/AM/Launcher", "launchApp" ); msg << (QUuid)run; msg.send(); } 
		}
		out=1;
		RecargarIconos();
	  }
	}
	else //if ( k->key() == Z6KEY_RED )
	{
		//Pos=0; 
		out=1;
		RecargarIconos();
	}

	if ( k->key() == Z6KEY_GREEN )
	{
		/*AM_ActionData ad;
		ad.setAction("dial");
		ad.setValue("Number", "");
		ad.invoke(); */

		if ( UTIL_GetPhoneInCall() )
		{
			//toLog( QString("Raising Call...") );
			system("echo \"[MOTODESK]\" > /tmp/pid");
			system("num=`ps | grep -i \"phone\" | sed -e '/grep/d' | awk '{ print $1 }'`; echo \"PID = $num\" >> /tmp/pid");
			ZConfig pid(QString("/tmp/pid"), true); int ppid = pid.readNumEntry(QString("MOTODESK"), QString("PID"), 0);
			QCopChannel::send( QCString("EZX/Application/"+QString::number(ppid)), QCString ( "raise()" ), QByteArray() );
			return;
		}
		else 
		{
			QString run = "{ce59717d-fb23-4b7c-8800-25ce613f4042}";
			ZMsg msg( "/AM/Launcher", "launchApp" ); msg << (QUuid)run; msg.send();
		}
	}

	if ( k->key() == Z6KEY_SIDE_SELECT )
	{
		ZConfig Vini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
		QString run = Vini.readEntry(QString("BUTTONS"), QString("Exec1"), "");
		if (run != "") { 
			QRegExp rx( "{*-*-*-*-*}" ); if ( run.find(rx)!=-1 )
			{ ZMsg msg( "/AM/Launcher", "launchApp" ); msg << (QUuid)run; msg.send(); } 
		}
	}
	else if ( ( k->key() == Z6KEY_VOLUP ) || ( k->key() == Z6KEY_VOLDOWN ) )
	{
		if ( isMediaPlayer==1 )
		{
			if (k->key()==Z6KEY_VOLUP) QCopChannel::send("EZX/Application/SDL", "VOLUP", 0);
			else if (k->key()==Z6KEY_VOLDOWN) QCopChannel::send("EZX/Application/SDL", "VOLDOWN", 0);
			return;
		}


		SETUP_Utility pepe;
		if ( QCopChannel::isRegistered("asdEZX/Application/mediaplayer") ) 
		{
			if ( (mpvol<7) && (k->key()==Z6KEY_VOLUP) ) { ++mpvol; pepe.setSpeakerVolume(mpvol); }
			if ( (mpvol>0) && (k->key()==Z6KEY_VOLDOWN) ) { --mpvol; pepe.setSpeakerVolume(mpvol); }
		}
		else
		{		
			if ( ! pepe.isAirplaneModeOn() )
			{
			  if (llamando==0) 
			  {
				llamando=1;
				//zProfile * alerta = new zProfile(Idioma);
				alerta->CreateDialog(Idioma);
				alerta->exec();
				delete alerta;
				alerta = NULL;
				alerta = new zProfile(Idioma);
			  }
			}
		}
	}
	else if ( ( (k->key() >= 0x0030) && (k->key() <= 0x0039) ) || (k->key() <= 0x0023) || (k->key() <= 0x002a) )
	{
		QString numero;
		if ( k->key() == KEY_0 ) numero = "0"; else if ( k->key() == KEY_1 ) numero = "1";
		else if ( k->key() == KEY_2 ) numero = "2"; else if ( k->key() == KEY_3 ) numero = "3";
		else if ( k->key() == KEY_4 ) numero = "4"; else if ( k->key() == KEY_5 ) numero = "5";
		else if ( k->key() == KEY_6 ) numero = "6"; else if ( k->key() == KEY_7 ) numero = "7";
		else if ( k->key() == KEY_8 ) numero = "8"; else if ( k->key() == KEY_9 ) numero = "9";
		else if ( k->key() == KEY_AST ) numero = "*"; else if ( k->key() == KEY_NUM ) numero = "#";
		
		if (llamando==0) 
		{
			llamando=1;
			//llamada = new zCalls(Idioma);
			llamada->CreateDialog(Idioma);
			llamada->insertText(numero);
			llamada->exec();
			delete llamada;
			llamada = NULL;
			llamada = new zCalls(Idioma);
		}

	}

	CheckBT();

  }

}

void ScreenX::cancelcall()
{
	llamando=0;

}

void ScreenX::slotShutdown()
{
	qApp->processEvents();
}

void ScreenX::slotQuickQuit()
{
	qApp->processEvents();
}

void ScreenX::slotRaise()
{
	qApp->processEvents();
}

void ScreenX::Slider()
{
	//toLog( QString("Slider Closed...") );
	toLog( QString("Slider Status: %1").arg(UTIL_GetSliderStatus()) );
	if ( (UTIL_GetSliderStatus()==0)&&(keyLocked=="YES") ) { DesbloquearAhora(); }
}

void ScreenX::Salir()
{
	out=1;
	RecargarIconos();
	if ( QString("%1").arg(app->getAppChannelName()) == "EZX/Application/MotoDesk" ) 
	{
//		toLog( QString("Forced %1 to stay loaded").arg(app->getAppChannelName()) );
		app->exec(); 
	}

}

void ScreenX::chLanguage()
{
	ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
	Idioma = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
	if ( ! QFileInfo(Idioma).isFile() ) Idioma = ProgDir + "/languages/en-us";
	out=1;
	RecargarIconos();
	softKey = new ZSoftKey ( NULL , this , 0 );
	Create_MenuApp();
	Create_SoftKey();
	setSoftKey ( softKey );
	if ( QString("%1").arg(app->getAppChannelName()) == "EZX/Application/MotoDesk" ) { app->exec(); }

}

void ScreenX::RecargarIconos()
{
	QImage image;

	if ( isMediaPlayer==0 )
	{

		QString textox="";

		if ( TODO==1 ) 
		{
			CheckPositions();
			CargarProgramas();
		}

		int m1=0;

		if (iconOn>ic1) { m1=iconOn-ic1;m1=m1/2; } else { m1=0; }
		if ( (Pos==1) && (out==0) ) { Isize=iconOn; textox=Prog1; } else { Isize=ic1; }
		image = Icon1.convertToImage(); image = image.smoothScale( Isize, Isize ); Icon1.convertFromImage(image);
		if ( (Pos==1) && (out==0) ) { Panel->moveChild( Icono1, Left1-m1, Top1-m1 ); }
		else { Panel->moveChild( Icono1, Left1, Top1 ); }
		Icono1->setPixmap(Icon1);

		if (iconOn>ic2) { m1=iconOn-ic2;m1=m1/2; } else { m1=0; }
		if ( (Pos==2) && (out==0) ) { Isize=iconOn; textox=Prog2; } else { Isize=ic2; }
		image = Icon2.convertToImage(); image = image.smoothScale( Isize, Isize ); Icon2.convertFromImage(image);  
		if ( (Pos==2) && (out==0) ) { Panel->moveChild( Icono2, Left2-m1, Top2-m1 ); }
		else { Panel->moveChild( Icono2, Left2, Top2 ); }
		Icono2->setPixmap(Icon2);

		if (iconOn>ic3) { m1=iconOn-ic3;m1=m1/2; } else { m1=0; }
		if ( (Pos==3) && (out==0) ) { Isize=iconOn; textox=Prog3; } else { Isize=ic3; }
		image = Icon3.convertToImage(); image = image.smoothScale( Isize, Isize ); Icon3.convertFromImage(image);
		if ( (Pos==3) && (out==0) ) { Panel->moveChild( Icono3, Left3-m1, Top3-m1 ); }
		else { Panel->moveChild( Icono3, Left3, Top3 ); }
		Icono3->setPixmap(Icon3);

		if (iconOn>ic4) { m1=iconOn-ic4;m1=m1/2; } else { m1=0; }
		if ( (Pos==4) && (out==0) ) { Isize=iconOn; textox=Prog4; } else { Isize=ic4; }
		image = Icon4.convertToImage(); image = image.smoothScale( Isize, Isize ); Icon4.convertFromImage(image);
		if ( (Pos==4) && (out==0) ) { Panel->moveChild( Icono4, Left4-m1, Top4-m1 ); }
		else { Panel->moveChild( Icono4, Left4, Top4 ); }
		Icono4->setPixmap(Icon4);

		if (iconOn>ic5) { m1=iconOn-ic5;m1=m1/2; } else { m1=0; }
		if ( (Pos==5) && (out==0) ) { Isize=iconOn; textox=Prog5; } else { Isize=ic5; }
		image = Icon5.convertToImage(); image = image.smoothScale( Isize, Isize ); Icon5.convertFromImage(image);
		if ( (Pos==5) && (out==0) ) { Panel->moveChild( Icono5, Left5-m1, Top5-m1 ); }
		else { Panel->moveChild( Icono5, Left5, Top5 ); }
		Icono5->setPixmap(Icon5);


	
		if ( TODO==1 )
		{
			image = fizqi.convertToImage(); 
			image = image.smoothScale( 7, 7 ); fizqi.convertFromImage(image);
			image = fderi.convertToImage(); 
			image = image.smoothScale( 7, 7 ); fderi.convertFromImage(image);
			fizq->setPixmap(fizqi);
			fder->setPixmap(fderi);

			if ( IconText=="Yes") { 
				ZConfig lang(Idioma, true);
				QString Nval = lang.readEntry( QString("TRANSLATE"), textox, textox ); 
				eti1->setText(Nval); 
			} else  { 
				eti1->setText("");
			}
		}
		Panel->moveChild( trackinfo, 321, 321 );
	}
	else
	{
		CargarProgramas();
		eti1->setText(""); fder->setPixmap(fderi); fizq->setPixmap(fderi);
		Panel->moveChild( Icono2, 321, 321 ); Icono2->setPixmap(Icon2);

		if ( (BarPosition=="Up") || (BarPosition=="Down") )
		{
			if (mchannel==1)
			{
				image = Icon1.convertToImage(); image = image.smoothScale( 30, 30 ); Icon1.convertFromImage(image);
				image = Icon2.convertToImage(); image = image.smoothScale( 20, 20 ); Icon2.convertFromImage(image);
				image = Icon3.convertToImage(); image = image.smoothScale( 20, 20 ); Icon3.convertFromImage(image);
				image = Icon4.convertToImage(); image = image.smoothScale( 24, 24 ); Icon4.convertFromImage(image);
				image = Icon5.convertToImage(); image = image.smoothScale( 20, 20 ); Icon5.convertFromImage(image);
				Panel->moveChild( Icono1, Left1+20, Top1 ); Icono1->setPixmap(Icon1);
				Panel->moveChild( Icono3, Left3-10, Top3+8 ); Icono3->setPixmap(Icon3);
				Panel->moveChild( Icono4, Left4-20, Top4+4 ); Icono4->setPixmap(Icon4);
				Panel->moveChild( Icono5, Left5-26, Top5+8 ); Icono5->setPixmap(Icon5);
				Panel->moveChild( trackinfo, 321, 321 );
			}
			else
			{
				image = Icon1.convertToImage(); image = image.smoothScale( 30, 30 ); Icon1.convertFromImage(image);
				image = Icon2.convertToImage(); image = image.smoothScale( 14, 14 ); Icon2.convertFromImage(image);
				image = Icon3.convertToImage(); image = image.smoothScale( 14, 14 ); Icon3.convertFromImage(image);
				image = Icon4.convertToImage(); image = image.smoothScale( 14, 14 ); Icon4.convertFromImage(image);
				image = Icon5.convertToImage(); image = image.smoothScale( 14, 14 ); Icon5.convertFromImage(image);
				Panel->moveChild( Icono1, Left1+10, Top1 ); Icono1->setPixmap(Icon1);
				Panel->moveChild( Icono3, Left2+10, Top3 ); Icono3->setPixmap(Icon3);
				Panel->moveChild( Icono4, Left2+30, Top4 ); Icono4->setPixmap(Icon4);
				Panel->moveChild( Icono5, Left2+50, Top5 ); Icono5->setPixmap(Icon5);
				Panel->moveChild( trackinfo, Left2+10, Top1+16 );
			}
		} 
		else 
		{
			image = Icon1.convertToImage(); image = image.smoothScale( 30, 30 ); Icon1.convertFromImage(image);
			image = Icon2.convertToImage(); image = image.smoothScale( 20, 20 ); Icon2.convertFromImage(image);
			image = Icon3.convertToImage(); image = image.smoothScale( 20, 20 ); Icon3.convertFromImage(image);
			image = Icon4.convertToImage(); image = image.smoothScale( 24, 24 ); Icon4.convertFromImage(image);
			image = Icon5.convertToImage(); image = image.smoothScale( 20, 20 ); Icon5.convertFromImage(image);
			Panel->moveChild( Icono1, Left1, Top1+20 ); Icono1->setPixmap(Icon1);
			Panel->moveChild( Icono3, Left3+4, Top3-10 ); Icono3->setPixmap(Icon3);
			Panel->moveChild( Icono4, Left4, Top4-20 ); Icono4->setPixmap(Icon4);
			Panel->moveChild( Icono5, Left5+4, Top5-26 ); Icono5->setPixmap(Icon5);
			Panel->moveChild( trackinfo, 321, 321 );
		}
	}

}

void ScreenX::Recargar()
{
	ZConfig Vini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);

	animado=0; 
	QString cval = Vini.readEntry(QString("AM"), QString("Walltype"), "Default");
	sysval = Vini.readEntry(QString("AM"), QString("SysInfo"), "No");
	QString showBase="Yes"; QString ownBase="No";
	QPixmap f1; QString baseWPC;

	if ( cval=="Wallchanger" ) {
		if ( Vini.readEntry(QString("AM"), QString("Wall"), "") == "" ) { LeerDatos(); f1.load( SWall ); }
		else f1.load( Vini.readEntry(QString("AM"), QString("Wall"), "") );
	} 
	else if ( cval=="Animate" )
	{
		f1.load( Vini.readEntry(QString("AM"), QString("AnimationImage"), "") );
	}
	else if ( cval=="Wallclock" )
	{
		WCS = Vini.readEntry(QString("AM"), QString("WallclockSelected"), "01");
		ZConfig WCini( ProgDir + "/screen/" + WCS + "/settings.cfg", true);
		sysval = WCini.readEntry(QString("SCREEN"), QString("SysInfo"), "No");
		showBase = WCini.readEntry(QString("SCREEN"), QString("ShowBase"), "Yes");
		if (showBase=="Yes") {
			ownBase = WCini.readEntry(QString("SCREEN"), QString("OwnBase"), "No");
			if ( ownBase=="Yes" ) baseWPC = ProgDir + "/screen/" + WCS + "/base.png";

		}

		LeerDatos(); f1.load( SWall );
		if ( WCini.readEntry(QString("SCREEN"), QString("Wallchanger"), "No") == "Yes" )
		{
			if ( Vini.readEntry(QString("AM"), QString("Wall"), "") != "" ) f1.load( Vini.readEntry(QString("AM"), QString("Wall"), "") );
		}
		setScreenFSBA( f1 , 100, (PIXMAP_STRETCH_POLICY_E)5 );
		QPixmap* pepe = this->getScreenFSBA();
		QImage img1 = pepe->convertToImage(); 
		QPixmap pepe2; pepe2.convertFromImage(img1);	
		QPixmap rfondo = pepe2;
		QPainter p(&rfondo);
		QPixmap f2;
		f2.load( ProgDir + "/screen/" + WCS + "/" + WCini.readEntry(QString("SCREEN"), QString("Wallpaper"), "") );
		p.drawPixmap(0,0,f2);
		f1 = rfondo;
		readWallClock(); 
	}
	else if ( cval=="Default" )
	{
		f1.load( SWall );
	}

	if ( cval=="Animate" ) 
	{
		QString pepe = Vini.readEntry(QString("AM"), QString("AnimationImage"), "");
		fondoi = QMovie(pepe);
		fondo->setMovie(fondoi);
		Panel->moveChild( fondo, 0, 0 - this->headerSize().height() );
		animado=1;
	}
	else 
	{
		fondoi = NULL;
		fondo->setMovie(NULL); 
		Panel->moveChild( fondo, 321, 321 );
	}

	preClock = f1;
	setScreenFSBA( f1 , 100, (PIXMAP_STRETCH_POLICY_E)5 );

	if ( (showBase=="Yes") || (analogclock==1) || (shownotes=="Yes") )
	{
		QPixmap* pepe = this->getScreenFSBA();
		QImage img1 = pepe->convertToImage(); 
		QPixmap pepe2; pepe2.convertFromImage(img1);	
		QPixmap rfondo = pepe2;
		QPainter p0(&rfondo);
		//p0.drawPixmap(0,0,pepe2);
		basei.load(ProgDir+"/null.png");
		if ( barHidden==0 ) {  
			if (ownBase=="No") 
			{
				if ( (BarPosition=="Up") || (BarPosition=="Down") )
				{
				  if (showBase=="Yes") 
				  {
					if (BarPosition=="Up") basei.load(ProgDir + "/base/up.png");
					else if (BarPosition=="Down") basei.load(ProgDir + "/base/down.png");
					p0.drawPixmap(0,this->headerSize().height()+Top1-10,basei);
				  }
				}
				else
				{
				  if (showBase=="Yes") 
				  {
					if (BarPosition=="Left") basei.load(ProgDir + "/base/left.png");
					else if (BarPosition=="Right") basei.load(ProgDir + "/base/right.png");
					QImage img1 = basei.convertToImage(); 
					img1 = img1.smoothScale( 60, 320-this->headerSize().height()-softKey->height() ); 
					basei.convertFromImage(img1);
					if (BarPosition=="Left") p0.drawPixmap(0,this->headerSize().height(),basei);
					else if (BarPosition=="Right") p0.drawPixmap(180,this->headerSize().height(),basei);
				  }
				}
			}
			else
			{
				basei.load(baseWPC);
				p0.drawPixmap(0,this->headerSize().height()+Top-10,basei);
			}
		}

		if (analogclock==1) 
		{
			ianalog = QPixmap( iconReader.getIcon("analog_clock_face", false) );
			
			int CL2 = ClockPos; 
			//if ( CL2 > 6 ) CL2 = CL2 + 22 - ianalog.height();
			int left1 = 240-ianalog.width()-6;
			int bottomp = 320 - softKey->height() - 6 - this->headerSize().height() - ianalog.height();
			if ( BarPosition == "Up" ) { if (shownotes=="No") CL2 = bottomp; else CL2 = bottomp-24; }
			if ( BarPosition == "Down" ) { if (shownotes=="No") CL2 = 6; else CL2 = 30; }
			if ( BarPosition == "Right" ) { left1 = 6; CL2 = bottomp; }
			else if ( BarPosition == "Left" ) { CL2 = bottomp; }

			if ( cval=="Wallclock" )
			{
				ZConfig WCini( ProgDir + "/screen/" + WCS + "/settings.cfg", true);
				QString wci = WCini.readEntry(QString("ANALOG"), QString("Image"), "");
				ianalog.load(ProgDir + "/screen/" + WCS + "/" + wci);
				left1 = WCini.readNumEntry(QString("ANALOG"), QString("Left"), 0);
				CL2 = WCini.readNumEntry(QString("ANALOG"), QString("Top"), 0);
			}
			
			analogtop=this->headerSize().height()+CL2;
			analogleft=left1;
			analogheight=ianalog.height();
			analogwidth=ianalog.width();

			p0.drawPixmap(analogleft,analogtop,ianalog);

		}

		if ( (shownotes=="Yes") && (cval!="Wallclock") )
		{
			ianalog = QPixmap( iconReader.getIcon("notepad_std", false) );
			QImage img1 = ianalog.convertToImage(); img1 = img1.smoothScale( 20, 20 ); ianalog.convertFromImage(img1);
			if ( BarPosition == "Up" ) p0.drawPixmap(6,320-softKey->height()-28,ianalog);
			if ( BarPosition == "Down" ) p0.drawPixmap(6,this->headerSize().height()+6,ianalog);
			if ( BarPosition == "Left" ) p0.drawPixmap(216,this->headerSize().height()+6,ianalog);
			if ( BarPosition == "Right" ) p0.drawPixmap(6,this->headerSize().height()+6,ianalog);
		}

		preClock = rfondo;
		setScreenFSBA( rfondo , 100, (PIXMAP_STRETCH_POLICY_E)5 );
	}

	setFullScreenMode(true, true);
	setFullScreenMode(false, false);


}

void ScreenX::CargarProgramas()
{
	if ( isMediaPlayer==0 )
	{
		ZConfig Vini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
		ProgCount = Vini.readNumEntry(QString("AM"), QString("Icons"), 0);
		
		if ( BarStyle==2 )
		{
			int VM = prePos;
			int ix1=0, ix2=0, ix3=0, ix4=0, ix5=0;

			if ( (VM>0) && (VM+3<ProgCount) ) { ix1=VM; ix2=VM+1; ix3=VM+2; ix4=VM+3; ix5=VM+4; }
			else if ( VM==0 ) { ix1 = ProgCount; ix2=1; ix3=2; ix4=3; ix5=4; }
			else if ( VM==-1 ) { ix1 = ProgCount-1; ix2=ProgCount; ix3=1; ix4=2; ix5=3; }
			else if ( VM==-2 ) { ix1 = ProgCount-2; ix2=ProgCount-1; ix3=ProgCount; ix4=1; ix5=2; }
			else if ( VM==-3 ) { ix1 = ProgCount-3; ix2=ProgCount-2; ix3=ProgCount-1; ix4=ProgCount; ix5=1; }
			else if ( VM<-3 ) { VM=ProgCount-4; ix1=VM; ix2=VM+1; ix3=VM+2; ix4=VM+3; ix5=VM+4; prePos=ProgCount-4; }
			else if ( VM+4 == ProgCount+1 ) { ix1=VM; ix2=VM+1; ix3=VM+2; ix4=VM+3; ix5=1; }
			else if ( VM+4 == ProgCount+2 ) { ix1=VM; ix2=VM+1; ix3=VM+2; ix4=1; ix5=2; }
			else if ( VM+4 == ProgCount+3 ) { ix1=VM; ix2=VM+1; ix3=1; ix4=2; ix5=3; }
			else if ( VM+4 == ProgCount+4 ) { ix1=VM; ix2=1; ix3=2; ix4=3; ix5=4; }
			else if ( VM+4 == ProgCount+5 ) { VM=1; ix1=VM; ix2=VM+1; ix3=VM+2; ix4=VM+3; ix5=VM+4; prePos=1; }

			Prog1 = Vini.readEntry(QString("PROGRAMS"), QString("Program%1").arg(ix1), "");
			Prog2 = Vini.readEntry(QString("PROGRAMS"), QString("Program%1").arg(ix2), "");
			Prog3 = Vini.readEntry(QString("PROGRAMS"), QString("Program%1").arg(ix3), "");
			Prog4 = Vini.readEntry(QString("PROGRAMS"), QString("Program%1").arg(ix4), "");
			Prog5 = Vini.readEntry(QString("PROGRAMS"), QString("Program%1").arg(ix5), "");

			Exec1 = Vini.readEntry(QString("PROGRAMS"), QString("Exec%1").arg(ix1), "");
			Exec2 = Vini.readEntry(QString("PROGRAMS"), QString("Exec%1").arg(ix2), "");
			Exec3 = Vini.readEntry(QString("PROGRAMS"), QString("Exec%1").arg(ix3), "");
			Exec4 = Vini.readEntry(QString("PROGRAMS"), QString("Exec%1").arg(ix4), "");
			Exec5 = Vini.readEntry(QString("PROGRAMS"), QString("Exec%1").arg(ix5), "");

			QString val = Vini.readEntry(QString("PROGRAMS"), QString("Icon%1").arg(ix1), "");
			int i = val.findRev("/"); if ( i > -1 ) {
				if ( val[0] != QChar('/') ) { val=getFilePath(Exec1)+val; } Icon1.load(val);
			} else { Icon1 = QPixmap( iconReader.getIcon(val, false) ); }
			val = Vini.readEntry(QString("PROGRAMS"), QString("Icon%1").arg(ix2), "");
			i = val.findRev("/"); if ( i > -1 ) {
				if ( val[0] != QChar('/') ) { val=getFilePath(Exec2)+val; } Icon2.load(val);
			} else { Icon2 = QPixmap( iconReader.getIcon(val, false) ); }
			val = Vini.readEntry(QString("PROGRAMS"), QString("Icon%1").arg(ix3), "");
			i = val.findRev("/"); if ( i > -1 ) {
				if ( val[0] != QChar('/') ) { val=getFilePath(Exec3)+val; } Icon3.load(val);
			} else { Icon3 = QPixmap( iconReader.getIcon(val, false) ); }
			val = Vini.readEntry(QString("PROGRAMS"), QString("Icon%1").arg(ix4), "");
			i = val.findRev("/"); if ( i > -1 ) {
				if ( val[0] != QChar('/') ) { val=getFilePath(Exec4)+val; } Icon4.load(val);
			} else { Icon4 = QPixmap( iconReader.getIcon(val, false) ); }
			val = Vini.readEntry(QString("PROGRAMS"), QString("Icon%1").arg(ix5), "");
			i = val.findRev("/"); if ( i > -1 ) {
				if ( val[0] != QChar('/') ) { val=getFilePath(Exec5)+val; } Icon5.load(val);
			} else { Icon5 = QPixmap( iconReader.getIcon(val, false) ); }

			if ( ProgCount > 5 ) 
			{
				if ( (BarPosition=="Up") || (BarPosition=="Down") ) fizqi = QPixmap( iconReader.getIcon("mp_lft_arrw_p_nav", false) );
				if ( (BarPosition=="Left") || (BarPosition=="Right") ) fizqi = QPixmap( iconReader.getIcon("mp_up_arrw_p_nav", false) );
			}
			else fizqi.load( ProgDir + "/null.png" );

			if ( ProgCount > 5 )
			{
				if ( (BarPosition=="Up") || (BarPosition=="Down") ) fderi = QPixmap( iconReader.getIcon("mp_rt_arrw_p_nav", false) );
				if ( (BarPosition=="Left") || (BarPosition=="Right") ) fderi = QPixmap( iconReader.getIcon("mp_dwn_arrw_p_nav", false) );
			}
			else fderi.load( ProgDir + "/null.png" );

			Pos=IconFixed;

		}
		else
		{
			Prog1 = Vini.readEntry(QString("PROGRAMS"), QString("Program%1").arg(1+CM), "");
			Prog2 = Vini.readEntry(QString("PROGRAMS"), QString("Program%1").arg(2+CM), "");
			Prog3 = Vini.readEntry(QString("PROGRAMS"), QString("Program%1").arg(3+CM), "");
			Prog4 = Vini.readEntry(QString("PROGRAMS"), QString("Program%1").arg(4+CM), "");
			Prog5 = Vini.readEntry(QString("PROGRAMS"), QString("Program%1").arg(5+CM), "");

			Exec1 = Vini.readEntry(QString("PROGRAMS"), QString("Exec%1").arg(1+CM), "");
			Exec2 = Vini.readEntry(QString("PROGRAMS"), QString("Exec%1").arg(2+CM), "");
			Exec3 = Vini.readEntry(QString("PROGRAMS"), QString("Exec%1").arg(3+CM), "");
			Exec4 = Vini.readEntry(QString("PROGRAMS"), QString("Exec%1").arg(4+CM), "");
			Exec5 = Vini.readEntry(QString("PROGRAMS"), QString("Exec%1").arg(5+CM), "");

			QString val = Vini.readEntry(QString("PROGRAMS"), QString("Icon%1").arg(1+CM), "");
			int i = val.findRev("/"); if ( i > -1 ) {
				if ( val[0] != QChar('/') ) { val=getFilePath(Exec1)+val; } Icon1.load(val);
			} else { Icon1 = QPixmap( iconReader.getIcon(val, false) ); }
			val = Vini.readEntry(QString("PROGRAMS"), QString("Icon%1").arg(2+CM), "");
			i = val.findRev("/"); if ( i > -1 ) {
				if ( val[0] != QChar('/') ) { val=getFilePath(Exec2)+val; } Icon2.load(val);
			} else { Icon2 = QPixmap( iconReader.getIcon(val, false) ); }
			val = Vini.readEntry(QString("PROGRAMS"), QString("Icon%1").arg(3+CM), "");
			i = val.findRev("/"); if ( i > -1 ) {
				if ( val[0] != QChar('/') ) { val=getFilePath(Exec3)+val; } Icon3.load(val);
			} else { Icon3 = QPixmap( iconReader.getIcon(val, false) ); }
			val = Vini.readEntry(QString("PROGRAMS"), QString("Icon%1").arg(4+CM), "");
			i = val.findRev("/"); if ( i > -1 ) {
				if ( val[0] != QChar('/') ) { val=getFilePath(Exec4)+val; } Icon4.load(val);
			} else { Icon4 = QPixmap( iconReader.getIcon(val, false) ); }
			val = Vini.readEntry(QString("PROGRAMS"), QString("Icon%1").arg(5+CM), "");
			i = val.findRev("/"); if ( i > -1 ) {
				if ( val[0] != QChar('/') ) { val=getFilePath(Exec5)+val; } Icon5.load(val);
			} else { Icon5 = QPixmap( iconReader.getIcon(val, false) ); }

			if ( CM > 0 )
			{
				if ( (BarPosition=="Up") || (BarPosition=="Down") ) fizqi = QPixmap( iconReader.getIcon("mp_lft_arrw_p_nav", false) );
				if ( (BarPosition=="Left") || (BarPosition=="Right") ) fizqi = QPixmap( iconReader.getIcon("mp_up_arrw_p_nav", false) );
			} 
			else fizqi.load( ProgDir + "/null.png" );

			if ( CM < ProgCount-5 )
			{
				if ( (BarPosition=="Up") || (BarPosition=="Down") ) fderi = QPixmap( iconReader.getIcon("mp_rt_arrw_p_nav", false) );
				if ( (BarPosition=="Left") || (BarPosition=="Right") ) fderi = QPixmap( iconReader.getIcon("mp_dwn_arrw_p_nav", false) );
			} 
			else fderi.load( ProgDir + "/null.png" );


		}
	}
	else
	{
		fizqi.load( ProgDir + "/null.png" ); fderi.load( ProgDir + "/null.png" );
		if ( mchannel==1 ) 
		{
			Icon1 = QPixmap( iconReader.getIcon("media_player_std", false) );
		}
		else
		{
			getTrackInfo(); getAlbumArt();
		}
		Icon2.load( ProgDir + "/null.png" );
		Icon3 = QPixmap( iconReader.getIcon("mp_prev_unp_nav", false) );
		Icon4 = QPixmap( iconReader.getIcon("mp_play_unp_nav", false) );
		Icon5 = QPixmap( iconReader.getIcon("mp_next_unp_nav", false) );

	}
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

  ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
  int val = ini.readNumEntry(QString("AM"), QString("Menu"), 0);

  if ( val == 0 )
  {
	AM_MainMenuInterface pepe;
	pepe.ShowMainMenu();
  }
  else if ( val == 1 )
  {
	qApp->processEvents();
	MyMenu *zfod = new MyMenu(lugar);
	zfod->exec();
	delete zfod;
	zfod = NULL;
  }
  Clocktick();

}

void ScreenX::Create_SoftKey()
{
  QString IDval; QString IDval2; ZConfig IDini(Idioma, true);
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
  QString IDval; ZConfig IDini(Idioma, true);
  unlocktext = IDini.readEntry(QString("COMMON"), QString("MENU_UNLOCK"), "");
  QRect rect;
  QPixmap* pm  = new QPixmap();
  QPixmap pm2; QImage image;
  int idx = 0;
  menu = new ZOptionsMenu ( rect, softKey, NULL, 0);
  menu->setItemSpacing(10);  
  pm2 = QPixmap( iconReader.getIcon("security_std", false) );
  image = pm2.convertToImage(); image = image.smoothScale( 16, 16 ); pm2.convertFromImage(image);
  pm = new QPixmap(pm2);
  IDval = IDini.readEntry(QString("MAINMENU"), QString("LOCK"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( bloquear() ) );
  ++idx;
  pm2 = QPixmap( iconReader.getIcon("camera_std", false) );
  image = pm2.convertToImage(); image = image.smoothScale( 16, 16 ); pm2.convertFromImage(image);
  pm = new QPixmap(pm2);
  IDval = IDini.readEntry(QString("MAINMENU"), QString("CAPTURE"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( CamButton() ) );      
  ++idx;
  pm2 = QPixmap( iconReader.getIcon("msg_std", false) );
  image = pm2.convertToImage(); image = image.smoothScale( 16, 16 ); pm2.convertFromImage(image);
  pm = new QPixmap(pm2);
  IDval = IDini.readEntry(QString("MAINMENU"), QString("CREATESMS"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( CreateSMS() ) );      
  ++idx;
  pm2 = QPixmap( iconReader.getIcon("netwk_std", false) );
  image = pm2.convertToImage(); image = image.smoothScale( 16, 16 ); pm2.convertFromImage(image);
  pm = new QPixmap(pm2);
  SETUP_Utility pepe; int i = pepe.isAirplaneModeOn();
  if ( i==1 ) { IDval = IDini.readEntry(QString("MAINMENU"), QString("PLANEMODEOFF"), ""); }
  else { IDval = IDini.readEntry(QString("MAINMENU"), QString("PLANEMODEON"), ""); }
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( ModoAvion() ) );      
  ++idx;
  pm2 = QPixmap( iconReader.getIcon("bluetooth_std", false) );
  image = pm2.convertToImage(); image = image.smoothScale( 16, 16 ); pm2.convertFromImage(image);
  pm = new QPixmap(pm2);
  if ( QFileInfo("/tmp/BT.on").isFile() ) { IDval = IDini.readEntry(QString("MAINMENU"), QString("BLUETOOTHOFF"), ""); }
  else { IDval = IDini.readEntry(QString("MAINMENU"), QString("BLUETOOTHON"), ""); }
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( Bluetooth() ) );      
  ++idx;
  pm2 = QPixmap( iconReader.getIcon("settings_std", false) );
  image = pm2.convertToImage(); image = image.smoothScale( 16, 16 ); pm2.convertFromImage(image);
  pm = new QPixmap(pm2);
  IDval = IDini.readEntry(QString("MAINMENU"), QString("SCREEN"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( ScreenOptions() ) );      
  ++idx;
  pm2 = QPixmap( iconReader.getIcon("alarm_clock_std", false) );
  image = pm2.convertToImage(); image = image.smoothScale( 16, 16 ); pm2.convertFromImage(image);
  pm = new QPixmap(pm2);
  IDval = IDini.readEntry(QString("MAINMENU"), QString("ALARM"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( Alarm() ) );      
  ++idx;
  pm2 = QPixmap( iconReader.getIcon("info_pop", false) );
  image = pm2.convertToImage(); image = image.smoothScale( 16, 16 ); pm2.convertFromImage(image);
  pm = new QPixmap(pm2);

}

void ScreenX::bloquear()
{
  qApp->processEvents();
  softKey->setText ( ZSoftKey::LEFT, "", ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::RIGHT, unlocktext, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  keyLocked="YES"; //reloj->stop();
//  system(QString("%1/lock.sh&").arg(ProgDir));
	UTIL_KeyboardLockClient pepe;
	pepe.connectToServer();
	pepe.lockKeyboard();
	pepe.disConnect();
  if ( sysval=="Yes" ) SystemInfo();

}

void ScreenX::Desbloquear()
{
	tiempoespera=0; 
  	if ( timerStop==1 ) { timerStop=0; reloj->start(5000, false); Clocktick(); }
	
	zLock* msg = new zLock(Idioma);
	int i = msg->exec(); delete msg; msg = NULL;
	if (i==1)
	{ 
		keyLocked="NO"; 
		Create_SoftKey(); 
	}
	qApp->processEvents();
	if ( sysval=="Yes" ) SystemInfo();
	
}

void ScreenX::DesbloquearAhora()
{
	tiempoespera=0; 
  	//if ( timerStop==1 ) { timerStop=0; reloj->start(5000, false); Clocktick(); }
	keyLocked="NO"; Create_SoftKey();
	qApp->processEvents();
	
}

void ScreenX::Alarm()
{
	qApp->processEvents();
	QString run = "{6aff7618-c940-48d9-ad3b-3106d9a884c2}";
	ZMsg msg( "/AM/Launcher", "launchApp" ); msg << (QUuid)run; msg.send();

}

void ScreenX::CamButton()
{
	//qApp->processEvents();
	//QString run = "{c1e0df66-11ef-4b33-bddb-0cbd13827312}";
	//ZMsg msg( "/AM/Launcher", "launchApp" ); msg << (QUuid)run; msg.send();
	AM_ActionData ad;
	ad.setAction("TakingPhoto");
	ad.invoke();
    
}

void ScreenX::CreateSMS()
{
	ZSMS pepe;
	ZSMS_OutgoingMsg mail(QString(""),QString(""));
	pepe.composeMsg(mail);

	//MSGCTR_ENG_StatusApi* pepe = new MSGCTR_ENG_StatusApi(this,"");
	//pepe->gotoScreenOfUnreadMsgS();
	

}

void ScreenX::ModoAvion()
{
	qApp->processEvents();
	SETUP_Utility pepe;
	int i = pepe.isAirplaneModeOn();
	if ( i==1 )
	{
		pepe.setAirplaneMode(false); 
	}
	else
	{
		pepe.setAirplaneMode(true);
	}
	CheckAvion();
}

void ScreenX::CheckAvion()
{
	QString IDval; ZConfig IDini(Idioma, true);
	SETUP_Utility pepe;
	QPixmap pm2 = QPixmap( iconReader.getIcon("netwk_std", false) );
	QImage image = pm2.convertToImage(); image = image.smoothScale( 16, 16 ); pm2.convertFromImage(image);
	QPixmap* pm = new QPixmap(pm2);
	if ( pepe.isAirplaneModeOn() )
	{
		IDval = IDini.readEntry(QString("MAINMENU"), QString("PLANEMODEOFF"), "");
		menu->changeItem(3, IDval, pm);
	}
	else
	{
		IDval = IDini.readEntry(QString("MAINMENU"), QString("PLANEMODEON"), "");
		menu->changeItem(3, IDval, pm);
	}

}

void ScreenX::Bluetooth()
{
	qApp->processEvents();
	QString IDval; ZConfig IDini(Idioma, true);
	if ( QFileInfo("/tmp/BT.on").isFile() )
	{
	  MBTAPI_CTRL_PowerOff();
	  system("rm /tmp/BT.on");
	}
	else
	{
	  BT_PowerOn();
	  btoothon=1;
	}
	CheckBT();
}

void ScreenX::CheckBT()
{
	QString IDval; ZConfig IDini(Idioma, true);
	qApp->processEvents();
	QPixmap pm2 = QPixmap( iconReader.getIcon("bluetooth_std", false) );
	QImage image = pm2.convertToImage(); image = image.smoothScale( 16, 16 ); pm2.convertFromImage(image);
	QPixmap* pm = new QPixmap(pm2);

	if ( QFileInfo("/tmp/BT.on").isFile() ) {
		//ZHeader::changeStatus(ZHeader::BT, 0);
		IDval = IDini.readEntry(QString("MAINMENU"), QString("BLUETOOTHOFF"), "");
		menu->changeItem(4, IDval, pm);
	} else {
		//ZHeader::changeStatus(ZHeader::BT, 1);
		IDval = IDini.readEntry(QString("MAINMENU"), QString("BLUETOOTHON"), "");
		menu->changeItem(4, IDval, pm);
	}
}


void ScreenX::ScreenOptions()
{
	qApp->processEvents();
	ZScreen *zfod = new ZScreen();
	int ret = zfod->exec();
	delete zfod;
	zfod = NULL;
	ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);

	refrescar="Yes";

	ZConfig Fini1(QString("/ezxlocal/download/appwrite/setup/ezx_theme.cfg"), true);
	if ( currentTheme != Fini1.readEntry(QString("THEME_GROUP_LOG"), QString("CurrentSkin"), "") ) RecargarMenu();

	LeerDatos();

	eti1->setOutline(true,QRgb(TextColorO),QRgb(TextColor));	

	barHidden=0; TODO=1; timerCount=0;
	qApp->processEvents();
	CheckOn=0;
	CheckPositions();
	Recargar();
	RecargarIconos();
	Clocktick();
	if ( analogclock==1 ) AnalogClock();

}


void ScreenX::toLog(QString mes)
{
	if (mes != NULL)
		qDebug ( mes );
}


QString ScreenX::getProgramDir()
{
	ProgDir = QString ( qApp->argv() [0] ) ;
	int i = ProgDir.findRev ( "/" );
	ProgDir.remove ( i+1, ProgDir.length() - i );
	return ProgDir;
}

void ScreenX::CheckPositions()
{
	wallchangeron=0;
	showdate=0; showtime=0;
	LeerDatos();
	if ( CheckOn==1 ) return;
	CheckOn=1;
	analogclock=0;
	int y = this->headerSize().height();
	y = y + softKey->height() + 40;
	ZConfig Vini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
	centerkey = Vini.readNumEntry(QString("AM"), QString("CenterKey"), 0);
	QString cval = Vini.readEntry(QString("AM"), QString("Walltype"), "Default");
	sysval = Vini.readEntry(QString("AM"), QString("SysInfo"), "No");
	shownotes = Vini.readEntry(QString("AM"), QString("Notes"), "No");
	int bottompos = 320-this->headerSize().height()-softKey->height()-28;
	IconText = Vini.readEntry(QString("AM"), QString("IconText"), "No");
	BarPosition = Vini.readEntry(QString("AM"), QString("BarPosition"), "Up");
	BarStyle = Vini.readNumEntry(QString("AM"), QString("BarStyle"), 0);
	if ( Vini.readEntry(QString("AM"), QString("Clock"), "") == "Digital") showtime = 1;
	if ( Vini.readEntry(QString("AM"), QString("Date"), "") == "Yes") showdate = 1;
	BarStyle = Vini.readNumEntry(QString("AM"), QString("BarStyle"), 0);
	if ( Vini.readEntry(QString("AM"), QString("Clock"), "") == "Analog" ) analogclock=1;

	if ( cval=="Wallchanger" ) wallchangeron=1;
	Panel->moveChild( AlbumArt, 321, 321 );

	iconOn=40; ic1=30; ic2=30; ic3=30; ic4=30, ic5=30;
	Panel->moveChild( hora1, 321, 321 ); Panel->moveChild( hora2, 321, 321 ); Panel->moveChild( hora3, 321, 321 ); Panel->moveChild( hora4, 321, 321 );
	Panel->moveChild( dia1, 321, 321 ); Panel->moveChild( dia2, 321, 321 ); Panel->moveChild( dia3, 321, 321 ); Panel->moveChild( dia4, 321, 321 );

	if ( Vini.readEntry(QString("AM"), QString("Walltype"), "Default") == "Wallclock" )
	{
		showdate=0; showtime=0; analogclock=0;
		WCS = Vini.readEntry(QString("AM"), QString("WallclockSelected"), "01");
		ZConfig WCini(QString(ProgDir + "/screen/" + WCS + "/settings.cfg"), true);
		BarPosition = WCini.readEntry(QString("SCREEN"), QString("Icons"), "Top");
		sysval = WCini.readEntry(QString("SCREEN"), QString("SysInfo"), "No");
		shownotes = WCini.readEntry(QString("SCREEN"), QString("Notes"), "No");
		if ( WCini.readEntry(QString("SCREEN"), QString("Analog"), "No") == "Yes" ) analogclock=1;
		if ( WCini.readEntry(QString("SCREEN"), QString("ShowDate"), "") == "Yes" ) showdate=1;
		if ( WCini.readEntry(QString("SCREEN"), QString("ShowTime"), "") == "Yes" ) showtime=1;
		if ( BarPosition == "Top" ) { BarPosition = "Up"; }
		else if ( BarPosition == "Bottom") { BarPosition = "Down"; }
		int nlin = WCini.readNumEntry(QString("NOTES"), QString("Lines"), 1);
		lNotes->setNumLines(nlin); 
		nlin = WCini.readNumEntry(QString("NOTES"), QString("Leading"), 1);
		lNotes->setLeading(nlin);

		if ( WCini.readEntry(QString("SCREEN"), QString("Wallchanger"), "No") == "Yes" ) wallchangeron=1;

	}

	int notestop=0, notesleft=0; 
	int notespace=0; if (shownotes=="Yes") notespace=24;
	
	if ( BarPosition=="Up" ) 
	{
		Top = 10; Top1=Top; Top2=Top; Top3=Top; Top4=Top; Top5=Top;
		Left1 = 23; Left2 = 64; Left3 = 105; Left4 = 146; Left5 = 187;
		ClockPos=bottompos; Vini.writeEntry(QString("AM"), QString("ClockPos"), ClockPos-notespace);
		Panel->moveChild( eti1, 20, Top+40 ); eti1->setAlignment(ZLabel::AlignCenter);
		Panel->moveChild( fizq, 6, Top+10 ); Panel->moveChild( fder, 227, Top+10 );
		TopSI=320-softKey->height()-26; LeftSI=0;
		notestop = 320-softKey->height()-48; notesleft = 30;
	}
	else if ( BarPosition=="Down" ) 
	{
		Top = 320 - y; Top1=Top; Top2=Top; Top3=Top; Top4=Top; Top5=Top;
		Left1 = 23; Left2 = 64; Left3 = 105; Left4 = 146; Left5 = 187;
		ClockPos=6; Vini.writeEntry(QString("AM"), QString("ClockPos"), ClockPos+notespace);
		Panel->moveChild( eti1, 20, Top-36 ); eti1->setAlignment(ZLabel::AlignCenter);
		Panel->moveChild( fizq, 6, Top+10 ); Panel->moveChild( fder, 227, Top+10 );
		TopSI=this->headerSize().height(); LeftSI=0;
		notestop = 6; notesleft = 30;
	}
	else if ( BarPosition=="Left" ) 
	{
		int espacio = 320 - this->headerSize().height() - softKey->height() - 40;
		int espacio2 = espacio/5;
		Top = 30; Top1=Top; 
		Top2=Top+espacio2; Top3=Top+espacio2+espacio2; 
		Top4=Top+espacio2+espacio2+espacio2; Top5=Top+espacio2+espacio2+espacio2+espacio2;
		//Top = 10; Top1=30; Top2=76; Top3=122; Top4=168; Top5=214;
		Left1 = 10; Left2 = 10; Left3 = 10; Left4 = 10; Left5 = 10;
		ClockPos=6; Vini.writeEntry(QString("AM"), QString("ClockPos"), ClockPos+notespace);
		Panel->moveChild( eti1, 55, Top3+4 ); eti1->setAlignment(ZLabel::AlignLeft);
		Panel->moveChild( fizq, 20, 10 ); Panel->moveChild( fder, 20, 320-softKey->height()-this->headerSize().height()-17 );
		TopSI=this->headerSize().height(); LeftSI=115;
		notestop = 6; notesleft = 90;
	}
	else if ( BarPosition=="Right" ) 
	{
		int espacio = 320 - this->headerSize().height() - softKey->height() - 40;
		int espacio2 = espacio/5;
		Top = 30; Top1=Top; 
		Top2=Top+espacio2; Top3=Top+espacio2+espacio2; 
		Top4=Top+espacio2+espacio2+espacio2; Top5=Top+espacio2+espacio2+espacio2+espacio2;
		//Top = 10; Top1=30; Top2=76; Top3=122; Top4=168; Top5=214;
		Left1 = 200; Left2 = 200; Left3 = 200; Left4 = 200; Left5 = 200;
		ClockPos=6; Vini.writeEntry(QString("AM"), QString("ClockPos"), ClockPos+notespace);
		Panel->moveChild( eti1, -15, 130 ); eti1->setAlignment(ZLabel::AlignRight);
		Panel->moveChild( fizq, 210, 10 ); Panel->moveChild( fder, 210, 320-softKey->height()-this->headerSize().height()-17 );
		TopSI=this->headerSize().height(); LeftSI=0;
		notestop = 6; notesleft = 30;
	}
	
	int extra=0; int extra2=0;
	if ( (sysval=="Yes") && (cval!="Wallclock") )
	{
		systemInfo1->setOutline(true,QRgb(TextColorO),QRgb(TextColor));
		systemInfo2->setOutline(true,QRgb(TextColorO),QRgb(TextColor));
		systemInfo3->setOutline(true,QRgb(TextColorO),QRgb(TextColor));
		showSItext="Yes";
		if ( BarPosition=="Down") 
		{
			if ( (showdate==1) && (Vini.readEntry(QString("AM"),QString("Clock"),"")=="Analog") ) 
				extra=24+notespace; else extra=0+notespace;
			single="No"; //extra=26; systemInfo1->setAlignment(ZLabel::AlignCenter);
			QFont f = eti1->font(); f.setPixelSize(TextSize); systemInfo1->setFont(f); systemInfo2->setFont(f); systemInfo3->setFont(f);
			systemInfo1->setGeometry(8,6+extra,0,26); systemInfo2->setGeometry(8,30+extra,0,0); systemInfo3->setGeometry(8,54+extra,130,0);
			systemInfo1->setAlignment(ZLabel::AlignLeft); systemInfo2->setAlignment(ZLabel::AlignLeft); systemInfo3->setAlignment(ZLabel::AlignLeft);
		}
		else if ( BarPosition=="Up") 
		{
			if ( (showdate==1) && (Vini.readEntry(QString("AM"),QString("Clock"),"")=="Analog") ) 
				extra=320-softKey->height()-48-notespace; else extra=320-softKey->height()-24-notespace;
			single="No"; //extra=26; systemInfo1->setAlignment(ZLabel::AlignCenter);
			QFont f = eti1->font(); f.setPixelSize(TextSize); systemInfo1->setFont(f); systemInfo2->setFont(f); systemInfo3->setFont(f);
			systemInfo1->setGeometry(8,extra-24,0,26); systemInfo2->setGeometry(8,extra-48,0,0); systemInfo3->setGeometry(8,extra-72,130,0);
			systemInfo1->setAlignment(ZLabel::AlignLeft); systemInfo2->setAlignment(ZLabel::AlignLeft); systemInfo3->setAlignment(ZLabel::AlignLeft);
		}
		else if ( BarPosition=="Left") 
		{
			single="No"; extra=74; 
			QFont f = eti1->font(); f.setPixelSize(TextSize); systemInfo1->setFont(f); systemInfo2->setFont(f); systemInfo3->setFont(f);
			systemInfo1->setAlignment(ZLabel::AlignRight); systemInfo2->setAlignment(ZLabel::AlignRight); systemInfo3->setAlignment(ZLabel::AlignRight);
			systemInfo1->setGeometry(112,6+notespace,120,26);
			systemInfo2->setGeometry(112,30+notespace,120,26);
			systemInfo3->setGeometry(102,54+notespace,130,26);
		}
		else if ( BarPosition=="Right") 
		{
			single="No"; extra=74; 
			QFont f = eti1->font(); f.setPixelSize(TextSize); systemInfo1->setFont(f); systemInfo2->setFont(f); systemInfo3->setFont(f);
			systemInfo1->setAlignment(ZLabel::AlignRight); systemInfo2->setAlignment(ZLabel::AlignRight); systemInfo3->setAlignment(ZLabel::AlignRight);
			systemInfo1->setAlignment(ZLabel::AlignLeft); systemInfo2->setAlignment(ZLabel::AlignLeft); systemInfo3->setAlignment(ZLabel::AlignLeft);
			systemInfo1->setGeometry(8,6+notespace,120,26);
			systemInfo2->setGeometry(8,30+notespace,120,26);
			systemInfo3->setGeometry(8,54+notespace,130,26);
		}
	}
	else if ( (sysval=="Yes") && (cval=="Wallclock") )
	{
		ZConfig WCini(QString(ProgDir + "/screen/" + WCS + "/settings.cfg"), true);
		int xxx1, xxx2;
		single = WCini.readEntry(QString("SYSINFO"), QString("SingleLine"), "No");
		if ( single=="Yes" ) 
		{
			xxx1 = WCini.readNumEntry(QString("SYSINFO"), QString("Line1left"), 321);
			xxx2 = WCini.readNumEntry(QString("SYSINFO"), QString("Line1top"), 321);
			systemInfo1->setGeometry(xxx1,xxx2,215,26); Panel->moveChild(systemInfo1, xxx1, xxx2);
			systemInfo2->setGeometry(321,321,0,0); systemInfo3->setGeometry(321,321,0,0);
			int col1 = WCini.readNumEntry(QString("SYSINFO"), QString("Color1"), TextColor);
			int col2 = WCini.readNumEntry(QString("SYSINFO"), QString("Color2"), TextColorO);
			TextSize = WCini.readNumEntry(QString("SYSINFO"), QString("TextSize"), 12);
			QFont f = eti1->font(); f.setPixelSize(TextSize); systemInfo1->setFont(f); 
			if ( col2 == -1 ) { 
				systemInfo1->setFontColor(QRgb(col1)); 
				systemInfo1->setOutline(false);
			} else {
				systemInfo1->setOutline(true,QRgb(col2),QRgb(col1));
			}
			QString TextAlign = WCini.readEntry(QString("SYSINFO"), QString("Line1align"), "Left");
			if (TextAlign=="Left") systemInfo1->setAlignment(ZLabel::AlignLeft);
			else if (TextAlign=="Right") systemInfo1->setAlignment(ZLabel::AlignRight);
			else if (TextAlign=="Center") systemInfo1->setAlignment(ZLabel::AlignCenter);
		} 
		else
		{
			xxx1 = WCini.readNumEntry(QString("SYSINFO"), QString("Line1left"), 321);
			xxx2 = WCini.readNumEntry(QString("SYSINFO"), QString("Line1top"), 321);
			systemInfo1->setGeometry(xxx1,xxx2,120,26); Panel->moveChild(systemInfo1, xxx1, xxx2);
			xxx1 = WCini.readNumEntry(QString("SYSINFO"), QString("Line2left"), 321);
			xxx2 = WCini.readNumEntry(QString("SYSINFO"), QString("Line2top"), 321);
			systemInfo2->setGeometry(xxx1,xxx2,120,26); Panel->moveChild(systemInfo2, xxx1, xxx2);
			xxx1 = WCini.readNumEntry(QString("SYSINFO"), QString("Line3left"), 321);
			xxx2 = WCini.readNumEntry(QString("SYSINFO"), QString("Line3top"), 321);
			systemInfo3->setGeometry(xxx1,xxx2,120,26); Panel->moveChild(systemInfo3, xxx1, xxx2);
			showSItext = WCini.readEntry(QString("SYSINFO"), QString("ShowText"), "Yes");
			QString TextAlign = WCini.readEntry(QString("SYSINFO"), QString("Line1align"), "Left");
			if (TextAlign=="Left") systemInfo1->setAlignment(ZLabel::AlignLeft);
			else if (TextAlign=="Right") systemInfo1->setAlignment(ZLabel::AlignRight);
			else if (TextAlign=="Center") systemInfo1->setAlignment(ZLabel::AlignCenter);
			TextAlign = WCini.readEntry(QString("SYSINFO"), QString("Line2align"), "Left");
			if (TextAlign=="Left") systemInfo2->setAlignment(ZLabel::AlignLeft);
			else if (TextAlign=="Right") systemInfo2->setAlignment(ZLabel::AlignRight);
			else if (TextAlign=="Center") systemInfo2->setAlignment(ZLabel::AlignCenter);
			TextAlign = WCini.readEntry(QString("SYSINFO"), QString("Line3align"), "Left");
			if (TextAlign=="Left") systemInfo3->setAlignment(ZLabel::AlignLeft);
			else if (TextAlign=="Right") systemInfo3->setAlignment(ZLabel::AlignRight);
			else if (TextAlign=="Center") systemInfo3->setAlignment(ZLabel::AlignCenter);
			int col1 = WCini.readNumEntry(QString("SYSINFO"), QString("Color1"), TextColor);
			int col2 = WCini.readNumEntry(QString("SYSINFO"), QString("Color2"), TextColorO);
			TextSize = WCini.readNumEntry(QString("SYSINFO"), QString("TextSize"), 12);
			QFont f = eti1->font(); f.setPixelSize(TextSize); systemInfo1->setFont(f); systemInfo2->setFont(f); systemInfo3->setFont(f);
			if ( col2 == -1 ) { 
				systemInfo1->setFontColor(QRgb(col1)); systemInfo2->setFontColor(QRgb(col1)); systemInfo3->setFontColor(QRgb(col1));
				systemInfo1->setOutline(false);
				systemInfo2->setOutline(false);
				systemInfo3->setOutline(false);
			} else {
				systemInfo1->setOutline(true,QRgb(col2),QRgb(col1));
				systemInfo2->setOutline(true,QRgb(col2),QRgb(col1));
				systemInfo3->setOutline(true,QRgb(col2),QRgb(col1));
			}
		}
	}
	else
	{
		systemInfo1->setGeometry(321,321,0,0); systemInfo2->setGeometry(321,321,0,0); systemInfo3->setGeometry(321,321,0,0);
	}

	if ( showdate == 1 )  
	{
		if (cval!="Wallclock")
		{
			if ( BarPosition=="Up") { 
			  if (Vini.readEntry(QString("AM"),QString("Clock"),"")=="Analog")
					{ Panel->moveChild( date, 8, 320-softKey->height()-48-notespace ); date->setAlignment(ZLabel::AlignLeft); }
			  else if (sysval=="No") { Panel->moveChild( date, 8, 320-softKey->height()-48-notespace ); date->setAlignment(ZLabel::AlignLeft); }
			  else { Panel->moveChild( date, 130, 320-softKey->height()-48-notespace ); date->setAlignment(ZLabel::AlignRight); }
			}
			else if ( BarPosition=="Down") { 
			  if (Vini.readEntry(QString("AM"),QString("Clock"),"")=="Analog")
					{ Panel->moveChild( date, 8, 6+notespace ); date->setAlignment(ZLabel::AlignLeft); }
			  else if (sysval=="No") { Panel->moveChild( date, 8, 6+notespace ); date->setAlignment(ZLabel::AlignLeft); }
			  else { Panel->moveChild( date, 130, 6+notespace ); date->setAlignment(ZLabel::AlignRight); }
			}
			//else if ( BarPosition=="Down") { Panel->moveChild( date, 6, ClockPos ); date->setAlignment(ZLabel::AlignLeft); }
			else if ( BarPosition=="Left") { extra2=24; Panel->moveChild( date, 130, ClockPos+extra+notespace ); date->setAlignment(ZLabel::AlignRight); }
			else if ( BarPosition=="Right") { extra2=24; Panel->moveChild( date, 8, ClockPos+extra+notespace ); date->setAlignment(ZLabel::AlignLeft); }
			QFont f = eti1->font(); f.setPixelSize(TextSize); date->setFont(f);
			date->setOutline(true,QRgb(TextColorO),QRgb(TextColor));
		}
	}
	else Panel->moveChild( date, 321, 321 );

	if ( showtime == 1 ) 
	{
		if (cval!="Wallclock")
		{
			if ( BarPosition=="Up") { 
				if ( (showdate==1) && (sysval=="Yes") )
					 { Panel->moveChild( clock, 130, 320-softKey->height()-72-notespace  ); clock->setAlignment(ZLabel::AlignRight); }
				else { Panel->moveChild( clock, 130, 320-softKey->height()-48-notespace  ); clock->setAlignment(ZLabel::AlignRight); }
			}
			else if ( BarPosition=="Down") { 
				if ( (showdate==1) && (sysval=="Yes") )
					 { Panel->moveChild( clock, 130, 30+notespace ); clock->setAlignment(ZLabel::AlignRight); }
				else { Panel->moveChild( clock, 130, 6+notespace ); clock->setAlignment(ZLabel::AlignRight); }
			}
			//else if ( BarPosition=="Down") { Panel->moveChild( clock, 130, ClockPos+extra ); clock->setAlignment(ZLabel::AlignRight); }
			else if ( BarPosition=="Left") { Panel->moveChild( clock, 130, ClockPos+extra+extra2+notespace ); clock->setAlignment(ZLabel::AlignRight); }
			else if ( BarPosition=="Right") { Panel->moveChild( clock, 8, ClockPos+extra+extra2+notespace ); clock->setAlignment(ZLabel::AlignLeft); }
			QFont f = eti1->font(); f.setPixelSize(TextSize); clock->setFont(f);
			clock->setOutline(true,QRgb(TextColorO),QRgb(TextColor));
		}
	}
	else Panel->moveChild( clock, 321, 321 );

	if ( Vini.readEntry(QString("AM"), QString("Walltype"), "Default") == "Wallclock" )
	{
		ZConfig WCini(QString(ProgDir + "/screen/" + WCS + "/settings.cfg"), true);

		notestop = this->headerSize().height() + WCini.readNumEntry(QString("NOTES"), QString("Top"), 6);
		notesleft = WCini.readNumEntry(QString("NOTES"), QString("Left"), 6);

		if ( WCini.readEntry(QString("SCREEN"), QString("AlbumArt"), "No") == "Yes" )
		{
			AAon=1;
			AAleft = WCini.readNumEntry(QString("ALBUMART"), QString("Left"), 321);
			AAtop = WCini.readNumEntry(QString("ALBUMART"), QString("Top"), 321);
			AAheight = WCini.readNumEntry(QString("ALBUMART"), QString("Height"), 30);
			AAwidth = WCini.readNumEntry(QString("ALBUMART"), QString("Width"), 30);
			Panel->moveChild( AlbumArt, AAleft, AAtop );
			getAlbumArt(); 
		}
		else { Panel->moveChild( AlbumArt, 321, 321 ); }

		if ( WCini.readEntry(QString("ICONS"), QString("IconText"), "Yes") == "Yes" )
		{
			int col1i = WCini.readNumEntry(QString("ICONTEXT"), QString("Color1"), TextColor);
			int col2i = WCini.readNumEntry(QString("ICONTEXT"), QString("Color2"), TextColorO);
			int iTextSize = WCini.readNumEntry(QString("ICONTEXT"), QString("TextSize"), TextSize);
			QFont f = eti1->font(); f.setPixelSize(iTextSize); eti1->setFont(f);
			if ( col2i == -1 ) { 
				eti1->setFontColor(QRgb(col1i));
				eti1->setOutline(false);
			} else {
				eti1->setOutline(true,QRgb(col2i),QRgb(col1i));
			}
		}
		
		Left1 = WCini.readNumEntry(QString("ICONS"), QString("Left1"), Left1);
		Left2 = WCini.readNumEntry(QString("ICONS"), QString("Left2"), Left2);
		Left3 = WCini.readNumEntry(QString("ICONS"), QString("Left3"), Left3);
		Left4 = WCini.readNumEntry(QString("ICONS"), QString("Left4"), Left4);
		Left5 = WCini.readNumEntry(QString("ICONS"), QString("Left5"), Left5);
		Top1 = WCini.readNumEntry(QString("ICONS"), QString("Top1"), Top1);
		Top2 = WCini.readNumEntry(QString("ICONS"), QString("Top2"), Top2);
		Top3 = WCini.readNumEntry(QString("ICONS"), QString("Top3"), Top3);
		Top4 = WCini.readNumEntry(QString("ICONS"), QString("Top4"), Top4);
		Top5 = WCini.readNumEntry(QString("ICONS"), QString("Top5"), Top5);
		IconText = WCini.readEntry(QString("SCREEN"), QString("IconText"), IconText);
		if ( IconText == "Yes" )
		{
			int TextLeft = WCini.readNumEntry(QString("ICONTEXT"), QString("Left"), 0);
			int TextTop = WCini.readNumEntry(QString("ICONTEXT"), QString("Top"), 0);
			QString TextAlign = WCini.readEntry(QString("ICONTEXT"), QString("Align"), "");
			if ( (TextLeft!=0) && (TextTop!=0) ) Panel->moveChild( eti1, TextLeft, TextTop ); 
			if ( TextAlign=="Right" ) eti1->setAlignment(ZLabel::AlignRight);
			else if ( TextAlign=="Left" ) eti1->setAlignment(ZLabel::AlignLeft);
			else if ( TextAlign=="Center" ) eti1->setAlignment(ZLabel::AlignCenter);
		} else Panel->moveChild( eti1, 321, 321 ); 
		BarStyle = WCini.readNumEntry(QString("ICONS"), QString("BarStyle"), BarStyle);
		IconFixed = WCini.readNumEntry(QString("ICONS"), QString("IconFixed"), 3);
		iconOn = WCini.readNumEntry(QString("ICONS"), QString("IconOn"), 40);
		ic1 = WCini.readNumEntry(QString("ICONS"), QString("Icon1size"), 30);
		ic2 = WCini.readNumEntry(QString("ICONS"), QString("Icon2size"), 30);
		ic3 = WCini.readNumEntry(QString("ICONS"), QString("Icon3size"), 30);
		ic4 = WCini.readNumEntry(QString("ICONS"), QString("Icon4size"), 30);
		ic5 = WCini.readNumEntry(QString("ICONS"), QString("Icon5size"), 30);

		if ( WCini.readEntry(QString("SCREEN"), QString("ShowDate"), "No") == "Yes" )
		{
			Panel->moveChild( date, WCini.readNumEntry(QString("TEXTDATE"), QString("Left"), 0),
									WCini.readNumEntry(QString("TEXTDATE"), QString("Top"), 0) );
			QString TextAlign = WCini.readEntry(QString("TEXTDATE"), QString("Align"), "Left");
			if (TextAlign=="Left") date->setAlignment(ZLabel::AlignLeft);
			else if (TextAlign=="Right") date->setAlignment(ZLabel::AlignRight);
			else if (TextAlign=="Center") date->setAlignment(ZLabel::AlignCenter);
			int xxx1 = WCini.readNumEntry(QString("TEXTDATE"), QString("Left"), 321);
			int xxx2 = WCini.readNumEntry(QString("TEXTDATE"), QString("Top"), 321);
			Panel->moveChild(date, xxx1,xxx2);
			int col1 = WCini.readNumEntry(QString("TEXTDATE"), QString("Color1"), TextColor);
			int col2 = WCini.readNumEntry(QString("TEXTDATE"), QString("Color2"), TextColorO);
			TextSize = WCini.readNumEntry(QString("TEXTDATE"), QString("TextSize"), 12);
			QFont f = eti1->font(); f.setPixelSize(TextSize); date->setFont(f);
			if ( col2 == -1 ) { 
				date->setFontColor(QRgb(col1));
				date->setOutline(false);
			} else {
				date->setOutline(true,QRgb(col2),QRgb(col1));
			}
		}

		if ( WCini.readEntry(QString("SCREEN"), QString("ShowTime"), "No") == "Yes" )
		{
			Panel->moveChild( clock, WCini.readNumEntry(QString("TEXTTIME"), QString("Left"), 0), 
									 WCini.readNumEntry(QString("TEXTTIME"), QString("Top"), 0) );
			QString TextAlign = WCini.readEntry(QString("TEXTTIME"), QString("Align"), "Left");
			if (TextAlign=="Left") clock->setAlignment(ZLabel::AlignLeft);
			else if (TextAlign=="Right") clock->setAlignment(ZLabel::AlignRight);
			else if (TextAlign=="Center") clock->setAlignment(ZLabel::AlignCenter);
			int xxx1 = WCini.readNumEntry(QString("TEXTTIME"), QString("Left"), 321);
			int xxx2 = WCini.readNumEntry(QString("TEXTTIME"), QString("Top"), 321);
			Panel->moveChild(clock, xxx1,xxx2);
			int col1 = WCini.readNumEntry(QString("TEXTTIME"), QString("Color1"), TextColor);
			int col2 = WCini.readNumEntry(QString("TEXTTIME"), QString("Color2"), TextColorO);
			TextSize = WCini.readNumEntry(QString("TEXTTIME"), QString("TextSize"), 12);
			QFont f = clock->font(); f.setPixelSize(TextSize); clock->setFont(f);
			if ( col2 == -1 ) { 
				clock->setFontColor(QRgb(col1));
				clock->setOutline(false);
			} else {
				clock->setOutline(true,QRgb(col2),QRgb(col1));
			}
		}
		if ( WCini.readEntry(QString("ICONS"), QString("ShowArrow"), "No") == "Yes" )
		{
			Panel->moveChild( fizq, WCini.readNumEntry(QString("ICONS"), QString("Arrow1left"), 0), 
									WCini.readNumEntry(QString("ICONS"), QString("Arrow1top"), 0) );
			Panel->moveChild( fder, WCini.readNumEntry(QString("ICONS"), QString("Arrow2left"), 0), 
									WCini.readNumEntry(QString("ICONS"), QString("Arrow2top"), 0) );

		} else if ( WCini.readEntry(QString("ICONS"), QString("ShowArrow"), "Yes") == "No" )
			{ Panel->moveChild( fizq, 321, 321 ); Panel->moveChild( fder, 321, 321 ); }

		if ( shownotes=="Yes" )
		{
			int notesh = WCini.readNumEntry(QString("NOTES"), QString("Height"), 26);
			int notesw = WCini.readNumEntry(QString("NOTES"), QString("Width"), 200);
			lNotes->setGeometry(notesleft,notestop,notesw,notesh);
			QString TextAlign = WCini.readEntry(QString("NOTES"), QString("Align"), "Left");
			int col1 = WCini.readNumEntry(QString("NOTES"), QString("Color1"), TextColor);
			int col2 = WCini.readNumEntry(QString("NOTES"), QString("Color2"), TextColorO);
			TextSize = WCini.readNumEntry(QString("NOTES"), QString("TextSize"), 12);
			QFont f = lNotes->font(); f.setPixelSize(TextSize); lNotes->setFont(f);
			if ( col2 == -1 ) { 
				lNotes->setFontColor(QRgb(col1));
				lNotes->setOutline(false);
			} else {
				lNotes->setOutline(true,QRgb(col2),QRgb(col1));
			}
		}
		else { lNotes->setGeometry(321,321,10,10); lNotes->setLeading(12); }

	}
	else 
	{
		if ( shownotes=="Yes" )
		{
			int valup = this->headerSize().height();
			if ( BarPosition=="Up") { lNotes->setGeometry(notesleft,valup+notestop,200,26); lNotes->setAlignment(ZLabel::AlignLeft); }
			if ( BarPosition=="Down") { lNotes->setGeometry(notesleft,valup+notestop,200,26); lNotes->setAlignment(ZLabel::AlignLeft); }
			if ( BarPosition=="Left") { lNotes->setGeometry(notesleft,valup+notestop,120,26); lNotes->setAlignment(ZLabel::AlignRight); }
			if ( BarPosition=="Right") { lNotes->setGeometry(notesleft,valup+notestop,120,26); lNotes->setAlignment(ZLabel::AlignLeft); }
			QFont f = lNotes->font(); f.setPixelSize(TextSize); lNotes->setFont(f);
			lNotes->setOutline(true,QRgb(TextColorO),QRgb(TextColor));
			lNotes->setNumLines(1);
		}
		else lNotes->setGeometry(321,321,10,10);
		QFont f = eti1->font(); f.setPixelSize(TextSize); eti1->setFont(f);
		eti1->setOutline(true,QRgb(TextColorO),QRgb(TextColor));
	}

}


void ScreenX::readWallClock()
{
  ZConfig ini("/ezxlocal/download/appwrite/setup/ezx_motodesk.cfg", true);
  WCS = ini.readEntry(QString("AM"), QString("WallclockSelected"), "01");
  ZConfig Vini(QString(ProgDir + "/screen/" + WCS + "/settings.cfg"), true);
  QString val; QPixmap pm;

  if ( Vini.readEntry("SCREEN", "Binary", "No") == "No" )
  {
	reloj->changeInterval(5000);
	int ti; QString temp;

	if ( Vini.readEntry("SCREEN", "DigitalClock", "Yes") == "Yes" )
	{
		QTime hora = QTime::currentTime();
		ti = hora.hour();
		temp = QString("%1").arg(ti);
		if ( (ti!=ht1) || (refrescar=="Yes") )
		{
		  ht1 = ti;
		  if ( temp.length()==2 ) { 
			temp.remove(1,1);
			pm.load(ProgDir + "/screen/" + WCS + "/hour/" + temp + ".png" );
			hora1->setPixmap(pm); 
			ti = hora.hour(); temp = QString("%1").arg(ti); temp.remove(0,1); 
			pm.load(ProgDir + "/screen/" + WCS + "/hour/" + temp + ".png" );
			hora2->setPixmap(pm); 
		  } else { 
			pm.load(ProgDir + "/screen/" + WCS + "/hour/0.png" );
			hora1->setPixmap(pm); 
			pm.load(ProgDir + "/screen/" + WCS + "/hour/" + temp + ".png" );
			hora2->setPixmap(pm); 
		  }
		}

		ti = hora.minute();
		temp = QString("%1").arg(ti);
		if ( (ti!=ht2) || (refrescar=="Yes") )
		{
		  ht2 = ti; 
		  if ( temp.length()==2 ) { 
			temp.remove(1,1);
			pm.load(ProgDir + "/screen/" + WCS + "/hour/" + temp + ".png" );
			hora3->setPixmap(pm); 
			ti = hora.minute(); temp = QString("%1").arg(ti); temp.remove(0,1); 
			pm.load(ProgDir + "/screen/" + WCS + "/hour/" + temp + ".png" );
			hora4->setPixmap(pm); 
		  } else { 
			pm.load(ProgDir + "/screen/" + WCS + "/hour/0.png" );
			hora3->setPixmap(pm); 
			pm.load(ProgDir + "/screen/" + WCS + "/hour/" + temp + ".png" );
			hora4->setPixmap(pm); 
		  }
		}
		Panel->moveChild( hora1, Vini.readNumEntry("HOUR", "Left", 20), Vini.readNumEntry("HOUR", "Top", 20) - this->headerSize().height());
		Panel->moveChild( hora2, Vini.readNumEntry("HOUR", "Left", 20) + Vini.readNumEntry("HOUR", "Space", 5), Vini.readNumEntry("HOUR", "Top", 20) - this->headerSize().height());
		Panel->moveChild( hora3, Vini.readNumEntry("MIN", "Left", 20), Vini.readNumEntry("MIN", "Top", 20) - this->headerSize().height());
		Panel->moveChild( hora4, Vini.readNumEntry("MIN", "Left", 20) + Vini.readNumEntry("MIN", "Space", 5), Vini.readNumEntry("MIN", "Top", 20) - this->headerSize().height());
	}
	else 
	{
		Panel->moveChild( hora1, 321, 321);
		Panel->moveChild( hora2, 321, 321);
		Panel->moveChild( hora3, 321, 321);
		Panel->moveChild( hora4, 321, 321);
	}


	if ( Vini.readEntry("SCREEN", "Calendar", "Yes") == "Yes" )
	{
		QDate dia = QDate::currentDate();
		ti = dia.dayOfWeek();
		if ( (dt1!=ti) || (refrescar=="Yes") )
		{
		  dt1 = ti;
		  temp = QString("%1").arg(ti); 
		  pm.load(ProgDir + "/screen/" + WCS + "/days/" + QString("%1").arg(temp) + ".png" );
		  dia1->setPixmap(pm); 
		}

		ti = dia.day();
		if ( (dt2!=ti) || (refrescar=="Yes") )
		{
		  dt2 = ti;
		  temp = QString("%1").arg(ti); 
		  if ( temp.length()==2 ) { 
			ti = dia.day(); temp = QString("%1").arg(ti); temp.remove(1,1); 
			pm.load(ProgDir + "/screen/" + WCS + "/day/" + temp + ".png" );
			dia2->setPixmap(pm); 
			ti = dia.day(); temp = QString("%1").arg(ti); temp.remove(0,1); 
			pm.load(ProgDir + "/screen/" + WCS + "/day/" + temp + ".png" );
			dia3->setPixmap(pm); 
		  } else { 
			pm.load(ProgDir + "/screen/" + WCS + "/day/0.png" );
			dia2->setPixmap(pm); 
			pm.load(ProgDir + "/screen/" + WCS + "/day/" + temp + ".png" );
			dia3->setPixmap(pm); 
		  }
		}

		ti = dia.month();
		if ( (dt3!=ti) || (refrescar=="Yes") )
		{
		  dt3 = ti;
		  temp = QString("%1").arg(ti); 
		  pm.load(ProgDir + "/screen/" + WCS + "/months/" + QString("%1").arg(temp) + ".png" );
		  dia4->setPixmap(pm); 
		}
		refrescar="No";
		Panel->moveChild( dia1, Vini.readNumEntry("DAYOFWEEK", "Left", 20), Vini.readNumEntry("DAYOFWEEK", "Top", 20) - this->headerSize().height());
		Panel->moveChild( dia2, Vini.readNumEntry("DATE", "Left", 20), Vini.readNumEntry("DATE", "Top", 20) - this->headerSize().height());
		Panel->moveChild( dia3, Vini.readNumEntry("DATE", "Left", 20) + Vini.readNumEntry("DATE", "Space", 5), Vini.readNumEntry("DATE", "Top", 20) - this->headerSize().height());
		Panel->moveChild( dia4, Vini.readNumEntry("MONTH", "Left", 20), Vini.readNumEntry("MONTH", "Top", 20) - this->headerSize().height());
	}
	else 
	{
		Panel->moveChild( dia1, 321, 321);
		Panel->moveChild( dia2, 321, 321);
		Panel->moveChild( dia3, 321, 321);
		Panel->moveChild( dia4, 321, 321);
	}

  }
  else
  {
	intervalo = Vini.readNumEntry("SCREEN", "Interval", 5000);
	reloj->changeInterval(intervalo);
	QTime hora = QTime::currentTime();
	int ti = hora.hour();
	QString temp = QString("%1").arg(ti);
	if ( (ti!=ht1) || (refrescar=="Yes") )
	{
	  ht1 = ti;
	  pm.load(ProgDir + "/screen/" + WCS + "/hour/" + temp + ".png" );
	  hora1->setPixmap(pm); 
	}

	ti = hora.minute();
	temp = QString("%1").arg(ti);
	if ( (ti!=ht2) || (refrescar=="Yes") )
	{
	  ht2 = ti; 
	  pm.load(ProgDir + "/screen/" + WCS + "/min/" + temp + ".png" );
	  hora2->setPixmap(pm); 
	}

	if ( Vini.readEntry("SCREEN", "Secs", "Yes") == "Yes" )
	{
		ti = hora.second();
		temp = QString("%1").arg(ti);
		if ( (ti!=ht2) || (refrescar=="Yes") )
		{
		  ht2 = ti; 
		  pm.load(ProgDir + "/screen/" + WCS + "/min/" + temp + ".png" );
		  hora3->setPixmap(pm); 
		}
		Panel->moveChild( hora3, Vini.readNumEntry("SEC", "Left", 20), Vini.readNumEntry("SEC", "Top", 20) - this->headerSize().height() );
	} 
	else
	{
		Panel->moveChild( hora3, 321, 321 );
	}

	Panel->moveChild( hora1, Vini.readNumEntry("HOUR", "Left", 20), Vini.readNumEntry("HOUR", "Top", 20) - this->headerSize().height() );
	Panel->moveChild( hora2, Vini.readNumEntry("MIN", "Left", 20), Vini.readNumEntry("MIN", "Top", 20) - this->headerSize().height() );

	Panel->moveChild( hora4, 321, 321 );
	Panel->moveChild( dia1, 321, 321 );
	Panel->moveChild( dia2, 321, 321 );
	Panel->moveChild( dia3, 321, 321 );
	Panel->moveChild( dia4, 321, 321 );


  }

}


void ScreenX::RecargarMenu()
{
  if ( this->hasFocus() )
  {
	ZConfig Fini1(QString("/ezxlocal/download/appwrite/setup/ezx_theme.cfg"), true);
	currentTheme = Fini1.readEntry(QString("THEME_GROUP_LOG"), QString("CurrentSkin"), "");

	QString IDval; ZConfig IDini(Idioma, true);
	QPixmap pm2 = QPixmap( iconReader.getIcon("security_std", false) );
	QImage image = pm2.convertToImage(); image = image.smoothScale( 16, 16 ); pm2.convertFromImage(image);
	QPixmap* pm = new QPixmap(pm2);
	IDval = IDini.readEntry(QString("MAINMENU"), QString("LOCK"), "");
	menu->changeItem(0, IDval, pm);

	pm2 = QPixmap( iconReader.getIcon("camera_std", false) );
	image = pm2.convertToImage(); image = image.smoothScale( 16, 16 ); pm2.convertFromImage(image);
	pm = new QPixmap(pm2);
	IDval = IDini.readEntry(QString("MAINMENU"), QString("CAPTURE"), "");
	menu->changeItem(1, IDval, pm);

	pm2 = QPixmap( iconReader.getIcon("msg_std", false) );
	image = pm2.convertToImage(); image = image.smoothScale( 16, 16 ); pm2.convertFromImage(image);
	pm = new QPixmap(pm2);
	IDval = IDini.readEntry(QString("MAINMENU"), QString("CREATESMS"), "");
	menu->changeItem(2, IDval, pm);

	pm2 = QPixmap( iconReader.getIcon("settings_std", false) );
	image = pm2.convertToImage(); image = image.smoothScale( 16, 16 ); pm2.convertFromImage(image);
	pm = new QPixmap(pm2);
	IDval = IDini.readEntry(QString("MAINMENU"), QString("SCREEN"), "");
	menu->changeItem(5, IDval, pm);

	pm2 = QPixmap( iconReader.getIcon("alarm_clock_std", false) );
	image = pm2.convertToImage(); image = image.smoothScale( 16, 16 ); pm2.convertFromImage(image);
	pm = new QPixmap(pm2);
	IDval = IDini.readEntry(QString("MAINMENU"), QString("ALARM"), "");
	menu->changeItem(6, IDval, pm);


  }

}

void ScreenX::SystemInfo()
{

  QString INFO;
  QString tmp1, tmp2, tmp2a, tmp2b, tmp3;
  int tmp2i;
  QFile entrada("/sys/mpm/op");
  QTextStream stentrada(&entrada);
  if ( entrada.open(IO_ReadOnly | IO_Translate) )
  { tmp1 = stentrada.readLine(); } entrada.close();
  if (showSItext=="Yes") INFO = "CPU: " + tmp1;
	else INFO = tmp1;
  systemInfo1->setText(INFO);


  QFile entrada2("/proc/meminfo");
  QTextStream stentrada2(&entrada2);
  if ( entrada2.open(IO_ReadOnly | IO_Translate) )
  { stentrada2.readLine(); tmp2a=stentrada2.readLine(); 
	stentrada2.readLine(); tmp2b=stentrada2.readLine();
	stentrada2.readLine(); stentrada2.readLine(); stentrada2.readLine(); 
	stentrada2.readLine(); stentrada2.readLine(); stentrada2.readLine();
	stentrada2.readLine(); stentrada2.readLine(); tmp3=stentrada2.readLine();
  } entrada2.close();

  tmp2a = tmp2a.right(10); int i=tmp2a.findRev(" "); tmp2a.remove(i,3);
  tmp2b = tmp2b.right(10); i=tmp2b.findRev(" "); tmp2b.remove(i,3);
  tmp3 = tmp3.right(10); i=tmp3.findRev(" "); tmp3.remove(i,3);
  while ( tmp2a[0]==QChar(' ') ) { tmp2a = tmp2a.right( tmp2a.length()-1 ); }
  while ( tmp2b[0]==QChar(' ') ) { tmp2b = tmp2b.right( tmp2b.length()-1 ); }
  while ( tmp3[0]==QChar(' ') ) { tmp3 = tmp3.right( tmp3.length()-1 ); }

  tmp2i = tmp2a.toInt() + tmp2b.toInt();
  tmp2 = QString("%1").arg(tmp2i);

  if (showSItext=="Yes") INFO = "Mem: " + GetSize(tmp2);
	else INFO = GetSize(tmp2);
  systemInfo2->setText(INFO);

  if (showSItext=="Yes") INFO = "Swap: " + GetSize(tmp3);
	else INFO = GetSize(tmp3);
  systemInfo3->setText(INFO);

  if ( single=="Yes" ) 
  {
	INFO = "CPU: " + tmp1 + " - " + "Mem: " + GetSize(tmp2) + " + " + GetSize(tmp3);
	systemInfo1->setText(INFO);
  }

}


QString ScreenX::GetSize(QString num)
{
	QString ss;
    int size = num.toInt();
	if (size < 1024) { 
	  ss = QString("%1 b").arg(size);
	} else if (size < 1024*1024) {
	  ss = QString("%1").arg( (double)(size) / 1024);
	  size = ss.find(QChar('.')); if ( size > -1 ) { ss.remove(size+3, ss.length()-size-3); }
	  ss = ss + " Mb";
	} else {
	  ss = QString("%1").arg( (double)(size) / 1024 / 1024);
	  size = ss.find(QChar('.')); if ( size > -1 ) { ss.remove(size+3, ss.length()-size-3); }
	  ss = ss + " Gb";
	}
	return ss;
}

void ScreenX::showIMEI()
{
	delete llamada;
	llamada = NULL;
	llamada = new zCalls(Idioma);
	
	unsigned int res;
	TAPI_IMEI_NUMBER_A imei;
	TAPI_CLIENT_Init(NULL, 0);
	res = TAPI_ACCE_GetImei(imei);
	QString pImei;
	if ( res == 0) pImei = QString::fromLatin1( reinterpret_cast<char*>( imei ) );
	else { qDebug(QString("*** FAIL: error %1").arg(res)); }
	TAPI_CLIENT_Fini();

	ZNoticeDlg* dlgw = new ZNoticeDlg(ZNoticeDlg::Information, 10000, QString(""), pImei, this, "z", true, 0);
	dlgw->show(); dlgw = NULL;

	llamando=0;

}

void ScreenX::showFLEX()
{
	delete llamada;
	llamada = NULL;
	llamada = new zCalls(Idioma);
	
	RES_ICON_Reader iconReader;
	ZConfig IDini(Idioma, true);
	QString INFO = QString( "\n%1:\n" ).arg( IDini.readEntry(QString("CALLS"), QString("VERSIONAP"), "") );
	QFile entrada("/etc/base_version.txt"); QTextStream stentrada(&entrada);
	if ( entrada.open(IO_ReadOnly | IO_Translate) ) { INFO += QString( "%1\n\n" ).arg( stentrada.readLine() ); } entrada.close();

	QString pBP, pFLEX;
	unsigned int res;
	TAPI_BP_VERSION_INFO_S* pbp = new TAPI_BP_VERSION_INFO_S();
	//TAPI_HARDWARE_VERSION_A pflex;
	TAPI_CLIENT_Init(NULL, 0);
	res = TAPI_ACCE_GetBpVersionInfo(pbp);
	if ( res == 0) pBP = QString::fromLatin1( reinterpret_cast<char*>( pbp ) );
	else { qDebug(QString("*** FAIL: error %1").arg(res)); }
	//res = TAPI_ACCE_GetHardwareVersion(pflex);
	//if ( res == 0) pFLEX = QString::fromLatin1( reinterpret_cast<char*>( pflex ) );
	//else { qDebug(QString("*** FAIL: error %1").arg(res)); }
	TAPI_CLIENT_Fini();
	INFO += QString( "%1:\n" ).arg( IDini.readEntry(QString("CALLS"), QString("VERSIONBP"), "") );
	INFO += QString( "%1\n" ).arg( pBP );
	//INFO += QString( "%1\n" ).arg( IDini.readEntry(QString("CALLS"), QString("VERSIONFLEX"), "") );
	//INFO += QString( "%1\n" ).arg( pFLEX );

	ZNoticeDlg* dlgw = new ZNoticeDlg(ZNoticeDlg::Information, 10000, QString(""), INFO, this, "z", true, 0);
	dlgw->setTitle( IDini.readEntry(QString("CALLS"), QString("PROPERTIES"), "") );
	dlgw->setTitleIcon(iconReader.getIcon("ok_pop", false));
	dlgw->show(); dlgw = NULL;

	llamando=0;

}

void ScreenX::getTrackInfo()
{
  if (mchannel==1) return;

  QFile entrada("/tmp/rbcurrent");
  QTextStream stentrada(&entrada); QString archivo;
  if ( entrada.open(IO_ReadOnly | IO_Translate) ) { archivo = stentrada.readLine(); } entrada.close();

  QString archivo2 = getFilePath(archivo);
  int i = archivo2.findRev("/MMC/"); 
  if ( i==0 ) { archivo2.remove( 0, 5 ); archivo2 = "/mmc/mmca1/" + archivo2; }
  i = archivo2.findRev("/Phone/"); 
  if ( i==0 ) { archivo2.remove( 0, 7 ); archivo2 = "/ezxlocal/download/mystuff/" + archivo2; }

  Icon1.load( "/mmc/mmca1/.system/QTDownLoad/rockbox/rockbox_usr.png" );
  Icon1.load(archivo2 + "folder.jpg"); Icon1.load(archivo2 + "cover.jpg");
  QImage image = Icon1.convertToImage(); image = image.smoothScale( 30, 30 ); Icon1.convertFromImage(image);  
  Icono1->setPixmap(Icon1);

  Icon1.load(archivo2 + "folder.jpg"); Icon1.load(archivo2 + "cover.jpg");
  image = Icon1.convertToImage(); image = image.smoothScale( AAwidth, AAheight ); Icon1.convertFromImage(image);  

  QString str = archivo;
  i = str.findRev ( "/" );
  str.remove ( 0, i); if (str.left(1) = "/") str.remove ( 0, 1);
  i = str.findRev ( "." ); str.remove ( i, str.length()-i);

  trackinfo->setText(str);
  trackinfo->setTruncateString("...");

}

void ScreenX::getAlbumArt()
{
  if ( QCopChannel::isRegistered("EZX/Application/rockboxui.portrait") ) mchannel=2;
  else if ( QCopChannel::isRegistered("EZX/Application/rockboxui.landscape") ) mchannel=2;
  else mchannel=0;

  if (mchannel==2)
  {

	QFile entrada("/tmp/rbcurrent");
	QTextStream stentrada(&entrada); QString archivo;
	if ( entrada.open(IO_ReadOnly | IO_Translate) ) { archivo = stentrada.readLine(); } entrada.close();

	QString archivo2 = getFilePath(archivo);
	int i = archivo2.findRev("/MMC/"); 
	if ( i==0 ) { archivo2.remove( 0, 5 ); archivo2 = "/mmc/mmca1/" + archivo2; }
	i = archivo2.findRev("/Phone/"); 
	if ( i==0 ) { archivo2.remove( 0, 7 ); archivo2 = "/ezxlocal/download/mystuff/" + archivo2; }

	QPixmap aaico;
	aaico.load( ProgDir + "/null.png" ); 
	aaico.load(archivo2 + "folder.jpg"); aaico.load(archivo2 + "cover.jpg");
	QImage image = aaico.convertToImage(); image = image.smoothScale( AAwidth, AAheight ); aaico.convertFromImage(image);  
	AlbumArt->setPixmap(aaico);
  }
  else
  {
	QPixmap aaico;
	aaico.load( ProgDir + "/null.png" ); 
	AlbumArt->setPixmap(aaico);
	AAon=0;
  }

}

