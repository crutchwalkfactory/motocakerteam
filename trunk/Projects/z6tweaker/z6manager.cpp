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
#include <qfont.h>
#include <qfile.h>
#include <qfileinfo.h>
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
#include <qdatastream.h>
#include <qfontinfo.h>


#define APPTITLE "Z6 Tweaker"

using namespace std;

QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");

QString MENU_MENU, MENU_SELECT, MENU_CANCEL, MENU_EXIT, MENU_ABOUT, CONFIG_ALARM, CONFIG_BLUETOOTH, CONFIG_CONTACTS, CONFIG_EMPTYSMS, CONFIG_SLIDER, 
CONFIG_ORANGEBAR, CONFIG_EQUALIZER, CONFIG_JAVA, CONFIG_CAM, CONFIG_SLIDERTONES, CONFIG_SHORTCUT, CONFIG_BOOTLOGO, CONFIG_FONT, CONFIG_OVERCLOCK, CONFIG_WEBLINK, ALARM_MSG1, CONTACTS_MSG1, OPTIONS_ENABLED, OPTIONS_DISABLED, FONT_APPLY, FONT_CHANGING, JAVA_MOD1, JAVA_MOD2, JAVA_MOD3, JAVA_MOD4, JAVA_MOD5, JAVA_MOD6, CAM_MOD1, CAM_MOD2, CAM_MOD3, BOOTLOGO_ASK, BOOTLOGO_CHANGING, BOOTLOGO_DONE, BOOTLOGO_ERROR, OVERCLOCK_MOD1, OVERCLOCK_MOD2, OVERCLOCK_MOD3, OVERCLOCK_MOD4, OVERCLOCK_MOD5, CONFIG_VR, MENUICONS, BIGICONS, SMALLICONS, NOICONS, AMVIB, FONT_INCREASE, FONT_DECREASE, FONT_SUCCESS, FONT_SIZE, FUNLIGHTS, FUN_FLASH;



Z6Man::Z6Man ( const QString &argument, QWidget* parent, const char* name, WFlags fl )
    : ZKbMainWidget ( ZHeader::FULL_TYPE, NULL, "ZMainWidget", 0 )
{
  ProgDir = getProgramDir();
  
  QString val; 
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = "ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/en-us";

  CargarIdioma();

  QString argum = argument;
  QFile f(argum);
  if ( f.exists() ) {
	Changing(argum);
  } else {
	CreateWindow ( parent );
	system(QString("echo \"HOLA\""));
 	( ( ZApplication* ) qApp )->showMainWidget ( this );
  }

}

Z6Man::~Z6Man()
{
}


void Z6Man::CreateWindow ( QWidget* parent )
{
  setMainWidgetTitle ( APPTITLE );

  tabWidget = new ZNavTabWidget(ZNavTabWidget::SEPARATE, false, false, this);

  appListBox = new ZListBox ( QString ( "%I%M" ), this, 0);
  appListBox->setFixedWidth ( 240 );

  modListBox = new ZListBox ( QString ( "%I%M" ), this, 0);
  modListBox->setFixedWidth ( 240 );

  QPixmap p1;
  p1.load(ProgDir + "/images/themes.png");
  tabWidget->addTab(appListBox, QIconSet(p1), "");
  p1.load(ProgDir + "/images/settings.png");
  tabWidget->addTab(modListBox, QIconSet(p1), "");

  CargarListaApps();
  connect(appListBox, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(appClicked(ZListBoxItem *)));
  connect(modListBox, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(appClicked(ZListBoxItem *)));

  setContentWidget ( tabWidget );
  softKey = new ZSoftKey ( NULL , this , this );

  Create_MenuApp();
  
  Create_SoftKey();
  setSoftKey ( softKey );

  appListBox->setFocus();

}

void Z6Man::CargarIdioma()
{
ZConfig IDini(Idioma, false);
MENU_MENU = IDini.readEntry(QString("COMMON"), QString("MENU_MENU"), "");
MENU_SELECT = IDini.readEntry(QString("COMMON"), QString("MENU_SELECT"), "");
MENU_CANCEL = IDini.readEntry(QString("COMMON"), QString("MENU_CANCEL"), "");
MENU_EXIT = IDini.readEntry(QString("COMMON"), QString("MENU_EXIT"), "");
MENU_ABOUT = IDini.readEntry(QString("COMMON"), QString("MENU_ABOUT"), "");
CONFIG_ALARM = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_ALARM"), "");
CONFIG_BLUETOOTH = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_BLUETOOTH"), "");
CONFIG_CONTACTS = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_CONTACTS"), "");
CONFIG_EMPTYSMS = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_EMPTYSMS"), "");
CONFIG_SLIDER = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_SLIDER"), "");
CONFIG_ORANGEBAR = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_ORANGEBAR"), "");
CONFIG_EQUALIZER = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_EQUALIZER"), "");
CONFIG_JAVA = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_JAVA"), "");
CONFIG_CAM = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_CAM"), "");
CONFIG_SLIDERTONES = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_SLIDERTONES"), "");
CONFIG_SHORTCUT = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_SHORTCUT"), "");
CONFIG_VR = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_VR"), "");
CONFIG_BOOTLOGO = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_BOOTLOGO"), "");
CONFIG_FONT = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_FONT"), "");
CONFIG_OVERCLOCK = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_OVERCLOCK"), "");
CONFIG_WEBLINK = IDini.readEntry(QString("Z6TWEAKER"), QString("CONFIG_WEBLINK"), "");
ALARM_MSG1 = IDini.readEntry(QString("Z6TWEAKER"), QString("ALARM_MSG1"), "");
CONTACTS_MSG1 = IDini.readEntry(QString("Z6TWEAKER"), QString("CONTACTS_MSG1"), "");
OPTIONS_ENABLED = IDini.readEntry(QString("Z6TWEAKER"), QString("OPTIONS_ENABLED"), "");
OPTIONS_DISABLED = IDini.readEntry(QString("Z6TWEAKER"), QString("OPTIONS_DISABLED"), "");
FONT_APPLY = IDini.readEntry(QString("Z6TWEAKER"), QString("FONT_APPLY"), "");
FONT_CHANGING = IDini.readEntry(QString("Z6TWEAKER"), QString("FONT_CHANGING"), "");
JAVA_MOD1 = IDini.readEntry(QString("Z6TWEAKER"), QString("JAVA_MOD1"), ""); 
JAVA_MOD2 = IDini.readEntry(QString("Z6TWEAKER"), QString("JAVA_MOD2"), ""); 
JAVA_MOD3 = IDini.readEntry(QString("Z6TWEAKER"), QString("JAVA_MOD3"), ""); 
JAVA_MOD4 = IDini.readEntry(QString("Z6TWEAKER"), QString("JAVA_MOD4"), ""); 
JAVA_MOD5 = IDini.readEntry(QString("Z6TWEAKER"), QString("JAVA_MOD5"), ""); 
JAVA_MOD6 = IDini.readEntry(QString("Z6TWEAKER"), QString("JAVA_MOD6"), ""); 
CAM_MOD1 = IDini.readEntry(QString("Z6TWEAKER"), QString("CAM_MOD1"), "");
CAM_MOD2 = IDini.readEntry(QString("Z6TWEAKER"), QString("CAM_MOD2"), "");
CAM_MOD3 = IDini.readEntry(QString("Z6TWEAKER"), QString("CAM_MOD3"), "");
BOOTLOGO_ASK = IDini.readEntry(QString("Z6TWEAKER"), QString("BOOTLOGO_ASK"), "");
BOOTLOGO_CHANGING = IDini.readEntry(QString("Z6TWEAKER"), QString("BOOTLOGO_CHANGING"), "");
BOOTLOGO_DONE = IDini.readEntry(QString("Z6TWEAKER"), QString("BOOTLOGO_DONE"), "");
BOOTLOGO_ERROR = IDini.readEntry(QString("Z6TWEAKER"), QString("BOOTLOGO_ERROR"), "");
OVERCLOCK_MOD1 = IDini.readEntry(QString("Z6TWEAKER"), QString("OVERCLOCK_MOD1"), "");
OVERCLOCK_MOD2 = IDini.readEntry(QString("Z6TWEAKER"), QString("OVERCLOCK_MOD2"), "");
OVERCLOCK_MOD3 = IDini.readEntry(QString("Z6TWEAKER"), QString("OVERCLOCK_MOD3"), "");
OVERCLOCK_MOD4 = IDini.readEntry(QString("Z6TWEAKER"), QString("OVERCLOCK_MOD4"), "");
OVERCLOCK_MOD5 = IDini.readEntry(QString("Z6TWEAKER"), QString("OVERCLOCK_MOD5"), "");
MENUICONS = IDini.readEntry(QString("Z6TWEAKER"), QString("MENUICONS"), "");
BIGICONS = IDini.readEntry(QString("Z6TWEAKER"), QString("BIGICONS"), "");
SMALLICONS = IDini.readEntry(QString("Z6TWEAKER"), QString("SMALLICONS"), "");
NOICONS = IDini.readEntry(QString("Z6TWEAKER"), QString("NOICONS"), "");
AMVIB = IDini.readEntry(QString("Z6TWEAKER"), QString("AMVIB"), "");
FONT_SIZE = IDini.readEntry(QString("Z6TWEAKER"), QString("FONT_SIZE"), "");
FONT_INCREASE = IDini.readEntry(QString("Z6TWEAKER"), QString("FONT_INCREASE"), "");
FONT_DECREASE = IDini.readEntry(QString("Z6TWEAKER"), QString("FONT_DECREASE"), "");
FONT_SUCCESS = IDini.readEntry(QString("Z6TWEAKER"), QString("FONT_SUCCESS"), "");
FUNLIGHTS = IDini.readEntry(QString("Z6TWEAKER"), QString("FUNLIGHTS"), "");
FUN_FLASH = IDini.readEntry(QString("Z6TWEAKER"), QString("FUN_FLASH"), "");

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

void Z6Man::about()
{
  ZAboutDialog* aboutDlg = new ZAboutDialog();
  aboutDlg->exec();
  delete aboutDlg;
  aboutDlg = NULL;
}

void Z6Man::Create_SoftKey()
{
  QString IDval; QString IDval2; ZConfig IDini(Idioma, false);
  softKey->setOptMenu ( ZSoftKey::LEFT, menu ); 
  softKey->setText ( ZSoftKey::LEFT, MENU_MENU , ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setTextForOptMenuHide(MENU_MENU);
  softKey->setTextForOptMenuShow(MENU_SELECT, MENU_CANCEL);
  softKey->setText ( ZSoftKey::RIGHT, MENU_EXIT, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, qApp, SLOT ( quit() ) );      
}

void Z6Man::Create_MenuApp()
{
  QString IDval; ZConfig IDini(Idioma, false);
  QRect rect;
  QPixmap* pm  = new QPixmap();
  int idx = 0;
  menu = new ZOptionsMenu ( rect, softKey, NULL, 0);
  menu->setItemSpacing(10);  
  pm->load(ProgDir + "images/about.png");
  menu->insertItem ( MENU_ABOUT, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, this, SLOT ( about() ) );      
  ++idx;
  pm->load(ProgDir + "images/exit.png");
  menu->insertItem ( MENU_EXIT, NULL, pm, true, idx, idx );
  menu->connectItem ( idx, qApp, SLOT ( quit() ) );
}

bool Z6Man::getFlexBit(unsigned short flexId, bool& value )
{
  unsigned int index; 
  unsigned int remainder;
  unsigned int temp=1;
 
  index = flexId/32; 
  //remainder = 31-flexId%32;
  remainder = flexId%32;
  temp <<= remainder;
   
  QString keyNameItem;
  keyNameItem.sprintf("%d",index);
  ZConfig cfg("/ezxlocal/download/appwrite/setup/ezx_flexbit.cfg", true);
  int r = cfg.readNumEntry("SYS_FLEX_TABLE", keyNameItem, 0);
  if((r&temp)!=0)
  {
    value = true;
  }
  else {
    value = false;
  } 
  return true;
}

bool Z6Man::setFlexBit(unsigned short flexId, bool value)
{
  unsigned int index;
  unsigned int remainder;
  unsigned int temp=1; 
 
  index = flexId/32;
  //remainder = 31-flexId%32;
  remainder = flexId%32;
  temp<<=remainder; 
 
  QString keyNameItem;
  keyNameItem.sprintf("%d",index);
  ZConfig cfg("/ezxlocal/download/appwrite/setup/ezx_flexbit.cfg", true);
  int r = cfg.readNumEntry("SYS_FLEX_TABLE", keyNameItem, 0);
 
  if(value == true) r |= temp;
  else r &= ~temp;
 
  //system("mount -o remount,rw /usr/setup");
  r = cfg.writeEntry("SYS_FLEX_TABLE", keyNameItem, r);
  //system("mount -o remount,ro /usr/setup");
 
  return r;
}

void Z6Man::CargarListaApps()
{
  QPixmap p1; ZConfig IDini(Idioma, false);

  ZConfig Fini(ProgDir + "z6tweaker.cfg", false);
  QString Fval = Fini.readEntry(QString("OPTIONS"), QString("AlarmTime"), "YES");
  if ( Fval == "YES" ) {
	ZListBoxItem* listitem1 = new ZListBoxItem ( modListBox, QString ( "%I%M" ) );
	p1.load(ProgDir + "/images/alarm.png");
	listitem1->setPixmap ( 0, p1 );
	listitem1->appendSubItem ( 1, CONFIG_ALARM, false, NULL );
	modListBox->insertItem ( listitem1,-1,true );
  }
  Fval = Fini.readEntry(QString("OPTIONS"), QString("Bluetooth"), "YES");
  if ( Fval == "YES" ) {
	ZListBoxItem* listitem2 = new ZListBoxItem ( modListBox, QString ( "%I%M" ) );
	p1.load(ProgDir + "/images/bluetooth.png");
	listitem2->setPixmap ( 0, p1 );
	listitem2->appendSubItem ( 1, CONFIG_BLUETOOTH, false, NULL );
	modListBox->insertItem ( listitem2,-1,true );
  }
  Fval = Fini.readEntry(QString("OPTIONS"), QString("ContactsID"), "YES");
  if ( Fval == "YES" ) {
	ZListBoxItem* listitem3 = new ZListBoxItem ( modListBox, QString ( "%I%M" ) );
	p1.load(ProgDir + "/images/contacts.png");
	listitem3->setPixmap ( 0, p1 );
	listitem3->appendSubItem ( 1, CONFIG_CONTACTS, false, NULL );
	modListBox->insertItem ( listitem3,-1,true );
  }
  Fval = Fini.readEntry(QString("OPTIONS"), QString("EmptySMS"), "YES");
  if ( Fval == "YES" ) {
	ZListBoxItem* listitem4 = new ZListBoxItem ( modListBox, QString ( "%I%M" ) );
	p1.load(ProgDir + "/images/sms.png");
	listitem4->setPixmap ( 0, p1 );
	listitem4->appendSubItem ( 1, CONFIG_EMPTYSMS, false, NULL );
	modListBox->insertItem ( listitem4,-1,true );
  }
  Fval = Fini.readEntry(QString("OPTIONS"), QString("SliderCalls"), "YES");
  if ( Fval == "YES" ) {
	ZListBoxItem* listitem5 = new ZListBoxItem ( modListBox, QString ( "%I%M" ) );
	p1.load(ProgDir + "/images/calls.png");
	listitem5->setPixmap ( 0, p1 );
	listitem5->appendSubItem ( 1, CONFIG_SLIDER, false, NULL );
	modListBox->insertItem ( listitem5,-1,true );
  }
  Fval = Fini.readEntry(QString("OPTIONS"), QString("OrangeBar"), "YES");
  if ( Fval == "YES" ) {
	ZListBoxItem* listitem7 = new ZListBoxItem ( appListBox, QString ( "%I%M" ) );
	p1.load(ProgDir + "/images/sidebar.png");
	listitem7->setPixmap ( 0, p1 );
	listitem7->appendSubItem ( 1, CONFIG_ORANGEBAR, false, NULL );
	appListBox->insertItem ( listitem7,-1,true );
  }
  Fval = Fini.readEntry(QString("OPTIONS"), QString("Equalizer"), "YES");
  if ( Fval == "YES" ) {
	ZListBoxItem* listitem6 = new ZListBoxItem ( modListBox, QString ( "%I%M" ) );
	p1.load(ProgDir + "/images/music.png");
	listitem6->setPixmap ( 0, p1 );
	listitem6->appendSubItem ( 1, CONFIG_EQUALIZER, false, NULL );
	modListBox->insertItem ( listitem6,-1,true );
  }
  Fval = Fini.readEntry(QString("OPTIONS"), QString("JavaHeapSize"), "YES");
  if ( Fval == "YES" ) {
	ZListBoxItem* listitem8 = new ZListBoxItem ( modListBox, QString ( "%I%M" ) );
	p1.load(ProgDir + "/images/java.png");
	listitem8->setPixmap ( 0, p1 );
	listitem8->appendSubItem ( 1, CONFIG_JAVA, false, NULL );
	modListBox->insertItem ( listitem8,-1,true );
  }
  Fval = Fini.readEntry(QString("OPTIONS"), QString("Funlights"), "YES");
  if ( Fval == "YES" ) {
	ZListBoxItem* listitem8b = new ZListBoxItem ( modListBox, QString ( "%I%M" ) );
	p1.load(ProgDir + "/images/bulb.png");
	listitem8b->setPixmap ( 0, p1 );
	listitem8b->appendSubItem ( 1, FUNLIGHTS, false, NULL );
	modListBox->insertItem ( listitem8b,-1,true );
  }
  Fval = Fini.readEntry(QString("OPTIONS"), QString("VidCameraMod"), "YES");
  if ( Fval == "YES" ) {
	ZListBoxItem* listitem10 = new ZListBoxItem ( appListBox, QString ( "%I%M" ) );
	p1.load(ProgDir + "/images/vidcam.png");
	listitem10->setPixmap ( 0, p1 );
	listitem10->appendSubItem ( 1, CONFIG_CAM, false, NULL );
	appListBox->insertItem ( listitem10,-1,true );
  }
  Fval = Fini.readEntry(QString("OPTIONS"), QString("SliderTones"), "YES");
  if ( Fval == "YES" ) {
	ZListBoxItem* listitem13 = new ZListBoxItem ( appListBox, QString ( "%I%M" ) );
	p1.load(ProgDir + "/images/tones.png");
	listitem13->setPixmap ( 0, p1 );
	listitem13->appendSubItem ( 1, CONFIG_SLIDERTONES, false, NULL );
	appListBox->insertItem ( listitem13,-1,true );
  }
  Fval = Fini.readEntry(QString("OPTIONS"), QString("SmartButton"), "YES");
  if ( Fval == "YES" ) {
	ZListBoxItem* listitem11 = new ZListBoxItem ( appListBox, QString ( "%I%M" ) );
	p1.load(ProgDir + "/images/shortcut.png");
	listitem11->setPixmap ( 0, p1 );
	listitem11->appendSubItem ( 1, CONFIG_SHORTCUT, false, NULL );
	appListBox->insertItem ( listitem11,-1,true );
  }
  Fval = Fini.readEntry(QString("OPTIONS"), QString("VRButton"), "YES");
  if ( Fval == "YES" ) {
	ZListBoxItem* listitem11 = new ZListBoxItem ( appListBox, QString ( "%I%M" ) );
	p1.load(ProgDir + "/images/vr.png");
	listitem11->setPixmap ( 0, p1 );
	listitem11->appendSubItem ( 1, CONFIG_VR, false, NULL );
	appListBox->insertItem ( listitem11,-1,true );
  }
  Fval = Fini.readEntry(QString("OPTIONS"), QString("BootLogo"), "YES");
  if ( Fval == "YES" ) {
	ZListBoxItem* listitem12 = new ZListBoxItem ( appListBox, QString ( "%I%M" ) );
	p1.load(ProgDir + "/images/boot.png");
	listitem12->setPixmap ( 0, p1 );
	listitem12->appendSubItem ( 1, CONFIG_BOOTLOGO, false, NULL );
	appListBox->insertItem ( listitem12,-1,true );
  }
  Fval = Fini.readEntry(QString("OPTIONS"), QString("SystemFont"), "YES");
  if ( Fval == "YES" ) {
	ZListBoxItem* listitem9 = new ZListBoxItem ( appListBox, QString ( "%I%M" ) );
	p1.load(ProgDir + "/images/font.png");
	listitem9->setPixmap ( 0, p1 );
	listitem9->appendSubItem ( 1, CONFIG_FONT, false, NULL );
	appListBox->insertItem ( listitem9,-1,true );
  }
  Fval = Fini.readEntry(QString("OPTIONS"), QString("CPUFrecuency"), "YES");
  if ( Fval == "YES" ) {
	ZListBoxItem* listitem14 = new ZListBoxItem ( appListBox, QString ( "%I%M" ) );
	p1.load(ProgDir + "/images/clock.png");
	listitem14->setPixmap ( 0, p1 );
	listitem14->appendSubItem ( 1, CONFIG_OVERCLOCK, false, NULL );
	modListBox->insertItem ( listitem14,-1,true );
  }
  Fval = Fini.readEntry(QString("OPTIONS"), QString("OrangeBarLinks"), "YES");
  if ( Fval == "YES" ) {
	ZListBoxItem* listitem15 = new ZListBoxItem ( appListBox, QString ( "%I%M" ) );
	p1.load(ProgDir + "/images/links.png");
	listitem15->setPixmap ( 0, p1 );
	listitem15->appendSubItem ( 1, CONFIG_WEBLINK, false, NULL );
	appListBox->insertItem ( listitem15,-1,true );
  }
  Fval = Fini.readEntry(QString("OPTIONS"), QString("MenuIcons"), "YES");
  if ( Fval == "YES" ) {
	ZListBoxItem* listitem16 = new ZListBoxItem ( appListBox, QString ( "%I%M" ) );
	p1.load(ProgDir + "/images/icons.png");
	listitem16->setPixmap ( 0, p1 );
	listitem16->appendSubItem ( 1, MENUICONS, false, NULL );
	appListBox->insertItem ( listitem16,-1,true );
  }
  Fval = Fini.readEntry(QString("OPTIONS"), QString("MenuVibrate"), "YES");
  if ( Fval == "YES" ) {
	ZListBoxItem* listitem17 = new ZListBoxItem ( appListBox, QString ( "%I%M" ) );
	p1.load(ProgDir + "/images/vibrate.png");
	listitem17->setPixmap ( 0, p1 );
	listitem17->appendSubItem ( 1, AMVIB, false, NULL );
	modListBox->insertItem ( listitem17,-1,true );
  }
  Fval = Fini.readEntry(QString("OPTIONS"), QString("FontSize"), "YES");
  if ( Fval == "YES" ) {
	ZListBoxItem* listitem18 = new ZListBoxItem ( appListBox, QString ( "%I%M" ) );
	p1.load(ProgDir + "/images/font.png");
	listitem18->setPixmap ( 0, p1 );
	listitem18->appendSubItem ( 1, FONT_SIZE, false, NULL );
	appListBox->insertItem ( listitem18,-1,true );
  }

  appListBox->sort(true);
  modListBox->sort(true);
  appListBox->setCurrentItem(0,true);
  modListBox->setCurrentItem(0,true);

}


QString Z6Man::getFileName(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( 0, i);
  if (str.left(1) = "/") str.remove ( 0, 1);
  return str;    
}

void Z6Man::appClicked(ZListBoxItem *item)
{

  QString a = item->getSubItemText(1, 0, false);
  
  if ( a == CONFIG_ALARM ) { Time_Alarm(); }
  if ( a == CONFIG_BLUETOOTH ) { BT_Discover(); }
  if ( a == CONFIG_CONTACTS ) { ID_Contacts(); }
  if ( a == CONFIG_EMPTYSMS ) { SMS_Empty(); }
  if ( a == CONFIG_ORANGEBAR ) { Sidebar(); }
  if ( a == CONFIG_SLIDER ) { Slider(); }
  if ( a == CONFIG_EQUALIZER ) { Equalize(); }
  if ( a == CONFIG_FONT ) { SelectFont(); }
  if ( a == CONFIG_JAVA ) { JavaSize(); }
  if ( a == CONFIG_CAM ) { CameraMod(); }
  if ( a == CONFIG_BOOTLOGO ) { Bootlogo(); }
  if ( a == CONFIG_SHORTCUT ) { Shortcut(); }
  if ( a == CONFIG_VR ) { VR(); }
  if ( a == CONFIG_SLIDERTONES ) { Slidertones(); }
  if ( a == CONFIG_OVERCLOCK ) { Overclock(); }
  if ( a == MENUICONS ) { AMmod(); }
  if ( a == AMVIB ) { AMvib(); }
  if ( a == CONFIG_WEBLINK ) { Weblink(); }
  if ( a == FONT_SIZE ) { FontSize(); }
  if ( a == FUNLIGHTS ) { Funlights(); }

}


QString Z6Man::checkNorris()
{
  system("touch /usr/data_resource/norris.tmp");
  QString file = "/usr/data_resource/norris.tmp";
  if (QFileInfo(file).isFile())
  { return "Norris"; system("rm /usr/data_resource/norris.tmp"); }
  else
  { return "Fuck!"; }

}


void Z6Man::Time_Alarm()
{
    ZConfig IDini(Idioma, false);
	QString s1 = QString("/ezxlocal/download/appwrite/setup/ezx_system.cfg");
	int val;
	ZConfig ini(s1, true);
	val = ini.readNumEntry(QString("ALARMCLOCK"), QString("AlertTime"), 5);
	dlg_bt_discover = new ZNumPickerDlg(10, this, "ZNumPickerDlg", 0);
	ZNumModule* module = dlg_bt_discover->getNumModule();
	module->setMaxValue(10);
	module->setMinValue(1);
	module->setValue(val);
		
		ZLabel* label = new ZLabel(ALARM_MSG1, module, NULL,0, (ZSkinService::WidgetClsID)4);
		module->setLabel(label, (ZNumModule::RelativeAlignment)0);
	dlg_bt_discover->setTitle(CONFIG_ALARM);
	if (dlg_bt_discover->exec())
	{	
		int r = dlg_bt_discover->getNumValue();
		ini.writeEntry(QString("ALARMCLOCK"), QString("AlertTime"), r);
		qApp->processEvents();
	}
}


void Z6Man::BT_Discover()
{
    ZConfig IDini(Idioma, false);
	QString s1 = QString("/ezxlocal/download/appwrite/setup/ezx_bluetooth.cfg");
	int val;
	ZConfig ini(s1, true);
	val = ini.readNumEntry(QString("Bluetooth Setup"), QString("DiscoverablePeriod"), 5);
	QString a = QString("BTTime");
	dlg_bt_discover = new ZNumPickerDlg(10, this, "ZNumPickerDlg", 0);
	ZNumModule* module = dlg_bt_discover->getNumModule();
	module->setMaxValue(20);
	module->setMinValue(1);
	module->setValue(val);
	ZLabel* label = new ZLabel(ALARM_MSG1, module, NULL,0, (ZSkinService::WidgetClsID)4);
		module->setLabel(label, (ZNumModule::RelativeAlignment)0);
	dlg_bt_discover->setTitle(CONFIG_BLUETOOTH);
	if (dlg_bt_discover->exec())
	{	
		int r = dlg_bt_discover->getNumValue();

		ini.writeEntry(QString("Bluetooth Setup"), QString("DiscoverablePeriod"), r);
		qApp->processEvents();
	}
}

void Z6Man::ID_Contacts()
{
    ZConfig IDini(Idioma, false);
	QString s1 = QString("/ezxlocal/download/appwrite/setup/ezx_addressbook.cfg");
	int val;
	ZConfig ini(s1, true);
	val = ini.readNumEntry(QString("ADDRBK_PRELOAD_GROUP"), QString("NUMBER_MATCH_MINLEN"), 5);
	QString a = QString("BTTime");
	dlg_bt_discover = new ZNumPickerDlg(10, this, "ZNumPickerDlg", 0);
	ZNumModule* module = dlg_bt_discover->getNumModule();
	module->setMaxValue(12);
	module->setMinValue(6);
	module->setValue(val);
 		ZLabel* label = new ZLabel(CONTACTS_MSG1, module, NULL,0, (ZSkinService::WidgetClsID)4);
		module->setLabel(label, (ZNumModule::RelativeAlignment)0);
	dlg_bt_discover->setTitle(CONFIG_CONTACTS);
	if (dlg_bt_discover->exec())
	{	
		int r = dlg_bt_discover->getNumValue();
		ini.writeEntry(QString("ADDRBK_PRELOAD_GROUP"), QString("NUMBER_MATCH_MINLEN"), r);
		qApp->processEvents();
	}
}

void Z6Man::SMS_Empty()
{
    ZConfig IDini(Idioma, false);
	QString s1 = QString("/ezxlocal/download/appwrite/setup/ezx_flexbit.cfg");
	int val;
	ZConfig ini(s1, true);
	val = ini.readNumEntry(QString("SYS_FLEX_TABLE"), QString("6"), 12);
	ZSingleSelectDlg* eq =  new ZSingleSelectDlg(CONFIG_EMPTYSMS, QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list2;
	list2 = OPTIONS_ENABLED;
	list2 += OPTIONS_DISABLED;
	eq->addItemsList(list2);
	ZListBox* listboxx2 = eq->getListBox();
	bool HSA_Status; getFlexBit(0xCA, HSA_Status);
	if ( HSA_Status ) { listboxx2->checkItem(0, true); }
	else { listboxx2->checkItem(1, true); }
	eq->exec();
	int result = eq->result();
	if (result != 1) {
		result = 0;
	} else {
	  result = eq->getCheckedItemIndex();
	  if (listboxx2->itemChecked(0))
	  {
		setFlexBit(0xCA, true); 
	  }
	  if (listboxx2->itemChecked(1))
	  {
		setFlexBit(0xCA, false); 
	  }
	}


}


void Z6Man::AMmod()
{
    ZConfig IDini(Idioma, false);
	int val;
	ZConfig AMini("/ezxlocal/download/appwrite/setup/ezx_am.cfg", true);
	ZSingleSelectDlg* eq =  new ZSingleSelectDlg(MENUICONS, QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list2;
	list2 = BIGICONS;
	list2 += SMALLICONS;
	list2 += NOICONS;
	eq->addItemsList(list2);
	QString started = AMini.readEntry( "AM", "SubMenuIcons", "0" );
	ZListBox* listboxx2 = eq->getListBox();
	if ( started == "0" ) { listboxx2->checkItem(2, true); }
	else {
		started = AMini.readEntry( "AM", "BigListIcons", "0" );
		if ( started == "0" ) { listboxx2->checkItem(1, true); }
		else  { listboxx2->checkItem(0, true); }
	}
	eq->exec();
	int result = eq->result();
	if (result != 1) {
		result = 0;
	} else {
		result = eq->getCheckedItemIndex();
		if (listboxx2->itemChecked(0))
		{
			AMini.writeEntry( "AM", "SubMenuIcons", "1" );
			AMini.writeEntry( "AM", "BigListIcons", "1" );
			AMini.writeEntry( "AM", "LikeMainMenu", "1" );
		}
		if (listboxx2->itemChecked(1))
		{
			AMini.writeEntry( "AM", "SubMenuIcons", "1" );
			AMini.writeEntry( "AM", "BigListIcons", "0" );
			AMini.writeEntry( "AM", "LikeMainMenu", "0" );
		}
		if (listboxx2->itemChecked(2))
		{
			AMini.writeEntry( "AM", "SubMenuIcons", "0" );
			AMini.writeEntry( "AM", "BigListIcons", "0" );
			AMini.writeEntry( "AM", "LikeMainMenu", "0" );
		}
	}


}

void Z6Man::AMvib()
{
    ZConfig IDini(Idioma, false);
	int val;
	ZConfig AMini("/ezxlocal/download/appwrite/setup/ezx_am.cfg", true);
	ZSingleSelectDlg* eq =  new ZSingleSelectDlg(AMVIB, QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list2;
	list2 = OPTIONS_ENABLED;
	list2 += OPTIONS_DISABLED;
	eq->addItemsList(list2);
	QString started = AMini.readEntry( "AM", "MenuVibrate", "0" );
	ZListBox* listboxx2 = eq->getListBox();
	if ( started == "0" ) { listboxx2->checkItem(1, true); }
	else { listboxx2->checkItem(0, true); }
	eq->exec();
	int result = eq->result();
	if (result != 1) {
		result = 0;
	} else {
		result = eq->getCheckedItemIndex();
		if (listboxx2->itemChecked(0))
		{
			AMini.writeEntry( "AM", "MenuVibrate", "1" );
			system("touch /ezxlocal/LinXtend/.vibrate");
		}
		else
		{
			AMini.writeEntry( "AM", "MenuVibrate", "0" );
			system("rm /ezxlocal/LinXtend/.vibrate");
		}
	}


}

void Z6Man::Sidebar()
{
    ZConfig IDini(Idioma, false);
	QString s1 = QString("/ezxlocal/download/appwrite/setup/ezx_flexbit.cfg");
	int val;
	ZConfig ini(s1, true);
	val = ini.readNumEntry(QString("SYS_FLEX_TABLE"), QString("6"), 12);

	ZSingleSelectDlg* eq =  new ZSingleSelectDlg(CONFIG_ORANGEBAR, QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list2;
	list2 = OPTIONS_ENABLED;
	list2 += OPTIONS_DISABLED;
	eq->addItemsList(list2);
	ZListBox* listboxx2 = eq->getListBox();
	bool HSA_Status; getFlexBit(0xDC, HSA_Status);
	if ( HSA_Status ) { listboxx2->checkItem(0, true); }
	else { listboxx2->checkItem(1, true); }
	eq->exec();
	int result = eq->result();
	if (result != 1) {
		result = 0;
	} else {
	  result = eq->getCheckedItemIndex();
	  if (listboxx2->itemChecked(0))
	  {
		setFlexBit(0xDC, true); 
	  }
	  if (listboxx2->itemChecked(1))
	  {
		setFlexBit(0xDC, false); 
	  }
	}

}

void Z6Man::Slider()
{
	QString s1 = QString("/ezxlocal/download/appwrite/setup/ezx_flexbit.cfg");
	int val;
	ZConfig ini(s1, true);
	val = ini.readNumEntry(QString("SYS_FLEX_TABLE"), QString("10"), 12);
    ZConfig IDini(Idioma, false);
	ZSingleSelectDlg* eq =  new ZSingleSelectDlg(CONFIG_SLIDER, QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list2;
	list2 = OPTIONS_ENABLED;
	list2 += OPTIONS_DISABLED;
	eq->addItemsList(list2);
	ZListBox* listboxx2 = eq->getListBox();
	bool HSA_Status; getFlexBit(0x15B, HSA_Status);
	if ( HSA_Status ) { listboxx2->checkItem(0, true); }
	else { listboxx2->checkItem(1, true); }
	eq->exec();
	int result = eq->result();
	if (result != 1) {
		result = 0;
	} else {
	  result = eq->getCheckedItemIndex();
	  if (listboxx2->itemChecked(0))
	  {
		setFlexBit(0x15B, true); 
	  }
	  if (listboxx2->itemChecked(1))
	  {
		setFlexBit(0x15B, false); 
	  }
	}


}

void Z6Man::Equalize()
{
    ZConfig IDini(Idioma, false);
	ZMultiSelectDlg* eq1 =  new ZMultiSelectDlg(CONFIG_EQUALIZER, QString(""), NULL, "ZMultiSelectDlg", true, 0, 0);
	QStringList list;
	list="Bass Boost"; list += "Classical"; list += "Country"; list += "Dance"; list += "Hip Hop"; list += "Latin"; 
	list += "Oldies"; list += "Pop"; list += "RB Soul"; list += "Reggae"; list += "Rock"; list += "Small Speaker";
	list += "Spoken Word"; list += "Techno"; list += "Treble Boost"; list += "Vocal Booster"; list += "Vocal Reducer";
	eq1->addItemsList(list);
	ZListBox* listboxx = eq1->getListBox();
	QString s1 = QString("/ezxlocal/download/appwrite/setup/ezx_mp.cfg");
	int val; ZConfig ini(s1, true);
	val = ini.readNumEntry(QString("MediaControlItem"), QString("EqualizerFeature"), 12);

	ZSingleSelectDlg* eq =  new ZSingleSelectDlg( CONFIG_EQUALIZER, QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list2;
	list2 = OPTIONS_ENABLED;
	list2 += OPTIONS_DISABLED;
	eq->addItemsList(list2);
	ZListBox* listboxx2 = eq->getListBox();
	if ( val == 1 ) { listboxx2->checkItem(0, true); }
	if ( val == 0 ) { listboxx2->checkItem(1, true); }
	eq->exec();
	int result = eq->result();
	if (result != 1) {
		result = 0;
	} else {
	  result = eq->getCheckedItemIndex();
	  if (listboxx2->itemChecked(1))
	  {
		ini.writeEntry(QString("MediaControlItem"), QString("EqualizerFeature"), "0");
		ini.writeEntry(QString("MediaControlItem"), QString("EqSettingBassBoost"), "0");
		ini.writeEntry(QString("MediaControlItem"), QString("EqSettingClassical"), "0");
		ini.writeEntry(QString("MediaControlItem"), QString("EqSettingCountry"), "0");
		ini.writeEntry(QString("MediaControlItem"), QString("EqSettingDance"), "0");
		ini.writeEntry(QString("MediaControlItem"), QString("EqSettingHipHop"), "0");
		ini.writeEntry(QString("MediaControlItem"), QString("EqSettingLatin"), "0");
		ini.writeEntry(QString("MediaControlItem"), QString("EqSettingOldies"), "0");
		ini.writeEntry(QString("MediaControlItem"), QString("EqSettingPop"), "0");
		ini.writeEntry(QString("MediaControlItem"), QString("EqSettingRBSoul"), "0");
		ini.writeEntry(QString("MediaControlItem"), QString("EqSettingReggae"), "0");
		ini.writeEntry(QString("MediaControlItem"), QString("EqSettingRock"), "0");
		ini.writeEntry(QString("MediaControlItem"), QString("EqSettingSmallSpeaker"), "0");
		ini.writeEntry(QString("MediaControlItem"), QString("EqSettingSpokenWord"), "0");
		ini.writeEntry(QString("MediaControlItem"), QString("EqSettingTechno"), "0");
		ini.writeEntry(QString("MediaControlItem"), QString("EqSettingTrebleBoost"), "0");
		ini.writeEntry(QString("MediaControlItem"), QString("EqSettingVocalBooster"), "0");
		ini.writeEntry(QString("MediaControlItem"), QString("EqSettingVocalReducer"), "0");
		qApp->processEvents();

	  }
	  if (listboxx2->itemChecked(0))
	  {
		val = ini.readNumEntry(QString("MediaControlItem"), QString("EqSettingBassBoost"), 5);
		if ( val == 1 ) { listboxx->checkItem(0, true); }
		val = ini.readNumEntry(QString("MediaControlItem"), QString("EqSettingClassical"), 5);
		if ( val == 1 ) { listboxx->checkItem(1, true); }
		val = ini.readNumEntry(QString("MediaControlItem"), QString("EqSettingCountry"), 5);
		if ( val == 1 ) { listboxx->checkItem(2, true); }
		val = ini.readNumEntry(QString("MediaControlItem"), QString("EqSettingDance"), 5);
		if ( val == 1 ) { listboxx->checkItem(3, true); }
		val = ini.readNumEntry(QString("MediaControlItem"), QString("EqSettingHipHop"), 5);
		if ( val == 1 ) { listboxx->checkItem(4, true); }
		val = ini.readNumEntry(QString("MediaControlItem"), QString("EqSettingLatin"), 5);
		if ( val == 1 ) { listboxx->checkItem(5, true); }
		val = ini.readNumEntry(QString("MediaControlItem"), QString("EqSettingOldies"), 5);
		if ( val == 1 ) { listboxx->checkItem(6, true); }
		val = ini.readNumEntry(QString("MediaControlItem"), QString("EqSettingPop"), 5);
		if ( val == 1 ) { listboxx->checkItem(7, true); }
		val = ini.readNumEntry(QString("MediaControlItem"), QString("EqSettingRBSoul"), 5);
		if ( val == 1 ) { listboxx->checkItem(8, true); }
		val = ini.readNumEntry(QString("MediaControlItem"), QString("EqSettingReggae"), 5);
		if ( val == 1 ) { listboxx->checkItem(9, true); }
		val = ini.readNumEntry(QString("MediaControlItem"), QString("EqSettingRock"), 5);
		if ( val == 1 ) { listboxx->checkItem(10, true); }
		val = ini.readNumEntry(QString("MediaControlItem"), QString("EqSettingSmallSpeaker"), 5);
		if ( val == 1 ) { listboxx->checkItem(11, true); }
		val = ini.readNumEntry(QString("MediaControlItem"), QString("EqSettingSpokenWord"), 5);
		if ( val == 1 ) { listboxx->checkItem(12, true); }
		val = ini.readNumEntry(QString("MediaControlItem"), QString("EqSettingTechno"), 5);
		if ( val == 1 ) { listboxx->checkItem(13, true); }
		val = ini.readNumEntry(QString("MediaControlItem"), QString("EqSettingTrebleBoost"), 5);
		if ( val == 1 ) { listboxx->checkItem(14, true); }
		val = ini.readNumEntry(QString("MediaControlItem"), QString("EqSettingVocalBooster"), 5);
		if ( val == 1 ) { listboxx->checkItem(15, true); }
		val = ini.readNumEntry(QString("MediaControlItem"), QString("EqSettingVocalReducer"), 5);
		if ( val == 1 ) { listboxx->checkItem(16, true); }
		eq1->exec();

		int result = eq1->result();
		if (result != 1) {
			result = 0;
		} else {
		  ini.writeEntry(QString("MediaControlItem"), QString("EqualizerFeature"), "1");
		  if (listboxx->itemChecked(0)) { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingBassBoost"), "1"); } else  { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingBassBoost"), "0"); }
		  if (listboxx->itemChecked(1)) { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingClassical"), "1"); } else  { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingClassical"), "0"); }
		  if (listboxx->itemChecked(2)) { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingCountry"), "1"); } else  { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingCountry"), "0"); }
		  if (listboxx->itemChecked(3)) { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingDance"), "1"); } else  { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingDance"), "0"); }
		  if (listboxx->itemChecked(4)) { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingHipHop"), "1"); } else  { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingHipHop"), "0"); }
		  if (listboxx->itemChecked(5)) { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingLatin"), "1"); } else  { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingLatin"), "0"); }
		  if (listboxx->itemChecked(6)) { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingOldies"), "1"); } else  { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingOldies"), "0"); }
		  if (listboxx->itemChecked(7)) { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingPop"), "1"); } else  { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingPop"), "0"); }
		  if (listboxx->itemChecked(8)) { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingRBSoul"), "1"); } else  { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingRBSoul"), "0"); }
		  if (listboxx->itemChecked(9)) { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingReggae"), "1"); } else  { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingReggae"), "0"); }
		  if (listboxx->itemChecked(10)) { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingRock"), "1"); } else  { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingRock"), "0"); }
		  if (listboxx->itemChecked(11)) { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingSmallSpeaker"), "1"); } else  { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingSmallSpeaker"), "0"); }
		  if (listboxx->itemChecked(12)) { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingSpokenWord"), "1"); } else  { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingSpokenWord"), "0"); }
		  if (listboxx->itemChecked(13)) { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingTechno"), "1"); } else  { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingTechno"), "0"); }
		  if (listboxx->itemChecked(14)) { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingTrebleBoost"), "1"); } else  { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingTrebleBoost"), "0"); }
		  if (listboxx->itemChecked(15)) { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingVocalBooster"), "1"); } else  { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingVocalBooster"), "0"); }
		  if (listboxx->itemChecked(16)) { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingVocalReducer"), "1"); } else  { ini.writeEntry(QString("MediaControlItem"), QString("EqSettingVocalReducer"), "0"); }
		  qApp->processEvents();

		}
		delete eq1;
		eq1 = NULL;
	  }
	}

}

void Z6Man::SelectFont()
{
  QString app = appListBox->item ( appListBox->currentItem() )->getSubItemText ( 1, 0, true );

  ZSelectFont *zfod = new ZSelectFont();
  zfod->exec();
  if ( zfod->result() == 1 )
  {
    QString archivo = zfod->resultado();
    if (archivo != "")
	{
		Changing(archivo);
	}
  }
}

void Z6Man::Changing(QString fuente)
{
  QString font = fuente;
  ZConfig IDini(Idioma, false);

/*  QString TEXTO = "#SansMT SansMT_936.stf FS n 50 0 u\n";
  TEXTO += "#SansMT SansMT_936.stf FS n 75 0 u\n";
  TEXTO += "#SansMT SansMT_936.stf FS y 50 0 u\n";
  TEXTO += "#SansMT SansMT_936.stf FS y 75 0 u\n";
  TEXTO += "SSong_us AMCSL.TTF	FS n 50 0 u\n";
  TEXTO += "SSong_us AMCSL.TTF FS n 75 0 u\n";
  TEXTO += "SSong_us AMCSL.TTF FS y 50 0 u\n";
  TEXTO += "SSong_us AMCSL.TTF FS y 75 0 u\n";
  TEXTO += "SSong_ns AMCSL.TTF FS n 50 0 u\n";
  TEXTO += "SSong_ns AMCSL.TTF FS n 75 0 u\n";
  TEXTO += "SSong_ns AMCSL.TTF FS y 50 0 u\n";
  TEXTO += "SSong_ns AMCSL.TTF FS y 75 0 u\n";
  TEXTO += "SSong AMCSL.TTF	FS n 50 0 s\n";
  TEXTO += "SSong AMCSL.TTF FS n 75 0 s\n";
  TEXTO += "SSong AMCSL.TTF FS y 50 0 s\n";
  TEXTO += "SSong AMCSL.TTF FS y 75 0 s\n";
  TEXTO += "myFont " + fuente + " y 75 0 s\n";

  system(QString("echo \"%1\" > /tmp/fontdef").arg(TEXTO));
  system("mount -o bind /tmp/fontdef /usr/language/fonts/fontdef.cfg");


  ZMessageDlg* msg1 =  new ZMessageDlg ( QString("Font"), QString("Esta es una muestra de la fuente seleccionada"), 
										ZMessageDlg::just_ok, 0, NULL, "ZMessageDlg", true, 0 );
  msg1->setFont( QFont("myFont",14,QFont::Bold) );
  msg1->exec(); delete msg1; msg1 = NULL;

  system("umount /usr/language/fonts/fontdef.cfg");

*/

  ZMessageDlg* msg =  new ZMessageDlg ( QString("Font"), QString(FONT_APPLY).arg(getFileName(font)), 
										ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
  msg->setTitleIcon("fonts_std");
  int i = msg->exec();
  delete msg;
  msg = NULL;
  if (i)
  { 
    QString TEXTO = QString("#!/bin/sh\n#######################\n#   Linuxmod          #\n#   LinXtend          #\n#######################\nif [ \"'\"$LX_ENV\"'\" != \"'\"true\"'\" ]\nthen\n  . /ezxlocal/LinXtend/etc/initenv\nfi\n\nDIRNAME=\"'`'\"busybox dirname \"'\"$0\"'\"\"'`'\"\nFONT=%1\n\nif [ \"'\"$FONT\"'\" != \"'\"NO\"'\" ]\nthen\n    showNotify \"'\"\"'\" \"'\"%2\"'\" 1 4\n    mount -o bind \"'\"$FONT\"'\" /usr/language/fonts/AMCSL.TTF\n    mount -o bind \"'\"$FONT\"'\" /usr/language/fonts/NUMBER.TTF\n\n    chown 99:99 -R /ezxlocal/download/appwrite/skin /ezxlocal/download/appwrite/setuptheme\n    chown ezx:ezx -R /ezxlocal/download/mystuff\n    chmod 777 -R /ezxlocal/download/mystuff /ezxlocal/download/appwrite/skin /ezxlocal/download/appwrite/setuptheme\n    busybox killall opera\n    busybox killall am\n    sleep 1s\n    busybox killall phone\nfi\n\n").arg(fuente).arg(FONT_CHANGING);

 	system(QString("echo \"%1\" > /ezxlocal/download/mystuff/.system/LinXtend/usr/bin/mountfont").arg(TEXTO)); 
	system("/ezxlocal/LinXtend/usr/bin/mountfont &");
  }
  else { system("busybox killall z6tweaker"); }

}


void Z6Man::JavaSize()
{
    ZConfig IDini(Idioma, false);
	QString s1 = QString("/ezxlocal/download/appwrite/setup/ezx_system.cfg");
	int val;
	ZConfig ini(s1, true);
	val = ini.readNumEntry(QString("SYS_JAVA"), QString("JavaHeapSize"), 0);

	ZSingleSelectDlg* eq =  new ZSingleSelectDlg(CONFIG_JAVA, QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list2;
	list2 = JAVA_MOD1;
	list2 += JAVA_MOD2;
	list2 += JAVA_MOD3;
	list2 += JAVA_MOD4;
	list2 += JAVA_MOD5;
	list2 += JAVA_MOD6;
	eq->addItemsList(list2);
	ZListBox* listboxx2 = eq->getListBox();
	if ( val == 1 ) { listboxx2->checkItem(0, true); listboxx2->setSelected(0,true); }
	else if ( val == 2 ) { listboxx2->checkItem(1, true); listboxx2->setSelected(1,true); }
	else if ( val == 3 ) { listboxx2->checkItem(2, true); listboxx2->setSelected(2,true); }
	else if ( val == 4 ) { listboxx2->checkItem(3, true); listboxx2->setSelected(3,true); }
	else if ( val == 5 ) { listboxx2->checkItem(4, true); listboxx2->setSelected(4,true); }
	else if ( val == 6 ) { listboxx2->checkItem(5, true); listboxx2->setSelected(5,true); }
	eq->exec();
	int result = eq->result();
	if (result != 1) {
		result = 0;
	} else {
	  result = eq->getCheckedItemIndex();
		if (listboxx2->itemChecked(0)) { ini.writeEntry( QString("SYS_JAVA"), QString("JavaHeapSize"), 1 ); }
		if (listboxx2->itemChecked(1)) { ini.writeEntry( QString("SYS_JAVA"), QString("JavaHeapSize"), 2 ); }
		if (listboxx2->itemChecked(2)) { ini.writeEntry( QString("SYS_JAVA"), QString("JavaHeapSize"), 3 ); }
		if (listboxx2->itemChecked(3)) { ini.writeEntry( QString("SYS_JAVA"), QString("JavaHeapSize"), 4 ); }
		if (listboxx2->itemChecked(4)) { ini.writeEntry( QString("SYS_JAVA"), QString("JavaHeapSize"), 5 ); }
		if (listboxx2->itemChecked(5)) { ini.writeEntry( QString("SYS_JAVA"), QString("JavaHeapSize"), 6 ); }
	}

}

void Z6Man::CameraMod()
{
    ZConfig IDini(Idioma, false);
	QString s1 = QString("/ezxlocal/download/appwrite/setup/ezx_camera.cfg");
	int val;
	ZConfig ini(s1, true);
	val = ini.readNumEntry(QString("camvideo"), QString("videoSizeTableH1"), 0);

	ZSingleSelectDlg* eq =  new ZSingleSelectDlg(CONFIG_CAM, QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list2;
	list2 = CAM_MOD1;
	list2 += CAM_MOD2;
	list2 += CAM_MOD3;
	eq->addItemsList(list2);
	ZListBox* listboxx2 = eq->getListBox();
	if ( val == 144 ) { listboxx2->checkItem(0, true); listboxx2->setSelected(0,true); }
	else if ( val == 240 ) { listboxx2->checkItem(1, true); listboxx2->setSelected(1,true); }
	else if ( val == 288 ) { listboxx2->checkItem(2, true); listboxx2->setSelected(2,true); }
	eq->exec();
	int result = eq->result();
	if (result != 1) {
		result = 0;
	} else {
	  result = eq->getCheckedItemIndex();
		if (listboxx2->itemChecked(0))
		{
		  ini.writeEntry( QString("camvideo"), QString("videoSizeTableH0"), 96 );
		  ini.writeEntry( QString("camvideo"), QString("videoSizeTableW0"), 128 );
		  ini.writeEntry( QString("camvideo"), QString("videoSizeTableH1"), 144 );
		  ini.writeEntry( QString("camvideo"), QString("videoSizeTableW1"), 176 );
		  system("busybox gzip -c -d /usr/data_resource/linuxmod2/cam/.default.tar.gz | busybox tar -xv -f - -C /ezxlocal/download/mystuff/.system/LinXtend/usr/lib/");
		}

		if (listboxx2->itemChecked(1))
		{
		  ini.writeEntry( QString("camvideo"), QString("videoSizeTableH0"), 144 );
		  ini.writeEntry( QString("camvideo"), QString("videoSizeTableW0"), 176 );
		  ini.writeEntry( QString("camvideo"), QString("videoSizeTableH1"), 240 );
		  ini.writeEntry( QString("camvideo"), QString("videoSizeTableW1"), 320 );
		  system("busybox gzip -c -d /usr/data_resource/linuxmod2/cam/.240x320.tar.gz | busybox tar -xv -f - -C /ezxlocal/download/mystuff/.system/LinXtend/usr/lib/");
		}

		if (listboxx2->itemChecked(2))
		{
		  ini.writeEntry( QString("camvideo"), QString("videoSizeTableH0"), 144 );
		  ini.writeEntry( QString("camvideo"), QString("videoSizeTableW0"), 176 );
		  ini.writeEntry( QString("camvideo"), QString("videoSizeTableH1"), 288 );
		  ini.writeEntry( QString("camvideo"), QString("videoSizeTableW1"), 352 );
		  system("busybox gzip -c -d /usr/data_resource/linuxmod2/cam/.352x288.tar.gz | busybox tar -xv -f - -C /ezxlocal/download/mystuff/.system/LinXtend/usr/lib/");
		}
	}
}

void Z6Man::Bootlogo()
{
  QString titleapp = appListBox->item ( appListBox->currentItem() )->getSubItemText ( 1, 0, true );

  ZBootlogo *zfod = new ZBootlogo();
  zfod->exec();
  if ( zfod->result() == 1 )
  {
    QString archivo = zfod->resultado(); 
    if ( (archivo != "") && (QFileInfo(archivo).isFile()) )
	{
		QString IDval; QString IDval2; ZConfig IDini(Idioma, false);
		ZMessageDlg* msg =  new ZMessageDlg ( titleapp, QString(BOOTLOGO_ASK).arg(getFileName(archivo)), 
							ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
		int k = msg->exec(); delete msg; msg = NULL;
		if (k)
		{
		  ZMessageDlg* msg0 =  new ZMessageDlg ( titleapp, BOOTLOGO_CHANGING, ZMessageDlg::NONE, 200, NULL, "ZMessageDlg", true, 0 );
		  msg0->exec();
		  system(QString("/ezxlocal/LinXtend/usr/bin/chlogo \"%1\"; RET=$?; echo \"$RET\" > /tmp/flashlogo").arg(archivo));
		  delete msg0;
		  msg0 = NULL;

		  QString archivo = QString("/tmp/flashlogo");
		  QString line=""; QFile entrada(archivo); QTextStream stentrada(&entrada);
		  if ( entrada.open(IO_ReadOnly | IO_Translate) )
		  { 
			while ( !stentrada.eof() )
			{
			  line = stentrada.readLine();
			  if ( line == "0" )
			  {
				QString IDval; QString IDval2; ZConfig IDini(Idioma, false);
				ZNoticeDlg* znd = new ZNoticeDlg(ZNoticeDlg::Information, 3000, QString(""), BOOTLOGO_DONE, this, "z", true, 0);
				znd->show(); znd = NULL;
			  }
			  else
			  {
				QString IDval; QString IDval2; ZConfig IDini(Idioma, false);
				IDval = IDini.readEntry(QString("Z6TWEAKER"), QString("BOOTLOGO_ERROR"), "");
				ZNoticeDlg* znd = new ZNoticeDlg(ZNoticeDlg::Information, 3000, QString(""), BOOTLOGO_ERROR, this, "z", true, 0);
				znd->show(); znd = NULL;
			  }
			}
		  }
		  system("rm /tmp/flashlogo");
		
	  }

	}
  }
}

void Z6Man::Shortcut()
{
  QString titleapp = appListBox->item ( appListBox->currentItem() )->getSubItemText ( 1, 0, true );

  ZShortcut *zfod = new ZShortcut();
  zfod->exec();
}

void Z6Man::VR()
{
  QString titleapp = appListBox->item ( appListBox->currentItem() )->getSubItemText ( 1, 0, true );

  ZVoiceRecord *zfod = new ZVoiceRecord();
  zfod->exec();
}

void Z6Man::Slidertones()
{
  QString titleapp = appListBox->item ( appListBox->currentItem() )->getSubItemText ( 1, 0, true );

  ZSlidertones *zfod = new ZSlidertones();
  zfod->exec();
}

void Z6Man::Overclock()
{
    ZConfig IDini(Idioma, false);
	QString val;
	
	ZSingleSelectDlg* eq =  new ZSingleSelectDlg(CONFIG_OVERCLOCK, QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list2;
	list2 = OVERCLOCK_MOD1;
	list2 += OVERCLOCK_MOD2;
	list2 += OVERCLOCK_MOD3;
	list2 += OVERCLOCK_MOD4;
	list2 += OVERCLOCK_MOD5;
	eq->addItemsList(list2);
	ZListBox* listboxx2 = eq->getListBox();
	eq->exec();
	int result = eq->result();
	if (result != 1) {
		result = 0;
	} else {
	  result = eq->getCheckedItemIndex();
		if (listboxx2->itemChecked(0)) { system("pmtool -d 1"); }
		if (listboxx2->itemChecked(1)) { system("pmtool -d 0; echo 133 > /sys/mpm/op"); }
		if (listboxx2->itemChecked(2)) { system("pmtool -d 0; echo 266 > /sys/mpm/op"); }
		if (listboxx2->itemChecked(3)) { system("pmtool -d 0; echo 399 > /sys/mpm/op"); }
		if (listboxx2->itemChecked(4)) { system("pmtool -d 0; echo 532 > /sys/mpm/op"); }
	}
}

void Z6Man::Weblink()
{
  QString titleapp = appListBox->item ( appListBox->currentItem() )->getSubItemText ( 1, 0, true );

  ZWeblink *zfod = new ZWeblink();
  zfod->exec();
}

void Z6Man::FontSize()
{
    ZConfig IDini(Idioma, false);
	QString val;
	
	ZSingleSelectDlg* eq =  new ZSingleSelectDlg(FONT_SIZE, QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list2;
	list2 = FONT_INCREASE;
	list2 += FONT_DECREASE;
	eq->addItemsList(list2);
	ZListBox* listboxx2 = eq->getListBox();
	eq->exec();
	int result = eq->result();
	if (result != 1) {
		result = 0;
	} else {
	  result = eq->getCheckedItemIndex();
	  int index; 
	  if (listboxx2->itemChecked(0)) index=0; else index=1;

	  ZConfig FiniA(QString("/ezxlocal/download/appwrite/setup/ezx_theme.cfg"), true);
	  QString currentTheme = FiniA.readEntry(QString("THEME_GROUP_LOG"), QString("CurrentSkin"), "");

	  QDir dir ( currentTheme, "*" );
	  dir.setMatchAllDirs ( true );
	  dir.setFilter ( QDir::Files | QDir::Hidden );
	  if ( !dir.isReadable() )
	  return;
	  QStringList entries = dir.entryList();
	  QStringList::ConstIterator it = entries.begin();

	  while ( it != entries.end() ) {
		if ( ( *it != "." ) && ( *it != ".." ) )
		{
		  QString Eval = QString("%1").arg(*it).lower();
		  int i = Eval.findRev("."); Eval.remove ( 0, i +1 );
		  if (Eval=="ini")
		  {
			if ( index==0 ) 
			{
			  cambiar( QString(currentTheme + "/" + *it), QString("mas") );

			}
			else
			{
			  cambiar( QString(currentTheme + "/" + *it), QString("menos") );
			}
		  }
		}
		++it;
	  }
	  ZNoticeDlg* znd = new ZNoticeDlg(ZNoticeDlg::Information, 3000, QString(""), FONT_SUCCESS, this, "z", true, 0);
	  znd->show(); znd = NULL; delete znd;
	}


}

void Z6Man::cambiar(QString archivo, QString valor)
{
	int size; QString entry;
	ZConfig confTEMP ( archivo, true );
	QStringList grouplist;
	confTEMP.getGroupsKeyList ( grouplist );
	for ( QStringList::Iterator it = grouplist.begin(); it != grouplist.end(); ++it ) {	 
		size = confTEMP.readNumEntry ( *it, "Size", 0 );
		entry = *it;
		if ( size > 0 )
		{
		  if ( valor=="mas") ++size; else --size;
		  confTEMP.writeEntry( QString(entry), QString("Size"), QString("%1").arg(size) );
		}
	}
}

void Z6Man::Funlights()
{
    ZConfig IDini(Idioma, false);
	QString s1 = QString("/ezxlocal/download/appwrite/setup/ezx_am.cfg");
	int val;
	ZConfig ini(s1, true);
	val = ini.readNumEntry(QString("CALLS"), QString("Funlights"), 0);
	ZSingleSelectDlg* eq =  new ZSingleSelectDlg(FUNLIGHTS, QString(""), NULL, "ZSingleSelectDlg", true, 0, 0);
	QStringList list2;
	list2 = OPTIONS_ENABLED;
	list2 += OPTIONS_DISABLED;
	eq->addItemsList(list2);
	ZListBox* listboxx2 = eq->getListBox();
	if ( val==1 ) { listboxx2->checkItem(0, true); }
	else { listboxx2->checkItem(1, true); }
	eq->exec();
	int result = eq->result();
	if (result != 1) {
		result = 0;
	} else {
	  result = eq->getCheckedItemIndex();
	  if (listboxx2->itemChecked(0))
	  {
		ini.writeEntry(QString("CALLS"), QString("Funlights"), QString("1"));
		system("busybox killall funlights");
		system("/ezxlocal/LinXtend/usr/bin/funlights&");

		ZMessageDlg* msg =  new ZMessageDlg ( QString(FUNLIGHTS), QString(FUN_FLASH), ZMessageDlg::yes_no, 0, NULL, "ZMessageDlg", true, 0 );
		int i = msg->exec(); delete msg; msg = NULL;
		if (i) ini.writeEntry(QString("CALLS"), QString("Flashlight"), QString("1"));
		else ini.writeEntry(QString("CALLS"), QString("Flashlight"), QString("0"));

	  }
	  if (listboxx2->itemChecked(1))
	  {
		ini.writeEntry(QString("CALLS"), QString("Funlights"), QString("0"));
		ini.writeEntry(QString("CALLS"), QString("Flashlight"), QString("0"));
		system("busybox killall funlights");
	  }
	}


}

