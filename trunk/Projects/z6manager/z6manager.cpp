#include <TAPI_ACCE_Client.h>
#include <TAPI_CLIENT_Func.h>
#include <TAPI_SIM_Client.h>
#include <TAPI_Result.h>
#include <TAPI_AppMsg.h>
#include <TAPI_AppMsgId.h>
#include <TAPI_General.h>
#include <TAPI_SMS_PduModeClient.h>
#include <TAPI_SMS_TextModeClient.h>
#include <TAPI_SMS_SettingClient.h>

#include "z6manager.h"
#include "GUI_Define.h"
#include "BaseDlg.h"
#include "ZAboutDlg.h"
#include "ZFileBrowser.h"
#include "ZFileDlg.h"
#include "ZConfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qlabel.h>
#include <qfile.h>
#include <qdir.h>
#include <string.h>
#include <qtextstream.h>
#include <qvector.h>
#include <ZApplication.h>
#include <ZListBox.h>
#include <ZSoftKey.h>
#include <ZOptionsMenu.h>
#include <ZMessageDlg.h>
#include <ZMultiSelectDlg.h>
#include <ZSingleSelectDlg.h>
#include <qtextcodec.h>
#include <ezxres.h>
#include <iostream>
#include <fstream>
#include <string>
#include <qdatastream.h>
#include <qcopchannel_qws.h>

#define APPTITLE "Z6 Manager"
int Cambios;
QString modo;
QString installed="NO";


extern "C" int UTIL_GetSliderStatus(void);

using namespace std;

QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");

Z6Man::Z6Man ( const QString &argument, QWidget* parent, const char* name, WFlags fl )
    : ZKbMainWidget ( ZHeader::FULL_TYPE, NULL, "ZMainWidget", 0 )
{
  Cambios=0;
  ProgDir = getProgramDir();
  QString val; 

  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = "ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/en-us";

  QString argum = argument;

  if ( argum == "zNothief" )
  {
	runZNothief();		
	( ( ZApplication* ) qApp )->quit();
	system ( QString ( "/bin/busybox killall z6manager" ) );
  }
  else
  {
	QFile f(argum);
	if ( f.exists())
	{
	  Instalar(argum);
	  ( ( ZApplication* ) qApp )->quit();
	  system ( QString ( "/bin/busybox killall z6manager" ) );
	}
	else
	{
	  CreateWindow ( parent );
	  ( ( ZApplication* ) qApp )->showMainWidget ( this );
	}
  }
}

Z6Man::~Z6Man()
{
}


void Z6Man::CreateWindow ( QWidget* parent )
{
  setMainWidgetTitle ( APPTITLE );

//  LimpiarCR();
  getIMEI();
//  AsociarMPKGs();

  tabWidget = new ZNavTabWidget(ZNavTabWidget::SEPARATE, false, false, this);

  appListBox = new ZListBox ( QString ( "%I%M%I" ), this, 0);
  appListBox->setFixedWidth ( 240 );

  runListBox = new ZListBox ( QString ( "%I%M" ), this, 0);
  runListBox->setFixedWidth ( 240 );  

  javaListBox = new ZListBox ( QString ( "%I%M%I" ), this, 0);
  javaListBox->setFixedWidth ( 240 );  

  skinListBox = new ZListBox ( QString ( "%I%M" ), this, 0);
  skinListBox->setFixedWidth ( 240 );  

  roboListBox = new ZListBox ( QString ( "%I%M" ), this, 0);
  roboListBox->setFixedWidth ( 240 );  


  QPixmap p1;
  p1.load(ProgDir + "/images/apps.png");
  tabWidget->addTab(appListBox, QIconSet(p1), "");
  p1.load(ProgDir + "/images/java.png");
  tabWidget->addTab(javaListBox, QIconSet(p1), "");
  p1.load(ProgDir + "/images/autorun.png");
  tabWidget->addTab(runListBox, QIconSet(p1), "");
  p1.load(ProgDir + "/images/security.png");
  tabWidget->addTab(roboListBox, QIconSet(p1), "");
  p1.load(ProgDir + "/images/themes.png");
  tabWidget->addTab(skinListBox, QIconSet(p1), "");


  CargarListaApps();
  connect(appListBox, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(appClicked(ZListBoxItem *)));

  CargarListaJava();

  CargarListaAutorun();
  connect(runListBox, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(runClicked(ZListBoxItem *)));

  CargarListaRobo();
  connect(roboListBox, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(itemClicked(ZListBoxItem *)));

  CargarListaSkins();

  setContentWidget ( tabWidget );
  softKey = new ZSoftKey ( NULL , this , this );

  Create_SubMenuApp();
  Create_MenuApp();

  connect(tabWidget,SIGNAL(currentChanged(QWidget* )),this,SLOT(slotCurrentChanged(QWidget* )));
  
  Create_SoftKey();
  setSoftKey ( softKey );

  appListBox->setFocus();

}

QString Z6Man::getProgramDir()
{
  ProgDir = QString ( qApp->argv() [0] ) ;
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );
  return ProgDir;
}

QString Z6Man::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("SETTINGS"), QString("UNICODE"), "");
	if ( IDval == "1" ) { return txtcodec->toUnicode(xString); }
	else { return xString; }
}

void Z6Man::getIMEI()
{
  QString file = ProgDir + "/IMEI";
  if (QFileInfo(file).isFile())
  {
  }
  else
  {
	unsigned int res;
	TAPI_IMEI_NUMBER_A imei;
	TAPI_CLIENT_Init(NULL, 0);
	res = TAPI_ACCE_GetImei(imei);
	QString pImei;
	if ( res == 0)
	{
		pImei = QString::fromLatin1( reinterpret_cast<char*>( imei ) );
		qDebug(QString("\nIMEI = %1").arg(pImei));
	}
	else { qDebug(QString("*** FAIL: error %1").arg(res)); }
	TAPI_CLIENT_Fini();
	system(QString("touch %1/IMEI").arg(ProgDir));
	system(QString("echo \"IMEI = %1\" > %2/IMEI").arg(pImei).arg(ProgDir));
  }
}


void Z6Man::about()
{
  ZAboutDialog* aboutDlg = new ZAboutDialog();
  aboutDlg->exec();
  delete aboutDlg;
  aboutDlg = NULL;
}


void Z6Man::IngresoClave()
{

  QString IDval; QString IDval2; ZConfig IDini(Idioma, false);


    ClaveCorrecta = "NO";
	QString s1 = QString("%1/Z6Manager.cfg").arg(ProgDir);
	QString val;
	ZConfig ini(s1, true);
	val = ini.readEntry(QString("Z6Manager"), QString("Password"), "");

	IDval = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_ENTERPASS"), "");
	ZSingleCaptureDlg* zscd = new ZSingleCaptureDlg(QString(APPTITLE), IDval, ZSingleCaptureDlg::TypeLineEdit, NULL, "ZSingleCaptureDlg", true, 0, 0);
	ZLineEdit* zle = (ZLineEdit*)zscd->getLineEdit(); 
	zle->setEchoMode(ZLineEdit::Password); 
	zle->setText("");
	zle->setEdited(true);  

	if (zscd->exec() == 1)
	{
	  QString val2 = zle->text();
	  if ( val == val2) 
	  { 
		QString clave1; QString clave2;
		IDval = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_ENTERPASSNEW1"), "");
		ZSingleCaptureDlg* zscd = new ZSingleCaptureDlg(QString(APPTITLE), IDval, ZSingleCaptureDlg::TypeLineEdit, 
									NULL, "ZSingleCaptureDlg", true, 0, 0);
		ZLineEdit* zle = (ZLineEdit*)zscd->getLineEdit();  
		zle->setEchoMode(ZLineEdit::Password);
		zle->setText("");
		zle->setEdited(true);  
		if (zscd->exec() == 1)
		{ 
			clave1 = zle->text();
			IDval = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_ENTERPASSNEW2"), "");
			ZSingleCaptureDlg* zscd2 = new ZSingleCaptureDlg(QString(APPTITLE), IDval, ZSingleCaptureDlg::TypeLineEdit,
										 NULL, "ZSingleCaptureDlg", true, 0, 0);
			ZLineEdit* zle2 = (ZLineEdit*)zscd2->getLineEdit();
			zle2->setEchoMode(ZLineEdit::Password); 
			zle2->setText("");
			zle2->setEdited(true);  
			if (zscd2->exec() == 1)
			{ 
				clave2 = zle2->text();
				if ( clave1 == clave2 )
				{
					ini.writeEntry(QString("Z6Manager"), QString("Password"), clave2);
					qApp->processEvents();

					IDval = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_SUCCES"), "");
					znd = new ZNoticeDlg(ZNoticeDlg::Success, 3000, QString(""),IDval, this, "z", true, 0);
					znd->show(); znd = NULL;
				}
				else
				{
					IDval = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_NOMATCH"), "");
					znd = new ZNoticeDlg(ZNoticeDlg::Information, 3000, QString(""),IDval, this, "z", true, 0);
					znd->show(); znd = NULL;
				}
			}
		}
	  }
	  else
	  {
		IDval = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_ERROR"), "");
		znd = new ZNoticeDlg(ZNoticeDlg::Information, 3000, QString(""),IDval, this, "z", true, 0);
		znd->show(); znd = NULL;
	  }
	}  
	roboListBox->setCurrentItem(2,true);
}

void Z6Man::AsociarMPKGs()
{
  QString card = QString("/ezxlocal/download/appwrite/am/UserMime");
  QString line="";
  QFile entrada(card);
  QTextStream stentrada(&entrada);
  QString INFO;
  if ( entrada.open(IO_ReadOnly | IO_Translate) )
  {
	while ( !stentrada.eof() )
    {           
	  line = stentrada.readLine();
	  if ( line == "[test/x-mpkg]" )
	  {
		INFO += QString("%1\n").arg(line);
		line = stentrada.readLine(); line = stentrada.readLine();
		INFO += QString("AppID = {fdcc1e4f-0c31-4f50-92fe-f333e7d16622}\n");
		INFO += QString("AppList = {fdcc1e4f-0c31-4f50-92fe-f333e7d16622}\n");
	  }
	  else
	  {
		INFO += QString("%1\n").arg(line);
	  }
    }
  }
  entrada.close();
  system(QString("echo \"%1\" > /ezxlocal/download/appwrite/am/UserMime").arg(INFO));

}

void Z6Man::LimpiarCR()
{
  QString card = QString("/mmc/mmca1/.system/java/CardRegistry");
  QString line="";
  QFile entrada(card);
  QTextStream stentrada(&entrada);
  QString INFO;
  if ( entrada.open(IO_ReadOnly | IO_Translate) )
  {
	while ( !stentrada.eof() )
    {           
	  line = stentrada.readLine();
	  if ( line.length() != 0 )
	  {
		if ( line[0] == QChar( '[' ) ) 
		{
		  if ( INFO != "" )
		  { INFO += QString("\n\n%1").arg(line); }
		  else
		  { INFO += QString("%1").arg(line); }
		}
		else
		{
		  INFO += QString("\n%1").arg(line);
		}
	  }
    }
  }
  entrada.close();
  system("rm /mmc/mmca1/.system/java/CardTemp");
  system("touch /mmc/mmca1/.system/java/CardTemp");
  system(QString("echo \"%1\" > /mmc/mmca1/.system/java/CardTemp").arg(INFO));
  system("mv /mmc/mmca1/.system/java/CardTemp /mmc/mmca1/.system/java/CardRegistry");

  QString card2 = QString("/ezxlocal/download/appwrite/am/InstalledDB");
  QString line2="";
  QFile entrada2(card2);
  QTextStream stentrada2(&entrada2);
  QString INFO2;
  if ( entrada2.open(IO_ReadOnly | IO_Translate) )
  {
	while ( !stentrada2.eof() )
    {           
	  line2 = stentrada2.readLine();
	  if ( line2.length() != 0 )
	  {
		if ( line2[0] == QChar( '[' ) ) 
		{
		  if ( INFO2 != "" )
		  { INFO2 += QString("\n\n%1").arg(line2); }
		  else
		  { INFO2 += QString("%1").arg(line2); }
		}
		else
		{
		  INFO2 += QString("\n%1").arg(line2);
		}
	  }
    }
  }
  entrada2.close();
  system("rm /ezxlocal/download/appwrite/am/UserTemp");
  system("touch /ezxlocal/download/appwrite/am/UserTemp");
  system(QString("echo \"%1\" > /ezxlocal/download/appwrite/am/UserTemp").arg(INFO2));
  system("mv /ezxlocal/download/appwrite/am/UserTemp /ezxlocal/download/appwrite/am/InstalledDB");

}
void Z6Man::slotCurrentChanged(QWidget* AWidget)
{
  softKey->setOptMenu ( ZSoftKey::LEFT, NULL );
  int i = tabWidget->currentPageIndex();
  switch (i)
  {
    case 0:
    {
	 Create_SubMenuApp();
     Create_MenuApp();
     softKey->setOptMenu ( ZSoftKey::LEFT, menu );
     break; 
    }
    case 1:
    {
	  Create_SubMenuApp();
      Create_MenuJava();
      softKey->setOptMenu ( ZSoftKey::LEFT, menu );
      break; 
    }     
    case 2:
    {
     Create_MenuAutorun();
     softKey->setOptMenu ( ZSoftKey::LEFT, menu );
     break; 
    }
    case 3:
    {
      Create_MenuConfig();
      softKey->setOptMenu ( ZSoftKey::LEFT, menu );
      break; 
    }     
    case 4:
    {
      Create_MenuTheme();
      softKey->setOptMenu ( ZSoftKey::LEFT, menu );
      break; 
    }
  }
}
    
void Z6Man::Create_SoftKey()
{
  QString IDval; QString IDval2; ZConfig IDini(Idioma, false);
  softKey->setOptMenu ( ZSoftKey::LEFT, menu ); 
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_MENU"), "");
  softKey->setText ( ZSoftKey::LEFT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_MENU"), "");
  softKey->setTextForOptMenuHide( IDval);
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_SELECT"), "");
  IDval2 = IDini.readEntry(QString("COMMON"), QString("MENU_CANCEL"), "");
  softKey->setTextForOptMenuShow( IDval, IDval2);
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_EXIT"), "");
  softKey->setText ( ZSoftKey::RIGHT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, qApp, SLOT ( quit() ) );      
}

void Z6Man::Create_SubMenuApp()
{
  QString IDval; ZConfig IDini(Idioma, false);
  QRect rect;
  QPixmap* pm  = new QPixmap();
  int idx = 0;
  submenu = new ZOptionsMenu ( rect, softKey, NULL, 0);
  submenu->setItemSpacing(10);  
  pm->load(ProgDir + "images/pname.png");
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("PERSONALIZE_NAME"), "");
  submenu->insertItem ( IDval, NULL, pm, true, idx, idx );
  submenu->connectItem ( idx, this, SLOT ( CambiarNombre() ) );
  ++idx;
  pm->load(ProgDir + "images/picon.png");
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("PERSONALIZE_ICON"), "");
  submenu->insertItem ( IDval, NULL, pm, true, idx, idx );
  submenu->connectItem ( idx, this, SLOT ( CambiarIcono() ) );
}

void Z6Man::Create_MenuApp()
{
  QString IDval; ZConfig IDini(Idioma, false);
  QRect rect;
  QPixmap* pm  = new QPixmap();
  int idx = 0;
  menu = new ZOptionsMenu ( rect, softKey, NULL, 0);
  menu->setItemSpacing(10);  
  pm->load(ProgDir + "images/remove.png");
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("MENU_UNINSTALL"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( BorrarMPKG() ) );
  ++idx;
  pm->load(ProgDir + "images/personalize.png");
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("PERSONALIZE"), "");
  menu->insertItem ( IDval, submenu, pm , true , idx , idx );
  ++idx;
  menu->insertSeparator(idx, idx); 
  ++idx;
/*  pm->load(ProgDir + "images/lang.png");
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("MENU_LANGUAGE"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( language() ) );
  ++idx;
  menu->insertSeparator(idx, idx); 
  ++idx;*/
  pm->load(ProgDir + "images/about.png");
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_ABOUT"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( about() ) );      
  ++idx;
  pm->load(ProgDir + "images/exit.png");
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_EXIT"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, qApp, SLOT ( quit() ) );
}

void Z6Man::Create_MenuTheme()
{
  QString IDval; ZConfig IDini(Idioma, false);
  QRect rect;
  QPixmap* pm  = new QPixmap();
  int idx = 0;
  menu = new ZOptionsMenu ( rect, softKey, NULL, 0);
  menu->setItemSpacing(10);  
  pm->load(ProgDir + "images/skindel.png");
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("MENU_UNINSTALL"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( BorrarTema() ) );
  ++idx;
  menu->insertSeparator(idx, idx); 
  ++idx;
/*  pm->load(ProgDir + "images/lang.png");
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("MENU_LANGUAGE"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( language() ) );
  ++idx;
  menu->insertSeparator(idx, idx); 
  ++idx;*/
  pm->load(ProgDir + "images/about.png");
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_ABOUT"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( about() ) );      
  ++idx;
  pm->load(ProgDir + "images/exit.png");
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_EXIT"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, qApp, SLOT ( quit() ) );
}


void Z6Man::Create_MenuConfig()
{
  QRect rect; QString IDval; ZConfig IDini(Idioma, false);
  QPixmap* pm  = new QPixmap();
  int idx = 0;
  menu = new ZOptionsMenu ( rect, softKey, NULL, 0);
  menu->setItemSpacing(10);  
/*  pm->load(ProgDir + "images/lang.png");
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("MENU_LANGUAGE"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( language() ) );
  ++idx;
  menu->insertSeparator(idx, idx); 
  ++idx;*/
  pm->load(ProgDir + "images/about.png");
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_ABOUT"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( about() ) );      
  ++idx;
  pm->load(ProgDir + "images/exit.png");
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_EXIT"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, qApp, SLOT ( quit() ) );
}

void Z6Man::Create_MenuAutorun()
{
  QRect rect; QString IDval; ZConfig IDini(Idioma, false);
  QPixmap* pm  = new QPixmap();
  int idx = 0;
  menu = new ZOptionsMenu ( rect, softKey, NULL, 0);
  menu->setItemSpacing(10);  
  pm->load(ProgDir + "images/runadd.png");
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("MENU_RUNADD"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( AutorunAdd() ) );
  ++idx;
  pm->load(ProgDir + "images/runremove.png");
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("MENU_RUNDELETE"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( AutorunRemove() ) );
  ++idx;
  menu->insertSeparator(idx, idx); 
  ++idx;
/*  pm->load(ProgDir + "images/lang.png");
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("MENU_LANGUAGE"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( language() ) );
  ++idx;
  menu->insertSeparator(idx, idx); 
  ++idx;*/
  pm->load(ProgDir + "images/about.png");
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_ABOUT"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( about() ) );      
  ++idx;
  pm->load(ProgDir + "images/exit.png");
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_EXIT"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, qApp, SLOT ( quit() ) );
}

void Z6Man::Create_MenuJava()
{
  QRect rect; QString IDval; ZConfig IDini(Idioma, false);
  QPixmap* pm  = new QPixmap();
  int idx = 0;
  menu = new ZOptionsMenu ( rect, softKey, NULL, 0);
  menu->setItemSpacing(10);  
  pm->load(ProgDir + "images/runadd.png");
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("MENU_JAVAADD"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( JavaAdd() ) );
  ++idx;
  pm->load(ProgDir + "images/runremove.png");
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("MENU_JAVADELETE"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( JavaRemove() ) );
  ++idx;
  menu->insertSeparator(idx, idx); 
  ++idx;
  pm->load(ProgDir + "images/personalize.png");
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("PERSONALIZE"), "");
  menu->insertItem ( IDval, submenu, pm , true , idx , idx );
  ++idx;
  menu->insertSeparator(idx, idx); 
  ++idx;
  pm->load(ProgDir + "images/about.png");
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_ABOUT"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( about() ) );      
  ++idx;
  pm->load(ProgDir + "images/exit.png");
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_EXIT"), "");
  menu->insertItem ( IDval, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, qApp, SLOT ( quit() ) );
}


void Z6Man::CargarListaApps()
{

	RES_ICON_Reader iconReader;
	QString ico, name, exec, dir;
	QPixmap p1;
	appListBox->clear();

	ZConfig IDini ( Idioma, false );
	QString mmc = IDini.readEntry(QString("Z6MANAGER"), QString("INSTALL_SD"), "");
	QString phone = IDini.readEntry(QString("Z6MANAGER"), QString("INSTALL_PHONE"), "");

	ZConfig confCARD ( "/mmc/mmca1/.system/java/CardRegistry", true );
	QStringList grouplist;
	confCARD.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
		name = confCARD.readEntry ( *it, "Name", "" );
		ico = confCARD.readEntry ( *it, "Icon", "" );
		dir = confCARD.readEntry ( *it, "Directory", "" );
		exec = confCARD.readEntry ( *it, "Exec", "" );
		if (ico=="") { ico = confCARD.readEntry ( *it, "BigIcon", "" ); }
		if ( exec!="" && name!="" && exec!="java.sh" )
		{ 
			int i = ico.findRev("../..");
			if ( i==0 ) { ico.remove( 0, 5 ); }
			p1 = QPixmap(iconReader.getIcon(ico, false));
			p1.load(ico);
			if ( p1.height() > 19 ) { QImage image; image = p1.convertToImage(); 
			image = image.smoothScale(19, 19); p1.convertFromImage(image); }     
			ZListBoxItem* listitem = new ZListBoxItem ( appListBox, QString ( "%I%M%I" ) );
			listitem->setPixmap ( 0, p1 );
			listitem->appendSubItem ( 1, name, false, 0 );
			QPixmap* p2;
			p2 = new QPixmap(ProgDir + "/images/sd.png"); 
			listitem->appendSubItem ( 1, mmc, false, p2 );
			appListBox->insertItem ( listitem, 2, true );
		}
	}	
	
/*	ZConfig confUSER ( "/ezxlocal/download/appwrite/am/UserRegistry", true );
	confUSER.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
		name = confUSER.readEntry ( *it, "Name", "" );
		ico = confUSER.readEntry ( *it, "Icon", "" );
		dir = confUSER.readEntry ( *it, "Directory", "" );
		exec = confUSER.readEntry ( *it, "Exec", "" );
		if (ico=="") { ico = confUSER.readEntry ( *it, "BigIcon", "" ); }
		if ( exec!="" && name!="" )
		{
			int i = ico.findRev("../../");
			if ( i==0 ) { ico.remove( 0, 5 ); }
			p1 = QPixmap(iconReader.getIcon(ico, false));
			p1.load(dir + "/" + ico);
			if ( p1.height() > 19 ) { QImage image; image = p1.convertToImage(); 
			image = image.smoothScale(19, 19); p1.convertFromImage(image); }     
			ZListBoxItem* listitem = new ZListBoxItem ( appListBox, QString ( "%I%M%I" ) );
			listitem->setPixmap ( 0, p1 );
			listitem->appendSubItem ( 1, name, false, 0 );
			QPixmap* p2;
			p2 = new QPixmap(ProgDir + "/images/phone.png"); 
			listitem->appendSubItem ( 1, phone, false, p2 );
			appListBox->insertItem ( listitem, 2, true );
		}
	}	
*/
	ZConfig confCARD2 ( "/ezxlocal/download/appwrite/am/InstalledDB", true );
	confCARD2.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
		name = confCARD2.readEntry ( *it, "Name", "" );
		ico = confCARD2.readEntry ( *it, "Icon", "" );
		dir = confCARD2.readEntry ( *it, "Directory", "" );
		exec = confCARD2.readEntry ( *it, "Exec", "" );
		if (ico=="") { ico = confCARD2.readEntry ( *it, "BigIcon", "" ); }
		if ( exec!="" && name!="" && exec!="java.sh" )
		{ 
			int i = ico.findRev("../..");
			if ( i==0 ) { ico.remove( 0, 5 ); }
			p1 = QPixmap(iconReader.getIcon(ico, false));
			p1.load(ico);
			if ( p1.height() > 19 ) { QImage image; image = p1.convertToImage(); 
			image = image.smoothScale(19, 19); p1.convertFromImage(image); }     
			ZListBoxItem* listitem = new ZListBoxItem ( appListBox, QString ( "%I%M%I" ) );
			listitem->setPixmap ( 0, p1 );
			listitem->appendSubItem ( 1, name, false, 0 );
			QPixmap* p2;
			p2 = new QPixmap(ProgDir + "/images/phone.png"); 
			listitem->appendSubItem ( 1, phone, false, p2 );
			appListBox->insertItem ( listitem, 2, true );
		}
	}	

	appListBox->sort(true);
	appListBox->setCurrentItem(0,true);

}

void Z6Man::CargarListaJava()
{
	RES_ICON_Reader iconReader;
	QString name, type, typeback, dir, ico, appid; QPixmap p1;
	QStringList grouplist;
	ZConfig ini ( Idioma, false );
	javaListBox->clear();

	ZConfig confINST ( "/ezxlocal/download/appwrite/am/InstalledDB", true );
	confINST.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
		name = confINST.readEntry ( *it, "Name", "" );
		type = confINST.readEntry ( *it, "JavaId", "" );
		typeback = confINST.readEntry ( *it, "JavaIdback", "" );
		dir = confINST.readEntry ( *it, "Directory", "" );
		ico = confINST.readEntry ( *it, "BigIcon", "" );
		appid = *it;
		int i = dir.findRev ( "../../" ); dir.remove ( i, 5 );
		QString IsJavaApp = "NO";
		if ( type!="") { IsJavaApp = "YES"; }
		if ( typeback!="") { IsJavaApp = "YES"; }
		if ( (appid!="") && (name!="") && (IsJavaApp=="YES") )
		{
			QFile entrada(dir + "/registry.txt"); QTextStream stentrada(&entrada); QString INFO="NO";
			if ( entrada.open(IO_ReadOnly | IO_Translate) ) {
			  while ( !stentrada.eof() ) { QString line = stentrada.readLine(); if ( line == "Is-Trusted: 1" ) { INFO="YES"; } }
			}
			ZListBoxItem* listitem = new ZListBoxItem ( javaListBox, QString ( "%I20%M%I" ) );
			p1 = QPixmap(iconReader.getIcon(ico, false));
			if (INFO=="YES") { p1.load(dir+"/"+ico); } else { p1.load(ico); }
			if ( p1.height() > 19 ) { QImage image; image = p1.convertToImage(); 
			image = image.smoothScale(19, 19); p1.convertFromImage(image); } 
			listitem->setPixmap ( 0, p1 );
			listitem->appendSubItem ( 1, name, false, 0 );
			QPixmap* p2;
			if ( INFO == "YES" ) { p2 = new QPixmap(ProgDir + "/images/javaok.png");  }
			else { p2 = new QPixmap(ProgDir + "/images/javalock.png");  }
			QString val = ini.readEntry ( "Z6MANAGER", "INSTALL_PHONE", "" ); listitem->appendSubItem ( 1, val, false, p2 );
			javaListBox->insertItem ( listitem, 2, true );
		}
	}	

	ZConfig confCARD ( "/mmc/mmca1/.system/java/CardRegistry", true );
	confCARD.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
		name = confCARD.readEntry ( *it, "Name", "" );
		type = confCARD.readEntry ( *it, "JavaId", "" );
		typeback = confCARD.readEntry ( *it, "JavaIdback", "" );
		dir = confCARD.readEntry ( *it, "Directory", "" );
		ico = confCARD.readEntry ( *it, "BigIcon", "" );
		appid = *it;
		int i = dir.findRev ( "../../" ); dir.remove ( i, 5 );
		QString IsJavaApp = "NO";
		if ( type!="") { IsJavaApp = "YES"; }
		if ( typeback!="") { IsJavaApp = "YES"; }
		if ( (appid!="") && (name!="") && (IsJavaApp=="YES") )
		{
			QFile entrada(dir + "/registry.txt"); QTextStream stentrada(&entrada); QString INFO="NO";
			if ( entrada.open(IO_ReadOnly | IO_Translate) ) {
			  while ( !stentrada.eof() ) { QString line = stentrada.readLine(); if ( line == "Is-Trusted: 1" ) { INFO="YES"; } }
			}
			ZListBoxItem* listitem = new ZListBoxItem ( javaListBox, QString ( "%I20%M%I" ) );
			p1 = QPixmap(iconReader.getIcon(ico, false));
			if (type!="") { p1.load(dir+"/"+ico); } else { p1.load(ico); }
			if ( p1.height() > 19 ) { QImage image; image = p1.convertToImage(); 
			image = image.smoothScale(19, 19); p1.convertFromImage(image); } 
			listitem->setPixmap ( 0, p1 );
			listitem->appendSubItem ( 1, name, false, 0 );
			QPixmap* p2;
			if ( INFO == "YES" ) { p2 = new QPixmap(ProgDir + "/images/javaok.png");  }
			else { p2 = new QPixmap(ProgDir + "/images/javalock.png");  }
			QString val = ini.readEntry ( "Z6MANAGER", "INSTALL_SD", "" ); listitem->appendSubItem ( 1, val, false, p2 );
			javaListBox->insertItem ( listitem, 2, true );
		}
	}	

	javaListBox->sort(true);
	javaListBox->setCurrentItem(0,true);

}

void Z6Man::CargarListaRobo()
{
  QPixmap p1; ZConfig IDini(Idioma, false);
  QString s1 = "";
  QString IDval = IDini.readEntry(QString("Z6MANAGER"), QString("OPTIONS_ENABLED"), ""); QString IDena=IDval;
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("OPTIONS_DISABLED"), ""); QString IDdis=IDval;

  QString file2 = QString("%1/zNothief").arg(ProgDir);
  ZListBoxItem* listitem2 = new ZListBoxItem ( roboListBox, QString ( "%I%M" ) );
  p1.load(ProgDir + "/images/security.png");
  listitem2->setPixmap ( 0, p1 );
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_INI"), "");
  listitem2->appendSubItem ( 1, IDval, false, NULL );
  QPixmap* p2 = new QPixmap(ProgDir + "/images/null.png");
  if (QFileInfo(file2).isFile()) { listitem2->appendSubItem ( 1, IDena, false, p2 ); }
  else { listitem2->appendSubItem ( 1, IDdis, false, p2 ); }
  roboListBox->insertItem ( listitem2,-1,true );

  s1 = ProgDir + QString("/Z6Manager.cfg");ZConfig ini6(s1, false);
  QString val2 = ini6.readEntry(QString("Z6Manager"), QString("Number"), "");
  ZListBoxItem* listitem9 = new ZListBoxItem ( roboListBox, QString ( "%I%M" ) );
  p1.load(ProgDir + "/images/security.png");
  listitem9->setPixmap ( 0, p1 );
  ZConfig NTini("/ezxlocal/download/appwrite/setup/ezx_znothief.cfg", false);
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_NUMBER"), "");
  listitem9->appendSubItem ( 1, IDval, false, NULL );
  QString NTval = NTini.readEntry(QString("ZNOTHIEF"), QString("NUMBERS"), "");
  listitem9->appendSubItem ( 1, QString ("%1").arg(NTval), false, p2 );
  roboListBox->insertItem ( listitem9,-1,true );

  ZListBoxItem* listitem3 = new ZListBoxItem ( roboListBox, QString ( "%I%M" ) );
  p1.load(ProgDir + "/images/security.png");
  listitem3->setPixmap ( 0, p1 );
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_PASSWORD"), "");
  listitem3->appendSubItem ( 1, IDval, false, NULL );
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_CHANGE"), "");
  listitem3->appendSubItem ( 1, IDval, false, p2 );
  roboListBox->insertItem ( listitem3,-1,true );
}

void Z6Man::CargarListaAutorun()
{
  QPixmap p1; ZConfig IDini(Idioma, false);
  QString s1 = "";
  QString IDval = IDini.readEntry(QString("Z6MANAGER"), QString("OPTIONS_ENABLED"), ""); QString IDena=IDval;
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("OPTIONS_DISABLED"), ""); QString IDdis=IDval;

  ZListBoxItem* listitem1 = new ZListBoxItem ( runListBox, QString ( "%I%M" ) );
  p1.load(ProgDir + "/images/cron.png");
  listitem1->setPixmap ( 0, p1 );
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("AUTORUN_CRON"), "");
  listitem1->appendSubItem ( 1, IDval, false, NULL );
  ZConfig Fini(QString("%1/Z6Manager.cfg").arg(ProgDir), false);
  QString Fval = Fini.readEntry(QString("Z6Manager"), QString("Cron"), "");
  QPixmap* p2 = new QPixmap(ProgDir + "/images/null.png");
  if ( Fval != "NO" ) { listitem1->appendSubItem ( 1, IDena, false, p2 ); }
  else { listitem1->appendSubItem ( 1, IDdis, false, p2 ); }
  runListBox->insertItem ( listitem1,-1,true );

  ZListBoxItem* listitem2 = new ZListBoxItem ( runListBox, QString ( "%I%M" ) );
  p1.load(ProgDir + "/images/telnet.png");
  listitem2->setPixmap ( 0, p1 );
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("AUTORUN_INETD"), "");
  listitem2->appendSubItem ( 1, IDval, false, NULL );
  Fval = Fini.readEntry(QString("Z6Manager"), QString("INETD"), "");
  if ( Fval != "NO" ) { listitem2->appendSubItem ( 1, IDena, false, p2 ); }
  else { listitem2->appendSubItem ( 1, IDdis, false, p2 ); }
  runListBox->insertItem ( listitem2,-1,true );

  ZListBoxItem* listitem5 = new ZListBoxItem ( runListBox, QString ( "%I%M" ) );
  p1.load(ProgDir + "/images/swap.png");
  listitem5->setPixmap ( 0, p1 );
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("AUTORUN_SWAP"), "");
  listitem5->appendSubItem ( 1, IDval, false, NULL );
  Fval = Fini.readEntry(QString("Z6Manager"), QString("Swap"), "");
  if ( Fval != "NO" ) { listitem5->appendSubItem ( 1, IDena, false, p2 ); }
  else { listitem5->appendSubItem ( 1, IDdis, false, p2 ); }
  runListBox->insertItem ( listitem5,-1,true );

  ZListBoxItem* listitem8 = new ZListBoxItem ( runListBox, QString ( "%I%M" ) );
  p1.load(ProgDir + "/images/font.png");
  listitem8->setPixmap ( 0, p1 );
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("AUTORUN_FONT"), "");
  listitem8->appendSubItem ( 1, IDval, false, NULL );
  Fval = Fini.readEntry(QString("Z6Manager"), QString("Font"), "");
  if ( Fval != "NO" ) { listitem8->appendSubItem ( 1, getFileName(Fval), false, p2 ); }
  else { IDval = IDini.readEntry(QString("Z6MANAGER"), QString("FONT_SYSTEM"), "");
  listitem8->appendSubItem ( 1, IDval, false, p2 ); }
  runListBox->insertItem ( listitem8,-1,true );

  QString autorun = QString("/ezxlocal/download/mystuff/.autorun.csh");
  QString line="";
  QFile entrada(autorun);
  QTextStream stentrada(&entrada);
  if ( entrada.open(IO_ReadOnly | IO_Translate) )
  {
	while ( !stentrada.eof() )
	{
	  line = stentrada.readLine();
	  if ( line != "" )
	  {
		  ZListBoxItem* autoitem = new ZListBoxItem ( runListBox, QString ( "%I%M" ) );
		  p1.load(ProgDir + "/images/autorun.png");
		  autoitem->setPixmap ( 0, p1 );
		  QString aux = line; QString line2;
		  int tam=aux.length(); int j=0;
		  for(int i=1;i<tam;i++) { line2[j]=aux[i]; j++; }
		  autoitem->appendSubItem ( 1, line2, false, NULL );
		  line = stentrada.readLine();
		  if ( line[0] != QChar('#') ) { autoitem->appendSubItem ( 1, IDena, false, p2 ); }
		  else { autoitem->appendSubItem ( 1, IDdis, false, p2 ); }
		  runListBox->insertItem ( autoitem,-1,true );
      }
	}
  }

}

void Z6Man::CargarListaSkins()
{
  QPixmap p1; ZConfig IDini(Idioma, false);
  system(QString("find /usr/data_resource/theme/ /ezxlocal/download/appwrite/setuptheme/ -name \"*.*hm\" > %1/skintemp").arg(ProgDir));
  QFile entrada(QString("%1/skintemp").arg(ProgDir)); QTextStream stentrada(&entrada); QString Skin;
  if ( entrada.open(IO_ReadOnly | IO_Translate) )
  {
	while ( !stentrada.eof() )
	{           
		Skin = stentrada.readLine(); 
		ZConfig Sini(Skin, false);
		ZListBoxItem* listitem = new ZListBoxItem ( skinListBox, QString ( "%I%M" ) );
		p1.load(ProgDir + "/images/skin.png");
		listitem->setPixmap ( 0, p1 );
		QString IDval = Sini.readEntry(QString("THEME_CONFIG_TABLE"), QString("ThemeName"), "");
		listitem->appendSubItem ( 1, IDval, false, NULL );
		QString IDval2 = Sini.readEntry(QString("THEME_PROP_TABLE"), QString("CopyRight"), "");
		if ( IDval2 == "" ) { IDval2 = IDini.readEntry(QString("Z6MANAGER"), QString("THEME_NODATA"), ""); }
		QPixmap* p2 = new QPixmap(ProgDir + "/images/locked.png");
		if ( (IDval=="Alkali") || (IDval=="Indium") || (IDval=="Plush") ) { listitem->appendSubItem ( 1, IDval2, false, p2 ); }
		else { 
			IDval = Sini.readEntry(QString("THEME_CONFIG_TABLE"), QString("Skin"), "");
			QString temp1 = getFileName(IDval);
			if ( QFileInfo("/mmc/mmca1/.system/skins/"+temp1).isDir() )
				 { p2 = new QPixmap(ProgDir + "/images/sd.png"); }
			else { p2 = new QPixmap(ProgDir + "/images/phone.png"); }
			listitem->appendSubItem ( 1, IDval2, false, p2 ); 
		}
		skinListBox->insertItem ( listitem, -1, true );
	}
  }
  entrada.close();
  system(QString("rm %1/skintemp").arg(ProgDir));
  skinListBox->sort(true);
  skinListBox->setCurrentItem(0,true);

}

void Z6Man::salir()
{

 	ZMessageDlg* msg =  new ZMessageDlg ( QString(APPTITLE), QString("Desea salir?\nRecuerde que algunos cambios no tendran efecto hasta reiniciar."),
        ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
	int i = msg->exec();
	delete msg;
	msg = NULL;
	if (i)
	{
	  qApp->quit();
	}
		
}

void Z6Man::appClicked(ZListBoxItem *item)
{
  menu->close();
  qApp->processEvents();
  int index = appListBox->currentItem();
  if (index == -1) {return;}
  ZListBoxItem* listitem = appListBox->item ( index );
  QString app = listitem->getSubItemText ( 1, 1, true );  
  EjecutarMPKG();
}

void Z6Man::BorrarMPKG()
{
  QString IDval; ZConfig IDini(Idioma, false);
  menu->close();
  qApp->processEvents();
  int index = appListBox->currentItem();
  if (index == -1) {return;}
  ZListBoxItem* listitem = appListBox->item ( index );
  QString app = listitem->getSubItemText ( 1, 0, true );
  QString Place = listitem->getSubItemText ( 1, 1, true );  

  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("UNINSTALL_MSG"), "");
  ZMessageDlg* msg =  new ZMessageDlg ( QString(APPTITLE), QString("%1 %2?").arg(IDval).arg(app),
	ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
  int i = msg->exec();
  delete msg;
  msg = NULL;
  if (i)
  {
	IDval = IDini.readEntry(QString("Z6MANAGER"), QString("INSTALL_PHONE"), "");
	QString card;
	if ( Place != IDval) { card = "/mmc/mmca1/.system/java/CardRegistry"; }
	else { card = "/ezxlocal/download/appwrite/am/InstalledDB"; }

	system(QString("sed -n -e '/^AppID =/p' /mmc/mmca1/.system/java/CardRegistry /ezxlocal/download/appwrite/am/InstalledDB | awk '{print $3}' | sed 's/{//g' | sed 's/}//g' > /tmp/TempApps"));


	QFile napps(QString("/tmp/TempApps")); 
	QTextStream sapps(&napps);
	QString line;

	if ( napps.open(IO_ReadOnly | IO_Translate) )
	{
	  while ( !sapps.eof() )
	  {           
		line = sapps.readLine();
		QString val, val2; ZConfig ini(card, true);
		val = ini.readEntry(line, QString("Name"), "");
		if ( val == app )
		{
		
		  val = ini.readEntry(line, QString("AppID"), "");
		  val2 = ini.readEntry(line, QString("Directory"), "");
		  //system(QString("APP=`echo %1 | sed s/}//g | sed s/{//g`; sed -i \"/$APP/,/Visible/d\" %2").arg(val).arg(card));
		  //system(QString("sed -i s/^\(\n\)*/\1/ %1").arg(card));
		  //system(QString("APP=`echo %1 | sed s/}//g | sed s/{//g`; sed -i \"N;s/$APP;//g\" /ezxlocal/download/appwrite/am/UserMenuTree").arg(val));
		  system(QString("rm -f -r %1").arg(val2));

			QString aid = val;
			aid.remove(0,1); aid.remove(36,1);

			ini.clearGroup(aid);
			ini.flush();

			QStringList grouplist; QString folderid;
			ZConfig confINST ( "/ezxlocal/download/appwrite/am/UserMenuTree", true );
			confINST.getGroupsKeyList ( grouplist );
			QString folderID = QString::null;
		    for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {
	            QStringList list = confINST.readListEntry ( *it, QString ( "Items" ), QChar ( ';' ) );
	            QStringList::Iterator it2 = list.find ( aid );
	            if ( it2 != list.end() ) {
                    list.remove ( it2 );
                    confINST.writeEntry ( *it, QString ( "Items" ), list, QChar ( ';' ) );
                    confINST.flush();
					folderID =  *it;
			
					//system(QString("echo \"DESINSTALANDO %1 %2 \n\"").arg(aid).arg(folderID));
					AMupdate2( aid, folderID );
		
                    break;
	            }
		    }

		  LimpiarCR();
		}
	  }
	}
	napps.close();
	//system(QString("rm /tmp/TempApps"));
	qApp->processEvents();
	IDval = IDini.readEntry(QString("Z6MANAGER"), QString("UNINSTALL_SUCCESS"), "");
	znd = new ZNoticeDlg(ZNoticeDlg::Information, 8000, QString(""), QString("%1 %2").arg(app).arg(IDval) , this, "z", true, 0);
	znd->show(); znd = NULL;
	while (appListBox->count() != 0) { appListBox->removeItem(0); } CargarListaApps();
  }

}


void Z6Man::EjecutarMPKG()
{

  int index = appListBox->currentItem();
  if (index == -1) {return;}
  ZListBoxItem* listitem = appListBox->item ( index );
  QString app = listitem->getSubItemText ( 1, 0, true );  
  QString Place = listitem->getSubItemText ( 1, 1, true );  

  QString IDval; ZConfig IDini(Idioma, false);
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("INSTALL_PHONE"), "");

  QString name, exec, dir;

  if ( Place != IDval) {
	ZConfig confCARD ( "/mmc/mmca1/.system/java/CardRegistry", false );
	QStringList grouplist;
	confCARD.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
	  name = confCARD.readEntry ( *it, "Name", "" );
	  dir = confCARD.readEntry ( *it, "Directory", "" );
	  exec = confCARD.readEntry ( *it, "Exec", "" );
	  int i = dir.findRev("../../"); if ( i==0 ) { dir.remove(0,5); }
	  if ( name == app) { system(QString("%1/%2").arg(dir).arg(exec)); }
	}	
  }
  else {
	ZConfig confUSER ( "/ezxlocal/download/appwrite/am/UserRegistry", false );
	QStringList grouplist;
	confUSER.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
	  name = confUSER.readEntry ( *it, "Name", "" );
	  dir = confUSER.readEntry ( *it, "Directory", "" );
	  exec = confUSER.readEntry ( *it, "Exec", "" );
	  int i = dir.findRev("../../"); if ( i==0 ) { dir.remove(0,5); }
	  if ( name == app) { system(QString("echo \"%1/%2\"").arg(dir).arg(exec)); 
		system(QString("%1/%2").arg(dir).arg(exec)); }
	}	
  }


}

void Z6Man::runClicked(ZListBoxItem *item)
{

  QString a = "";
  a = item->getSubItemText(1, 0, false);
  QString b = "";
  b = item->getSubItemText(1, 1, false);
  int index = runListBox->currentItem();
  ZConfig IDini(Idioma, false);
  QString IDval1 = IDini.readEntry(QString("Z6MANAGER"), QString("AUTORUN_CRON"), "");
  QString IDval2 = IDini.readEntry(QString("Z6MANAGER"), QString("AUTORUN_INETD"), "");
  QString IDval3 = IDini.readEntry(QString("Z6MANAGER"), QString("AUTORUN_SWAP"), "");
  QString IDval4 = IDini.readEntry(QString("Z6MANAGER"), QString("AUTORUN_FONT"), "");
  if ( a == IDval1 ) { Cron(); } else {
  if ( a == IDval2 ) { Inetd(); } else {
  if ( a == IDval3 ) { Swap(); } else {
  if ( a == IDval4 ) { ChangeFont(b); } else {
    IDval1 = IDini.readEntry(QString("Z6MANAGER"), QString("OPTIONS_ENABLED"), "");
	QString b = item->getSubItemText(1, 1, false);
	if ( IDval1 != b ) { a = item->getSubItemText(1, 0, false); EntradaAutorun(a, index, "NO"); }
	else { a = item->getSubItemText(1, 0, false); EntradaAutorun(a, index, "SI"); }
  }}}}

}

void Z6Man::itemClicked(ZListBoxItem *item)
{

  QString a = "";
  a = item->getSubItemText(1, 0, false);
  QString b = "";
  b = item->getSubItemText(1, 1, false);
  
  QString IDval; ZConfig IDini(Idioma, false);
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_NUMBER"), "");
  if ( a == IDval ) { zNothief(b); }
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_INI"), "");
  if ( a == IDval ) { Antirrobo(); }
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_PASSWORD"), "");
  if ( a == IDval ) { IngresoClave(); }

}

void Z6Man::zNothief(QString numeros)
{
    ZConfig IDini(Idioma, false);
	QString s1 = QString("%1/Z6Manager.cfg").arg(ProgDir);
	QString val;
	ZConfig ini(s1, true);
	val = ini.readEntry(QString("Z6Manager"), QString("Password"), "");

	QString IDval = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_ENTERPASS"), "");
	ZSingleCaptureDlg* zscd = new ZSingleCaptureDlg(QString(APPTITLE), IDval, ZSingleCaptureDlg::TypeLineEdit, NULL, "ZSingleCaptureDlg", true, 0, 0);
	ZLineEdit* zle = (ZLineEdit*)zscd->getLineEdit();  
	zle->setEchoMode(ZLineEdit::Password);
	zle->setText("");
	zle->setEdited(true);  

	if (zscd->exec() == 1)
	{
	  QString val2 = zle->text();
	  if ( val == val2) 
	  { 

		QString IDval = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_MSG1"), "");
		QString IDval2 = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_ENTERNUM"), "");
		ZSingleCaptureDlg* zscd = new ZSingleCaptureDlg(IDval, IDval2, ZSingleCaptureDlg::TypeLineEdit, NULL, "ZSingleCaptureDlg", true, 0, 0);
		ZLineEdit* zle = (ZLineEdit*)zscd->getLineEdit();  
		zle->setText(numeros);
		zle->setEdited(true);  

		if (zscd->exec() == 1)
		{
			val = zle->text();
			qApp->processEvents();

			unsigned int res;
			TAPI_IMEI_NUMBER_A imsi;
			TAPI_CLIENT_Init(NULL, 0);
			res = TAPI_ACCE_GetImsi(imsi);
			QString pImsi;
			if ( res == 0)
			{
				pImsi = QString::fromLatin1( reinterpret_cast<char*>( imsi ) );
				qDebug(QString("\nIMEI = %1").arg(pImsi));
			}
			else { qDebug(QString("*** FAIL: error %1").arg(res)); }
			TAPI_CLIENT_Fini();

			system(QString("echo \"[ZNOTHIEF]\nIMSI = %1\nNUMBERS = %2\n\" > "
			"/ezxlocal/download/appwrite/setup/ezx_znothief.cfg").arg(pImsi).arg(val));


			while (roboListBox->count() != 0) { roboListBox->removeItem(0); } CargarListaRobo();
		}  
	  }
	  else
	  {
		IDval = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_ERROR"), "");
		znd = new ZNoticeDlg(ZNoticeDlg::Information, 3000,QString(""), IDval, this, "z", true, 0);
		znd->show(); 
		znd = NULL;
	  }
	}
	roboListBox->setCurrentItem(1,true);

}

void Z6Man::Cron()
{
    ZConfig IDini(Idioma, false);
	QString IDval = IDini.readEntry(QString("Z6MANAGER"), QString("CRON_MSG1"), "");
	ZSingleSelectDlg* eq1 =  new ZSingleSelectDlg(IDval, QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list;
	IDval = IDini.readEntry(QString("Z6MANAGER"), QString("OPTIONS_ENABLED"), "");
	list = IDval; 
	IDval = IDini.readEntry(QString("Z6MANAGER"), QString("OPTIONS_DISABLED"), "");
	list += IDval;
	eq1->addItemsList(list);
	ZListBox* listboxx = eq1->getListBox();
	ZConfig Fini(QString("%1/Z6Manager.cfg").arg(ProgDir), true);
	QString Fval = Fini.readEntry(QString("Z6Manager"), QString("Cron"), "");
	if ( Fval != "NO" ) { listboxx->checkItem(0, true); }
	else { listboxx->checkItem(1, true); }
	eq1->exec();
	int result = eq1->result();
	if (result != 1) {
		result = 0;
	} else {
		if (listboxx->itemChecked(1))
		{
			Fini.writeEntry(QString("Z6Manager"), QString("Cron"), "NO");
			while (runListBox->count() != 0) { runListBox->removeItem(0); } CargarListaAutorun();
		}
		if (listboxx->itemChecked(0))
		{
			Fini.writeEntry(QString("Z6Manager"), QString("Cron"), "YES");
			while (runListBox->count() != 0) { runListBox->removeItem(0); } CargarListaAutorun();
		}
	}
	runListBox->setCurrentItem(0,true);

}

void Z6Man::Inetd()
{
    ZConfig IDini(Idioma, false);
	QString IDval = IDini.readEntry(QString("Z6MANAGER"), QString("AUTORUN_INETD"), "");
	ZSingleSelectDlg* eq1 =  new ZSingleSelectDlg(IDval, QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list;
	IDval = IDini.readEntry(QString("Z6MANAGER"), QString("OPTIONS_ENABLED"), "");
	list = IDval; 
	IDval = IDini.readEntry(QString("Z6MANAGER"), QString("OPTIONS_DISABLED"), "");
	list += IDval;
	eq1->addItemsList(list);
	ZListBox* listboxx = eq1->getListBox();
	ZConfig Fini(QString("%1/Z6Manager.cfg").arg(ProgDir), true);
	QString Fval = Fini.readEntry(QString("Z6Manager"), QString("INETD"), "");
	if ( Fval != "NO" ) { listboxx->checkItem(0, true); }
	else { listboxx->checkItem(1, true); }
	eq1->exec();
	int result = eq1->result();
	if (result != 1) {
		result = 0;
	} else {
		if (listboxx->itemChecked(1))
		{
			Fini.writeEntry(QString("Z6Manager"), QString("INETD"), "NO");
			while (runListBox->count() != 0) { runListBox->removeItem(0); } CargarListaAutorun();
		}
		if (listboxx->itemChecked(0))
		{
			Fini.writeEntry(QString("Z6Manager"), QString("INETD"), "YES");
			while (runListBox->count() != 0) { runListBox->removeItem(0); } CargarListaAutorun();
		}
	}
	runListBox->setCurrentItem(1,true);
}

void Z6Man::Swap()
{
    ZConfig IDini(Idioma, false);
	QString IDval = IDini.readEntry(QString("Z6MANAGER"), QString("AUTORUN_SWAP"), "");
	ZSingleSelectDlg* eq1 =  new ZSingleSelectDlg(IDval, QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list;
	IDval = IDini.readEntry(QString("Z6MANAGER"), QString("OPTIONS_ENABLED"), "");
	list = IDval; 
	IDval = IDini.readEntry(QString("Z6MANAGER"), QString("OPTIONS_DISABLED"), "");
	list += IDval;
	eq1->addItemsList(list);
	ZListBox* listboxx = eq1->getListBox();
	ZConfig Fini(QString("%1/Z6Manager.cfg").arg(ProgDir), true);
	QString Fval = Fini.readEntry(QString("Z6Manager"), QString("Swap"), "");
	if ( Fval != "NO" ) { listboxx->checkItem(0, true); }
	else { listboxx->checkItem(1, true); }
	eq1->exec();
	int result = eq1->result();
	if (result != 1) {
		result = 0;
	} else {
		if (listboxx->itemChecked(1))
		{
			system("busybox swapoff /mmc/mmca1/.system/swapfile");
			system("rm /mmc/mmca1/.system/swapfile");
			Fini.writeEntry(QString("Z6Manager"), QString("Swap"), "NO");
			system("mv /ezxlocal/LinXtend/usr/bin/runswap /ezxlocal/LinXtend/usr/bin/runswap1");
			while (runListBox->count() != 0) { runListBox->removeItem(0); } CargarListaAutorun();
		}
		if (listboxx->itemChecked(0))
		{

			swapspace = new ZNumPickerDlg(10, this, "ZNumPickerDlg", 0);
			ZNumModule* module = swapspace->getNumModule();
			module->setMaxValue(164);
			module->setMinValue(8);
			int Sval = Fini.readNumEntry(QString("Z6Manager"), QString("SwapSpace"), 8);
			module->setValue(Sval);
			IDval = IDini.readEntry(QString("Z6MANAGER"), QString("SWAP_SPACE"), "");
			ZLabel* label = new ZLabel( IDval, module, NULL,0, (ZSkinService::WidgetClsID)4);
			module->setLabel(label, (ZNumModule::RelativeAlignment)0);
			IDval = IDini.readEntry(QString("Z6MANAGER"), QString("AUTORUN_SWAP"), "");
			swapspace->setTitle(IDval);
			if (swapspace->exec())
			{	
				QString Ival1 = IDini.readEntry(QString("Z6MANAGER"), QString("SWAP_CREATING"), "");

				ZMessageDlg* msg0 =  new ZMessageDlg ( QString(APPTITLE),  Ival1, ZMessageDlg::NONE, 200, NULL, "ZMessageDlg", true, 0 );
				msg0->exec();

				int r = swapspace->getNumValue();
				Fini.writeEntry(QString("Z6Manager"), QString("SwapSpace"), r);

				system("busybox swapoff /mmc/mmca1/.system/swapfile");
				system("rm /mmc/mmca1/.system/swapfile");
				system(QString("dd if=/dev/zero of=/mmc/mmca1/.system/swapfile bs=1024k count=%1").arg(r));
				system("busybox mkswap /mmc/mmca1/.system/swapfile");
				system("busybox swapon /mmc/mmca1/.system/swapfile");

				Fini.writeEntry(QString("Z6Manager"), QString("Swap"), "YES");
				system("mv /ezxlocal/LinXtend/usr/bin/runswap1 /ezxlocal/LinXtend/usr/bin/runswap");
				while (runListBox->count() != 0) { runListBox->removeItem(0); } CargarListaAutorun();

				delete msg0;
				msg0 = NULL;

			}
		}
	}
	runListBox->setCurrentItem(2,true);

}

void Z6Man::Antirrobo()
{
    ZConfig IDini(Idioma, false);
	QString s1 = QString("%1/Z6Manager.cfg").arg(ProgDir);
	QString val;
	ZConfig ini(s1, false);
	val = ini.readEntry(QString("Z6Manager"), QString("Password"), "");
	
	QString IDval = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_ENTERPASS"), "");
	ZSingleCaptureDlg* zscd = new ZSingleCaptureDlg(QString(APPTITLE), IDval, ZSingleCaptureDlg::TypeLineEdit, NULL, "ZSingleCaptureDlg", true, 0, 0);
	ZLineEdit* zle = (ZLineEdit*)zscd->getLineEdit();  
	zle->setEchoMode(ZLineEdit::Password);
	zle->setText("");
	zle->setEdited(true);  

	if (zscd->exec() == 1)
	{
	  QString val2 = zle->text();
	  if ( val == val2)
	  { 
		IDval = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_MSG1"), "");
		QString file1 = QString("%1/zNothief").arg(ProgDir);
		ZSingleSelectDlg* eq1 =  new ZSingleSelectDlg(IDval, QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
		QStringList list;
		IDval = IDini.readEntry(QString("Z6MANAGER"), QString("OPTIONS_ENABLED"), "");
		list = IDval;
		IDval = IDini.readEntry(QString("Z6MANAGER"), QString("OPTIONS_DISABLED"), "");
		list += IDval;
		eq1->addItemsList(list);
		ZListBox* listboxx = eq1->getListBox();
		if (QFileInfo(file1).isFile()) { listboxx->checkItem(0, true); }
		else { listboxx->checkItem(1, true); }
		eq1->exec();
		int result = eq1->result();
		if (result != 1) {
			result = 0;
		} else {
			if (listboxx->itemChecked(1))
			{
				system(QString("mv %1/zNothief %2/zNothief1").arg(ProgDir).arg(ProgDir));
				while (roboListBox->count() != 0) { roboListBox->removeItem(0); } CargarListaRobo();
			}
			if (listboxx->itemChecked(0))
			{
				system(QString("mv %1/zNothief1 %2/zNothief").arg(ProgDir).arg(ProgDir));
				while (roboListBox->count() != 0) { roboListBox->removeItem(0); } CargarListaRobo();
			}
		}
	  }
	  else
	  {
		ZConfig IDini(Idioma, false);
		QString IDval = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_ERROR"), "");
		znd = new ZNoticeDlg(ZNoticeDlg::Information, 3000, QString(""), IDval, this, "z", true, 0);
		znd->show(); znd = NULL;
	  }
	}
	roboListBox->setCurrentItem(0,true);
}

void Z6Man::language()
{

  QString val; ZConfig ini(Idioma, false);
  val = ini.readEntry(QString("Z6MANAGER"), QString("MENU_LANGUAGE"), "");

  QString lang1= ini.readEntry(QString("Z6MANAGER"), QString("LANG_NAME"), "");

  ZSingleSelectDlg* eq1 =  new ZSingleSelectDlg(val, QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
  system(QString("ls %1/languages/ > %2/ltemp").arg(ProgDir).arg(ProgDir));	

  QString card = QString("%1/ltemp").arg(ProgDir);
  QString line="";
  int cantl=0;  
  QStringList list;
  QFile entrada(card);
  QTextStream stentrada(&entrada);
  QString aux="";
  int active=0;
  QString mlang = lang1;
  if ( entrada.open(IO_ReadOnly | IO_Translate) )
  {
	while ( !stentrada.eof() )
    {           
	  line = stentrada.readLine();
	  ZConfig IDini(QString("%1/languages/%2").arg(ProgDir).arg(line), false);
	  QString val = IDini.readEntry(QString("Z6MANAGER"), QString("LANG_NAME"), "");
	  QString val2 = IDini.readEntry(QString("Z6MANAGER"), QString("UNICODE"), "");
	  if ( val2 == "1" ) { cantl++; aux=val; list +=val; }
	  else { cantl++; aux=val; list +=val; }
	  if ( val == mlang ) { active = cantl-1; }
    }
  }
  entrada.close();
  eq1->addItemsList(list);
  ZListBox* listboxx = eq1->getListBox();
  listboxx->checkItem(active, true);
  eq1->exec();
	int result = eq1->result();
	if (result != 1) {
		result = 0;
	} else {
		result = eq1->getCheckedItemIndex();

		QString card2 = QString("%1/ltemp").arg(ProgDir);
		QString line2="";
		int cantl2=0;  
		QStringList list2;
		QFile entrada2(card2);
		QTextStream stentrada2(&entrada2);
		QString aux2="";
		cantl2=0;
		if ( entrada2.open(IO_ReadOnly | IO_Translate) )
		{
		while ( !stentrada2.eof() )
		{           
		  line2 = stentrada2.readLine();
		  if ( result == cantl2 )
		  {
			Idioma = ProgDir + "/languages/" + line2;
			ZConfig Xini(QString("%1/Z6Manager.cfg").arg(ProgDir), true);
			Xini.writeEntry(QString("Z6Manager"), QString("Language"), line2);
			qApp->processEvents();

		  int tw = tabWidget->currentPageIndex();
		  switch (tw)
		  { case 0:	{ Create_MenuApp(); break; }
			case 1: { Create_MenuAutorun(); break; }
			case 2: { Create_MenuJava(); break; }     
			case 3: { Create_MenuConfig(); break; }     
			case 4: { Create_MenuTheme(); break; }
		  }
		  Create_SoftKey();
		  while (runListBox->count() != 0) { runListBox->removeItem(0); } CargarListaAutorun();
		  while (javaListBox->count() != 0) { javaListBox->removeItem(0); } CargarListaJava();
		  while (appListBox->count() != 0) { appListBox->removeItem(0); } CargarListaApps();
		  while (roboListBox->count() != 0) { roboListBox->removeItem(0); } CargarListaRobo();
		  while (skinListBox->count() != 0) { skinListBox->removeItem(0); } CargarListaSkins();

		  }
		  cantl2++;
		  aux2=line2;
		  list2 +=aux2;
		}
		entrada.close();		
	}
  }
  system(QString("rm %1/ltemp").arg(ProgDir));

}

QString Z6Man::getFileName(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( 0, i);
  if (str.left(1) = "/") str.remove ( 0, 1);
  return str;    
}

QString Z6Man::getFilePath(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( i+1, str.length() - i );
  return str;  
}


void Z6Man::Instalar(QString mpkg)
{

	system("mkdir /mmc/mmca1/.system/skins");
	ZConfig IDini(Idioma, false);
	QString Ival1 = IDini.readEntry(QString("Z6MANAGER"), QString("INSTALL_READING"), "");
	QString Ival2 = IDini.readEntry(QString("Z6MANAGER"), QString("INSTALL_WAIT"), "");

	ZMessageDlg* msg0 =  new ZMessageDlg ( QString(APPTITLE), 
		QString("%1\n%2").arg(Ival1).arg(Ival2), ZMessageDlg::NONE, 200, NULL, "ZMessageDlg", true, 0 );
	msg0->exec();
	system("rm -r /mmc/mmca1/.system/tempmpkg");
	system("mkdir /mmc/mmca1/.system/tempmpkg");
	system(QString("/usr/bin/busybox tar xf \"%1\" -C /mmc/mmca1/.system/tempmpkg/").arg(mpkg));  //For use with new busybox
	//system(QString("busybox gzip -c -d \"%1\" | busybox tar -xv -f - -C /mmc/mmca1/.system/tempmpkg/").arg(mpkg));
	system(QString("cat /mmc/mmca1/.system/tempmpkg/*/*.desktop > %1/tempdesktop").arg(ProgDir));
	delete msg0;
	msg0 = NULL;

	ZConfig ini(QString("%1/tempdesktop").arg(ProgDir), false);
	QString val = ini.readEntry(QString("Desktop Entry"), QString("Name"), "");

	QString line; installed = "NO";


	system(QString("sed -n -e '/^Name =/p' /mmc/mmca1/.system/java/CardRegistry | awk '{print $3}' | sed 's/{//g' | sed 's/}//g' > /tmp/TempApps"));
	QFile napps(QString("/tmp/TempApps")); QTextStream sapps(&napps); if ( napps.open(IO_ReadOnly | IO_Translate) )
	{ while ( !sapps.eof() ) { line = sapps.readLine(); if ( line == val ) { installed = "SD"; } } } napps.close();
	system(QString("rm /tmp/TempApps"));

	system(QString("sed -n -e '/^Name =/p' /ezxlocal/download/appwrite/am/UserRegistry | awk '{print $3}' | sed 's/{//g' | sed 's/}//g' > /tmp/TempApps"));
	QFile napps2(QString("/tmp/TempApps")); QTextStream sapps2(&napps2); if ( napps2.open(IO_ReadOnly | IO_Translate) )
	{ while ( !sapps2.eof() ) { line = sapps2.readLine(); if ( line == val ) { installed = "PHONE"; } } } napps2.close();
	system(QString("rm /tmp/TempApps"));

	if ( installed == "NO" )
	{
		if ( val != "" ) { InstalarApp(mpkg, val); }
		val = ini.readEntry(QString("Desktop Entry"), QString("ThemeName"), "");
		if ( val != "" ) InstalarSkin(mpkg, val);
		val = ini.readEntry(QString("Desktop Entry"), QString("Update"), "");
		if ( val == "Yes" ) Actualizar(mpkg);
		system("rm -r /mmc/mmca1/.system/tempmpkg");
	}
	else
	{
		ZConfig IDini(Idioma, false);
		QString IDval = IDini.readEntry(QString("Z6MANAGER"), QString("INSTALL_INSTALLED"), "");
	 	ZMessageDlg* msg =  new ZMessageDlg ( QString::fromUtf8(APPTITLE), IDval, ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
		int i = msg->exec(); delete msg; msg = NULL;
	  	if (i)
		{ 
			if ( val != "" ) { InstalarApp(mpkg, val); }
			val = ini.readEntry(QString("Desktop Entry"), QString("ThemeName"), "");
			if ( val != "" ) InstalarSkin(mpkg, val);
			val = ini.readEntry(QString("Desktop Entry"), QString("Update"), "");
			if ( val == "Yes" ) Actualizar(mpkg);
			system("rm -r /mmc/mmca1/.system/tempmpkg");
		}
		else
		{		
			system("rm -r /mmc/mmca1/.system/tempmpkg");
		}
	}

}

void Z6Man::Actualizar(QString mpkg)
{
    ZConfig IDini(Idioma, false);
	QString val = IDini.readEntry(QString("Z6MANAGER"), QString("INSTALL_UPDATE"), "");
	ZMessageDlg* msg =  new ZMessageDlg ( QString(APPTITLE), QString("%1?").arg(val),
				ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
	int i = msg->exec(); delete msg; msg = NULL;
	if (i)
	{
		ZConfig pack(QString("%1/tempdesktop").arg(ProgDir), false);
		QString valor = pack.readEntry(QString("Desktop Entry"), QString("Folder"), "");
		system("chmod -R 755 /mmc/mmca1/.system/tempmpkg/*"); 
		system(QString("mv /mmc/mmca1/.system/tempmpkg/* %1").arg(valor)); 

		val = IDini.readEntry(QString("Z6MANAGER"), QString("INSTALL_UPDATED"), "");

		ZMessageDlg* msg =  new ZMessageDlg ( QString(APPTITLE), QString("%1 %2").arg(valor).arg(val),
	  	ZMessageDlg::just_ok, 0, NULL, "ZMessageDlg", true, 0 );
		msg->exec(); delete msg; msg = NULL;
	}

}

void Z6Man::InstalarSkin(QString mpkg, QString name)
{
	QString desktop;
	QString narch = getFileName(mpkg);
    ZConfig IDini(Idioma, false);

//INSTALL_SKIN_ASK

	QString val = IDini.readEntry(QString("Z6MANAGER"), QString("INSTALL_SKIN_ASK"), "");
	ZMessageDlg* msg =  new ZMessageDlg ( QString(APPTITLE), QString("%1 %2?").arg(val).arg(name),
				ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
	int i = msg->exec(); delete msg; msg = NULL;
	if (i)
	{
	  QString Ival1 = IDini.readEntry(QString("Z6MANAGER"), QString("INSTALL_INSTALLING_SKIN"), "");
	  QString Ival2 = IDini.readEntry(QString("Z6MANAGER"), QString("INSTALL_WAIT"), "");

	  ZMessageDlg* msg0 =  new ZMessageDlg ( QString(APPTITLE),
	  	QString("%1\n%2").arg(Ival1).arg(Ival2), ZMessageDlg::NONE, 200, NULL, "ZMessageDlg", true, 0 );
	  msg0->exec();

	  ZConfig pack(QString("%1/tempdesktop").arg(ProgDir), false);
	  QString valor = pack.readEntry(QString("Desktop Entry"), QString("Folder"), "");
	  QString FolderName = valor;
	  QString SkinPlace;

	    system(QString("mkdir /mmc/mmca1/.system/skins/%1").arg(FolderName));
		system(QString("mv /mmc/mmca1/.system/tempmpkg/*/skin/* /mmc/mmca1/.system/skins/%1/").arg(FolderName));

		SkinPlace = "/mmc/mmca1/.system/skins";
		QString INFO = "";
		INFO += QString("[THEME_CONFIG_TABLE]");
		valor = pack.readEntry(QString("Desktop Entry"), QString("ThemeName"), "");
		INFO += QString("\nThemeName = %1").arg(valor);
		INFO += QString("\nSkin = %1/%1").arg(SkinPlace).arg(FolderName);
		valor = pack.readEntry(QString("Desktop Entry"), QString("WallPaper"), "");
		if (valor != "") { INFO += QString("\nWallPaper = %1/%2/%3").arg(SkinPlace).arg(FolderName).arg(valor); }
		valor = pack.readEntry(QString("Desktop Entry"), QString("CalendarAlarm"), "");
		if (valor != "") { INFO += QString("\nCalendarAlarm = %1/%2/%3").arg(SkinPlace).arg(FolderName).arg(valor); }
		valor = pack.readEntry(QString("Desktop Entry"), QString("CallAlertLine1"), "");
		if (valor != "") { INFO += QString("\nCallAlertLine1 = %1/%2/%3").arg(SkinPlace).arg(FolderName).arg(valor); }
		valor = pack.readEntry(QString("Desktop Entry"), QString("CallAlertLine2"), "");
		if (valor != "") { INFO += QString("\nCallAlertLine2 = %1/%2/%3").arg(SkinPlace).arg(FolderName).arg(valor); }
		valor = pack.readEntry(QString("Desktop Entry"), QString("EmailAlert"), "");
		if (valor != "") { INFO += QString("\nEmailAlert = %1/%2/%3").arg(SkinPlace).arg(FolderName).arg(valor); }
		valor = pack.readEntry(QString("Desktop Entry"), QString("IMAlert"), "");
		if (valor != "") { INFO += QString("\nIMAlert = %1/%2/%3").arg(SkinPlace).arg(FolderName).arg(valor); }
		valor = pack.readEntry(QString("Desktop Entry"), QString("LightAlert"), "");
		if (valor != "") { INFO += QString("\nLightAlert = %1/%2/%3").arg(SkinPlace).arg(FolderName).arg(valor); }
		valor = pack.readEntry(QString("Desktop Entry"), QString("MsgAlert"), "");
		if (valor != "") { INFO += QString("\nMsgAlert = %1/%2/%3").arg(SkinPlace).arg(FolderName).arg(valor); }
		valor = pack.readEntry(QString("Desktop Entry"), QString("PowerOffScreen"), "");
		if (valor != "") { INFO += QString("\nPowerOffScreen = %1/%2/%3").arg(SkinPlace).arg(FolderName).arg(valor); }
		valor = pack.readEntry(QString("Desktop Entry"), QString("PowerOnScreen"), "");
		if (valor != "") { INFO += QString("\nPowerOnScreen = %1/%2/%3").arg(SkinPlace).arg(FolderName).arg(valor); }
		valor = pack.readEntry(QString("Desktop Entry"), QString("ScreenSaver"), "");
		if (valor != "") { INFO += QString("\nScreenSaver = %1/%2/%3").arg(SkinPlace).arg(FolderName).arg(valor); }
		valor = pack.readEntry(QString("Desktop Entry"), QString("VMAlert"), "");
		if (valor != "") { INFO += QString("\nVMAlert = /%1/%2/%3").arg(SkinPlace).arg(FolderName).arg(valor); }

		valor = pack.readEntry(QString("Desktop Entry"), QString("Author"), "");
		INFO += QString("\n\n[THEME_PROP_TABLE]\nCopyRight = %1\nCreatedTime = \nThemeSize = \n").arg(valor);
		
		valor = pack.readEntry(QString("Desktop Entry"), QString("ThemeName"), "");
		system("mkdir /ezxlocal/download/appwrite/setuptheme");
		system(QString("echo \"%1\" > /ezxlocal/download/appwrite/setuptheme/\"%2.chm\"").arg(INFO).arg(valor));

		system(QString("chmod 755 /mmc/mmca1/.system/skins/%1/*").arg(FolderName));
		system(QString("chmod 755 /ezxlocal/download/appwrite/setuptheme/*"));
		system(QString("chown 500:513 /mmc/mmca1/.system/skins/%1/*").arg(FolderName));
		system(QString("chown ezx:ezx /ezxlocal/download/appwrite/setuptheme/*"));

		system(QString("rm %1/tempdesktop").arg(ProgDir));
		system(QString("rm -r /mmc/mmca1/.system/tempmpkg"));

	  delete msg0;
	  msg0 = NULL;

	  QString val; ZConfig ini(Idioma, false);
	  val = ini.readEntry(QString("Z6MANAGER"), QString("INSTALL_SKIN"), "");

	  ZMessageDlg* msg =  new ZMessageDlg ( QString(APPTITLE), QString("%1 %2").arg(valor).arg(val),
	  	ZMessageDlg::just_ok, 0, NULL, "ZMessageDlg", true, 0 );
	  msg->exec(); delete msg; msg = NULL;
	  system("/usr/SYSqtapp/systemsetup/themesetup");
	}
 
}

void Z6Man::InstalarApp(QString mpkg, QString name)
{
	QString desktop;
	QString AppID;
	QString narch = getFileName(mpkg);

	QString Lugar = "Nada";
    ZConfig IDini(Idioma, false);
	QString IDval;

	if ( installed == "NO" )
	{
		IDval = IDini.readEntry(QString("Z6MANAGER"), QString("INSTALL_PLACE_APP"), "");
		ZSingleSelectDlg* eq1 =  new ZSingleSelectDlg(APPTITLE, QString("%1 %2?").arg(IDval).arg(name), NULL, "ZSingleSelectDlg", true, 0, 0);
		QStringList list;
		IDval = IDini.readEntry(QString("Z6MANAGER"), QString("INSTALL_PHONE"), "");
		list = IDval;
		IDval = IDini.readEntry(QString("Z6MANAGER"), QString("INSTALL_SD"), "");
		list += IDval;
		eq1->addItemsList(list);
		ZListBox* listboxx = eq1->getListBox();
		int checked = -1;
		if (checked >= 0)
			listboxx->checkItem(checked, true);
		eq1->exec();
		int result = eq1->result();
		if (result != 1) {
			result = 0;
		} else {
			if (listboxx->itemChecked(0)) {	Lugar = "Telefono"; }
			if (listboxx->itemChecked(1)) { Lugar = "SD"; }
		}
	}
	if ( installed == "SD" ) Lugar = "SD";
	if ( installed == "PHONE" ) Lugar = "Telefono";

	if ( Lugar != "Nada" )
	{
		QString Ival1 = IDini.readEntry(QString("Z6MANAGER"), QString("INSTALL_INSTALLING"), "");
		QString Ival2 = IDini.readEntry(QString("Z6MANAGER"), QString("INSTALL_WAIT"), "");

		ZMessageDlg* msg0 =  new ZMessageDlg ( QString(APPTITLE),
		QString("%1\n%2").arg(Ival1).arg(Ival2), ZMessageDlg::NONE, 200, NULL, "ZMessageDlg", true, 0 );
		msg0->exec();

		QString valor;
		ZConfig pack(QString("%1/tempdesktop").arg(ProgDir), false);
		valor = pack.readEntry(QString("Desktop Entry"), QString("Directory"), "");
		if ( Lugar != "Telefono" )
		{
			system("mkdir /mmc/mmca1/.system/QTDownLoad");
			system("mv /mmc/mmca1/.system/tempmpkg/* /mmc/mmca1/.system/QTDownLoad/"); 
		}
		else
		{
			system("mkdir /ezxlocal/download/mystuff/.system/QTDownLoad");
			system("mv /mmc/mmca1/.system/tempmpkg/* /ezxlocal/download/mystuff/.system/QTDownLoad/");
		}

		if ( installed == "NO" )
		{
			if ( pack.readEntry(QString("Desktop Entry"), QString("AppID"), "") == "" )
			{
				QString App0;
				system(QString("date +'%y%m%d%H%M%S' > %1/tempID").arg(ProgDir));
				QFile entrada3(QString("%1/tempID").arg(ProgDir));
				QTextStream stentrada3(&entrada3);
				if ( entrada3.open(IO_ReadOnly | IO_Translate) )
				{ App0 = stentrada3.readLine(); }
				entrada3.close();
				system(QString("rm %1/tempID").arg(ProgDir));
				AppID = QString("10000000-0000-0000-0000-%1").arg(App0);
			}
			else
			{
				AppID = pack.readEntry(QString("Desktop Entry"), QString("AppID"), "");
			}
			//QString menu ="2a58c0d6-05c6-45aa-8938-63059d41a4e9"; Principal
			QString menu = "67dc1191-2928-4938-abce-1a73cb5ce490";	//Games

			QString INFO = "";
			INFO += QString("\n[%1]").arg(AppID);
			INFO += QString("\nAppID = {%1}").arg(AppID);

			valor = pack.readEntry(QString("Desktop Entry"), QString("AniIcon"), "");
			QString valor2 = pack.readEntry(QString("Desktop Entry"), QString("Directory"), "");
			if ( Lugar != "Telefono" )
			{ INFO += QString("\nAniIcon = /mmc/mmca1/.system/QTDownLoad/%1/%2").arg(valor2).arg(valor); }
			else
			{ INFO += QString("\nAniIcon = /ezxlocal/download/mystuff/.system/QTDownLoad/%1/%2").arg(valor2).arg(valor); }
			INFO += QString("\nArgs = ");
			INFO += QString("\nAttribute = 1");
			valor = pack.readEntry(QString("Desktop Entry"), QString("BigIcon"), "");
			valor2 = pack.readEntry(QString("Desktop Entry"), QString("Directory"), "");
			if ( Lugar != "Telefono" )
			{ INFO += QString("\nBigIcon = /mmc/mmca1/.system/QTDownLoad/%1/%2").arg(valor2).arg(valor); }
			else
			{ INFO += QString("\nBigIcon = /ezxlocal/download/mystuff/.system/QTDownLoad/%1/%2").arg(valor2).arg(valor); }
			INFO += QString("\nDaemon = 0");
			valor = pack.readEntry(QString("Desktop Entry"), QString("Directory"), "");
			if ( Lugar != "Telefono" )
			{ INFO += QString("\nDirectory = ../../mmc/mmca1/.system/QTDownLoad/%1").arg(valor); }
			else
			{ INFO += QString("\nDirectory = ../../ezxlocal/download/mystuff/.system/QTDownLoad/%1").arg(valor); }
			valor = pack.readEntry(QString("Desktop Entry"), QString("Exec"), "");
			INFO += QString("\nExec = %1").arg(valor);
			if ( Lugar != "Telefono" )
			{
			  QFile entrada2(QString("%1/IMEI").arg(ProgDir)); QTextStream stentrada2(&entrada2); QString IMEI;
			  if ( entrada2.open(IO_ReadOnly | IO_Translate) ) { IMEI = stentrada2.readLine(); } entrada2.close();
			  INFO += QString("\n%1").arg(IMEI); 
			}
			INFO += QString("\nFixedAllPosition = 0");
			INFO += QString("\nFixedPosition = 0");
			INFO += QString("\nGroupID = ezx");
			valor = pack.readEntry(QString("Desktop Entry"), QString("Icon"), "");
			valor2 = pack.readEntry(QString("Desktop Entry"), QString("Directory"), "");
			if ( valor != "" )
			{
			  if ( Lugar != "Telefono" )
			  { INFO += QString("\nIcon = /mmc/mmca1/.system/QTDownLoad/%1/%2").arg(valor2).arg(valor); }
			  else
			  { INFO += QString("\nIcon = /ezxlocal/download/mystuff/.system/QTDownLoad/%1/%2").arg(valor2).arg(valor); }
			}
			else
			{
			  INFO += QString("\nIcon = ");
			}
			valor = pack.readEntry(QString("Desktop Entry"), QString("Autorun"), "");
			if ( valor != "" )
			{
			  QString autorun = QString("/ezxlocal/download/mystuff/.autorun.csh");
			  QString line=""; QString archivo1;
			  QFile entrada(autorun);
			  QTextStream stentrada(&entrada);
			  if ( entrada.open(IO_ReadOnly | IO_Translate) )
			  {
				while ( !stentrada.eof() )
				{
				  line = stentrada.readLine();
				  if ( line != "" )
				  {
					archivo1 += QString("%1\n").arg(line);
				  }
				}
				valor = pack.readEntry(QString("Desktop Entry"), QString("Name"), "");
				archivo1 += QChar('#'); archivo1 += QString("%1\n").arg(valor); archivo1 += QChar('#');
				if ( Lugar != "Telefono" ) { archivo1 += QString("/mmc/mmca1/.system/QTDownLoad/"); }
				else { archivo1 += QString("/ezxlocal/download/mystuff/.system/QTDownLoad/"); }
				valor = pack.readEntry(QString("Desktop Entry"), QString("Directory"), "");
				archivo1 += QString("%1").arg(valor); archivo1 += QChar('/');
				valor = pack.readEntry(QString("Desktop Entry"), QString("Autorun"), "");
				archivo1 += QString("%1\n").arg(valor);
			  }
			  system("rm /ezxlocal/download/mystuff/.autorun.csh");
			  system("touch /ezxlocal/download/mystuff/.autorun.csh");
			  system(QString("echo \"%1\" >> /ezxlocal/download/mystuff/.autorun.csh").arg(archivo1));
			}
			INFO += QString("\nLockEnabled = 1");
			INFO += QString("\nMassStorageStatus = 1");
			valor = pack.readEntry(QString("Desktop Entry"), QString("Name"), "");
			INFO += QString("\nName = %1").arg(valor);
			INFO += QString("\nType = 2");
			INFO += QString("\nUserID = root");
			INFO += QString("\nVisible = 0");
			if ( Lugar != "Telefono" )
			{ system(QString("echo \"%1\" >> /mmc/mmca1/.system/java/CardRegistry").arg(INFO)); }
			else
			{ //system(QString("echo \"%1\" >> /ezxlocal/download/appwrite/am/UserRegistry").arg(INFO)); 
			  system(QString("echo \"%1\" >> /ezxlocal/download/appwrite/am/InstalledDB").arg(INFO)); }
		
			if ( Lugar != "Telefono" ) { 
				AMupdate( AppID, menu, -1, 2 );
			}
			else
			{
				AMupdate( AppID, menu, -1, 1 );
			}

			ZConfig UMT("/ezxlocal/download/appwrite/am/UserMenuTree", false);
			QString UMTval = UMT.readEntry(QString("67dc1191-2928-4938-abce-1a73cb5ce490"), QString("Items"), "");
			valor2 = QString("%1;%2").arg(UMTval).arg(AppID);
			UMT.writeEntry(QString("67dc1191-2928-4938-abce-1a73cb5ce490"), QString("Items"), valor2);

		}

		//system(QString("mkdir %1/Installed").arg(ProgDir));
		//if ( Lugar != "Telefono" )
		//{ system(QString("mv -f /mmc/mmca1/.system/QTDownLoad/*/*.desktop %1/Installed").arg(ProgDir)); }
		//else
		//{ system(QString("mv -f /ezxlocal/download/mystuff/.system/QTDownLoad/*/*.desktop %1/Installed").arg(ProgDir)); }

		if ( Lugar != "Telefono" )
		{ system(QString("chmod 755 /mmc/mmca1/.system/QTDownLoad/*/*").arg(ProgDir)); }
		else
		{ system(QString("chmod 755 /ezxlocal/download/mystuff/.system/QTDownLoad/*/*").arg(ProgDir)); }

		delete msg0;
		msg0 = NULL;

		QString val; ZConfig ini(Idioma, false);
		val = ini.readEntry(QString("Z6MANAGER"), QString("INSTALL_SUCCESS"), "");

		system(QString("rm %1/tempdesktop").arg(ProgDir));
		system(QString("rm -r /mmc/mmca1/.system/tempmpkg"));
		LimpiarCR();
		ZMessageDlg* msg =  new ZMessageDlg ( QString(APPTITLE),QString("%1 %2").arg(valor).arg(val),
		ZMessageDlg::just_ok, 0, NULL, "ZMessageDlg", true, 0 );
		msg->exec(); delete msg; msg = NULL;

	}

}

void Z6Man::AMupdate( const QString &AppId, const QString &DestMenuId, int pos, int lugar )
{

	QByteArray data; QDataStream stream( data, IO_WriteOnly );
	stream << lugar;          // 1 - InstalledDB; 2 - CardRegistry
	stream << AppId;
	stream << DestMenuId;
	stream << -1;
	QCopChannel::send("AM/RegistryiMgr/Channel", "InstallApp(int, QString, QString, int)", data);

	QByteArray data2;
	QDataStream stream2( data2, IO_WriteOnly );
	stream2 << AppId;
	stream2 << DestMenuId;
	stream2 << -1;
	QCopChannel::send("AM/RegistryiMgr/Channel", "AddAppToFolder(QString, QString, int)", data2);

}

void Z6Man::AMupdate2( const QString &AppId, const QString &DestMenuId )
{
	QByteArray data;
	QDataStream stream( data, IO_WriteOnly );
	stream << AppId;
	stream << DestMenuId;
	QCopChannel::send( "AM/RegistryiMgr/Channel", "UninstallApp(QString)", data );

}

void Z6Man::BuscarMPKG()
{
  ZConfig ini(Idioma, false);
  QString val1 = ini.readEntry(QString("Z6MANAGER"), QString("INSTALL_SEARCH"), "");
  QString val2 = ini.readEntry(QString("Z6MANAGER"), QString("INSTALL_WAIT"), "");

  ZMessageDlg* msg =  new ZMessageDlg ( QString(APPTITLE), QString("%1\n%2").arg(val1).arg(val2),
	ZMessageDlg::NONE, 200, NULL, "ZMessageDlg", true, 0 );
  msg->exec();

  ZSingleSelectDlg* eq1 =  new ZSingleSelectDlg(QString(APPTITLE), QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
  system(QString("find /ezxlocal/download/mystuff -name \"*.mpkg\" > %1/mpkgtemp").arg(ProgDir));	
  system(QString("find /mmc/mmca1 -name \"*.mpkg\" >> %1/mpkgtemp").arg(ProgDir));	

  QString card = QString("%1/mpkgtemp").arg(ProgDir);
  QString line="";
  int cantl=0;  
  QStringList list;
  QFile entrada(card);
  QTextStream stentrada(&entrada);
  cantl=0;
  if ( entrada.open(IO_ReadOnly | IO_Translate) )
  {
	while ( !stentrada.eof() )
    {           
          line = stentrada.readLine();
          cantl++;
		  list +=getFileName(line);
    }
  }
  entrada.close();

  delete msg;
  msg = NULL;

  if ( cantl == 0 )
  {
	ZConfig IDini(Idioma, false);
	QString IDval = IDini.readEntry(QString("Z6MANAGER"), QString("INSTALL_NOTHING"), "");
	znd = new ZNoticeDlg(ZNoticeDlg::Information, 3000, QString(""), IDval, this, "z", true, 0);
	znd->show(); znd = NULL;
  }
  else
  {
	  eq1->addItemsList(list);
	  ZListBox* listboxx = eq1->getListBox();
	  int checked = -1;
	  if (checked >= 0)
	 	listboxx->checkItem(checked, true);
		eq1->exec();
		int result = eq1->result();
		if (result != 1) {
			result = 0;
		} else {
			result = eq1->getCheckedItemIndex();

			QString card2 = QString("%1/mpkgtemp").arg(ProgDir);
			QString line2="";
			int cantl2=0;  
			QStringList list2;
			QFile entrada2(card2);
			QTextStream stentrada2(&entrada2);
			cantl2=0;
			if ( entrada2.open(IO_ReadOnly | IO_Translate) )
			{
			while ( !stentrada2.eof() )
			{           
			  line2 = stentrada2.readLine();
			  if ( result == cantl2 )
			  {
	 			Instalar(line2);
				while (appListBox->count() != 0) { appListBox->removeItem(0); } CargarListaApps();
			  }
			  cantl2++;
			}
			entrada.close();		
		}
	  }
	  system(QString("rm %1/mpkgtemp").arg(ProgDir));	
  }

}

void Z6Man::EntradaAutorun(QString entrada, int ubicacion, QString active)
{
	QString entry = entrada;
    ZConfig IDini(Idioma, false);
	ZSingleSelectDlg* eq1 =  new ZSingleSelectDlg(entrada, QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list;
	QString IDval = IDini.readEntry(QString("Z6MANAGER"), QString("OPTIONS_ENABLED"), "");
	list = IDval; 
	IDval = IDini.readEntry(QString("Z6MANAGER"), QString("OPTIONS_DISABLED"), "");
	list += IDval;
	eq1->addItemsList(list);
	ZListBox* listboxx = eq1->getListBox();
	QString active2 = active;
	if ( active2 != "SI" ) { listboxx->checkItem(1, true); }
	else { listboxx->checkItem(0, true); }
	eq1->exec();
	int result = eq1->result();
	if (result != 1) {
		result = 0;
	}
	else
	{
	  QString autorun = QString("/ezxlocal/download/mystuff/.autorun.csh");
	  QString line=""; QString archivo1;
	  QFile entrada(autorun);
	  QTextStream stentrada(&entrada);
	  if ( entrada.open(IO_ReadOnly | IO_Translate) )
	  {
		while ( !stentrada.eof() )
		{           
		  line = stentrada.readLine(); archivo1 += QString("%1\n").arg(line);
		  QString aux = line; QString line2;
		  int tam=aux.length(); int j=0;
		  for(int i=1;i<tam;i++) { line2[j]=aux[i]; j++; }

		  if ( line2 == entry )
		  {
		    line = stentrada.readLine();
			if ( line[0] == QChar('#') )
			{
			  if (listboxx->itemChecked(0))
			  {
		  		QString aux = line; QString line2;
				int tam=aux.length(); int j=0;
				for(int i=1;i<tam;i++) { line2[j]=aux[i]; j++; }
				archivo1 += QString("%1\n").arg(line2);
			  }
			  if (listboxx->itemChecked(1))
			  {
				archivo1 += QString("%1\n").arg(line);
			  }
			}			
			else
			{
			  if (listboxx->itemChecked(0))
			  {
				archivo1 += QString("%1\n").arg(line);
			  }
			  if (listboxx->itemChecked(1))
			  {
				archivo1 += QChar('#');
				archivo1 += QString("%1\n").arg(line);
			  }
			}			
		  }
		  else { line = stentrada.readLine(); archivo1 += QString("%1\n").arg(line); }
		}
	  }
	  system("rm /ezxlocal/download/mystuff/.autorun.csh");
	  system("touch /ezxlocal/download/mystuff/.autorun.csh");
	  system(QString("echo \"%1\" >> /ezxlocal/download/mystuff/.autorun.csh").arg(archivo1));

	  while (runListBox->count() != 0) { runListBox->removeItem(0); } CargarListaAutorun();
	  runListBox->setCurrentItem(ubicacion,true);

	}

}

void Z6Man::AutorunAdd()
{

  QString archivo = "";
  ZFileOpenDialog *zopen  = new ZFileOpenDialog();
  zopen->exec();
  if ( zopen->result() == 1 )
  {
    archivo = zopen->getFileName();
    if (archivo != "")
	{
	  ZConfig IDini(Idioma, false);
	  QString IDval = IDini.readEntry(QString("Z6MANAGER"), QString("AUTORUN_NAME"), "");
	  ZSingleCaptureDlg* zscd = new ZSingleCaptureDlg(QString(APPTITLE), 
			IDval, ZSingleCaptureDlg::TypeLineEdit, NULL, "ZSingleCaptureDlg", true, 0, 0);
	  ZLineEdit* zle = (ZLineEdit*)zscd->getLineEdit(); 
	  zle->setText(getFileName(archivo));
	  zle->setEdited(true);  
	  if (zscd->exec() == 1)
	  {
		if ( zle->text() != "" )
		{
		  QString autorun = QString("/ezxlocal/download/mystuff/.autorun.csh");
		  QString line=""; QString archivo1;
		  QFile entrada(autorun);
		  QTextStream stentrada(&entrada);
		  if ( entrada.open(IO_ReadOnly | IO_Translate) )
		  {
			while ( !stentrada.eof() )
			{
			  line = stentrada.readLine();
			  if ( line != "" )
			  {
				archivo1 += QString("%1\n").arg(line);
			  }
			}
			archivo1 += QChar('#');
			archivo1 += QString("%1\n").arg(zle->text());
			archivo1 += QString("%1 &\n").arg(archivo);
		  }
		  system("rm /ezxlocal/download/mystuff/.autorun.csh");
		  system("touch /ezxlocal/download/mystuff/.autorun.csh");
		  system(QString("echo \"%1\" >> /ezxlocal/download/mystuff/.autorun.csh").arg(archivo1));
		  while (runListBox->count() != 0) { runListBox->removeItem(0); } CargarListaAutorun();
		}
	  }
	}
  }
  delete zopen;
  zopen = NULL;

}

void Z6Man::AutorunRemove()
{
  menu->close();
  qApp->processEvents();
  int index = runListBox->currentItem();
  if (index == -1) {return;}
  ZListBoxItem* listitem = runListBox->item ( index );
  QString app = listitem->getSubItemText ( 1, 0, true );

  ZConfig IDini(Idioma, false);
  QString IDval1 = IDini.readEntry(QString("Z6MANAGER"), QString("AUTORUN_CRON"), "");
  QString IDval2 = IDini.readEntry(QString("Z6MANAGER"), QString("AUTORUN_INETD"), "");
  QString IDval3 = IDini.readEntry(QString("Z6MANAGER"), QString("AUTORUN_SWAP"), "");
  QString IDval4 = IDini.readEntry(QString("Z6MANAGER"), QString("AUTORUN_FONT"), "");
  QString Autorize = "YES";
  if ( app == IDval1 ) { Autorize = "NO"; }
  if ( app == IDval2 ) { Autorize = "NO"; }
  if ( app == IDval3 ) { Autorize = "NO"; }
  if ( app == IDval4 ) { Autorize = "NO"; }
  if ( Autorize == "YES" )
  {
	  QString val; ZConfig ini1(Idioma, false);
	  val = ini1.readEntry(QString("Z6MANAGER"), QString("AUTORUN_DELETE"), "");
	  ZMessageDlg* msg =  new ZMessageDlg ( QString(APPTITLE),QString("%1 %2?").arg(val).arg(app),
		ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
	  int i = msg->exec(); delete msg; msg = NULL;
	  if (i)
	  {
		  QString autorun = QString("/ezxlocal/download/mystuff/.autorun.csh");
		  QString line=""; QString archivo1;
		  QFile entrada(autorun);
		  QTextStream stentrada(&entrada);
		  if ( entrada.open(IO_ReadOnly | IO_Translate) )
		  {
			while ( !stentrada.eof() )
			{
			  line = stentrada.readLine();
			  QString aux = line; QString line2; int tam=aux.length(); int j=0;
			  for(int i=1;i<tam;i++) { line2[j]=aux[i]; j++; }
			  if ( line2 == app )
			  {
				line = stentrada.readLine();
			  }
			  else { archivo1 += QString("%1\n").arg(line); }
			}
		  }
		  system("rm /ezxlocal/download/mystuff/.autorun.csh");
		  system("touch /ezxlocal/download/mystuff/.autorun.csh");
		  system(QString("echo \"%1\" >> /ezxlocal/download/mystuff/.autorun.csh").arg(archivo1));
		  while (runListBox->count() != 0) { runListBox->removeItem(0); } CargarListaAutorun();
	  }
  }
  else
  {
	ZConfig Eini(Idioma, false);
	QString Eval = Eini.readEntry(QString("Z6MANAGER"), QString("AUTORUN_LOCKED"), "");
	znd = new ZNoticeDlg(ZNoticeDlg::Information, 3000, QString(""), Eval, this, "z", true, 0);
	znd->show(); znd = NULL;
  }

}

void Z6Man::runZNothief()
{

	unsigned int res;
	TAPI_IMEI_NUMBER_A imsi;
	TAPI_CLIENT_Init(NULL, 0);
	res = TAPI_ACCE_GetImsi(imsi);
	QString pImsi;
	if ( res == 0)
	{
		pImsi = QString::fromLatin1( reinterpret_cast<char*>( imsi ) );
		qDebug(QString("\nIMEI = %1").arg(pImsi));
	}
	else { qDebug(QString("*** FAIL: error %1").arg(res)); }
	TAPI_CLIENT_Fini();


	ZConfig Zini("/ezxlocal/download/appwrite/setup/ezx_znothief.cfg", false);
	QString Zval1 = Zini.readEntry(QString("ZNOTHIEF"), QString("IMSI"), "");
	QString Zval2 = Zini.readEntry(QString("ZNOTHIEF"), QString("NUMBERS"), "");
	system(QString("echo \"\nIMSI = %1\nNUMBERS = %2\n\"").arg(Zval1).arg(Zval2));

	if ( Zval1 == pImsi )
	{
		system("echo \"\nIMSI comparation Ok!\n\"");
	}
	else
	{
		system("echo \"\nStolen phone!!! Sending messages...\n\"");

		ZConfig IDini(Idioma, false);

		//QString IDval = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_BLOCKED"), "");
		//ZMessageDlg* msg0 =  new ZMessageDlg ( QString(APPTITLE), QString("%1").arg(IDval), ZMessageDlg::NONE, 200, NULL, "ZMessageDlg", true, 0 );
		//msg0->exec();

		//system("busybox killall windowsserver");

		QString IDval = IDini.readEntry(QString("Z6MANAGER"), QString("NOTHIEF_MESSAGE"), "");
		QString mensaje= QString("%1 %2").arg(IDval).arg(pImsi);

		QStringList list = Zini.readListEntry(QString("ZNOTHIEF"), QString("NUMBERS"), QChar(' ') );
		for (int j=0;j<list.count();++j)
		{
			SendSMS(list[j], mensaje);
		}

		//delete msg0;
		//msg0 = NULL;

	}

}


void Z6Man::SendSMS(QString number, QString message)
{

	INT32 sms_fd ;
	TAPI_APP_MSGID_T msg_id[] = { TAPI_SMS_NOTIFY_SEND_CNF_MSGID } ;
	TAPI_RESULT_E res ;
	TAPI_SMS_MO_SM_S sms_mo ;
	UINT8 ref_num ;
	char TARGET_TXT[160]="";
	char TARGET_NUM[TAPI_MAX_PHONE_NUMBER_LENGTH]="";
	char SERV[TAPI_MAX_PHONE_NUMBER_LENGTH]="";
	TAPI_PHONE_NUMBER_A servicio;

	strcpy(TARGET_NUM,number);
	strcpy(TARGET_TXT, message);

	sms_fd = TAPI_CLIENT_Init( msg_id, sizeof(msg_id)/sizeof(TAPI_APP_MSGID_T) ) ;
	if((sms_fd == TAPI_INVALID_SOCKETFD) || (sms_fd == 0))
	{
		cout<<"failed to create sms fd..."<<endl ;
		return;
	}

	memset(&sms_mo, 0, sizeof(sms_mo));
	sms_mo.expirePeriod = TAPI_SMS_EXPIRE_PERIOD_DAY_EV;
	sms_mo.isNeedAck = FALSE ;
	sms_mo.msgRefNumber = 0 ;
	sms_mo.smBase.isReplyPathValid = TRUE;
	sms_mo.smBase.protocol = TAPI_SMS_PROTOCOL_TEXT_EV;
	sms_mo.smBase.encodeType = TAPI_SMS_TYPE_ASCII_TEXT_EV ;

	res = TAPI_SMS_GetServiceCenter(servicio);
	if ( res == TAPI_RESULT_SUCC ) cout<<"Si lo tenemos!."<<endl;
	else cout<<"No lo tenemos."<<endl ;
	cout<<servicio;

	strcpy(SERV,(char*)servicio);
	cout<<SERV;

	strcpy((char*)sms_mo.smBase.scAddress, SERV);
	strcpy((char*)sms_mo.smBase.peerAddress, TARGET_NUM);

	if(sms_mo.smBase.encodeType == TAPI_SMS_TYPE_ASCII_TEXT_EV)
	{
		strcpy((char *)(sms_mo.smBase.asciiContent.msgBody), TARGET_TXT) ;
		sms_mo.smBase.asciiContent.msgLength = strlen(TARGET_TXT) ;
	}

	res = TAPI_SMS_SendSmBlockMode( &sms_mo, &ref_num ); 

	if ( res == TAPI_RESULT_SUCC ) cout<<"sms enviado con exito"<<endl ;
	else cout<<"fallo el envio del sms"<<endl ;

}

void Z6Man::BorrarTema()
{
  menu->close();
  qApp->processEvents();
  int index = skinListBox->currentItem();
  if (index == -1) {return;}
  ZListBoxItem* listitem = skinListBox->item ( index );
  QString Cskin = listitem->getSubItemText ( 1, 0, true );

  if ( (Cskin=="Alkali") || (Cskin=="Indium") || (Cskin=="Plush") )
  {
	ZConfig Eini(Idioma, false);
	QString Eval = Eini.readEntry(QString("Z6MANAGER"), QString("THEME_LOCKED"), "");
	znd = new ZNoticeDlg(ZNoticeDlg::Information, 3000, QString(""), Eval, this, "z", true, 0);
	znd->show(); znd = NULL;
  }
  else
  {
	
	system(QString("find /usr/data_resource/theme/ /ezxlocal/download/appwrite/setuptheme -name \"*.*hm\" > %1/skintemp").arg(ProgDir));
	QFile entrada(QString("%1/skintemp").arg(ProgDir)); QTextStream stentrada(&entrada); QString Skin;
	if ( entrada.open(IO_ReadOnly | IO_Translate) )
	{
	  while ( !stentrada.eof() )
	  {           
		QString Skin0 = stentrada.readLine();
		ZConfig IDini(Skin0, false);
		QString IDval = IDini.readEntry(QString("THEME_CONFIG_TABLE"), QString("ThemeName"), "");
		if ( IDval == Cskin ) { Skin = Skin0; }
	  }
	  entrada.close();
	}
	system(QString("rm %1/skintemp").arg(ProgDir));
	ZConfig CHini("/ezxlocal/download/appwrite/setup/ezx_theme.cfg", false);
	QString CHval = CHini.readEntry(QString("THEME_GROUP_LOG"), QString("CurrentSkin"), "");
	ZConfig Fini(Skin, false);
	QString Fval = Fini.readEntry(QString("THEME_CONFIG_TABLE"), QString("Skin"), "");
	if ( Fval == CHval )
	{
	  ZConfig Eini(Idioma, false);
	  QString Eval = Eini.readEntry(QString("Z6MANAGER"), QString("THEME_CURRENT"), "");
	  znd = new ZNoticeDlg(ZNoticeDlg::Information, 3000, QString(""), Eval, this, "z", true, 0);
	  znd->show(); znd = NULL;
	}
	else
	{ 
	  ZConfig Eini(Idioma, false);
	  QString Eval = Eini.readEntry(QString("Z6MANAGER"), QString("UNINSTALL_MSG"), "");
	  ZMessageDlg* msg =  new ZMessageDlg ( QString(APPTITLE), QString("%1 %2?").arg(Eval).arg(Cskin),
		ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
	  int i = msg->exec();
	  delete msg;
	  msg = NULL;
	  if (i)
	  {
		  ZConfig Rini(Skin, false);
		  QString Rval = Rini.readEntry(QString("THEME_CONFIG_TABLE"), QString("Skin"), "");

		  system(QString("rm -r \"%1\"").arg(Rval));
		  QString Mval = getFileName(Rval);
		  system(QString("rm -r \"/mmc/mmca1/.system/skins/%1\"").arg(Mval));
		  system(QString("rm \"%1\"").arg(Skin));

		  Eval = Eini.readEntry(QString("Z6MANAGER"), QString("UNINSTALL_SUCCESS"), "");
		  znd = new ZNoticeDlg(ZNoticeDlg::Information, 8000, QString(""), QString("%1 %2").arg(Cskin).arg(Eval) , this, "z", true, 0);
		  znd->show(); 
		  znd = NULL;

		  while (skinListBox->count() != 0) { skinListBox->removeItem(0); } CargarListaSkins();

	  }
	}
  }

}

void Z6Man::JavaAdd()
{

	QString app = javaListBox->item ( javaListBox->currentItem() )->getSubItemText ( 1, 0, true );
	int jindex = javaListBox->currentItem();

	QString name, type, dir, ico, appid; QPixmap p1;
	QStringList grouplist;
	system("cat /ezxlocal/download/appwrite/am/InstalledDB /mmc/mmca1/.system/java/CardRegistry > /tmp/javalisttemp");
	ZConfig confJAVA ( "/tmp/javalisttemp", false );
	confJAVA.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it )
	{
	  name = confJAVA.readEntry ( *it, "Name", "" );
	  type = confJAVA.readEntry ( *it, "JavaId", "" );
	  dir = confJAVA.readEntry ( *it, "Directory", "" );
	  ico = confJAVA.readEntry ( *it, "BigIcon", "" );
	  appid = *it;
	  int i = dir.findRev ( "../../" ); dir.remove ( i, 5 );
	  if ( (appid!="") && (name==app) && (type!="") )
	  {
		QFile entrada(dir + "/registry.txt"); QTextStream stentrada(&entrada); QString INFO;
		if ( entrada.open(IO_ReadOnly | IO_Translate) )
		{
		  while ( !stentrada.eof() )
		  {
			QString path;
			QString line = stentrada.readLine(); int i = line.find("Jar-Path:",0);
			if ( i==0 )
			{
			  path = line; path.remove(0,10);
			  int k = path.findRev(".jar"); path.remove(k,4);
			  system(QString("echo \"#!/bin/sh\nrm -f /tmp/.javarunningid\nJAVAID=%1\nexport LD_PRELOAD=%2/jaidaid.so\nexec /lib/ld-linux.so.3 /usr/SYSjava/kvm -launch %3\" > %4/java.sh").arg(type).arg(ProgDir).arg(type).arg(dir));
			  system(QString("cp %1/%2 %3/Files/jaidaid_usr.png").arg(dir).arg(ico).arg(dir));
			  system(QString("cp %1/java.jad %2.jad").arg(ProgDir).arg(path));
			  system(QString("cp %1/java.ii %2.ii").arg(ProgDir).arg(path));
			  system(QString("cp %1/java.ss %2.ss").arg(ProgDir).arg(path));
			  system(QString("cp %1/java.rh %2.rh").arg(ProgDir).arg(path));
			  INFO += QString("%1\n").arg(line);
			  INFO += QString("Jad-Path: %1.jad\n").arg(path);
			}
			else
			{
			  int j1 = line.find("Jad-Path:",0);
			  if ( j1==0 )
			  {
				//INFO += QString("Jad-Path: %1.jad\n").arg(path);
			  } 
			  else
			  {
				int j2 = line.find("Is-Trusted:",0);
				if ( j2==0 )
				{
				  INFO += "Is-Trusted: 1\n"; 
				}
				else
				{ 
				  int j3 = line.find("Domain:",0);
				  if ( j3==0 )
				  {
					INFO += "Domain: Manufacturer\n"; 
				  }
				  else
				  { 
					int j4 = line.find("Signer-Name:",0);
					if ( j4==0 ) { 
					  INFO += QString("Signer-Name: C=US;L=Libertyville;O=Motorola Inc;OU=Mobile Devices;CN=Manufacturer Signing Key 186-1-1;SN=08 3a\n"); 
					}
					else
					{
					  INFO += QString("%1\n").arg(line);
					}
				  }
				}
			  }
			}
		  }
		  entrada.close();
		  if ( INFO != "NO" ) {
			system(QString("echo \"%1\" > %2/registry.txt").arg(INFO).arg(dir)); 
			int i3 = dir.findRev("mmc/mmca1/"); QString registro;
			if ( i3>0 ) { registro="/mmc/mmca1/.system/java/CardRegistry"; }
			else  { registro="/ezxlocal/download/appwrite/am/InstalledDB"; }
			ZConfig conf ( registro, true );
			conf.writeEntry( appid, "Exec", "java.sh" );
			conf.writeEntry( appid, "Attribute", "1" );
			conf.writeEntry( appid, "OldIcon", QString(ico) );
			conf.removeEntry( appid, "Icon" );
			QString temp1 = conf.readEntry( appid, "Directory", "" );
			int i = temp1.findRev ( "../../" ); temp1.remove ( i, 5 );
			conf.writeEntry( appid, "Directory", QString("../.."+temp1) );
			temp1 = conf.readEntry( appid, "JavaId", "" );
			if ( temp1!="" ) { 
			  conf.writeEntry( appid, "BigIcon", QString(dir+"/Files/jaidaid_usr.png") );
			}
			conf.removeEntry( appid, "JavaId" );
			conf.writeEntry( appid, "JavaIdback", type );
		  }
		}
	  }
			
	}
	system("rm /tmp/javalisttemp");
	ZConfig Eini(Idioma, false);
	QString Eval = Eini.readEntry(QString("Z6MANAGER"), QString("JAVAADD_SUCCESS"), "");
 	ZMessageDlg* msg =  new ZMessageDlg ( QString::fromUtf8(APPTITLE), Eval, ZMessageDlg::just_ok, 0, NULL, "ZMessageDlg", true, 0 );
	msg->exec();
	while (javaListBox->count() != 0) { javaListBox->removeItem(0); } CargarListaJava(); 
	javaListBox->setCurrentItem(jindex,true); javaListBox->ensureCurrentVisible();
	delete msg; msg = NULL;


}

void Z6Man::JavaRemove()
{

	QString app = javaListBox->item ( javaListBox->currentItem() )->getSubItemText ( 1, 0, true );
	int jindex = javaListBox->currentItem();

	QString name, type, dir, ico, appid; QPixmap p1;
	QStringList grouplist;
	system("cat /ezxlocal/download/appwrite/am/InstalledDB /mmc/mmca1/.system/java/CardRegistry > /tmp/javalisttemp");
	ZConfig confJAVA ( "/tmp/javalisttemp", false );
	confJAVA.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it )
	{
	  name = confJAVA.readEntry ( *it, "Name", "" );
	  type = confJAVA.readEntry ( *it, "JavaIdback", "" );
	  dir = confJAVA.readEntry ( *it, "Directory", "" );
	  ico = confJAVA.readEntry ( *it, "Icon", "" );
	  if ( ico == "" ) { ico = confJAVA.readEntry ( *it, "OldIcon", "" ); }
	  appid = *it;
	  int i = dir.findRev ( "../../" ); dir.remove ( i, 5 );
	  if ( (appid!="") && (name==app) && (type!="") )
	  {
		system(QString("rm %1/java.sh").arg(dir));
		system(QString("rm %1/Files/jaidaid_usr.png").arg(dir));
		int i2 = dir.findRev("mmc/mmca1/"); QString registro;
		if ( i2>0 ) { registro="/mmc/mmca1/.system/java/CardRegistry"; }
		else  { registro="/ezxlocal/download/appwrite/am/InstalledDB"; }
		ZConfig conf ( registro, true );
		conf.writeEntry( appid, "Exec", "" );
		conf.writeEntry( appid, "Attribute", "5" );
		QString temp1 = conf.readEntry( appid, "Directory", "" );
		int i = temp1.findRev ( "../../" ); temp1.remove ( i, 5 );
		conf.writeEntry( appid, "Directory", temp1 );
		conf.writeEntry( appid, "JavaId", type );
		conf.writeEntry( appid, "BigIcon", ico );
		conf.writeEntry( appid, "Icon", ico );
		conf.removeEntry( appid, "JavaIdback" );
		conf.removeEntry( appid, "OldIcon" );

		QFile entrada(dir + "/registry.txt"); QTextStream stentrada(&entrada); QString INFO;
		if ( entrada.open(IO_ReadOnly | IO_Translate) )
		{
		  while ( !stentrada.eof() )
		  {
			QString line = stentrada.readLine(); 
			int i = line.find("Is-Trusted:",0);
			if ( i==0 )
			{
			  INFO += QString("Is-Trusted: 0\n");
			}
			else
			{
			  INFO += QString("%1\n").arg(line);
			}
		  }
		  entrada.close();
		  system(QString("echo \"%1\" > %2/registry.txt").arg(INFO).arg(dir)); 
		}
	  }
			
	}
	system("rm /tmp/javalisttemp");
	ZConfig Eini(Idioma, false);
	QString Eval = Eini.readEntry(QString("Z6MANAGER"), QString("JAVAREMOVE_SUCCESS"), "");
 	ZMessageDlg* msg =  new ZMessageDlg ( QString::fromUtf8(APPTITLE), Eval, ZMessageDlg::just_ok, 0, NULL, "ZMessageDlg", true, 0 );
	msg->exec();
	while (javaListBox->count() != 0) { javaListBox->removeItem(0); } CargarListaJava();
	javaListBox->setCurrentItem(jindex,true); javaListBox->ensureCurrentVisible();
	delete msg; msg = NULL;

}

void Z6Man::ChangeFont(QString valor)
{
    ZConfig IDini(Idioma, false);
	QString IDval = IDini.readEntry(QString("Z6MANAGER"), QString("FONT_MSG1"), "");
	ZSingleSelectDlg* eq1 =  new ZSingleSelectDlg(IDval, QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list;
	IDval = IDini.readEntry(QString("Z6MANAGER"), QString("FONT_MSG2"), "");
	list = IDval; 
	IDval = IDini.readEntry(QString("Z6MANAGER"), QString("FONT_MSG3"), "");
	list += IDval;
	eq1->addItemsList(list);
	ZListBox* listboxx = eq1->getListBox();
	IDval = IDini.readEntry(QString("Z6MANAGER"), QString("FONT_SYSTEM"), "");
	if ( IDval != valor) { listboxx->checkItem(1, true); }
	else { listboxx->checkItem(0, true); }
	eq1->exec();
	int result = eq1->result();
	if (result != 1) {
		result = 0;
	} else {
		if (listboxx->itemChecked(1))
		{
			SelectFont(valor);
		}
		if (listboxx->itemChecked(0))
		{
			ZConfig Xini(QString("%1/Z6Manager.cfg").arg(ProgDir), true);
			Xini.writeEntry(QString("Z6Manager"), QString("Font"), "NO");
			system("rm /ezxlocal/LinXtend/MYFONT.TTF");
		}
	}
	while (runListBox->count() != 0) { runListBox->removeItem(0); } CargarListaAutorun();
	runListBox->setCurrentItem(3,true);

}

void Z6Man::SelectFont(QString valor)
{
  ZConfig ini(Idioma, false);
  QString val1 = ini.readEntry(QString("Z6MANAGER"), QString("FONT_SEARCH"), "");
  QString val2 = ini.readEntry(QString("Z6MANAGER"), QString("INSTALL_WAIT"), "");

  ZMessageDlg* msg =  new ZMessageDlg ( QString(APPTITLE), QString("%1\n%2").arg(val1).arg(val2),
		ZMessageDlg::NONE, 200, NULL, "ZMessageDlg", true, 0 );
  msg->exec();

  val1 = ini.readEntry(QString("Z6MANAGER"), QString("FONT_MSG3"), "");
  ZSingleSelectDlg* eq1 =  new ZSingleSelectDlg(val1, QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
  system(QString("find /ezxlocal/download/mystuff/replace_files/font -name \"*.ttf\" > %1/fonttemp").arg(ProgDir));	
  system(QString("find /ezxlocal/download/mystuff/replace_files/font -name \"*.TTF\" >> %1/fonttemp").arg(ProgDir));	
  system(QString("find /mmc/mmca1/font -name \"*.ttf\" >> %1/fonttemp").arg(ProgDir));	
  system(QString("find /mmc/mmca1/font -name \"*.TTF\" >> %1/fonttemp").arg(ProgDir));	

  QString card = QString("%1/fonttemp").arg(ProgDir);
  QString line="";
  int cantl=0;  
  QStringList list;
  QFile entrada(card);
  QTextStream stentrada(&entrada);
  QString fval = valor;
  int seleccion=-1;
  if ( entrada.open(IO_ReadOnly | IO_Translate) )
  {
	while ( !stentrada.eof() )
    {           
          line = stentrada.readLine();
		  list +=getFileName(line);
		  if ( fval != getFileName(line) ) { } else { seleccion = cantl; }
		  cantl++;
    }
  }
  entrada.close();

  delete msg;
  msg = NULL;

  if ( cantl == 0 )
  {
	ZConfig IDini(Idioma, false);
	QString IDval = IDini.readEntry(QString("Z6MANAGER"), QString("FONT_NOTHING"), "");
	znd = new ZNoticeDlg(ZNoticeDlg::Information, 3000, QString(""), IDval, this, "z", true, 0);
	znd->show(); znd = NULL;
  }
  else
  {
	  eq1->addItemsList(list);
	  ZListBox* listboxf = eq1->getListBox();
	  listboxf->checkItem(seleccion, true);
		eq1->exec();
		int result = eq1->result();
		if (result != 1) {
			result = 0;
		} else {
		  result = eq1->getCheckedItemIndex();

		  QString card2 = QString("%1/fonttemp").arg(ProgDir);
		  QString line2="";
		  int cantl2=0;  
		  QStringList list2;
		  QFile entrada2(card2);
		  QTextStream stentrada2(&entrada2);
		  cantl2=0;
		  if ( entrada2.open(IO_ReadOnly | IO_Translate) )
		  {
			while ( !stentrada2.eof() )
			{           
			  line2 = stentrada2.readLine();
			  if ( result == cantl2 )
			  {
					ZConfig Xini(QString("%1/Z6Manager.cfg").arg(ProgDir), true);
					Xini.writeEntry(QString("Z6Manager"), QString("Font"), line2);
				 	system(QString("cp \"%1\" /ezxlocal/LinXtend/MYFONT.TTF").arg(line2));
				
			  }
			  cantl2++;
			}
			entrada2.close();		
		  }
	  }
	  system(QString("rm %1/fonttemp").arg(ProgDir));	
  }
  while (runListBox->count() != 3) { runListBox->removeItem(0); } CargarListaAutorun();
  runListBox->setCurrentItem(3,true);

}

void Z6Man::CambiarNombre()
{
	QString app; int item;
	int tabx = tabWidget->currentPageIndex();
	if ( tabx == 0 ) { 
		item = appListBox->currentItem();
		app = appListBox->item(appListBox->currentItem())->getSubItemText(1,0,true);
	} else {
		item = javaListBox->currentItem();
		app = javaListBox->item(javaListBox->currentItem())->getSubItemText(1,0,true);
	}
	ZConfig IDini( Idioma, false);
	QString IDval = IDini.readEntry(QString("Z6MANAGER"), QString("PERSONALIZE_NAME"), "");
	
	ZSingleCaptureDlg* zscd = new ZSingleCaptureDlg(IDval, "", ZSingleCaptureDlg::TypeLineEdit, NULL, "ZSingleCaptureDlg", true, 0, 0);
	ZLineEdit* zle = (ZLineEdit*)zscd->getLineEdit();  
	zle->setText(app);
	zle->setEdited(true);  
	if (zscd->exec() == 1) {
		QString valnew = zle->text();
		if ( (valnew!=app) && (valnew!="") )
		{ 
			QString name, appid;	
			ZConfig confCARD ( "/mmc/mmca1/.system/java/CardRegistry", true );
			QStringList grouplist;
			confCARD.getGroupsKeyList ( grouplist );
			for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
				name = confCARD.readEntry ( *it, "Name", "" ); appid = *it;
				if ( name==app ) { confCARD.writeEntry(appid, "Name", valnew);  }
			}	
			ZConfig confSYS ( "/ezxlocal/download/appwrite/am/UserRegistry", true );
			confSYS.getGroupsKeyList ( grouplist );
			for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
				name = confSYS.readEntry ( *it, "Name", "" ); appid = *it;
				if ( name==app ) { confSYS.writeEntry(appid, "Name", valnew); }
			}	
			ZConfig confDB ( "/ezxlocal/download/appwrite/am/InstalledDB", true );
			confDB.getGroupsKeyList ( grouplist );
			for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
				name = confDB.readEntry ( *it, "Name", "" ); appid = *it;
				if ( name==app ) { confDB.writeEntry(appid, "Name", valnew);  }
			}	
			int tabx = tabWidget->currentPageIndex();
			if ( tabx == 0 ) { CargarListaApps(); }
			else { CargarListaJava(); }
			
		}
	}


}

void Z6Man::CambiarIcono()
{
	QString currentItem; int curItem;
	int tabx = tabWidget->currentPageIndex();
	if ( tabx == 0 ) { 
		curItem = appListBox->currentItem();
		currentItem = appListBox->item(appListBox->currentItem())->getSubItemText(1,0,true);
	} else {
		curItem = javaListBox->currentItem();
		currentItem = javaListBox->item(javaListBox->currentItem())->getSubItemText(1,0,true);
	}

	ZConfig IDini(Idioma, false);
	QString	unic = IDini.readEntry(QString("SETTINGS"), QString("UNICODE"), "");
	QStringList list;
	int checked = -1;
	int timeout = 0;

	QString val = IDini.readEntry(QString("Z6MANAGER"), QString("ICON_FILE"), "");
	list += val;
	val = IDini.readEntry(QString("Z6MANAGER"), QString("ICON_ICONRES"), "");
	list += val;
	val = IDini.readEntry(QString("Z6MANAGER"), QString("ICON_PLACE"), "");
	ZSingleSelectDlg* dlg = new ZSingleSelectDlg("Z6 Manager", val, NULL, "ZSingleSelectDlg", true, 0, 0);
	dlg->addItemsList(list);
	ZListBox* listbox = dlg->getListBox();
	if (checked >= 0) listbox->checkItem(checked, true);
	if (timeout) dlg->setAutoDismissTime(timeout*1000);
	dlg->exec();
	int result = dlg->result();
	if (result != 1) {
		result = 0;
	} else {
	  result = dlg->getCheckedItemIndex();
	  if (listbox->itemChecked(0)) { 
		ZFileOpenDialog *zopen  = new ZFileOpenDialog();
		zopen->exec();
		if ( zopen->result() == 1 )
		{
		  QString archivo = zopen->getFileName();
		  if (archivo != "")
		  {
			QString name, appid, attr, folder, dir, ico;	
			ZConfig confCARD ( "/mmc/mmca1/.system/java/CardRegistry", true );
			QStringList grouplist;
			confCARD.getGroupsKeyList ( grouplist );
			for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
				name = confCARD.readEntry ( *it, "Name", "" ); appid = *it;
				attr = confCARD.readEntry ( *it, "Attribute", "" );
				dir = confCARD.readEntry ( *it, "Directory", "" );
				ico = confCARD.readEntry ( *it, "BigIcon", "" );
				if ( attr=="5" ) folder = getFilePath(dir + "/" + ico); else folder = getFilePath(ico);
				if ( name==currentItem ) {
					QPixmap p1; 
					if ( ! QFileInfo(folder + "/" + "iconbackup.png").isFile()) { 
						if ( attr=="5" ) { p1.load(dir + "/" + ico); } else { p1.load(ico); }
						p1.save(folder + "/" + "iconbackup.png", "PNG"); }
					p1.load(archivo);
					if ( attr=="5" ) { p1.save(dir + "/" + ico, "PNG"); } else { p1.save(ico, "PNG"); } 
				}
			}
			ZConfig confSYS ( "/ezxlocal/download/appwrite/am/UserRegistry", true );
			confSYS.getGroupsKeyList ( grouplist );
			for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
				name = confSYS.readEntry ( *it, "Name", "" ); appid = *it;
				attr = confSYS.readEntry ( *it, "Attribute", "" );
				dir = confSYS.readEntry ( *it, "Directory", "" );
				ico = confSYS.readEntry ( *it, "BigIcon", "" );
				if ( attr=="5" ) folder = getFilePath(dir + "/" + ico); else folder = getFilePath(ico);
				if ( name==currentItem ) {
					QPixmap p1; 
					if ( ! QFileInfo(folder + "/" + "iconbackup.png").isFile()) { 
						if ( attr=="5" ) { p1.load(dir + "/" + ico); } else { p1.load(ico); }
						p1.save(folder + "/" + "iconbackup.png", "PNG"); }
					p1.load(archivo); 
					if ( attr=="5" ) { p1.save(dir + "/" + ico, "PNG"); } else { p1.save(ico, "PNG"); } 
				}
			}
			ZConfig confDB ( "/ezxlocal/download/appwrite/am/InstalledDB", true );
			confDB.getGroupsKeyList ( grouplist );
			for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
				name = confDB.readEntry ( *it, "Name", "" ); appid = *it;
				attr = confDB.readEntry ( *it, "Attribute", "" );
				dir = confDB.readEntry ( *it, "Directory", "" );
				ico = confDB.readEntry ( *it, "BigIcon", "" );
				if ( attr=="5" ) folder = getFilePath(dir + "/" + ico); else folder = getFilePath(ico);
				if ( name==currentItem ) {
					QPixmap p1; 
					if ( ! QFileInfo(folder + "/" + "iconbackup.png").isFile()) { 
						if ( attr=="5" ) { p1.load(dir + "/" + ico); } else { p1.load(ico); }
						p1.save(folder + "/" + "iconbackup.png", "PNG"); }
					p1.load(archivo); 
					if ( attr=="5" ) { p1.save(dir + "/" + ico, "PNG"); } else { p1.save(ico, "PNG"); } 
				}
			}
			if ( tabx == 0 ) { CargarListaApps(); appListBox->setCurrentItem(curItem,true); appListBox->ensureCurrentVisible(); }
			else { CargarListaJava(); javaListBox->setCurrentItem(curItem,true); javaListBox->ensureCurrentVisible(); }
			
		  }
		}
	  }
	  if (listbox->itemChecked(1)) { 
		RES_ICON_Reader iconReader;
		ZIconSelect *zopen  = new ZIconSelect();
		zopen->exec();
		if ( zopen->result() == 1 )
		{
			QString archivo = zopen->getFilePath();
			QString name, appid, attr, folder, dir, ico;
			ZConfig confCARD ( "/mmc/mmca1/.system/java/CardRegistry", true );
			QStringList grouplist;
			confCARD.getGroupsKeyList ( grouplist );
			for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
				name = confCARD.readEntry ( *it, "Name", "" ); appid = *it;
				attr = confCARD.readEntry ( *it, "Attribute", "" );
				dir = confCARD.readEntry ( *it, "Directory", "" );
				ico = confCARD.readEntry ( *it, "BigIcon", "" );
				if ( attr=="5" ) folder = getFilePath(dir + "/" + ico); else folder = getFilePath(ico);
				if ( name==currentItem ) {
					QPixmap p1;
					if ( ! QFileInfo(folder + "/" + "iconbackup.png").isFile()) { 
						if ( attr=="5" ) { p1.load(dir + "/" + ico); } else { p1.load(ico); }
						p1.save(folder + "/" + "iconbackup.png", "PNG"); }
					p1 = QPixmap(iconReader.getIcon(archivo, false));
					if ( attr=="5" ) { p1.save(dir + "/" + ico, "PNG"); } else { p1.save(ico, "PNG"); } 
				}
			}			
			ZConfig confSYS ( "/ezxlocal/download/appwrite/am/UserRegistry", true );
			confSYS.getGroupsKeyList ( grouplist );
			for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
				name = confSYS.readEntry ( *it, "Name", "" ); appid = *it;
				attr = confSYS.readEntry ( *it, "Attribute", "" );
				dir = confSYS.readEntry ( *it, "Directory", "" );
				ico = confSYS.readEntry ( *it, "BigIcon", "" );
				if ( attr=="5" ) folder = getFilePath(dir + "/" + ico); else folder = getFilePath(ico);
				if ( name==currentItem ) {
					QPixmap p1;
					if ( ! QFileInfo(folder + "/" + "iconbackup.png").isFile()) { 
						if ( attr=="5" ) { p1.load(dir + "/" + ico); } else { p1.load(ico); }
						p1.save(folder + "/" + "iconbackup.png", "PNG"); }
					p1 = QPixmap(iconReader.getIcon(archivo, false));
					if ( attr=="5" ) { p1.save(dir + "/" + ico, "PNG"); } else { p1.save(ico, "PNG"); } 
				}
			}
			ZConfig confDB ( "/ezxlocal/download/appwrite/am/InstalledDB", true );
			confDB.getGroupsKeyList ( grouplist );
			for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
				name = confDB.readEntry ( *it, "Name", "" ); appid = *it;
				attr = confDB.readEntry ( *it, "Attribute", "" );
				dir = confDB.readEntry ( *it, "Directory", "" );
				ico = confDB.readEntry ( *it, "BigIcon", "" );
				if ( attr=="5" ) folder = getFilePath(dir + "/" + ico); else folder = getFilePath(ico);
				if ( name==currentItem ) {
					QPixmap p1;
					if ( ! QFileInfo(folder + "/" + "iconbackup.png").isFile()) { 
						if ( attr=="5" ) { p1.load(dir + "/" + ico); } else { p1.load(ico); }
						p1.save(folder + "/" + "iconbackup.png", "PNG"); }
					p1 = QPixmap(iconReader.getIcon(archivo, false));
					if ( attr=="5" ) { p1.save(dir + "/" + ico, "PNG"); } else { p1.save(ico, "PNG"); } 
				}
			}
			if ( tabx == 0 ) { CargarListaApps(); appListBox->setCurrentItem(curItem,true); appListBox->ensureCurrentVisible(); }
			else { CargarListaJava(); javaListBox->setCurrentItem(curItem,true); javaListBox->ensureCurrentVisible(); }
		}
	  }
	}
}


