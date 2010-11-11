#include <TAPI_ACCE_Client.h>
#include <TAPI_CLIENT_Func.h>
#include <TAPI_SIM_Client.h>
#include <TAPI_Result.h>
#include <TAPI_AppMsg.h>
#include <TAPI_AppMsgId.h>
#include <TAPI_General.h>

#include "launcher.h"
#include "myPopup.h"
#include "myam.h"

//ezx
#include <SETUP_THEME_ThemeAPI.h>
#include <ZApplication.h>
#include <qobject.h>
#include <zglobal.h>
#include <ZMessageDlg.h>
#include <ZLineEdit.h>
#include <ZFormItem.h>
#include <ZSingleSelectDlg.h>
#include <ezxres.h>
#include <ZIconView.h>
#include <ZListBox.h>

//qt
#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <qpixmap.h>

#include <qvbox.h>
#include <qarray.h>

#include <ZLabel.h>
#include <ZMainWidget.h>    
#include "resources.h"

QString ActiveSel="NO";
QString CopyPath="";
QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
int ItemSize;
QString currentView;
int Playing=0;
QString argumento;

QString APPLYSUBS, ASOCIATEFILE, CONTROL, ABOUT, EXIT, BACK, CANCEL, FUNCTIONS, CHOISE, GOTO, ERROR, WARNING, CLOSE_APP, COPY, DELETE, MOVE, PASTE,  RENAME, PROPERTIES, ATTRS, MKDIR, PERSONALIZE, NAME, ICON, RESET, OK, FAVS, ADMINFAV, ADDFAV, FAVNAME, SELECTDEST, SELECTICON, COPYING, RENAMING, MOVEING, MAKING, ENTER_FILENAME, ENTER_DIRNAME, MKDIR_ERROR, RENAME_ERROR, RENAME_FILEEXISTS, FOLDER, AFILE, OPEN, OPENWITH, PROPERTIES_DATE, PROPERTIES_SIZE, PROPERTIES_OWNER, PROPERTIES_GROUP, PROPERTIES_ATTRS, PROPERTIES_RIGHTS, DELETE_DIR, DELETE_FILE, UNABLE_DELETE, ERROR_GET_FREE_SPACE, DELETE_MULTIPLE, ERROR_NON_FREE_SPACE, PROGRESS_PERCENT, SENDTO, RARFILE, MENULINK, RARCREATING, PHONE, SDCARD, APPLYTO, RINGTONE, WALLPAPER, SETTINGS, APPLY_ERROR, SEARCH;


ZLauncher::ZLauncher ( const QString &argument, QWidget* parent, const char* name, WFlags fl )
    : ZKbMainWidget ( ZHeader::FULL_TYPE, NULL, "ZMainWidget", 0 )
{

  fileName = QString::null;
  filePath = QString::null;
  
  ProgramDir=getProgramDir();

  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = ProgramDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = ProgramDir + "/languages/en-us";

  LoadSettings();
  argumento = argument;
  
  ActiveSel = "NO";  

  if ( QFileInfo("/tmp/clipboard").isFile() ) system("rm /tmp/clipboard");
  if ( QFileInfo("/tmp/clipboardplus").isFile() ) system("rm /tmp/clipboardplus");

  CreateWindow ( parent );
  ( ( ZApplication* ) qApp )->showMainWidget ( this ); 
  
}

ZLauncher::~ZLauncher()
{
	system("busybox killall alertprocess");
}

void ZLauncher::CreateWindow ( QWidget* parent )
{
  
  setMainWidgetTitle ( APP_NAME );
  QWidget *myWidget = new ZWidget ( this, NULL, 0);
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );
  myWidget->setFixedWidth(240);
  //----------------------------------------------------------------------    
  pAIA = new ZAppInfoArea(myWidget, NULL, 0);
  //pAIA->setInputArea();
  pAIA->setActArea(ZAppInfoArea::no_progress_meter);
  setAppInfoArea(pAIA);
  myVBoxLayout->addWidget ( pAIA, 1, 1 );

  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
  if ( val != "big" )
  { browser = new myListBox(QString("%C18%I%M"), myWidget, 0);
	browser->setFixedWidth(240);
	//connect(browser, SIGNAL(highlighted(int)), SLOT(itemHighlighted()) );
	connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));
	connect(browser, SIGNAL(selected(ZListBoxItem *)), SLOT(brSelected()));
  } else {
	Bbrowser = new myIconView ( myWidget ); Bbrowser->setFixedWidth ( 240 );
	//connect(Bbrowser, SIGNAL(selectionChanged()), SLOT(itemHighlighted()));
	connect(Bbrowser, SIGNAL(returnPressed(ZIconViewItem *)), SLOT(brClicked()));
  }
  currentView=val;

  QString home = ini.readEntry(QString("FM"), QString("Start"), "");
  if ( home == "0" ) home = ini.readEntry(QString("FM"), QString("LastFolder"), "");

  setContentWidget ( myWidget );

  softKey = new ZSoftKey ( NULL , this , this );
  CreateSubMenu();
  CreateMenu();
  CreateSoftKeys();
  setSoftKey ( softKey );

  if (argumento==NULL) CargarBrowser(home); 
  else CargarBrowser(argumento);

  qApp->installEventFilter( this );

  int y = 0;
  y = y + this->headerSize().height();
  y = y + softKey->height();
  y = y + pAIA->height();
  
  if ( val != "big" ) { browser->setFixedHeight(320-y); browser->setFocus(); }
  else { Bbrowser->setFixedHeight(320-y); Bbrowser->setFocus(); }


}

QString ZLauncher::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { return xString; }
	else { return xString; }
}  


void ZLauncher::CreateSubMenu()
{
  QRect rect;
  ZConfig lang(Idioma, false);
  menuControl = new ZOptionsMenu ( rect, softKey, NULL , 0);
  menuControl->setItemSpacing ( 10 ); int idx = 0;
  QPixmap* pm  = new QPixmap(ProgramDir+ IDB_IMAGE_DELETE);
  menuControl->insertItem ( DELETE, NULL, pm , true , idx , idx );
  menuControl->connectItem ( idx, this, SLOT ( slot_delete() ) ); ++idx;
  pm->load(ProgramDir+ IDB_IMAGE_COPY);
  menuControl->insertItem ( COPY, NULL, pm , true , idx , idx ); 
  menuControl->connectItem ( idx, this, SLOT ( slot_copy() ) ); ++idx;
  pm->load(ProgramDir+ IDB_IMAGE_PASTE);
  menuControl->insertItem ( PASTE, NULL, pm , true , idx , idx ); 
  menuControl->connectItem ( idx, this, SLOT ( slot_paste() ) ); ++idx;
  pm->load(ProgramDir+ IDB_IMAGE_MOVE);
  menuControl->insertItem ( MOVE, NULL, pm , true , idx , idx ); 
  menuControl->connectItem ( idx, this, SLOT ( slot_move() ) ); ++idx;
  pm->load(ProgramDir+ IDB_IMAGE_RENAME);
  menuControl->insertItem ( RENAME, NULL, pm , true , idx , idx ); 
  menuControl->connectItem ( idx, this, SLOT ( slot_rename() ) ); ++idx;
  menuControl->insertSeparator(idx , idx); ++idx; 
  pm->load(ProgramDir+ IDB_IMAGE_PROPERTIES);
  menuControl->insertItem ( PROPERTIES, NULL, pm , true , idx , idx ); 
  menuControl->connectItem ( idx, this, SLOT ( slot_properties() ) ); ++idx;
  pm->load(ProgramDir+ IDB_IMAGE_ATTRS);
  menuControl->insertItem ( ATTRS, NULL, pm , true , idx , idx ); 
  menuControl->connectItem ( idx, this, SLOT ( slot_attrs() ) ); ++idx;
  menuControl->insertSeparator(idx , idx); ++idx;      
  pm->load(ProgramDir+ IDB_IMAGE_MKDIR);
  menuControl->insertItem ( MKDIR, NULL, pm , true , idx , idx ); 
  menuControl->connectItem ( idx, this, SLOT ( slot_makedir() ) ); 
      
  menuPersonalize = new ZOptionsMenu ( rect, softKey, NULL , 0);
  menuPersonalize->setItemSpacing ( 10 );
  pm->load(ProgramDir+ IDB_IMAGE_CUSTOMNAME);
  menuPersonalize->insertItem ( NAME, NULL, pm , true , 0 , 0 );
  menuPersonalize->connectItem ( 0, this, SLOT ( slot_personalizeName() ) );  
  pm->load(ProgramDir+ IDB_IMAGE_CUSTOMICON);
  menuPersonalize->insertItem ( ICON, NULL, pm , true , 1 , 1 ); 
  menuPersonalize->connectItem ( 1, this, SLOT ( slot_personalize() ) );

  menuApply = new ZOptionsMenu ( rect, softKey, NULL , 0);
  menuApply->setItemSpacing ( 10 );
  pm->load(ProgramDir+ IDB_IMAGE_RINGTONE);
  menuApply->insertItem ( RINGTONE, NULL, pm , true , 1 , 1 ); 
  menuApply->connectItem ( 1, this, SLOT ( applyRingtone() ) );
  pm->load(ProgramDir+ IDB_IMAGE_WALLPAPER);
  menuApply->insertItem ( WALLPAPER, NULL, pm , true , 2 , 2 ); 
  menuApply->connectItem ( 2, this, SLOT ( applyWallpaper() ) );

  menuSend = new ZOptionsMenu ( rect, softKey, NULL , 0);
  menuSend->setItemSpacing ( 10 );
  pm->load(ProgramDir+ IDB_IMAGE_SDCARD);
  menuSend->insertItem ( SDCARD, NULL, pm , true , 1 , 1 ); 
  menuSend->connectItem ( 1, this, SLOT ( sendtoDevice() ) );
  pm->load(ProgramDir+ IDB_IMAGE_TARFILE);
  menuSend->insertItem ( RARFILE, NULL, pm , true , 2 , 2 ); 
  menuSend->connectItem ( 2, this, SLOT ( sendRarFile() ) );
  pm->load(ProgramDir+ IDB_IMAGE_MENULINK);
  menuSend->insertItem ( MENULINK, NULL, pm , true , 3 , 3 ); 
  menuSend->connectItem ( 3, this, SLOT ( sendMenuLink() ) );

  menuFavs = new ZOptionsMenu ( rect, softKey, NULL , 0);
  menuFavs->setItemSpacing ( 10 );
  pm->load(ProgramDir+ IDB_IMAGE_FAV_ADD);
  menuFavs->insertItem ( ADDFAV, NULL, pm , true , 0 , 0 );
  menuFavs->connectItem ( 0, this, SLOT ( AgregarFav() ) );  
  pm->load(ProgramDir+ IDB_IMAGE_FAV_ADMIN);
  menuFavs->insertItem ( ADMINFAV, NULL, pm , true , 1 , 1 ); 
  menuFavs->connectItem ( 1, this, SLOT ( AdminFav() ) );

}

void ZLauncher::CreateMenu()
{
  QRect rect;
  ZConfig lang(Idioma, false);
  menuMain = new ZOptionsMenu ( rect, softKey, NULL , 0);
  menuMain->setItemSpacing ( 10 ); int idx = 0;
  QPixmap* pm  = new QPixmap(ProgramDir+ IDB_IMAGE_EXEC);
  menuMain->insertItem ( OPEN, NULL , pm, true , idx , idx );
  menuMain->connectItem ( idx, this, SLOT ( Open() ) ); ++idx;
  pm->load(ProgramDir+ IDB_IMAGE_EXEC);
  menuMain->insertItem ( OPENWITH, NULL , pm, true , idx , idx );
  menuMain->connectItem ( idx, this, SLOT ( OpenWith() ) ); ++idx;
  pm->load(ProgramDir+ IDB_IMAGE_FM);
  menuMain->insertItem ( CONTROL, menuControl, pm , true , idx , idx ); ++idx;
  pm->load(ProgramDir+ IDB_IMAGE_CONTROL);
  menuMain->insertItem ( PERSONALIZE, menuPersonalize, pm , true , idx , idx ); ++idx;
  pm->load(ProgramDir+ IDB_IMAGE_SENDTO);
  menuMain->insertItem ( SENDTO, menuSend, pm , true , idx , idx ); ++idx;
  pm->load(ProgramDir+ IDB_IMAGE_APPLYTO);
  menuMain->insertItem ( APPLYTO, menuApply, pm , true , idx , idx ); ++idx;
  pm->load(ProgramDir+ IDB_IMAGE_FAV);
  menuMain->insertItem ( FAVS, menuFavs, pm , true , idx , idx ); ++idx;
  pm->load(ProgramDir+ IDB_IMAGE_SEARCH);
  menuMain->insertItem ( SEARCH, NULL, pm , true , idx , idx ); 
  menuMain->connectItem ( idx, this, SLOT ( Buscar() ) ); ++idx;
  menuMain->insertSeparator(idx , idx); ++idx;
  pm->load(ProgramDir+ IDB_IMAGE_SETTINGS);
  menuMain->insertItem ( SETTINGS, NULL, pm , true , idx , idx ); 
  menuMain->connectItem ( idx, this, SLOT ( Settings() ) ); ++idx;
  pm->load(ProgramDir+ IDB_IMAGE_ABOUT);
  menuMain->insertItem ( ABOUT, NULL, pm , true , idx , idx ); 
  menuMain->connectItem ( idx, this, SLOT ( slot_about() ) ); ++idx;
  pm->load(ProgramDir+ IDB_IMAGE_EXIT);
  //menuMain->insertItem ( EXIT, NULL , pm, true , idx , idx );
  //menuMain->connectItem ( idx, qApp, SLOT ( quit() ) ); ++idx;

  connect(menuMain, SIGNAL(aboutToShow()), SLOT(checkearMenu()));


}

void ZLauncher::checkearMenu()
{
	QString app, fName;
	QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
	val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	if ( val != "big" )
	{ 
		app = browser->item ( browser->currentItem() )->getSubItemText ( 2, 0, true );
		if ( app == "" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
	}
	else { app = Bbrowser->currentItem()->text(); }
	fName = sPath + "/" + app;

	menuMain->setItemEnabled(0,true);
	menuMain->setItemEnabled(1,true);
	menuMain->setItemEnabled(3,true);
	menuMain->setItemEnabled(4,true);
	menuMain->setItemEnabled(5,true);
	menuControl->setItemEnabled(0,true);
	menuControl->setItemEnabled(1,true);
	menuControl->setItemEnabled(3,true);
	menuControl->setItemEnabled(4,true);
	menuControl->setItemEnabled(5,true);
	menuControl->setItemEnabled(6,true);
	menuControl->setItemEnabled(7,true);
	menuFavs->setItemEnabled(0,true);

	if ( app == ".." )
	{
		menuMain->setItemEnabled(0,false);
		menuMain->setItemEnabled(1,false);
		menuMain->setItemEnabled(3,false);
		menuMain->setItemEnabled(4,false);
		menuMain->setItemEnabled(5,false);
		menuControl->setItemEnabled(0,false);
		menuControl->setItemEnabled(1,false);
		menuControl->setItemEnabled(3,false);
		menuControl->setItemEnabled(4,false);
		menuControl->setItemEnabled(5,false);
		menuControl->setItemEnabled(6,false);
		menuControl->setItemEnabled(7,false);
		menuFavs->setItemEnabled(0,false);
		return;
	}

	if ( ActiveSel == "YES" ) {
		menuMain->setItemEnabled(0,false);
		menuMain->setItemEnabled(1,false);
		menuMain->setItemEnabled(3,false);
		menuMain->setItemEnabled(5,false);
		menuControl->setItemEnabled(6,false);
		menuControl->setItemEnabled(7,false);
		menuSend->setItemEnabled(3,false);
	}
	else {
		menuMain->setItemEnabled(0,true);
		menuMain->setItemEnabled(1,true);
		menuMain->setItemEnabled(3,true);
		menuMain->setItemEnabled(5,false);
		menuControl->setItemEnabled(6,true);
		menuControl->setItemEnabled(7,true);
		menuSend->setItemEnabled(3,true);

		if ( QFileInfo(fName).isFile() ) 
		{
			menuPersonalize->setItemEnabled(0,false);
			menuMain->setItemEnabled(5,true);
			QString Eval = fName;
			int i = Eval.findRev ( "." ); Eval.remove ( 0, i +1 );
			if ( (Eval.lower()=="jpg") || (Eval.lower()=="jpeg") || (Eval.lower()=="png") || (Eval.lower()=="gif") || (Eval.lower()=="bmp") )
				menuApply->setItemEnabled(2,true);
			else menuApply->setItemEnabled(2,false);
			if ( (Eval.lower()=="mp3") || (Eval.lower()=="wav") || (Eval.lower()=="m4a") || (Eval.lower()=="imy"))
				menuApply->setItemEnabled(1,true);
			else menuApply->setItemEnabled(1,false);
			menuFavs->setItemEnabled(0,false);
		}
		if ( QFileInfo(fName).isDir() ) 
		{
			menuMain->setItemEnabled(0,true);
			menuMain->setItemEnabled(1,false);
			menuPersonalize->setItemEnabled(0,true);
			menuFavs->setItemEnabled(0,true);
		}
	}

	checkClip();
	checkPlayer();
}

void ZLauncher::CreateSoftKeys()
{
  ZConfig lang(Idioma, false);
  softKey->setTextForOptMenuHide ( FUNCTIONS );
  softKey->setTextForOptMenuShow ( CHOISE, CANCEL );
  softKey->setText ( ZSoftKey::LEFT, FUNCTIONS, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::RIGHT, BACK, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setOptMenu ( ZSoftKey::LEFT, menuMain );
  softKey->setClickedSlot(ZSoftKey::RIGHT, this, SLOT( Atras() ));
}

void ZLauncher::Atras()
{
  if ( ActiveSel == "NO" )
  {
	  ZConfig cini(ProgramDir + "/foldername.cfg", true);	
	  QString ant = cini.readEntry(QString("FOLDERNAME"), sPath, "");
	  if ( ant == "") { ant = getFileName(sPath); }
	  int i = sPath.findRev ( "//" ); sPath.remove ( i, 1 );
	  if ( sPath.length() > 1 ) 
	  {
		QString nPath = sPath;
		int i = nPath.findRev ( "/" );
		nPath.remove ( i, nPath.length() - i );
		CargarBrowser( nPath );
		QString nombre;
		QString val; ZConfig fini(QString("%1/settings.cfg").arg(ProgramDir), false);
		val = fini.readEntry(QString("FM"), QString("IconStyle"), "");
		if ( val == "big" )
		{ 
		  int k2 = Bbrowser->count(); for( int k=0; k < k2; k++ )	 
		  {
			nombre = Bbrowser->item( k )->text();
			if ( nombre == ant ) {
			  ZIconViewItem* Item = Bbrowser->item( k );
			  Bbrowser->setSelected(Item, true, true);
			  Bbrowser->ensureCurrentVisible();
			}
		  }
		}
		else
		{
		  for( int k=0; k < browser->count(); k++ )	 
		  {
			ZListBoxItem* listitem = browser->item ( k );
			QString sapp = listitem->getSubItemText ( 1, 0, true );  
			if ( sapp == ant ) {
			  browser->setSelected( k, true);
			  browser->ensureCurrentVisible();
			}
		  }
		}
	 }
  }
  else
  {
	  QString app, fName;
	  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
	  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	  if ( val != "big" )
	  { 
		app = browser->item ( browser->currentItem() )->getSubItemText ( 2, 0, true );
		if ( app == "" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
	  }
	  else { app = Bbrowser->currentItem()->text(); }
	  fName = sPath + "/" + app;

	  if ( Accion == "Browser" )
	  {
		if ( app == ".." )
		{ 
		  ZConfig cini(ProgramDir + "/foldername.cfg", true);	
		  QString ant = cini.readEntry(QString("FOLDERNAME"), sPath, "");
		  if ( ant == "") { ant = getFileName(sPath); }
		  int i = sPath.findRev ( "//" ); sPath.remove ( i, 1 );
		  if ( sPath.length() > 1 ) 
		  {
			QString nPath = sPath;
			int i = nPath.findRev ( "/" );
			nPath.remove ( i, nPath.length() - i );
			CargarBrowser( nPath );
			QString nombre;
			QString val; ZConfig fini(QString("%1/settings.cfg").arg(ProgramDir), false);
			val = fini.readEntry(QString("FM"), QString("IconStyle"), "");
			if ( val == "big" )
			{ 
			  int k2 = Bbrowser->count(); for( int k=0; k < k2; k++ )	 
			  {
				nombre = Bbrowser->item( k )->text();
				if ( nombre == ant ) {
				  ZIconViewItem* Item = Bbrowser->item( k );
				  Bbrowser->setSelected(Item, true, true);
				  Bbrowser->ensureCurrentVisible();
				}
			  }
			}
			else
			{
			  for( int k=0; k < browser->count(); k++ )	 
			  {
				ZListBoxItem* listitem = browser->item ( k );
				QString sapp = listitem->getSubItemText ( 2, 0, true ); 
				if ( sapp == ant ) {
				  browser->setSelected( k, true);
				  browser->ensureCurrentVisible();
				}
			  }
			}
		  }
		}
		else 
		{
		  fName = RealName(fName);
		  if ( QFileInfo(fName).isDir() ) { CargarBrowser( fName ); return; }
		}
	  }
  }
}

void ZLauncher::slot_about()
{
  ZAboutDialog* aboutDlg = new ZAboutDialog();
  aboutDlg->exec();
  delete aboutDlg;
  aboutDlg = NULL;
}

QString ZLauncher::getProgramDir()
{
  return getFilePath(QString ( qApp->argv() [0] ));
}

QString ZLauncher::getFilePath(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( i+1, str.length() - i );
  return str;  
}

QString ZLauncher::getFileName(const QString &AFileName)
{
  QString str = AFileName;
  int i = str.findRev ( "/" );
  str.remove ( 0, i);
  if (str.left(1) = "/") str.remove ( 0, 1);
  return str;    
}

void ZLauncher::LoadSettings()
{
	ZConfig IDini(Idioma, false);	
	APPLYSUBS =  IDini.readEntry(QString("LANGUAGE"), QString("APPLYSUBS"), ""); 
	ASOCIATEFILE =  IDini.readEntry(QString("LANGUAGE"), QString("ASOCIATEFILE"), "");
	CONTROL =  IDini.readEntry(QString("LANGUAGE"), QString("CONTROL"), "");
	ABOUT =  IDini.readEntry(QString("LANGUAGE"), QString("ABOUT"), "");
	EXIT =  IDini.readEntry(QString("LANGUAGE"), QString("EXIT"), "");
	BACK =  IDini.readEntry(QString("LANGUAGE"), QString("BACK"), "");
	CANCEL =  IDini.readEntry(QString("LANGUAGE"), QString("CANCEL"), "");
	FUNCTIONS =  IDini.readEntry(QString("LANGUAGE"), QString("FUNCTIONS"), "");
	CHOISE =  IDini.readEntry(QString("LANGUAGE"), QString("CHOISE"), "");
	GOTO =  IDini.readEntry(QString("LANGUAGE"), QString("GOTO"), "");
	ERROR =  IDini.readEntry(QString("LANGUAGE"), QString("ERROR"), "");
	WARNING =  IDini.readEntry(QString("LANGUAGE"), QString("WARNING"), "");
	CLOSE_APP =  IDini.readEntry(QString("LANGUAGE"), QString("CLOSE_APP"), "");
	COPY =  IDini.readEntry(QString("LANGUAGE"), QString("COPY"), "");
	DELETE =  IDini.readEntry(QString("LANGUAGE"), QString("DELETE"), "");
	MOVE =  IDini.readEntry(QString("LANGUAGE"), QString("MOVE"), "");
	PASTE =  IDini.readEntry(QString("LANGUAGE"), QString("PASTE"), "");
	RENAME =  IDini.readEntry(QString("LANGUAGE"), QString("RENAME"), "");
	PROPERTIES =  IDini.readEntry(QString("LANGUAGE"), QString("PROPERTIES"), "");
	ATTRS =  IDini.readEntry(QString("LANGUAGE"), QString("ATTRS"), "");
	MKDIR =  IDini.readEntry(QString("LANGUAGE"), QString("MKDIR"), "");
	PERSONALIZE =  IDini.readEntry(QString("LANGUAGE"), QString("PERSONALIZE"), "");
	NAME =  IDini.readEntry(QString("LANGUAGE"), QString("NAME"), "");
	ICON =  IDini.readEntry(QString("LANGUAGE"), QString("ICON"), "");
	RESET =  IDini.readEntry(QString("LANGUAGE"), QString("RESET"), "");
	OK =  IDini.readEntry(QString("LANGUAGE"), QString("OK"), "");
	FAVS =  IDini.readEntry(QString("LANGUAGE"), QString("FAVS"), "");
	ADMINFAV =  IDini.readEntry(QString("LANGUAGE"), QString("ADMINFAV"), "");
	ADDFAV =  IDini.readEntry(QString("LANGUAGE"), QString("ADDFAV"), "");
	FAVNAME =  IDini.readEntry(QString("LANGUAGE"), QString("FAVNAME"), "");
	SELECTDEST =  IDini.readEntry(QString("LANGUAGE"), QString("SELECTDEST"), "");
	SELECTICON =  IDini.readEntry(QString("LANGUAGE"), QString("SELECTICON"), "");
	COPYING =  IDini.readEntry(QString("LANGUAGE"), QString("COPYING"), "");
	RENAMING =  IDini.readEntry(QString("LANGUAGE"), QString("RENAMING"), "");
	MOVEING =  IDini.readEntry(QString("LANGUAGE"), QString("MOVEING"), "");
	MAKING =  IDini.readEntry(QString("LANGUAGE"), QString("MAKING"), "");
	ENTER_FILENAME =  IDini.readEntry(QString("LANGUAGE"), QString("ENTER_FILENAME"), "");
	ENTER_DIRNAME =  IDini.readEntry(QString("LANGUAGE"), QString("ENTER_DIRNAME"), "");
	MKDIR_ERROR =  IDini.readEntry(QString("LANGUAGE"), QString("MKDIR_ERROR"), "");
	RENAME_ERROR =  IDini.readEntry(QString("LANGUAGE"), QString("RENAME_ERROR"), "");
	RENAME_FILEEXISTS =  IDini.readEntry(QString("LANGUAGE"), QString("RENAME_FILEEXISTS"), "");
	FOLDER =  IDini.readEntry(QString("LANGUAGE"), QString("FOLDER"), "");
	AFILE =  IDini.readEntry(QString("LANGUAGE"), QString("FILE"), "");
	OPEN =  IDini.readEntry(QString("LANGUAGE"), QString("OPEN"), "");
	OPENWITH =  IDini.readEntry(QString("LANGUAGE"), QString("OPENWITH"), "");
	PROPERTIES_SIZE = IDini.readEntry(QString("LANGUAGE"), QString("PROPERTIES_SIZE"), "");
	PROPERTIES_DATE = IDini.readEntry(QString("LANGUAGE"), QString("PROPERTIES_DATE"), "");
	PROPERTIES_OWNER = IDini.readEntry(QString("LANGUAGE"), QString("PROPERTIES_OWNER"), "");
	PROPERTIES_GROUP = IDini.readEntry(QString("LANGUAGE"), QString("PROPERTIES_GROUP"), "");
	PROPERTIES_ATTRS = IDini.readEntry(QString("LANGUAGE"), QString("PROPERTIES_ATTRS"), "");
	PROPERTIES_RIGHTS = IDini.readEntry(QString("LANGUAGE"), QString("PROPERTIES_RIGHTS"), "");
	DELETE_DIR =  IDini.readEntry(QString("LANGUAGE"), QString("DELETE_DIR"), "");
	DELETE_FILE =  IDini.readEntry(QString("LANGUAGE"), QString("DELETE_FILE"), "");
	DELETE_MULTIPLE =  IDini.readEntry(QString("LANGUAGE"), QString("DELETE_MULTIPLE"), "");
	UNABLE_DELETE =  IDini.readEntry(QString("LANGUAGE"), QString("UNABLE_DELETE"), "");
	ERROR_GET_FREE_SPACE =  IDini.readEntry(QString("LANGUAGE"), QString("ERROR_GET_FREE_SPACE"), "");
	ERROR_NON_FREE_SPACE =  IDini.readEntry(QString("LANGUAGE"), QString("ERROR_NON_FREE_SPACE"), "");
	PROGRESS_PERCENT =  IDini.readEntry(QString("LANGUAGE"), QString("PROGRESS_PERCENT"), "");
	SENDTO = IDini.readEntry(QString("LANGUAGE"), QString("SENDTO"), "");
	RARFILE = IDini.readEntry(QString("LANGUAGE"), QString("RARFILE"), "");
	RARCREATING = IDini.readEntry(QString("LANGUAGE"), QString("RARCREATING"), "");
	PHONE = IDini.readEntry(QString("LANGUAGE"), QString("PHONE"), "");
	SDCARD = IDini.readEntry(QString("LANGUAGE"), QString("SDCARD"), "");
	APPLYTO = IDini.readEntry(QString("LANGUAGE"), QString("APPLYTO"), "");
	APPLY_ERROR = IDini.readEntry(QString("LANGUAGE"), QString("APPLY_ERROR"), "");
	RINGTONE = IDini.readEntry(QString("LANGUAGE"), QString("RINGTONE"), "");
	WALLPAPER = IDini.readEntry(QString("LANGUAGE"), QString("WALLPAPER"), "");
	SETTINGS = IDini.readEntry(QString("LANGUAGE"), QString("SETTINGS"), "");
	SEARCH = IDini.readEntry(QString("LANGUAGE"), QString("SEARCH"), "");
	MENULINK = IDini.readEntry(QString("LANGUAGE"), QString("MENULINK"), "");

}

void ZLauncher::slot_clipboard()
{

  QString ARCHIVOS; QFile entrada("/tmp/clipboard"); QTextStream stentrada(&entrada);
  if ( entrada.open(IO_ReadOnly | IO_Translate) )
  {
	while ( !stentrada.eof() )
    { QString line = stentrada.readLine();
	  ARCHIVOS += line + "\n";
	} entrada.close();
  }

	if ( ActiveSel == "YES" )
	{
	  QStringList archivos;
	  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
	  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	  if ( val == "big" )
	  { 
		int k2 = Bbrowser->count(); for( int k=0; k < k2; k++ )	 
		{
		  if ( Bbrowser->itemChecked(k) ) {
			QString sapp = Bbrowser->item( k )->text();
			QString nnn = sapp; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { sapp.remove( sapp.length() -1, 1 ); }
			QString fName = sPath + "/" + sapp;
			int encontro = fName.find( "//" , 0 );
			if ( encontro > -1 ) { fName.remove( encontro,1); }
			
			encontro = ARCHIVOS.find( fName, 0 );			
			if ( encontro == -1 ) { archivos += QString(fName); }

		  }
		}
	  }
	  else
	  {
		for( int k=0; k < browser->count(); k++ )	 
		{
		  if ( browser->itemChecked(k) ) {
			QString sapp = browser->item( k )->getSubItemText ( 2, 0, true );  
			QString nnn = sapp; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { sapp.remove( sapp.length() -1, 1 ); }
			QString fName = sPath + "/" + sapp;
			int encontro = fName.find( "//" , 0 );
			if ( encontro > -1 ) { fName.remove( encontro,1); }
			encontro = ARCHIVOS.find( fName, 0 );			
			if ( encontro == -1 ) { archivos += QString(fName); }
		  }
		}
	  }
	  if ( archivos.count() > 0 )
	  {
		int k2 = archivos.count(); for( int k=0; k < k2; k++ )	
		{
		  QString app = archivos[k];
		  QString FullName = app;
		  int encontro = FullName.find( "//" , 0 );
		  if ( encontro > -1 ) { FullName.remove( encontro,1); }
		  FullName = RealName(FullName);
		  if ( app != ".." ) {  
			  //qDebug(QString("PORTAPAPELES: " + FullName));
			  CopyPath=sPath;
			  system(QString("echo \"%1\" >> /tmp/clipboard").arg(FullName));
		  }  
		}
	  }

	}
	else
	{
	  QString app;
	  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
	  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	  if ( val != "big" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
	  else { app = Bbrowser->currentItem()->text(); }
	  QString nnn = app; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { app.remove( app.length() -1, 1 ); }
	  QString FullName = sPath + "/" + app;
	  int encontro = FullName.find( "//" , 0 );
	  if ( encontro > -1 ) { FullName.remove( encontro,1); }
	  FullName = RealName(FullName);
	  if ( app != ".." ) {  
		  //qDebug(QString("PORTAPAPELES: " + FullName));
		  CopyPath=sPath;
		  encontro = ARCHIVOS.find( FullName, 0 );			
		  if ( encontro == -1 ) { system(QString("echo \"%1\" >> /tmp/clipboard").arg(FullName)); }
	  }  
	} 
}    

void ZLauncher::slot_copy()
{

	if ( ActiveSel == "YES" )
	{
	  QStringList archivos;
	  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
	  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	  if ( val == "big" )
	  { 
		int k2 = Bbrowser->count(); for( int k=0; k < k2; k++ )	 
		{
		  if ( Bbrowser->itemChecked(k) ) {
			QString sapp = Bbrowser->item( k )->text();
			QString nnn = sapp; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { sapp.remove( sapp.length() -1, 1 ); }
			QString fName = sPath + "/" + sapp;
			int encontro = fName.find( "//" , 0 );
			if ( encontro > -1 ) { fName.remove( encontro,1); }
			archivos += QString(fName);
		  }
		}
	  }
	  else
	  {
		for( int k=0; k < browser->count(); k++ )	 
		{
		  if ( browser->itemChecked(k) ) {
			QString sapp = browser->item( k )->getSubItemText ( 2, 0, true );
			QString nnn = sapp; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { sapp.remove( sapp.length() -1, 1 ); } 
			QString fName = sPath + "/" + sapp;
			int encontro = fName.find( "//" , 0 );
			if ( encontro > -1 ) { fName.remove( encontro,1); }
			archivos += QString(fName);
			//qDebug(QString("Agregando: " + fName));
		  }
		}
	  }
	  if ( archivos.count() > 0 )
	  {
		system(QString("rm /tmp/clipboard"));
		system(QString("touch /tmp/clipboard"));
		int k2 = archivos.count(); for( int k=0; k < k2; k++ )	
		{
		  QString app = archivos[k];
		  QString FullName = app;
		  int encontro = FullName.find( "//" , 0 );
		  if ( encontro > -1 ) { FullName.remove( encontro,1); }
		  FullName = RealName(FullName);
		  if ( app != ".." ) {  
			  //qDebug(QString("PORTAPAPELES: " + FullName));
			  CopyPath=sPath;
			  system(QString("echo \"%1\" >> /tmp/clipboard").arg(FullName));
		  }  
		}
	  }

	}
	else
	{
	  QString app;
	  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
	  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	  if ( val != "big" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
	  else { app = Bbrowser->currentItem()->text(); }

	  QString FullName = sPath + "/" + app;
	  int encontro = FullName.find( "//" , 0 );
	  if ( encontro > -1 ) { FullName.remove( encontro,1); }
	  FullName = RealName(FullName);
	  if ( app != ".." ) {  
		  //qDebug(QString("PORTAPAPELES: " + FullName));
		  CopyPath=sPath;
		  system(QString("echo \"%1\" > /tmp/clipboard").arg(FullName));
	  }  
	} 

	checkClip();
	checkPlayer();

}    

void ZLauncher::slot_paste()
{
  QString folder; QFile entrada("/tmp/clipboard"); QTextStream stentrada(&entrada);
  if ( entrada.open(IO_ReadOnly | IO_Translate) )
  {
	while ( !stentrada.eof() )
    {           
	  QString line = stentrada.readLine();

	  if ( QFileInfo(line).isFile() ) 
	  {
		  PasteFiles(line, sPath, false);
	  }
	  else
	  {
		system(QString("find \"%1\" > /tmp/clipboard2").arg(line));
		QString folder; QFile entrada2("/tmp/clipboard2"); QTextStream stentrada2(&entrada2);
		if ( entrada2.open(IO_ReadOnly | IO_Translate) ) {
		  while ( !stentrada2.eof() ) {           
			QString line2 = stentrada2.readLine();
			QString destino = getFilePath(line2);
			int k = destino.find ( CopyPath, 0 ); 
			destino.remove( k, CopyPath.length() );
			destino = sPath + "/" + destino;
			if ( QFileInfo(line2).isFile() ) { PasteFiles(line2, destino, false); }
			else {
				QString carpeta = line2;
				int i = carpeta.find ( CopyPath, 0 );
				carpeta.remove( i, CopyPath.length() );
				QDir dd; if ( ! dd.exists(sPath + carpeta) ) { dd.mkdir(sPath + carpeta); }
			}
		  }
		}  entrada2.close();
	  }
	}
  }
  CargarBrowser(sPath);
}


bool ZLauncher::PasteFiles(const QString &ASourse, const QString &ADest, bool remove)
{
  if (ASourse.isEmpty() ) return false;
  if (ADest.isEmpty() ) return false;
  
  //qDebug(QString("PasteFiles started..."));
  ZConfig lang(Idioma, false);
  
  struct statfs stat;
  unsigned long freeSize = 0;  
  int TargetSize = 0;
  
  QFileInfo* fInfo = new QFileInfo ( ASourse );
  QString a = getFileName(ASourse);
  //qDebug(QString("a: %1").arg(a));
  
  QString destName = ADest;
  //qDebug(QString("destName: %1").arg(destName));
  //destName.prepend(a);

  
// check free space
  if ( statfs ( destName, &stat ) < 0 ) {
    ZMessageDlg* dlgw  = new ZMessageDlg ( COPYING, ERROR_GET_FREE_SPACE,  ZMessageDlg::just_ok, 0 , this , "dlgw" , true , 0 );
    dlgw->exec();
    delete dlgw;
  } else {
    
    destName.append("/" + a);
    //qDebug(QString("destName: %1").arg(destName));
       
    freeSize = stat.f_bsize*stat.f_bfree;
    //qDebug(QString("FreeSize: %1").arg(freeSize));
    QFileInfo* fInfoSource = new QFileInfo ( ASourse );
    if ( fInfoSource->isDir() ) {
      //qDebug(QString("Source is Dir..."));
      TargetSize = GetFolderSize ( ASourse );
      //qDebug(QString("Source size: %1").arg(TargetSize));
      int cntFiles = GetFilesCountInFolder( ASourse );
      //qDebug(QString("GetFilesCountInFolder(): %1").arg(cntFiles));
      if ( ( freeSize > TargetSize ) && ( TargetSize >= 0 ) ) {
		
        QStringList fullFiles;
        QStringList::Iterator it;
        fullFiles = GetFilesInFolder( ASourse );        
        if (fullFiles.count() > 0)
        {
          it = fullFiles.begin();
                                
          while ( it != fullFiles.end() ) {
            //qDebug("---------------------------------------------");
            //qDebug("a " + a);
            //qDebug("SourceFileName " + ASourse);
            //qDebug("*it: " + *it);
            QString b = *it;
            int k = b.findRev(ASourse);
            //qDebug("DestFileName: " + ASourse);
            //qDebug(QString("k: %1").arg(k));
            b.remove ( k, ASourse.length()-k);
            //qDebug("b " + b);
            QDir dd;
            QString ss;
            ss = ADest + b;
            int ii = ss.findRev ( "/" );
            ss.remove ( ii+1, ss.length() - ii );
			ss = destName;
			//qDebug("Folder: " + ss);
            if ( ! dd.exists(ss)) {
              dd.mkdir(ss);
            }
			if ( QFileInfo(destName).isDir() ) { CopyFile ( *it, destName + "/" + b); }
            else { CopyFile ( *it, ADest + b); }
            ++it;
          }
        }
        else { QString b;
          int k = b.findRev(ASourse);
          b.remove ( k, ASourse.length()-k);
          //qDebug("SourceFileName: " + ASourse);
          //qDebug("DestFileName: " + fInfo->filePath());
		  CopyFile ( ASourse, ADest + b);
          //system(QString("cp %1 %2").arg(ASourse.utf8()).arg(QString(ADest + b).utf8()));}
        }                        
        if ( remove ) {
			QFile f;
			f.remove ( ASourse );
        }                
      }
    } else {
      if ( fInfoSource->isFile() ) {
        //qDebug(QString("Source is file..."));
        TargetSize = fInfoSource->size(); 
        //qDebug(QString("Target size single file: %1").arg(TargetSize));  
      }
    }
    
    if ( ( freeSize > TargetSize ) && ( TargetSize >= 0 ) ) 
    {
      //qDebug(QString("Start copying files"));
      //qDebug(QString("CopyFile:\nASource: %1\nADest: %2").arg(ASourse).arg(ADest));
      CopyFile ( ASourse, ADest + "/" + a);
      if ( remove ) {
        QFile f;
        f.remove ( ASourse );
      }
    } else {
      ZMessageDlg* dlgw  = new ZMessageDlg ( COPYING, ERROR_NON_FREE_SPACE, ZMessageDlg::just_ok, 0 , this , "dlgw" , true , 0 );
      dlgw->exec();
      delete dlgw;
    }    
  }
}
/*
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
*/
int  ZLauncher::GetFolderSize (const QString &path )
{
  //recursive
  QDir dir ( path );
  QStringList::Iterator it;
  int size = 0;
  QStringList files = dir.entryList ( "*", QDir::Files );
  it = files.begin();
  while ( it != files.end() ) {
    size += QFileInfo ( path, *it ).size();
    ++it;
  }
  QStringList dirs = dir.entryList ( QDir::Dirs );
  it = dirs.begin();
  while ( it != dirs.end() ) {
    if ( *it != "." && *it != ".." )
      size += GetFolderSize ( path + "/" + *it );
    ++it;
  }
  //qDebug(QString("FolderSize: %1").arg(size));
  return size; 
}

int ZLauncher::GetFilesCountInFolder ( const QString &path )
{
  //recursive
  QDir dir ( path );
  QStringList::Iterator it;
  int cnt = 0;
  QStringList files = dir.entryList ( "*", QDir::Files );
  it = files.begin();
  while ( it != files.end() ) {
    cnt++;
    ++it;
  }
  QStringList dirs = dir.entryList ( QDir::Dirs );
  it = dirs.begin();
  while ( it != dirs.end() ) {
    if ( *it != "." && *it != ".." )
      cnt += GetFolderSize ( path + "/" + *it );
    ++it;
  }
  return cnt;  
}

QStringList ZLauncher::GetFilesInFolder(const QString &path)
{
  //recursive
  QDir dir ( path );
  QStringList::Iterator it;
  QStringList fullFiles;
  QStringList files = dir.entryList ( "*", QDir::Files );
  it = files.begin();
  while ( it != files.end() ) {
    fullFiles += QFileInfo ( path, *it ).filePath();
    ++it;
  }
  QStringList dirs = dir.entryList ( QDir::Dirs );
  it = dirs.begin();
  while ( it != dirs.end() ) {
    if ( *it != "." && *it != ".." )
      fullFiles += GetFilesInFolder ( path + "/" + *it );
    ++it;
  }
  return fullFiles;  
}

bool  ZLauncher::CopyFile ( QString ASourceName, QString ADestName )
{

  if ( QFileInfo(ADestName).isFile() )
  {

  }
  else
  {
	  ZConfig lang(Idioma, false);
	  ZProgressDlg* zp = new ZProgressDlg ( " ", " ", 1, NULL, NULL, true, ( ZSkinService::WidgetClsID ) 4 );
	  ZMeter* zm = zp->getMeter();
	  zm->setMaximumHeight(200);
	  QFileInfo* fInfo = new QFileInfo ( ASourceName );
	  if ( fInfo->isDir() ) { // copy directory
		zp->setTitle ( COPYING );
	  } else if ( fInfo->isFile() ) { //copy file
		long size = fInfo->size();
		long size2 = fInfo->size();
		if ( size == 0 ) {
		  delete zp;
		  return FALSE;
		} else {

		  zp->reset();
		  zp->setTitle ( COPYING );
		  zp->setInstructText(QString("%1\n").arg(ASourceName));
		                    //zp->setProgressTotalStep ( 0);
		  zm->setTotalSteps(10);
		  zm->setMaximumWidth(230);
		  zm->setFixedWidth(230);

		  QFile fDest ( ADestName );
		  QFile fSource ( ASourceName );
		  int blockSize = 1024;
		  unsigned long i = 0;
		  double u = 0;
		                    
		  zp->show();
		  qApp->processEvents();
		  char* p=NULL;
		  if ( fSource.open ( IO_ReadOnly ) ) {
		    p=new char[blockSize];
		    fDest.open ( IO_WriteOnly );
		    do {
		                                    
		      size = fSource.readBlock ( p, blockSize );
		      if ( size == -1 ) {
		        delete zp;
		        return FALSE;
		      }
		      if (fDest.writeBlock(p, size) == -1) {
		        delete zp;
		        return FALSE;
		      }
		      u = 100 * (double)i / size2 / 10;
		      zm->setProgress((int)u);
		      zp->setInstructText(QString("%1\n").arg(ASourceName) + QString( PROGRESS_PERCENT ).arg((int)u*10));
		      qApp->processEvents();
		      i += size;
		    } while (size == blockSize);
		    delete[] p;
		  }
		  
		  qApp->processEvents();
		}
	  }
	  delete zp;
	  return TRUE;
  }
}

bool ZLauncher::DeleteFile ( QString AFileName )
{

  QFileInfo* fInfo = new QFileInfo ( AFileName );
  if ( fInfo->isFile() ) {
    QFile f;
    if ( f.exists ( AFileName ) ) {
      f.remove ( AFileName );
      return true;
    } else return false;
  } else {
    QDir d;
    if ( d.exists ( AFileName, true ) ) {
                        //d.rmdir ( AFileName, true );
                        // delete folder recursive (with files and subdirs)
      //qDebug ( "Deleting directory " + AFileName );
      system ("rm -f -r \"" + AFileName.utf8() + "\"");

	  if ( QFileInfo(AFileName).isDir() )
	  {
		  ZConfig Cini(ProgramDir + "/foldername.cfg", true);
		  QString ico = Cini.readEntry(QString("FOLDERNAME"), AFileName, "");
		  Cini.removeEntry(QString("FOLDERNAME"), AFileName );
		  Cini.removeEntry(QString("CUSTOMNAME"), getFilePath(AFileName) + ico );
		  QString destino; ZConfig IC(ProgramDir + "/settings.cfg", true);
		  QString SIcon = IC.readEntry(QString("FM"), QString("IconSource"), "");
		  if ( SIcon=="0" ) { destino="/foldericons.cfg"; } else { destino="/sfoldericons.cfg"; }
		  ZConfig Sini(ProgramDir + destino, true);	
		  Sini.removeEntry(QString("FOLDERICONS"), AFileName );
	  }
      ;
       return true;
    } else return false;
  }  
}

void ZLauncher::slot_delete()
{

	ZConfig lang(Idioma, false);
	if ( ActiveSel == "YES" )
	{
	  QStringList archivos;
	  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
	  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	  if ( val == "big" )
	  { 
		int k2 = Bbrowser->count(); for( int k=0; k < k2; k++ )	 
		{
		  if ( Bbrowser->itemChecked(k) ) {
			QString sapp = Bbrowser->item( k )->text();
			QString nnn = sapp; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { sapp.remove( sapp.length() -1, 1 ); }
			QString fName = sPath + "/" + sapp;
			int encontro = fName.find( "//" , 0 );
			if ( encontro > -1 ) { fName.remove( encontro,1); }
			fName = RealName(fName);
			archivos += QString(fName);
			//qDebug(QString("Agregando: " + fName));
		  }
		}
	  }
	  else
	  {
		for( int k=0; k < browser->count(); k++ )	 
		{
		  if ( browser->itemChecked(k) ) {
			QString sapp = browser->item( k )->getSubItemText ( 2, 0, true );  
			QString nnn = sapp; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { sapp.remove( sapp.length() -1, 1 ); }
			QString fName = sPath + "/" + sapp;
			int encontro = fName.find( "//" , 0 );
			if ( encontro > -1 ) { fName.remove( encontro,1); }
			fName = RealName(fName);
			archivos += QString(fName);
		  }
		}
	  }
	  if ( archivos.count() > 1 )
	  {
		bool isDelete = false;
		int k2 = archivos.count();
		ZMessageDlg* dlg1  = new ZMessageDlg ( QString(APP_NAME), QString(DELETE_MULTIPLE), ZMessageDlg::yes_no , 0 , this , "dlg1" , true , 0 );
		if (dlg1->exec()) { isDelete = true; }
		delete dlg1;
		if (isDelete)
		{
		  for( int k=0; k < k2; k++ )
		  {
			QString fName = archivos[k];
			if ( ! fName.isEmpty() ) 
			{
			  if ( ! DeleteFile(fName) ) 
			  {
				ZMessageDlg* dlgz  = new ZMessageDlg ( ERROR, UNABLE_DELETE, ZMessageDlg::just_ok, 0 , this , "dlgz" , true , 0 );
				dlgz->exec(); 
				delete dlgz;
			  }
			}
		  }
		}
		CargarBrowser(sPath); 
	  }
	  else if ( archivos.count() == 1 )
	  {
		QString fName = archivos[0];
		if ( fName.isEmpty() ) return;
		bool isDelete = false;
		int k2 = archivos.count();
		ZMessageDlg* dlg1  = new ZMessageDlg ( QString(APP_NAME), QString(DELETE_FILE).arg(fName), ZMessageDlg::yes_no , 0 , this , "dlg1" , true , 0 );
		if (dlg1->exec()) { isDelete = true; }
		delete dlg1;
		if (isDelete)
		{
		  if ( ! DeleteFile(fName) ) 
		  {
			ZMessageDlg* dlgz  = new ZMessageDlg ( ERROR, UNABLE_DELETE, ZMessageDlg::just_ok, 0 , this , "dlgz" , true , 0 );
			dlgz->exec(); 
			delete dlgz;
		  }
		}
		CargarBrowser(sPath); 
	  }
	}
	else
	{
	  QString app;
	  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
	  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	  if ( val != "big" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
	  else { app = Bbrowser->currentItem()->text(); }
	  QString nnn = app; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { app.remove( app.length() -1, 1 ); }

	  QString current = sPath;
	  QString fName = sPath + "/" + app;
	  fName = RealName(fName);

	  int encontro = fName.find( "//" , 0 );
	  if ( encontro > -1 ) { fName.remove( encontro,1); }

	  //qDebug(QString("***********************\nDelete file/folder: %1").arg(fName));
	  if ( fName.isEmpty() ) return;
	  bool isDelete = false;
	  ZMessageDlg* dlg1  = new ZMessageDlg ( DELETE, QString(DELETE_FILE).arg(fName), ZMessageDlg::yes_no , 0 , this , "dlg1" , true , 0 );
	  if (dlg1->exec()) { isDelete = true; }
	  delete dlg1;
	  if (isDelete)
	  {
		  if ( ! DeleteFile(fName) ) 
		  {
		    ZMessageDlg* dlgz  = new ZMessageDlg ( ERROR, UNABLE_DELETE, ZMessageDlg::just_ok, 0 , this , "dlgz" , true , 0 );
		    dlgz->exec(); 
		    delete dlgz;
		  } else {
		   CargarBrowser(sPath); 
		  }
	  }  
	}
   
}

void ZLauncher::slot_move()
{
	ZConfig lang(Idioma, false);
	struct statfs stat;
	unsigned long freeSize = 0;  
	int TargetSize = 0;

	if ( ActiveSel == "YES" )
	{
	  QStringList archivos;
	  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
	  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	  if ( val == "big" )
	  { 
		int k2 = Bbrowser->count(); for( int k=0; k < k2; k++ )	 
		{
		  if ( Bbrowser->itemChecked(k) ) {
			QString sapp = Bbrowser->item( k )->text();
			QString nnn = sapp; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { sapp.remove( sapp.length() -1, 1 ); }
			QString fName = sPath + "/" + sapp;
			int encontro = fName.find( "//" , 0 );
			if ( encontro > -1 ) { fName.remove( encontro,1); }
			archivos += QString(fName);
		  }
		}
	  }
	  else
	  {
		for( int k=0; k < browser->count(); k++ )	 
		{
		  if ( browser->itemChecked(k) ) {
			QString sapp = browser->item( k )->getSubItemText ( 2, 0, true ); 
			QString nnn = sapp; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { sapp.remove( sapp.length() -1, 1 ); } 
			QString fName = sPath + "/" + sapp;
			int encontro = fName.find( "//" , 0 );
			if ( encontro > -1 ) { fName.remove( encontro,1); }
			archivos += QString(fName);
			//qDebug(QString("Agregando: " + fName));
		  }
		}
	  }

      
	  if ( archivos.count() > 0 )
	  { 
		int k2 = archivos.count(); for( int k=0; k < k2; k++ ) { TargetSize += GetFolderSize(archivos[k]); }

		setMainWidgetTitle(MOVEING);
		ZFileOpenDialog *zfod = new ZFileOpenDialog(true);
		if (zfod->exec() ) { 
		  QString fDestPath = zfod->getFilePath();  
		  if ( ! fDestPath.isEmpty() )
		  {
			if ( statfs ( fDestPath, &stat ) < 0 ) {
				//qDebug(QString("%1").arg(ERROR_GET_FREE_SPACE));
				ZMessageDlg* dlgw  = new ZMessageDlg ( MOVEING, ERROR_GET_FREE_SPACE,  ZMessageDlg::just_ok, 0 , this , "dlgw" , true , 0 );
				dlgw->exec();
				delete dlgw;
			} else {
			  statfs ( fDestPath, &stat );
			  freeSize = stat.f_bsize*stat.f_bfree;
	  		  //qDebug(QString("ESPACIO: %1 - %2").arg(TargetSize).arg(freeSize));

			  if ( ( freeSize > TargetSize ) && ( TargetSize >= 0 ) )
			  {
				int k2 = archivos.count(); for( int k=0; k < k2; k++ )
				{
				  QString app = archivos[k];
				  QString FullName = app;
				  int encontro = FullName.find( "//" , 0 );
				  if ( encontro > -1 ) { FullName.remove( encontro,1); }
				  FullName = RealName(FullName);
				  //qDebug(QString("MOVIENDO: " + FullName + " to " + fDestPath));
				  system(QString("mv %1 %2").arg(FullName).arg(fDestPath));
					app = getFileName(app);
					ZConfig Cini(ProgramDir + "/foldername.cfg", true);	
					QString sico = Cini.readEntry(QString("FOLDERNAME"), FullName, "");
					if ( sico != "" ) {
						Cini.removeEntry(QString("FOLDERNAME"), FullName );
						QString oName = getFilePath(FullName) + sico;
						sico = Cini.readEntry(QString("CUSTOMNAME"), oName, "");
						Cini.removeEntry(QString("CUSTOMNAME"), oName );
						QString temp = getFileName(FullName);
						Cini.writeEntry(QString("FOLDERNAME"), fDestPath + "/" + temp, app);
						Cini.writeEntry(QString("CUSTOMNAME"), fDestPath + "/" + app, fDestPath + "/" + temp);
					}
					QString destino; ZConfig IC(ProgramDir + "/settings.cfg", true);
					QString SIcon = IC.readEntry(QString("FM"), QString("IconSource"), "");
					if ( SIcon=="0" ) { destino="/foldericons.cfg"; } else { destino="/sfoldericons.cfg"; }
					ZConfig Sini(ProgramDir + destino, true);	
					QString ico = Sini.readEntry(QString("FOLDERICONS"), FullName, "");
					if ( ico != "" ) {
						Sini.removeEntry(QString("FOLDERICONS"), FullName );
						QString temp = getFileName(FullName);
						Sini.writeEntry(QString("FOLDERICONS"), fDestPath + "/" + temp, ico );
					}

				}
				CargarBrowser(sPath);
			  }
			  else
			  {
      			ZMessageDlg* dlgw  = new ZMessageDlg ( MOVEING, ERROR_NON_FREE_SPACE, ZMessageDlg::just_ok, 0 , this , "dlgw" , true , 0 );
			    dlgw->exec();
			    delete dlgw;
			  }
			}
		  }
	    }
		delete zfod;
		setMainWidgetTitle(QString(APP_NAME));
	  }

	}
	else
	{
	  QString app, fName;
	  sPath = RealName(sPath);
	  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
	  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	  if ( val != "big" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
	  else { app = Bbrowser->currentItem()->text(); }
	  QString nnn = app; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { app.remove( app.length() -1, 1 ); }

	  fName = sPath + "/" + app;
	  TargetSize += QFileInfo(fName).size();
	  fName = RealName(fName);
	  if (fName.isEmpty()) return;
		setMainWidgetTitle(MOVEING);
		ZFileOpenDialog *zfod = new ZFileOpenDialog(true);
		if (zfod->exec() ) { 
		  QString fDestPath = zfod->getFilePath();  
		  if ( ! fDestPath.isEmpty() )
		  {
			if ( statfs ( fDestPath, &stat ) < 0 ) {
				//qDebug(QString("%1").arg(ERROR_GET_FREE_SPACE));
				ZMessageDlg* dlgw  = new ZMessageDlg ( MOVEING, ERROR_GET_FREE_SPACE,  ZMessageDlg::just_ok, 0 , this , "dlgw" , true , 0 );
				dlgw->exec();
				delete dlgw;
			} else {
			  statfs ( fDestPath, &stat );
			  freeSize = stat.f_bsize*stat.f_bfree;
	  		  //qDebug(QString("ESPACIO: %1 - %2").arg(TargetSize).arg(freeSize));

			  if ( ( freeSize > TargetSize ) && ( TargetSize >= 0 ) )
			  {
				  //qDebug(QString("MOVIENDO: " + fName + " to " + fDestPath));
				  system(QString("mv %1 %2").arg(fName).arg(fDestPath));

					ZConfig Cini(ProgramDir + "/foldername.cfg", true);	
					QString sico = Cini.readEntry(QString("FOLDERNAME"), fName, "");
					if ( sico != "" ) {
						Cini.removeEntry(QString("FOLDERNAME"), fName );
						QString oName = getFilePath(fName) + sico;
						sico = Cini.readEntry(QString("CUSTOMNAME"), oName, "");
						Cini.removeEntry(QString("CUSTOMNAME"), oName );
						QString temp = getFileName(fName);
						Cini.writeEntry(QString("FOLDERNAME"), fDestPath + "/" + temp, app);
						Cini.writeEntry(QString("CUSTOMNAME"), fDestPath + "/" + app, fDestPath + "/" + temp);
					}
					QString destino; ZConfig IC(ProgramDir + "/settings.cfg", true);
					QString SIcon = IC.readEntry(QString("FM"), QString("IconSource"), "");
					if ( SIcon=="0" ) { destino="/foldericons.cfg"; } else { destino="/sfoldericons.cfg"; }
					ZConfig Sini(ProgramDir + destino, true);	
					QString ico = Sini.readEntry(QString("FOLDERICONS"), fName, "");
					if ( ico != "" ) {
						Sini.removeEntry(QString("FOLDERICONS"), fName );
						QString temp = getFileName(fName);
						Sini.writeEntry(QString("FOLDERICONS"), fDestPath + "/" + temp, ico );
					}

				  CargarBrowser(sPath);
			  }
			  else
			  {
      			ZMessageDlg* dlgw  = new ZMessageDlg ( MOVEING, ERROR_NON_FREE_SPACE, ZMessageDlg::just_ok, 0 , this , "dlgw" , true , 0 );
			    dlgw->exec();
			    delete dlgw;
			  }
			}
		  }
	    }
		delete zfod;
		setMainWidgetTitle( QString(APP_NAME) );
	}

}


  
void ZLauncher::slot_rename()
{
	ZConfig lang(Idioma,false);
	if ( ActiveSel == "YES" )
	{
	  QStringList archivos;
	  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
	  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	  if ( val == "big" )
	  { 
		int k2 = Bbrowser->count(); for( int k=0; k < k2; k++ )	 
		{
		  if ( Bbrowser->itemChecked(k) ) {
			QString sapp = Bbrowser->item( k )->text();
			QString nnn = sapp; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { sapp.remove( sapp.length() -1, 1 ); }
			QString fName = sPath + "/" + sapp;
			int encontro = fName.find( "//" , 0 );
			if ( encontro > -1 ) { fName.remove( encontro,1); }
			archivos += QString(fName);
		  }
		}
	  }
	  else
	  {
		for( int k=0; k < browser->count(); k++ )	 
		{
		  if ( browser->itemChecked(k) ) {
			QString sapp = browser->item( k )->getSubItemText ( 2, 0, true );  
			QString nnn = sapp; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { sapp.remove( sapp.length() -1, 1 ); }
			QString fName = sPath + "/" + sapp;
			int encontro = fName.find( "//" , 0 );
			if ( encontro > -1 ) { fName.remove( encontro,1); }
			archivos += QString(fName);
			//qDebug(QString("Agregando: " + fName));
		  }
		}
	  }
	  if ( archivos.count() > 0 )
	  {
		int k2 = archivos.count(); for( int k=0; k < k2; k++ )
		{
		  repeat1:  
		  QString FullName = archivos[k];
		  QString oldName =  FullName;
		  QString app = getFileName(FullName);
		  FullName = RealName(FullName);
		  QString tmpName =  FullName;

		  if (FullName.isEmpty()) return;
		  //qDebug(QString("FullName: %1").arg(FullName));  
		  QString fName = getFileName(FullName);
		  //qDebug(QString("fName: %1").arg(fName));  
		  QString fPath = getFilePath(FullName);
		  //qDebug(QString("fPath: %1").arg(fPath));    
		  
		  ZSingleCaptureDlg* zscd = new ZSingleCaptureDlg(RENAMING, ENTER_FILENAME, ZSingleCaptureDlg::TypeLineEdit, NULL, "ZSingleCaptureDlg", true, 0, 0);
		  ZLineEdit* zle = (ZLineEdit*)zscd->getLineEdit();  
		  zle->setText(fName);
		  zle->setEdited(true);  
		  
		  if (zscd->exec() == 1) {
			fName = zle->text();
			FullName = fPath + fName;   
			// проверка на существование
			QDir d;
			if ( ! d.exists(FullName)) {
			  if (d.rename(tmpName, FullName, true)) {
				if ( QFileInfo(FullName).isDir() )
				{
					ZConfig Cini(ProgramDir + "/foldername.cfg", true);	
					QString sico = Cini.readEntry(QString("FOLDERNAME"), tmpName, "");
					if ( sico != "" ) {
						Cini.removeEntry(QString("FOLDERNAME"), tmpName );
						Cini.removeEntry(QString("CUSTOMNAME"), oldName );
						Cini.writeEntry(QString("FOLDERNAME"), FullName, app);
						Cini.writeEntry(QString("CUSTOMNAME"), fPath + app, FullName);
					}
					QString destino; ZConfig IC(ProgramDir + "/settings.cfg", true);
					QString SIcon = IC.readEntry(QString("FM"), QString("IconSource"), "");
					if ( SIcon=="0" ) { destino="/foldericons.cfg"; } else { destino="/sfoldericons.cfg"; }
					ZConfig Sini(ProgramDir + destino, true);	
					QString ico = Sini.readEntry(QString("FOLDERICONS"), tmpName, "");
					if ( ico != "" ) {
						Sini.removeEntry(QString("FOLDERICONS"), tmpName );
						Sini.writeEntry(QString("FOLDERICONS"), FullName, ico );
					}
				}
			  } else {
				ZMessageDlg* dlgz  = new ZMessageDlg ( ERROR, RENAME_ERROR, ZMessageDlg::just_ok, 0 , this , "dlgz" , true , 0 );
				dlgz->exec();
				delete dlgz;
			  }
			} else {
			  ZMessageDlg* dlgz  = new ZMessageDlg ( ERROR, RENAME_FILEEXISTS,  ZMessageDlg::just_ok , 0 , this , "dlgz" , true , 0 );
			  if (dlgz->exec() == 1) { delete dlgz; goto repeat1; }
			  else {delete dlgz; }
			}
		  }  
		}
		CargarBrowser(sPath); 
	  }

	}
	else
	{
	  QString app;
	  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
	  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	  if ( val != "big" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
	  else { app = Bbrowser->currentItem()->text(); }
	  QString nnn = app; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { app.remove( app.length() -1, 1 ); }
	  QString current = sPath;

	repeat:  
	  QString FullName = sPath + "/" + app;
	  QString oldName =  FullName;
	  FullName = RealName(FullName);
	  QString tmpName =  FullName;

	  if (FullName.isEmpty()) return;
	  //qDebug(QString("FullName: %1").arg(FullName));  
	  QString fName = getFileName(FullName);
	  //qDebug(QString("fName: %1").arg(fName));  
	  QString fPath = getFilePath(FullName);
	  //qDebug(QString("fPath: %1").arg(fPath));    
	  
	  ZSingleCaptureDlg* zscd = new ZSingleCaptureDlg(RENAMING, ENTER_FILENAME, ZSingleCaptureDlg::TypeLineEdit, NULL, "ZSingleCaptureDlg", true, 0, 0);
	  ZLineEdit* zle = (ZLineEdit*)zscd->getLineEdit();  
	  zle->setText(fName);
	  zle->setEdited(true);  
	  
	  if (zscd->exec() == 1) {
		fName = zle->text();
		FullName = fPath + fName;   
		QDir d;
		if ( ! d.exists(FullName)) {
		  if (d.rename(tmpName, FullName, true)) {
			if ( QFileInfo(FullName).isDir() )
			{
				ZConfig Cini(ProgramDir + "/foldername.cfg", true);	
				QString sico = Cini.readEntry(QString("FOLDERNAME"), tmpName, "");
				if ( sico != "" ) {
					Cini.removeEntry(QString("FOLDERNAME"), tmpName );
					Cini.removeEntry(QString("CUSTOMNAME"), oldName );
					Cini.writeEntry(QString("FOLDERNAME"), FullName, app);
					Cini.writeEntry(QString("CUSTOMNAME"), fPath + app, FullName);
				}
				QString destino; ZConfig IC(ProgramDir + "/settings.cfg", true);
				QString SIcon = IC.readEntry(QString("FM"), QString("IconSource"), "");
				if ( SIcon=="0" ) { destino="/foldericons.cfg"; } else { destino="/sfoldericons.cfg"; }
				ZConfig Sini(ProgramDir + destino, true);	
				QString ico = Sini.readEntry(QString("FOLDERICONS"), tmpName, "");
				if ( ico != "" ) {
					Sini.removeEntry(QString("FOLDERICONS"), tmpName );
					Sini.writeEntry(QString("FOLDERICONS"), FullName, ico );
				}
			}
		    CargarBrowser(sPath);
		  } else {
		    ZMessageDlg* dlgz  = new ZMessageDlg ( ERROR, RENAME_ERROR, ZMessageDlg::just_ok, 0 , this , "dlgz" , true , 0 );
		    dlgz->exec();
		    delete dlgz;
		  }
		} else {
		  ZMessageDlg* dlgz  = new ZMessageDlg ( ERROR, RENAME_FILEEXISTS,  ZMessageDlg::just_ok , 0 , this , "dlgz" , true , 0 );
		  if (dlgz->exec() == 1) { delete dlgz; goto repeat; }
		  else {delete dlgz; }
		}
	  }  
	}
}
  
void ZLauncher::slot_properties()
{
  QString app;
  ZConfig lang(Idioma,false);
  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
  if ( val != "big" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
  else { app = Bbrowser->currentItem()->text(); }
  QString current = sPath;

  //qDebug(QString("*** slot_properties() started..."));  
  QString FullName = sPath + "/" + app;
  FullName = RealName(FullName);

  if (FullName.isEmpty()) return;
  //qDebug(QString("FullName: %1").arg(FullName));  
  QString fName = getFileName(FullName);
  //qDebug(QString("fName: %1").arg(fName));  
  QString fPath = getFilePath(FullName);
  //qDebug(QString("fPath: %1").arg(fPath));
  QFileInfo* fInfo = new QFileInfo ( FullName );
  
  QString s, ss;
  int size;
  if ( fInfo->isDir() ) {
	/*system( QString("busybox du -sk \"%1\" | awk '{ print $1 }' > /tmp/zfmsize").arg(FullName) );
	QString tmp1; QFile entrada("/tmp/zfmsize"); QTextStream stentrada(&entrada);
	if ( entrada.open(IO_ReadOnly | IO_Translate) ) { tmp1 = stentrada.readLine(); } entrada.close();

	tmp1 = tmp1.right(10); int i=tmp1.findRev(" "); tmp1.remove(i,3);
	while ( tmp1[0]==QChar(' ') ) { tmp1 = tmp1.right( tmp1.length()-1 ); }*/

	//system("rm /tmp/zfmsize");
    size = GetFolderSize(FullName);
	if (size < 1024) { 
	  ss = QString("%1 b").arg(size);
	} else if (size < 1024*1024) {
	  ss = QString("%1").arg( (double)(size) / 1024);
	  size = ss.find(QChar('.')); if ( size > -1 ) { ss.remove(size+3, ss.length()-size-3); }
	  ss = ss + " Kb";
	} else if (size < 1024*1024*1024) {
	  ss = QString("%1").arg( (double)(size) / 1024 / 1024);
	  size = ss.find(QChar('.')); if ( size > -1 ) { ss.remove(size+3, ss.length()-size-3); }
	  ss = ss + " Mb";
	} else {
	  ss = QString("%1").arg( (double)(size) / 1024 / 1024 / 1024);
	  size = ss.find(QChar('.')); if ( size > -1 ) { ss.remove(size+3, ss.length()-size-3); }
	  ss = ss + " Gb";
	}
	s = FOLDER;
  } else {
    size = fInfo->size();
	if (size < 1024) { 
	  ss = QString("%1 b").arg(size);
	} else if (size < 1024*1024) {
	  ss = QString("%1").arg( (double)(size) / 1024);
	  size = ss.find(QChar('.')); if ( size > -1 ) { ss.remove(size+3, ss.length()-size-3); }
	  ss = ss + " Kb";
	} else if (size < 1024*1024*1024) {
	  ss = QString("%1").arg( (double)(size) / 1024 / 1024);
	  size = ss.find(QChar('.')); if ( size > -1 ) { ss.remove(size+3, ss.length()-size-3); }
	  ss = ss + " Mb";
	} else {
	  ss = QString("%1").arg( (double)(size) / 1024 / 1024 / 1024);
	  size = ss.find(QChar('.')); if ( size > -1 ) { ss.remove(size+3, ss.length()-size-3); }
	  ss = ss + " Gb";
	}
    s = AFILE;
  }

  
  QString d = fInfo->lastModified().toString();
  QString owner = fInfo->owner();
  QString group = fInfo->group();  
  
  QString text = QString("%1: %2\n%3 %4\n%5 %6\n").arg(s).arg(FullName).arg(PROPERTIES_SIZE).arg(ss).arg(PROPERTIES_DATE).arg(d);
  text.append ( QString("%1 %2\n%3 %4\n").arg(PROPERTIES_OWNER).arg(owner).arg(PROPERTIES_GROUP).arg(group) );

  QString rights = "";
  if (fInfo->isDir()) rights = "d";
  else rights ="-";
  if (fInfo->permission(QFileInfo::ReadUser)) rights.append("r");
  else rights.append("-");
  if (fInfo->permission(QFileInfo::WriteUser)) rights.append("w");
  else rights.append("-");
  if (fInfo->permission(QFileInfo::ExeUser)) rights.append("x");
  else rights.append("-");
  rights.append(" ");
  if (fInfo->permission(QFileInfo::ReadGroup)) rights.append("r");
  else rights.append("-");
  if (fInfo->permission(QFileInfo::WriteGroup)) rights.append("w");
  else rights.append("-");
  if (fInfo->permission(QFileInfo::ExeGroup)) rights.append("x");
  else rights.append("-");
  rights.append(" ");
  if (fInfo->permission(QFileInfo::ReadOther)) rights.append("r");
  else rights.append("-");
  if (fInfo->permission(QFileInfo::WriteOther)) rights.append("w");
  else rights.append("-");
  if (fInfo->permission(QFileInfo::ExeOther)) rights.append("x");
  else rights.append("-");
  text.append( QString("%1 %2").arg(PROPERTIES_RIGHTS).arg(rights) );
  
  ZMessageDlg* dlgw  = new ZMessageDlg ( app, QString(text),  ZMessageDlg::just_ok, 0 , this , "dlgw" , true , 0 );
  QString mval;
  
  if ( fInfo->isDir() ) {
	ZConfig SFIini(ProgramDir + "/sfoldericons.cfg", false);	
	mval = SFIini.readEntry(QString("FOLDERICONS"), QString(FullName), "");
	if ( mval == "" ) { mval="fm_folder_thb"; }
	mval.remove( mval.length() -4 ,4 ); mval = mval + "_small";
  } else {
	ZConfig SAIini(ProgramDir + "/sfileicons.cfg", false);	
	QString Eval = app;
	int i = Eval.findRev ( "." ); Eval.remove ( 0, i +1 );
	mval = SAIini.readEntry(QString("FILES"), QString("%1").arg(Eval).lower(), "");
	if ( mval == "" ) { mval="msg_unsupport_file_type_thb"; }
	mval.remove( mval.length() -4 ,4 ); mval = mval + "_small";
  }
  dlgw->setTitleIcon(mval);

  dlgw->exec();  
  delete dlgw;  
}
  
void ZLauncher::slot_attrs()
{
  QString app;
  ZConfig lang(Idioma,false);
  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
  if ( val != "big" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
  else { app = Bbrowser->currentItem()->text(); }
  QString nnn = app; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { app.remove( app.length() -1, 1 ); }

  QString current = sPath;
  QString FullName = sPath + "/" + app;
  FullName = RealName(FullName);


  //qDebug(QString("*** slot_attrs() started..."));  
  if (FullName.isEmpty()) return;
  //qDebug(QString("FullName: %1").arg(FullName));  
  QString fName = getFileName(FullName);
  //qDebug(QString("fName: %1").arg(fName));  
  QString fPath = getFilePath(FullName);
  //qDebug(QString("fPath: %1").arg(fPath));
  QFileInfo* fInfo = new QFileInfo ( FullName );
  
  int rights = 0;
  
  if (fInfo->permission(QFileInfo::ReadUser)) rights += 400;
  if (fInfo->permission(QFileInfo::WriteUser)) rights += 200;
  if (fInfo->permission(QFileInfo::ExeUser)) rights += 100;
  if (fInfo->permission(QFileInfo::ReadGroup)) rights += 40;
  if (fInfo->permission(QFileInfo::WriteGroup)) rights += 20;
  if (fInfo->permission(QFileInfo::ExeGroup)) rights += 10;
  if (fInfo->permission(QFileInfo::ReadOther)) rights += 4;
  if (fInfo->permission(QFileInfo::WriteOther)) rights += 2;
  if (fInfo->permission(QFileInfo::ExeOther)) rights += 1;
  
  ZNumPickerDlg* znpd = new ZNumPickerDlg(10, NULL, "ZNumPickerDlg", 0);
  znpd->setTitle(ATTRS);
  ZNumModule* znpm = znpd->getNumModule();
  znpm->setMaxValue(999);
  znpm->setMinValue(0);
  znpm->setValue(rights);

  if (znpd->exec())
  {
    rights = znpd->getNumValue();
    system(QString("chmod -R %1 %2").arg(rights).arg(FullName.utf8()));
    
  } 
  delete znpm;
  delete znpd;
}
  
void ZLauncher::slot_makedir()
{
  ZConfig lang (Idioma,false);
  QString ASource = getFilePath(sPath);
  if (ASource.isEmpty()) return;
  QString dirName;
  ZSingleCaptureDlg* dlgq = new ZSingleCaptureDlg ( MAKING, ENTER_DIRNAME, ZSingleCaptureDlg::TypeLineEdit, this, NULL, true, 0, 0 );
  if ( dlgq->exec() ) {
    dirName = dlgq->getResultText();
    if ( dirName != "" ) {
	  QString fullDir = QString(sPath + "/" + dirName);
	  int encontro = fullDir.find( "//" , 0 );
	  if ( encontro > -1 ) { fullDir.remove( encontro,1); }
	  system(QString("mkdir \"%1\"").arg(fullDir));
	  QDir d;
	  if ( ! d.exists(fullDir) ) {
        ZMessageDlg* dlgz  = new ZMessageDlg ( ERROR, MKDIR_ERROR,  ZMessageDlg::just_ok , 0 , this , "about" , true , 0 );
        dlgz->exec();
        delete dlgz;
      } else {
        CargarBrowser(sPath); 
      }
    }
  }  
}

//bool  ZLauncher::eventFilter( QObject * o, QEvent * e)
void  ZLauncher::keyPressEvent(QKeyEvent* k)
{
  ZConfig lang(Idioma,false);
  //if ( e->type() == QEvent::KeyPress) { 
    //QKeyEvent *k = (QKeyEvent*)e;

	//if ( pAIA->isActiveWindow() ) {
		if (k->key() == Z6KEY_1 ) { 
		  QString val; ZConfig fini(QString("%1/settings.cfg").arg(ProgramDir), true);
		  val = fini.readEntry(QString("FM"), QString("ShowHidden"), "");
		  if ( val == "0" ) { fini.writeEntry(QString("FM"), QString("ShowHidden"), "1"); CargarBrowser(sPath); }
		  else { fini.writeEntry(QString("FM"), QString("ShowHidden"), "0"); CargarBrowser(sPath); }
		}

		if ( (k->key()==Z6KEY_2 ) || (k->key()==Z6KEY_SIDE_SELECT) ) { AdminFav(); }	  
		if ( (k->key()==Z6KEY_3 ) || (k->key()==Z6KEY_MUSIC) ) { Settings(); }	  

		if (k->key() == Z6KEY_NUM )
		{
		  ZConfig fini(QString("%1/settings.cfg").arg(ProgramDir), true);
		  QString val = fini.readEntry(QString("FM"), QString("IconStyle"), "small");
		  if ( val == "small" ) { fini.writeEntry(QString("FM"), QString("IconStyle"), "list"); }
		  else if ( val == "list" ) { fini.writeEntry(QString("FM"), QString("IconStyle"), "big"); }
		  else if ( val == "big" ) { fini.writeEntry(QString("FM"), QString("IconStyle"), "small"); }
		  
		  val = fini.readEntry(QString("FM"), QString("IconStyle"), "small");
		  if ( val != currentView )
		  {
			  setMainWidgetTitle ( APP_NAME );
			  QWidget *myWidget = new ZWidget ( this, NULL, 0);
			  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );
			  myWidget->setFixedWidth(240);
			  pAIA = new ZAppInfoArea(myWidget, NULL, 0);
			  pAIA->setActArea(ZAppInfoArea::no_progress_meter);
			  setAppInfoArea(pAIA);
			  myVBoxLayout->addWidget ( pAIA, 1, 1 );

			  if ( val != "big" )
			  { browser = new myListBox(QString("%C18%I%M"), myWidget, 0);
				browser->setFixedWidth(240);
				connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));
			  } else {
				Bbrowser = new myIconView ( myWidget ); Bbrowser->setFixedWidth ( 240 );
				connect(Bbrowser, SIGNAL(returnPressed(ZIconViewItem *)), SLOT(brClicked()));
			  }
			  currentView=val;

			  setContentWidget ( myWidget );

			  CargarBrowser(sPath);

			  qApp->installEventFilter( this );

			  int y = 0;
			  y = y + this->headerSize().height();
			  y = y + softKey->height();
			  y = y + pAIA->height();
			  
			  if ( val != "big" ) { browser->setFixedHeight(320-y); browser->setFocus(); }
			  else { Bbrowser->setFixedHeight(320-y); Bbrowser->setFocus(); }
		  }
		}


		if (k->key() == Z6KEY_AST )
		{
			QString val; ZConfig fini(QString("%1/settings.cfg").arg(ProgramDir), true);
			val = fini.readEntry(QString("FM"), QString("IconStyle"), "small");
			if ( val != "big" ) {
				for ( int i=0; i < browser->count(); ++i ) { browser->checkItem( i, ! browser->itemChecked(i) ); }
			} else {
				ZIconViewItem* papa = new ZIconViewItem;
				papa = Bbrowser->currentItem();
				for ( int i=0; i < Bbrowser->count(); ++i ) { Bbrowser->checkItem( i, ! Bbrowser->itemChecked(i) ); }
				Bbrowser->setSelected(papa, true, true);
			}
		}

		if (k->key() == Z6KEY_0 ) {
		  QString val; ZConfig fini(QString("%1/settings.cfg").arg(ProgramDir), false);
		  val = fini.readEntry(QString("FM"), QString("IconStyle"), "");
		  if ( val != "big" ) {
			  if ( ActiveSel != "YES" ) { ActiveSel = "YES"; int index = browser->currentItem();
				CargarBrowser(sPath); browser->setSelected( index, true); browser->ensureCurrentVisible();
				softKey->setText ( ZSoftKey::RIGHT, GOTO, ( ZSoftKey::TEXT_PRIORITY ) 0 ); }
			  else { ActiveSel = "NO"; int index = browser->currentItem();
				CargarBrowser(sPath); browser->setSelected( index, true); browser->ensureCurrentVisible();
				softKey->setText ( ZSoftKey::RIGHT, BACK, ( ZSoftKey::TEXT_PRIORITY ) 0 ); }
		  }
		  else
		  {
			  if ( ActiveSel != "YES" ) { ActiveSel = "YES"; 
				for( int k=0; k < Bbrowser->count(); k++ )	{ 
					if ( Bbrowser->item(k)->text()!="..") Bbrowser->item(k)->setType(ZIconViewItem::MARK_CHECKBOX); }
				softKey->setText ( ZSoftKey::RIGHT, GOTO, ( ZSoftKey::TEXT_PRIORITY ) 0 ); }
			  else { ActiveSel = "NO"; 
				for( int k=0; k < Bbrowser->count(); k++ )	{ Bbrowser->item(k)->setType(ZIconViewItem::MARK_NONE); }
				softKey->setText ( ZSoftKey::RIGHT, BACK, ( ZSoftKey::TEXT_PRIORITY ) 0 ); }
		  }

		}
	//}

    if (k->key() == Z6KEY_RED )
    {
        qApp->quit();
    }
    /*else
    { 
      return QWidget::eventFilter( o, e ); 
    }*/
  //}  
  //return QWidget::eventFilter( o, e ); 
}


void ZLauncher::CargarBrowser(QString direccion)
{
  int numero;
  if ( ActiveSel == "YES" ) { numero=1; } else { numero=0; }
  //qDebug("Abriendo Carpeta: " + direccion);
  ZConfig MPini(QString("%1/settings.cfg").arg(ProgramDir), false);
  //QString MPval = MPini.readEntry(QString("EXTRA"), QString("MPlayer"), "");
  MPini.writeEntry("FM", "LastFolder", direccion);


  RES_ICON_Reader iconReader;

  QPixmap* sendto;
  int dev1 = direccion.find( "/mmc/mmca1", 0 );
  if ( dev1 > -1 ) {
	sendto = new QPixmap(ProgramDir+ IDB_IMAGE_PHONE);
	menuSend->changeItem(1, PHONE, sendto);
  } else {
	sendto = new QPixmap(ProgramDir+ IDB_IMAGE_SDCARD);
	menuSend->changeItem(1, SDCARD, sendto);
  }

  QPixmap pixmap;
  ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), false);
  QString val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
  QString Hval = ini.readEntry(QString("FM"), QString("ShowHidden"), "");
  QString Pval = ini.readEntry(QString("FM"), QString("ShowPreview"), "");

  ZConfig SFIini(ProgramDir + "/sfoldericons.cfg", false);	
  ZConfig FNini(ProgramDir + "/foldername.cfg", false);	
  ZConfig SAIini(ProgramDir + "/sfileicons.cfg", false);	

  if ( val != "big" )
  {
	  browser->clear();
	  int ftemp = ini.readNumEntry(QString("FM"), QString("FontSize"), 20);
	  QFont f = browser->font();
	  f.setPixelSize( ftemp );

	  browser->setItemFont(ZListBox::LISTITEM_REGION_C, ZSkinBase::StStandard, f);
	  browser->setItemFont(ZListBox::LISTITEM_REGION_C, ZSkinBase::StHighlighted, f);
	  browser->setItemFont(ZListBox::LISTITEM_REGION_C, ZSkinBase::StSelected, f);
	  browser->setItemFont(ZListBox::LISTITEM_REGION_C, ZSkinBase::StHighlightSelected, f);

	  Accion = "Browser";
	  ZListBoxItem* Item0;
	  if ( val == "small" ) { Item0 = new ZListBoxItem ( browser, QString ( "%I%M" ) ); }
	  else { Item0 = new ZListBoxItem ( browser, QString ( "%I%M" ) ); }
	  Item0->appendSubItem ( 1, QString ( ".." ), false, NULL ); browser->insertItem ( Item0, 0,true );
	  ItemSize = browser->itemHeight(0)-2 ; /*if ( ItemSize>20 ) ItemSize=20*/; browser->clear();
	  if ( val == "list" ) { ItemSize = ItemSize*2; }
	
	  browser->clear();
	  QDir dir ( direccion, "*" );
	  sPath = direccion;
	  int i = sPath.findRev ( "//" ); sPath.remove ( i, 1 );
	  i = sPath.findRev ( "./" ); sPath.remove ( i, 1 );
	  dir.setMatchAllDirs ( true );
	  dir.setFilter ( QDir::Dirs | QDir::Hidden );
	  if ( !dir.isReadable() )
		return;
	  QStringList entries = dir.entryList();
	  entries.sort();
	  QStringList::ConstIterator it = entries.begin();

	  if ( (sPath != "") && (sPath != "/") && (sPath != "//") )
	  {
		pixmap=QPixmap( iconReader.getIcon("gen_back_to_arrw_thb", false) );
		if ( pixmap.height() > ItemSize ) { QImage image; image = pixmap.convertToImage(); 
		image = image.smoothScale( ItemSize, ItemSize ); pixmap.convertFromImage(image); } 
		ZListBoxItem* Item = new ZListBoxItem ( browser, QString("%I%1%M").arg(ItemSize) );
		Item->setPixmap ( 0, pixmap ); Item->appendSubItem ( 1, "..", false, NULL );
		browser->insertItem ( Item,-1,true );
	  }

	  while ( it != entries.end() ) {
		if ( (*it != ".") && (*it != "..") ) {
    	
			QString MYSTUFF = QString ( sPath + "/" + *it );

			QString	mval;
			mval = SFIini.readEntry(QString("FOLDERICONS"), QString(MYSTUFF), "");
			if ( mval == "" ) { mval="fm_folder_thb"; }
			if ( val == "small" ) { mval.remove( mval.length() -4 ,4 ); mval = mval + "_small"; }
			pixmap=QPixmap( iconReader.getIcon(mval, false) );

			/*if ( QFileInfo(MYSTUFF + "/folder.jpg").isFile() )
			{
				pixmap.load( MYSTUFF + "/folder.jpg" );				
			}
			double alto = pixmap.height();
			double ancho = pixmap.width();
			if ( alto > ancho ) { double medio=alto/ancho; 
			  while ( alto > ItemSize ) { alto=alto-medio; ancho=ancho-1; }
			  while ( ancho > ItemSize ) { alto=alto-medio; ancho=ancho-1; }
			} else if ( ancho > alto ) { double medio=ancho/alto;
			  while ( ancho > ItemSize ) { alto=alto-1; ancho=ancho-medio; }
			  while ( alto > ItemSize ) { alto=alto-1; ancho=ancho-medio; }
			} else if ( alto==ancho ) {  
			  while ( alto > ItemSize ) { alto=alto-1; ancho=ancho-1; }
			  while ( ancho > ItemSize ) { alto=alto-1; ancho=ancho-1; }
			}
			if ( pixmap.height() > ItemSize ) { QImage image; image = pixmap.convertToImage(); 
			image = image.smoothScale( ancho, alto ); pixmap.convertFromImage(image); }  
			*/

			if ( pixmap.height() > ItemSize ) { QImage image; image = pixmap.convertToImage(); 
			image = image.smoothScale( ItemSize, ItemSize ); pixmap.convertFromImage(image); }  

			ZListBoxItem* Item;
			if ( ActiveSel == "YES" ) { Item = new ZListBoxItem ( browser, QString("%C%I%1%M").arg(ItemSize) ); }
			else { Item = new ZListBoxItem ( browser, QString("%I%1%M").arg(ItemSize) ); }

			/*if ( QFileInfo(MYSTUFF).isSymLink() ) 
			{
				QPixmap pix0; pix0.load(ProgramDir+"/img/link.png");
				QPainter p0(&pixmap); p0.drawPixmap(0,0,pixmap);
				p0.drawPixmap(0,ItemSize-8,pix0);
			}*/

			Item->setPixmap ( numero, pixmap );
			mval = FNini.readEntry(QString("FOLDERNAME"), MYSTUFF, "");
			if ( mval == "" ) { mval = *it; }
			
			Item->appendSubItem ( numero+1, mval, false, NULL );
			if ( val == "list" ) { 
				QFileInfo* fInfo = new QFileInfo ( sPath + "/" + *it );
				QString dd = QString("%1").arg ( QDate (fInfo->lastModified().date()).day() );
				QString mm = QString("%1").arg ( QDate (fInfo->lastModified().date()).month() );
				QString yy = QString("%1").arg ( QDate (fInfo->lastModified().date()).year() );
				Item->appendSubItem ( numero+1, QString("%1/%2/%3").arg(dd).arg(mm).arg(yy), false, NULL );
			}

			QString hidden = QString ( *it );
			if ( hidden[0] == QChar('.') ) { if ( Hval == "1" ) { browser->insertItem ( Item,-1,true ); } }
			else { browser->insertItem ( Item,-1,true ); }
			
		}
		++it;
	  }

		browser->sort(true); 

		dir.setFilter ( QDir::Files | QDir::Hidden );

		entries = dir.entryList();
		entries.sort();
		it = entries.begin();
		while ( it != entries.end() ) {

		    QString Eval = QString("%1").arg(*it);
			int i = Eval.findRev ( "." ); Eval.remove ( 0, i +1 );

			QString	mval;
			mval = SAIini.readEntry(QString("FILES"), QString("%1").arg(Eval).lower(), "");
			if ( mval == "" ) { mval="msg_unsupport_type_thb"; }
			if ( val == "small" ) { mval.remove( mval.length() -4 ,4 ); mval = mval + "_small";
			if ( mval=="msg_unsupport_type_small" ) mval="msg_unsupport_file_type_small"; }
			pixmap=QPixmap( iconReader.getIcon(mval, false) );

				if ( Pval == "Yes" )
				{
					if ( (Eval.lower()=="jpg") || (Eval.lower()=="jpeg") || (Eval.lower()=="png") || (Eval.lower()=="gif") || (Eval.lower()=="bmp") )
					{
						pixmap.load( QString("%1/%1").arg(sPath).arg(*it) );				
					}
					double alto = pixmap.height();
					double ancho = pixmap.width();
					if ( alto > ancho ) { double medio=alto/ancho; 
					  while ( alto > ItemSize ) { alto=alto-medio; ancho=ancho-1; }
					  while ( ancho > ItemSize ) { alto=alto-medio; ancho=ancho-1; }
					} else if ( ancho > alto ) { double medio=ancho/alto;
					  while ( ancho > ItemSize ) { alto=alto-1; ancho=ancho-medio; }
					  while ( alto > ItemSize ) { alto=alto-1; ancho=ancho-medio; }
					} else if ( alto==ancho ) {  
					  while ( alto > ItemSize ) { alto=alto-1; ancho=ancho-1; }
					  while ( ancho > ItemSize ) { alto=alto-1; ancho=ancho-1; }
					}
					if ( pixmap.height() > ItemSize ) { QImage image; image = pixmap.convertToImage(); 
					image = image.smoothScale( ancho, alto ); pixmap.convertFromImage(image); }  
				} else {
					if ( pixmap.height() > ItemSize ) { QImage image; image = pixmap.convertToImage(); 
					image = image.smoothScale( ItemSize, ItemSize ); pixmap.convertFromImage(image); }  
				}

			//if ( pixmap.height() > ItemSize ) { QImage image; image = pixmap.convertToImage(); 
			//image = image.smoothScale( ItemSize, ItemSize ); pixmap.convertFromImage(image); }  
			
			ZListBoxItem* Item;
			if ( ActiveSel == "YES" ) { Item = new ZListBoxItem ( browser, QString("%C%I%1%M").arg(ItemSize) ); }
			else { Item = new ZListBoxItem ( browser, QString("%I%1%M").arg(ItemSize) ); }
		    Item->setPixmap ( numero, pixmap );
		    Item->appendSubItem ( numero+1, *it, false, NULL );

			if ( val == "list" ) { 
				QFileInfo* fInfo = new QFileInfo ( sPath + "/" + *it );
				QString dd = QString("%1").arg ( QDate (fInfo->lastModified().date()).day() );
				QString mm = QString("%1").arg ( QDate (fInfo->lastModified().date()).month() );
				QString yy = QString("%1").arg ( QDate (fInfo->lastModified().date()).year() );
				int size = fInfo->size(); QString ss;
				if (size < 1024) { 
				  ss = QString("%1 b").arg(size);
				} else if (size < 1024*1024) {
				  ss = QString("%1").arg( (double)(size) / 1024);
				  size = ss.find(QChar('.')); if ( size > -1 ) { ss.remove(size+3, ss.length()-size-3); }
				  ss = ss + " Kb";
				} else if (size < 1024*1024*1024) {
				  ss = QString("%1").arg( (double)(size) / 1024 / 1024);
				  size = ss.find(QChar('.')); if ( size > -1 ) { ss.remove(size+3, ss.length()-size-3); }
				  ss = ss + " Mb";
				} else {
				  ss = QString("%1").arg( (double)(size) / 1024 / 1024 / 1024);
				  size = ss.find(QChar('.')); if ( size > -1 ) { ss.remove(size+3, ss.length()-size-3); }
				  ss = ss + " Gb";
				}
				Item->appendSubItem ( numero+1, QString("%1/%2/%3  -  %4").arg(dd).arg(mm).arg(yy).arg(ss), false, NULL );
			}

			if ( Hval == "1" ) { browser->insertItem ( Item,-1,true ); }
			else
			{
			  QString hidden = QString ( *it );
			  if ( hidden[0] != QChar('.') ) { browser->insertItem ( Item,-1,true ); }
			  else { if ( hidden[1] == QChar('.') ) { browser->insertItem ( Item,-1,true ); } }
			}
			++it;
		}
	browser->enableMarquee(ZListBox::LISTITEM_REGION_C, true);
	//if ( Pval == "Yes" ) verMiniaturas();
  }
  else
  {
	  int cant=0;
	  Accion = "Browser";
	  ZConfig ini(ProgramDir + "/settings.cfg", true);
	  QString Hval = ini.readEntry(QString("FM"), QString("ShowHidden"), "");

	  Bbrowser->clear();
	  qApp->processEvents();
	  QDir dir ( direccion, "*" );
	  sPath = direccion;
	  dir.setMatchAllDirs ( true );
	  dir.setFilter ( QDir::Dirs | QDir::Hidden );
	  if ( !dir.isReadable() )
		return;
	  QStringList entries = dir.entryList();
	  entries.sort();
	  QStringList::ConstIterator it = entries.begin();
	  QPixmap pixmap, pAIA;
	  if ( (sPath != "") && (sPath != "/") && (sPath != "//") )
	  {
		pixmap=QPixmap( iconReader.getIcon("gen_back_to_arrw_thb", false) );
		if ( pixmap.height() > 48 ) { QImage image; image = pixmap.convertToImage(); 
		image = image.smoothScale(48, 48); pixmap.convertFromImage(image); }  
		ZIconViewItem* Item = new ZIconViewItem ( Bbrowser, "..", pixmap );
		Bbrowser->setSelected(Item, true, true);
	  }
	  while ( it != entries.end() ) {
	   if ( *it != "." ) {
		if ( *it != ".." ) {

		  ZConfig fini(ProgramDir + "/foldericons.cfg", true);	
		  ZConfig fsini(ProgramDir + "/sfoldericons.cfg", true);	
		  ZConfig cini(ProgramDir + "/foldername.cfg", true);	

		  if ( *it != ".." ) {
		    QString MYSTUFF = QString ( direccion + "/" + *it );
			int encontro = MYSTUFF.find( "//" , 0 );
			if ( encontro > -1 ) { MYSTUFF.remove( encontro,1); }
			int i = MYSTUFF.findRev ( "/../" );
			if ( i>0 ) {
				QString mystuff2 = MYSTUFF; mystuff2.remove ( i, mystuff2.length() - i );
				int j = mystuff2.findRev ( "/" ); mystuff2.remove ( j, mystuff2.length() - j );
				mystuff2 = QString( mystuff2 + "/" + *it);
				MYSTUFF = mystuff2; sPath = MYSTUFF;
			}
			QString	mval;
			mval = fsini.readEntry(QString("FOLDERICONS"), QString(MYSTUFF), "");
			if ( mval == "" ) { mval="fm_folder_thb"; }
			pixmap=QPixmap( iconReader.getIcon(mval, false) );

			if ( pixmap.height() > 48 ) { QImage image; image = pixmap.convertToImage(); 
			image = image.smoothScale(48, 48); pixmap.convertFromImage(image); }  
		  } 

		  QString MYSTUFF = QString ( direccion + "/" + *it );

		  QString mval = cini.readEntry(QString("FOLDERNAME"), MYSTUFF, "");
		  if ( mval == "" ) { mval = *it; }
		 
		  if ( Hval == "1" ) { ZIconViewItem* Item = new ZIconViewItem ( Bbrowser, mval, pixmap ); ++cant;
			if ( cant == 1 ) { Bbrowser->setSelected(Item, true, true); }
			if ( ActiveSel == "YES" ) { Item->setType(ZIconViewItem::MARK_CHECKBOX); }
		  }
		  else
		  {
			QString hidden = QString ( *it );
			if ( hidden[0] != QChar('.') ) { ZIconViewItem* Item = new ZIconViewItem ( Bbrowser, mval, pixmap ); ++cant;
				if ( ActiveSel == "YES" ) { Item->setType(ZIconViewItem::MARK_CHECKBOX); }
				if ( cant == 1 ) { Bbrowser->setSelected(Item, true, true); }}
			else { if ( hidden[1] == QChar('.') ) { ZIconViewItem* Item = new ZIconViewItem ( Bbrowser, mval, pixmap ); ++cant;
				if ( ActiveSel == "YES" ) { Item->setType(ZIconViewItem::MARK_CHECKBOX); }
				if ( cant == 1 ) { Bbrowser->setSelected(Item, true, true); }} }
		  }
		}
	   }
	   ++it;
	  }

	  dir.setFilter ( QDir::Files | QDir::Hidden );

		entries = dir.entryList();
		entries.sort();
		it = entries.begin();
		while ( it != entries.end() ) {
		  if ( *it != "." ) {
		    if ( *it != ".." ) {
		    	QString Eval = QString("%1").arg(*it);
				int i = Eval.findRev ( "." ); Eval.remove ( 0, i +1 );
				ZConfig ini(ProgramDir + "/fileicons.cfg", false);	
				ZConfig sini(ProgramDir + "/sfileicons.cfg", false);	

				QString	mval;
				mval = sini.readEntry(QString("FILES"), QString("%1").arg(Eval).lower(), "");
				if ( mval == "" ) { mval="msg_receipt_request_thb"; }
				pixmap=QPixmap( iconReader.getIcon(mval, false) );

				if ( Pval == "Yes" )
				{
					if ( (Eval.lower()=="jpg") || (Eval.lower()=="jpeg") || (Eval.lower()=="png") || (Eval.lower()=="gif") || (Eval.lower()=="bmp") )
					{
						pixmap.load( QString("%1/%1").arg(sPath).arg(*it) );				
					}
					double alto = pixmap.height();
					double ancho = pixmap.width();
					if ( alto > ancho ) { double medio=alto/ancho; 
					  while ( alto > 48 ) { alto=alto-medio; ancho=ancho-1; }
					  while ( ancho > 48 ) { alto=alto-medio; ancho=ancho-1; }
					} else if ( ancho > alto ) { double medio=ancho/alto;
					  while ( ancho > 48 ) { alto=alto-1; ancho=ancho-medio; }
					  while ( alto > 48 ) { alto=alto-1; ancho=ancho-medio; }
					} else if ( alto==ancho ) {  
					  while ( alto > 48 ) { alto=alto-1; ancho=ancho-1; }
					  while ( ancho > 48 ) { alto=alto-1; ancho=ancho-1; }
					}
					if ( pixmap.height() > 48 ) { QImage image; image = pixmap.convertToImage(); 
					image = image.smoothScale( ancho, alto ); pixmap.convertFromImage(image); }  
				} else {
					if ( pixmap.height() > 48 ) { QImage image; image = pixmap.convertToImage(); 
					image = image.smoothScale( 48, 48 ); pixmap.convertFromImage(image); }  
				}

		    } 
			//if ( pixmap.height() > 48 ) { QImage image; image = pixmap.convertToImage(); 
			//image = image.smoothScale( 48, 48 ); pixmap.convertFromImage(image); }

			if ( Hval == "1" ) { 
				ZIconViewItem* Item = new ZIconViewItem ( Bbrowser, *it, pixmap ); ++cant;
				if ( ActiveSel == "YES" ) { Item->setType(ZIconViewItem::MARK_CHECKBOX); }
				if ( cant == 1 ) { Bbrowser->setSelected(Item, true, true); } 
			}
			else
			{
			  QString hidden = QString ( *it );
			  if ( hidden[0] != QChar('.') ) { ZIconViewItem* Item = new ZIconViewItem ( Bbrowser, *it, pixmap ); ++cant;
				if ( ActiveSel == "YES" ) { Item->setType(ZIconViewItem::MARK_CHECKBOX); }
				if ( cant == 1 ) { Bbrowser->setSelected(Item, true, true); } }
			  else { if ( hidden[1] == QChar('.') ) { ZIconViewItem* Item = new ZIconViewItem ( Bbrowser, *it, pixmap ); ++cant;
				if ( ActiveSel == "YES" ) { Item->setType(ZIconViewItem::MARK_CHECKBOX); }
				if ( cant == 1 ) { Bbrowser->setSelected(Item, true, true); }} }
			}

		  }
		  ++it;
		}
	Bbrowser->enableMarquee(true); 
	//if ( Pval == "Yes" ) verMiniaturas(); 
  }

  pAIA->removeArea(ZAppInfoArea::activity);
  pAIA->setActArea(ZAppInfoArea::no_progress_meter);
  QString mval;
  mval = SFIini.readEntry(QString("FOLDERICONS"), QString(sPath), "");
  if ( mval == "" ) { mval="fm_folder_thb"; }
  if ( val == "small" ) { mval.remove( mval.length() -4 ,4 ); mval = mval + "_small"; }
  pixmap=QPixmap( iconReader.getIcon(mval, false) );
  if ( pixmap.height() > 19 ) { QImage image; image = pixmap.convertToImage(); 
  image = image.smoothScale( 19, 19 ); pixmap.convertFromImage(image); }  

  int i = sPath.findRev ( "//" ); sPath.remove ( i, 1 );
  i = sPath.findRev ( "./" ); sPath.remove ( i, 1 );
  //qDebug(sPath);  
  pAIA->appendIcon(ZAppInfoArea::activity,pixmap,sPath);
  mval = FNini.readEntry(QString("FOLDERNAME"), sPath, "");
  if ( mval == "" ) { mval=getFileName(sPath); }
  pAIA->appendText(ZAppInfoArea::activity, mval, "name");

  checkClip();
  checkPlayer();

}

void ZLauncher::checkClip()
{
  QPixmap pixmap;
  pAIA->removeArea(ZAppInfoArea::counter);
  if ( QFileInfo("/tmp/clipboard").isFile() )
  {
	pixmap.load(ProgramDir+ "/img/paste.png");
	pAIA->appendIcon(ZAppInfoArea::counter,pixmap,sPath);
	menuControl->setItemEnabled(2,true);
  }
  else menuControl->setItemEnabled(2,false);
}

void ZLauncher::checkPlayer()
{
  QPixmap pixmap;
  RES_ICON_Reader iconReader;
  if ( Playing==1 )
  {
	pixmap = QPixmap( iconReader.getIcon("mp_play_idle7", false) );
	pAIA->appendIcon(ZAppInfoArea::counter,pixmap,sPath);
  }
}


void ZLauncher::prevPlay()
{
  QString app, fName;
  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
  if ( val != "big" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
  else { app = Bbrowser->currentItem()->text(); }
  QString nnn = app; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { app.remove( app.length() -1, 1 ); }
  fName = sPath + "/" + app;

  QString Eval = fName;
  int i = Eval.findRev ( "." ); Eval.remove ( 0, i +1 );

  if ( (Eval.lower()=="mp3") || (Eval.lower()=="wav") || (Eval.lower()=="m4a") || (Eval.lower()=="imy"))
  {
	system("busybox killall alertprocess");
	system(QString("echo \"ABRIENDO %1\n\"").arg(fName));
	system(QString("%1/play \"%2\"&").arg(ProgramDir).arg(fName));
	Playing=1;
	checkClip();
	checkPlayer();
  }
  else
  {
	system("busybox killall alertprocess");
	Playing=0;
	checkClip();
	checkPlayer();
  }

}

void ZLauncher::prevStop()
{
/*	//pAIA->removeArea(ZAppInfoArea::input);
	system("busybox killall mplayer");
	Playing=0;
	checkClip();
	checkPlayer();*/
}

void ZLauncher::brClicked()
{
	if ( ActiveSel == "NO" )
	{
	  QString app, fName;
	  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
	  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	  if ( val != "big" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
	  else { app = Bbrowser->currentItem()->text(); }
	  QString nnn = app; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { app.remove( app.length() -1, 1 ); }
	  fName = sPath + "/" + app;
	  
	  if ( Accion == "Browser" )
	  {
		if ( app == ".." ) { Atras(); return; }
		else 
		{
		  fName = RealName(fName);
		  if ( QFileInfo(fName).isDir() ) { CargarBrowser( fName ); return; }
		  else
		  {
			QFileInfo *fInfo = new QFileInfo ( fName );
			ZConfig mini(ProgramDir + "/mimetype.cfg", false);
			QString extension = fName;
			int i = extension.findRev ( "." ); extension.remove ( 0, i +1 );
			QString mval = mini.readEntry(QString("MIME"), QString("%1").arg(extension).lower(), "");
			if ( mval != "" )
			{ 
				QString eten = QString("%1").arg(extension).lower();
				QString tipo = "";

				if ((eten=="jpg")||(eten=="jpeg")||(eten=="gif")||(eten=="png")||(eten=="bmp")||(eten=="tif")||(eten=="tiff")) tipo="Image";
				else if ((eten=="aac")||(eten=="ac3")||(eten=="amr")||(eten=="mp3")||(eten=="wav")||(eten=="imy")||(eten=="wma")) tipo="Music";
				//else if ((eten=="3gp")||(eten=="rm")||(eten=="mp4")) tipo="Video";

				if ( (mval=="/usr/SYSqtapp/mediaplayer/mediaplayer") && (tipo=="Image") )
				{
					AM_ActionData ad; ad.setAction("ViewPictureInMediaFinder");
					ad.setValue( "FileName", QString("%1").arg(fName.utf8()) ); 
					ad.invoke();    
				}
				else if ( (mval=="/usr/SYSqtapp/mediaplayer/mediaplayer") && (tipo=="Music") )
				{
					/*AM_ActionData ad; ad.setAction("LaunchMusicChannel");
					ad.setValue( "FileName", QString("%1").arg(fName.utf8()) ); 
					ad.invoke();*/  
					system("busybox killall alertprocess");
					Playing=0; checkClip(); checkPlayer();
					system("busybox killall mediaplayer");
					system(QString("%1 -d \"%2\" &").arg(mval.utf8()).arg(fName.utf8()));
				}
				else
				{
					system(QString("%1 -d \"%2\"").arg(mval.utf8()).arg(fName.utf8()));
				}
				return;
			}
			else
			{
				if (fInfo->isExecutable() && fInfo->isFile()) {
					qApp->processEvents();
					system(fName.utf8());
				}
			}
		  }
		}
	  }
	}
	else
	{
	  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
	  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	  if ( val != "big" )
	  {
		int index = browser->currentItem();
		if ( browser->itemChecked(index) ) { browser->checkItem(index,true); }
		else { browser->checkItem(index,false); }
	  }
	  else
	  {
		int index = Bbrowser->currentItem()->index();
		if ( Bbrowser->itemChecked(index) ) { Bbrowser->checkItem(index,true); }
		else { Bbrowser->checkItem(index,false); }
	  }
	}

}

void ZLauncher::slot_personalize()
{
  QString app, fName;
  QString val, SIcon; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), false);
  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
  SIcon = ini.readEntry(QString("FM"), QString("IconSource"), "");
  if ( val != "big" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
  else { app = Bbrowser->currentItem()->text(); }
  QString nnn = app; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { app.remove( app.length() -1, 1 ); }

  fName = sPath + "/" + app;
  fName = RealName(fName);

  system(QString("echo \"%1\" > /tmp/selectedfolder").arg(fName));

  ZIconSelect *zfod = new ZIconSelect();
  if (zfod->exec() ) { CargarBrowser(sPath); }
  delete zfod;

}


void ZLauncher::slot_personalizeName()
{
  ZConfig lang (Idioma, false);
  QString app, napp;
  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), false);
  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
  if ( val != "big" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
  else { app = Bbrowser->currentItem()->text(); }

  QString FullName = sPath + "/" + app;
  QString NewName, CustomName, rName, rapp;
  rName = FullName;
  FullName = RealName(FullName);
  rapp = getFileName(FullName);

  if (FullName.isEmpty()) return;

  if ( QFileInfo(FullName).isDir() )
  {
	  ZSingleCaptureDlg* zscd = new ZSingleCaptureDlg(PERSONALIZE, "", ZSingleCaptureDlg::TypeLineEdit, NULL, "ZSingleCaptureDlg", true, 0, 0);
	  ZLineEdit* zle = (ZLineEdit*)zscd->getLineEdit();  
	  zle->setText(app);
	  zle->setEdited(true);  

	  ZConfig Cini(ProgramDir + "/foldername.cfg", true);
	  QString sval = Cini.readEntry(QString("FOLDERNAME"), FullName, "");
	  CustomName = sPath + "/" + sval; 

	  if (zscd->exec()) {
		if ( zle->text() != "" ) {
		  napp = zle->text();
		  NewName = sPath + "/" + napp; 
		  Cini.writeEntry(QString("FOLDERNAME"), FullName, napp);
		  Cini.writeEntry(QString("CUSTOMNAME"), NewName, FullName);
		  if ( sval != napp ) { Cini.removeEntry(QString("CUSTOMNAME"), CustomName ); }
		  
		} else {
		  napp = rapp;
		  Cini.removeEntry(QString("FOLDERNAME"), FullName );
		  Cini.removeEntry(QString("CUSTOMNAME"), CustomName );
		}

		
		if ( val != "big" )
		{
			QPixmap pixmap;
			int ind = browser->currentItem();
			ZListBoxItem* Item = new ZListBoxItem ( browser, QString ( "%I%M" ) );
			Item->appendSubItem ( 1, napp, false, NULL );

			QString destino; ZConfig IC(ProgramDir + "/settings.cfg", true);
			QString SIcon = IC.readEntry(QString("FM"), QString("IconSource"), "");
			if ( SIcon=="0" ) { destino="/foldericons.cfg"; } else { destino="/sfoldericons.cfg"; }

			ZConfig fini(ProgramDir + destino, true); QString mval;
			if ( SIcon == "0" ) { 
				mval = fini.readEntry(QString("FOLDERICONS"), QString(FullName), "");
				if ( mval == "" ) { mval="normal.png"; }
				pixmap=QPixmap ( ProgramDir + "/img/folders/" + mval );
			} else { 
				RES_ICON_Reader iconReader;
				mval = fini.readEntry(QString("FOLDERICONS"), QString(FullName), "");
				if ( mval == "" ) { mval="fm_folder_thb"; }
				pixmap=QPixmap( iconReader.getIcon(mval, false) );
			}

			if ( val == "small" ) { 
				if ( pixmap.height() > 18 ) { QImage image; image = pixmap.convertToImage(); 
				image = image.smoothScale(18, 18); pixmap.convertFromImage(image); }  
			} else {
				if ( pixmap.height() > 38 ) { QImage image; image = pixmap.convertToImage(); 
				image = image.smoothScale(38, 38); pixmap.convertFromImage(image); }  
			}
			Item->setPixmap ( 0, pixmap );
			if ( val == "list" ) { 
				QFileInfo* fInfo = new QFileInfo ( rName );
				QString dd = QString("%1").arg ( QDate (fInfo->lastModified().date()).day() );
				QString mm = QString("%1").arg ( QDate (fInfo->lastModified().date()).month() );
				QString yy = QString("%1").arg ( QDate (fInfo->lastModified().date()).year() );
				Item->appendSubItem ( 1, QString("%1/%2/%3").arg(dd).arg(mm).arg(yy), false, NULL );
			}
			browser->changeItem ( Item, ind );
		}

		if ( val == "big" )
		{
			QPixmap pixmap;
			Bbrowser->currentItem()->setText(napp);
		}


	  }
  }

}

void ZLauncher::Open()
{
  menuMain->hide();
  qApp->processEvents();
  QString app, fName;
  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), false);
  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
  if ( val != "big" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
  else { app = Bbrowser->currentItem()->text(); }
  QString nnn = app; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { app.remove( app.length() -1, 1 ); }
  fName = sPath + "/" + app;

  if ( QFileInfo(fName).isFile() )
  {
	QFileInfo *fInfo = new QFileInfo ( fName );
	ZConfig ini(ProgramDir + "/mimetype.cfg", false);
	QString extension = fName;
	int i = extension.findRev ( "." ); extension.remove ( 0, i +1 );
	QString val = ini.readEntry(QString("MIME"), QString("%1").arg(extension), "");
	if ( val != "" )
	{ 
		QString eten = QString("%1").arg(extension).lower();
		QString tipo = "";

		if ((eten=="jpg")||(eten=="jpeg")||(eten=="gif")||(eten=="png")||(eten=="bmp")||(eten=="tif")||(eten=="tiff")) tipo="Image";
		else if ((eten=="aac")||(eten=="ac3")||(eten=="amr")||(eten=="mp3")||(eten=="wav")||(eten=="imy")||(eten=="wma")) tipo="Music";
		//else if ((eten=="3gp")||(eten=="rm")||(eten=="mp4")) tipo="Video";

		if ( (val=="/usr/SYSqtapp/mediaplayer/mediaplayer") && (tipo=="Image") )
		{
			AM_ActionData ad; ad.setAction("ViewPictureInMediaFinder");
			ad.setValue( "FileName", QString("%1").arg(fName.utf8()) ); 
			ad.invoke();    
		}
		else if ( (val=="/usr/SYSqtapp/mediaplayer/mediaplayer") && (tipo=="Music") )
		{
			/*AM_ActionData ad; ad.setAction("LaunchMusicChannel");
			ad.setValue( "FileName", QString("%1").arg(fName.utf8()) ); 
			ad.invoke();*/  
			system("busybox killall alertprocess");
			Playing=0; checkClip(); checkPlayer();
			system("busybox killall mediaplayer");
			system(QString("%1 -d \"%2\" &").arg(val.utf8()).arg(fName.utf8()));
		}
		else
		{
			system(QString("%1 -d \"%2\"").arg(val.utf8()).arg(fName.utf8()));
		}
		return;



		if ((eten=="jpg")||(eten=="jpeg")||(eten=="gif")||(eten=="png")||(eten=="bmp")||(eten=="tif")||(eten=="tiff")) tipo="Image";

		if ( (val=="/usr/SYSqtapp/mediaplayer/mediaplayer") && (tipo=="Image") )
		{
			AM_ActionData ad; ad.setAction("ViewPictureInMediaFinder");
			ad.setValue( "FileName", QString("%1").arg(fName.utf8()) ); 
			ad.invoke();    
		}
		else
		{
			system(QString("%1 -d \"%2\"").arg(val.utf8()).arg(fName.utf8()));
		}
		return;
	}
	else
	{
		if (fInfo->isExecutable() && fInfo->isFile()) {
			qApp->processEvents();
			//qDebug("EJECUTANDO " + fName);  
			system(fName.utf8());
		}
	}
  }
  else
  {
	CargarBrowser( fName );
  }

}

void ZLauncher::OpenWith()
{
  menuMain->hide();
  qApp->processEvents();
  QString app;
  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), false);
  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
  if ( val != "big" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
  else { app = Bbrowser->currentItem()->text(); }
  QString nnn = app; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { app.remove( app.length() -1, 1 ); }
  QString current = sPath;
  QString selected = sPath + "/" + app;

  if ( QFileInfo(selected).isFile() )
  {
	system(QString("echo \"%1\" > /tmp/selectedfolder").arg(selected));
	ZOpenWith *zfod = new ZOpenWith();
	zfod->exec();
	delete zfod;
  }

}

void ZLauncher::openText()
{
  menuMain->hide();
  qApp->processEvents();
  QString app, fName;
  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), false);
  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
  if ( val != "big" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
  else { app = Bbrowser->currentItem()->text(); }
  QString nnn = app; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { app.remove( app.length() -1, 1 ); }
  fName = sPath + "/" + app;

  if ( QFileInfo(fName).isFile() )
  {
	QString val = ini.readEntry(QString("FM"), QString("TextExec"), "");
	if ( val != "" )
	{ 
		system(QString("%1 -d \"%2\"").arg(val.utf8()).arg(fName.utf8()));
		return;
	}
  }
}

void ZLauncher::AdminFav()
{
  ZAdminFav *zfod = new ZAdminFav();
  if ( zfod->exec() )
  {
    QString fDestPath = zfod->getFileName(); 
	int i = fDestPath.findRev ( "//" ); fDestPath.remove ( i, 1 );
     if ( ! fDestPath.isEmpty())
     {
	   CargarBrowser(fDestPath);
	 }
  }
  delete zfod;
  zfod = NULL;
}

void ZLauncher::AgregarFav()
{
	ZConfig lang (Idioma, false);
	if ( ActiveSel == "YES" )
	{
	  QStringList archivos;
	  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
	  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	  if ( val == "big" )
	  { 
		int k2 = Bbrowser->count(); for( int k=0; k < k2; k++ )	
		{
		  if ( Bbrowser->itemChecked(k) ) {
			QString sapp = Bbrowser->item( k )->text();
			QString nnn = sapp; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { sapp.remove( sapp.length() -1, 1 ); }
			QString fName = sPath + "/" + sapp;
			int encontro = fName.find( "//" , 0 );
			if ( encontro > -1 ) { fName.remove( encontro,1); }
			archivos += QString(fName);
		  }
		}
	  }
	  else
	  {
		for( int k=0; k < browser->count(); k++ )	 
		{
		  if ( browser->itemChecked(k) ) {
			QString sapp = browser->item( k )->getSubItemText ( 2, 0, true );  
			QString nnn = sapp; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { sapp.remove( sapp.length() -1, 1 ); }
			QString fName = sPath + "/" + sapp;
			int encontro = fName.find( "//" , 0 );
			if ( encontro > -1 ) { fName.remove( encontro,1); }
			archivos += QString(fName);
			//qDebug(QString("Agregando: " + fName));
		  }
		}
	  }
	  if ( archivos.count() > 0 )
	  {
		int k2 = archivos.count(); for( int k=0; k < k2; k++ )
		{
		  QString fName = archivos[k];
		  QString app = getFileName(fName);
		  fName = RealName(fName);
		  if ( QFileInfo(fName).isDir() ) { 
			  ZSingleCaptureDlg* zscd = new ZSingleCaptureDlg(ADDFAV, FAVNAME, ZSingleCaptureDlg::TypeLineEdit, NULL, "ZSingleCaptureDlg", true, 0, 0);
			  ZLineEdit* zle = (ZLineEdit*)zscd->getLineEdit();  
			  zle->setText(app);
			  zle->setEdited(true);  
			  if (zscd->exec() == 1) {
				if ( zle->text() != "" ) {
				  QString sName = zle->text();
				  ZConfig ini(ProgramDir + "/favs.cfg", true);
				  ini.writeEntry(QString("BOOKMARKS"), sName, fName );
				}
			  }
		  }
		}
	  }
	}
	else
	{

	  QString app;
	  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), false);
	  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	  if ( val != "big" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
	  else { app = Bbrowser->currentItem()->text(); }
	  QString nnn = app; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { app.remove( app.length() -1, 1 ); }
	  QString fName = sPath + "/" + app;
	  fName = RealName(fName);
	  int encontro = fName.find( "//" , 0 );
	  if ( encontro==0 ) { fName.remove( encontro,1); }
	  if ( QFileInfo(fName).isDir() ) { 
		  ZSingleCaptureDlg* zscd = new ZSingleCaptureDlg(ADDFAV, FAVNAME, ZSingleCaptureDlg::TypeLineEdit, NULL, "ZSingleCaptureDlg", true, 0, 0);
		  ZLineEdit* zle = (ZLineEdit*)zscd->getLineEdit();  
		  zle->setText(app);
		  zle->setEdited(true);  
		  if (zscd->exec() == 1) {
			if ( zle->text() != "" ) {
			  QString sName = zle->text();
			  ZConfig ini(ProgramDir + "/favs.cfg", true);
			  ini.writeEntry(QString("BOOKMARKS"), sName, fName );
			}
		  }
	  }
	}
}

QString ZLauncher::RealName(const QString &AFileName)
{
  QString str = AFileName;
  ZConfig cini(ProgramDir + "/foldername.cfg", true);	
  QString mval = cini.readEntry(QString("CUSTOMNAME"), str, "");
  if ( mval == "" ) { mval = str; }
  return mval;  

}

void ZLauncher::Buscar()
{
  ZSearch *zfod = new ZSearch();
  zfod->exec();
  delete zfod;
  zfod = NULL;
}


void ZLauncher::Settings()
{
  ZSettings *zfod = new ZSettings();
  zfod->exec();
    //QString fDestPath = zfod->getFileName(); 
	//int i = fDestPath.findRev ( "//" ); fDestPath.remove ( i, 1 );
    //if ( ! fDestPath.isEmpty())
    //{
	  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
	  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");

	  if ( val != currentView )
	  {
		  setMainWidgetTitle ( APP_NAME );
		  QWidget *myWidget = new ZWidget ( this, NULL, 0);
		  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );
		  myWidget->setFixedWidth(240);
		  pAIA = new ZAppInfoArea(myWidget, NULL, 0);
		  pAIA->setActArea(ZAppInfoArea::no_progress_meter);
		  setAppInfoArea(pAIA);
		  myVBoxLayout->addWidget ( pAIA, 1, 1 );

		  if ( val != "big" )
		  { browser = new myListBox(QString("%C18%I%M"), myWidget, 0);
			browser->setFixedWidth(240);
			connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));
		  } else {
			Bbrowser = new myIconView ( myWidget ); Bbrowser->setFixedWidth ( 240 );
			connect(Bbrowser, SIGNAL(returnPressed(ZIconViewItem *)), SLOT(brClicked()));
		  }
		  currentView=val;

		  setContentWidget ( myWidget );

		  CargarBrowser(sPath);

		  qApp->installEventFilter( this );

		  int y = 0;
		  y = y + this->headerSize().height();
		  y = y + softKey->height();
		  y = y + pAIA->height();
		  
		  if ( val != "big" ) { browser->setFixedHeight(320-y); browser->setFocus(); }
		  else { Bbrowser->setFixedHeight(320-y); Bbrowser->setFocus(); }
	  }
	//}
  delete zfod;
  zfod = NULL;
}


void ZLauncher::sendMenuLink()
{
	QString app, fName; QString val; 
	ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
	val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	if ( val != "big" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
	else { app = Bbrowser->currentItem()->text(); }
	fName = sPath + "/" + app;
	fName = RealName(fName);

	if ( (QFileInfo(fName).isFile()) || (QFileInfo(fName).isDir()) ) 
	{
		ZConfig SFIini(ProgramDir + "/sfoldericons.cfg", false);	
		ZConfig FNini(ProgramDir + "/foldername.cfg", false);	
		ZConfig SAIini(ProgramDir + "/sfileicons.cfg", false);	
		ZConfig UR(QString("/ezxlocal/download/appwrite/am/UserRegistry"), true);
		QString App0;
		system(QString("date +'%y%m%d%H%M%S' > %1/tempID").arg(ProgramDir));
		QFile entrada3(QString("%1/tempID").arg(ProgramDir));
		QTextStream stentrada3(&entrada3);
		if ( entrada3.open(IO_ReadOnly | IO_Translate) )
		{ App0 = stentrada3.readLine(); }
		entrada3.close();
		system(QString("rm %1/tempID").arg(ProgramDir));
		QString AppID = QString("10000000-0000-0000-0000-%1").arg(App0);
		UR.writeEntry ( QString(AppID), "AppID", QString("{"+AppID+"}") );
		UR.writeEntry ( QString(AppID), "Attribute", QString("1") );
		UR.writeEntry ( QString(AppID), "Daemon", QString("0") );
		if (QFileInfo(fName).isFile()) {

			QString Eval = fName;
			int i = Eval.findRev ( "." ); Eval.remove ( 0, i +1 );
			QString menuicon = SAIini.readEntry(QString("FILES"), QString("%1").arg(Eval).lower(), "");
			if ( menuicon == "" ) { menuicon="msg_unsupport_type_thb"; }

			UR.writeEntry ( QString(AppID), "AniIcon", QString(menuicon) );
			UR.writeEntry ( QString(AppID), "BigIcon", QString(menuicon) );
			UR.writeEntry ( QString(AppID), "Args", QString("") );
			UR.writeEntry ( QString(AppID), "Directory", QString("../.." + getFilePath(fName)) );
			UR.writeEntry ( QString(AppID), "Exec", app );
			UR.writeEntry ( QString(AppID), "Name", app );

		} else {

			QString menuicon = SFIini.readEntry(QString("FOLDERICONS"), QString(fName), "");
			if ( menuicon == "" ) { menuicon="fm_folder_thb"; }
			QString foldername = FNini.readEntry(QString("FOLDERNAME"), QString(fName), QString(fName) );
			QString directorio = ProgramDir; directorio.remove(0,14); directorio.remove(directorio.length()-1,1);

			UR.writeEntry ( QString(AppID), "AniIcon", QString(menuicon) );
			UR.writeEntry ( QString(AppID), "BigIcon", QString(menuicon) );
			UR.writeEntry ( QString(AppID), "Args", QString(fName) );
			UR.writeEntry ( QString(AppID), "Directory", QString(directorio) );
			UR.writeEntry ( QString(AppID), "Exec", "zFileManager" );
			UR.writeEntry ( QString(AppID), "IMEI", getIMEI() );
			UR.writeEntry ( QString(AppID), "Name", app );
		}
		UR.writeEntry ( QString(AppID), "FixedAllPosition", QString("0") );
		UR.writeEntry ( QString(AppID), "FixedPosition", QString("0") );
		UR.writeEntry ( QString(AppID), "GroupID", QString("ezx") );
		UR.writeEntry ( QString(AppID), "Icon", QString("") );
		UR.writeEntry ( QString(AppID), "LockEnabled", QString("1") );
		UR.writeEntry ( QString(AppID), "MassStorageStatus", QString("1") );
		UR.writeEntry ( QString(AppID), "Type", QString("2") );
		UR.writeEntry ( QString(AppID), "UserID", QString("root") );
		UR.writeEntry ( QString(AppID), "Visible", QString("0") );
		UR.writeEntry ( QString(AppID), "FileManagerLink", QString("1") );

		ZConfig UMT(QString("/ezxlocal/download/appwrite/am/UserMenuTree"), true);
		QString nuevovalor = UMT.readEntry ( QString("2a58c0d6-05c6-45aa-8938-63059d41a4e9"), "Items", "" );
		QString nuevovalor2 = nuevovalor + ";" + AppID;
		UMT.writeEntry ( QString("2a58c0d6-05c6-45aa-8938-63059d41a4e9"), "Items", nuevovalor2 );


	}

}

QString ZLauncher::getIMEI()
{
	unsigned int res;
	TAPI_IMEI_NUMBER_A imei;
	TAPI_CLIENT_Init(NULL, 0);
	res = TAPI_ACCE_GetImei(imei);
	QString pImei;
	if ( res == 0)
	{
		pImei = QString::fromLatin1( reinterpret_cast<char*>( imei ) );
	}
	TAPI_CLIENT_Fini();
	return pImei;
}

void ZLauncher::sendRarFile()
{
	if ( ActiveSel == "YES" )
	{
	  QStringList archivos;
	  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
	  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	  if ( val == "big" )
	  { 
		int k2 = Bbrowser->count(); for( int k=0; k < k2; k++ )	 
		{
		  if ( Bbrowser->itemChecked(k) ) {
			QString sapp = Bbrowser->item( k )->text();
			QString nnn = sapp; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { sapp.remove( sapp.length() -1, 1 ); }
			QString fName = sPath + "/" + sapp;
			int encontro = fName.find( "//" , 0 );
			if ( encontro > -1 ) { fName.remove( encontro,1); }
			archivos += QString(fName);
		  }
		}
	  }
	  else
	  {
		for( int k=0; k < browser->count(); k++ )	 
		{
		  if ( browser->itemChecked(k) ) {
			QString sapp = browser->item( k )->getSubItemText ( 2, 0, true );
			QString nnn = sapp; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { sapp.remove( sapp.length() -1, 1 ); }  
			QString fName = sPath + "/" + sapp;
			int encontro = fName.find( "//" , 0 );
			if ( encontro > -1 ) { fName.remove( encontro,1); }
			archivos += QString(fName);
			//qDebug(QString("Agregando: " + fName));
		  }
		}
	  }
	  if ( archivos.count() > 0 )
	  {
		QString archivos2;
		system(QString("rm /tmp/clipboard"));
		system(QString("touch /tmp/clipboard"));
		int k2 = archivos.count(); for( int k=0; k < k2; k++ )	
		{
		  QString app = archivos[k];
		  QString FullName = app;
		  int encontro = FullName.find( "//" , 0 );
		  if ( encontro > -1 ) { FullName.remove( encontro,1); }
		  FullName = RealName(FullName);
				encontro = FullName.find( sPath , 0 );
				if ( encontro > -1 ) { FullName.remove( encontro, sPath.length() + 1 ); }
		  if ( app != ".." ) {  
			  //qDebug(QString("PORTAPAPELES: " + FullName));
			  CopyPath=sPath;
			  archivos2 = archivos2 + "\n" + FullName;
			  system(QString("echo \"%1\" > /tmp/clipboard").arg(archivos2));
			  //system(QString("echo \"%1\" > /tmp/clipboard").arg(FullName));
		  }  
		}
	  }

	}
	else
	{
	  QString app;
	  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
	  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	  if ( val != "big" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
	  else { app = Bbrowser->currentItem()->text(); }
	  QString nnn = app; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { app.remove( app.length() -1, 1 ); }

	  QString FullName = sPath + "/" + app;
	  int encontro = FullName.find( "//" , 0 );
	  if ( encontro > -1 ) { FullName.remove( encontro,1); }
	  FullName = RealName(FullName);
			encontro = FullName.find( sPath , 0 );
			if ( encontro > -1 ) { FullName.remove( encontro, sPath.length() + 1 ); }
	  if ( app != ".." ) {  
		  //qDebug(QString("PORTAPAPELES: " + FullName));
		  CopyPath=sPath;
		  system(QString("echo \"%1\" > /tmp/clipboard").arg(FullName));
	  }  
	} 

    qApp->processEvents();
	ZSingleCaptureDlg* zscd = new ZSingleCaptureDlg(PERSONALIZE, "", ZSingleCaptureDlg::TypeLineEdit, NULL, "ZSingleCaptureDlg", true, 0, 0);
	ZLineEdit* zle = (ZLineEdit*)zscd->getLineEdit();  
	zle->setText("");
	zle->setEdited(true);  
	if (zscd->exec() == 1) {
	    qApp->processEvents();

		QString RName = zle->text();

		ZMessageDlg* msg0 =  new ZMessageDlg ( QString("zFileManager"),
		  	QString("%1 %2.tar").arg(RARCREATING).arg(RName), ZMessageDlg::NONE, 200, NULL, "ZMessageDlg", true, 0 );
		msg0->exec();

		system(QString("RARFILES=`cat /tmp/clipboard`; cd %1; /ezxlocal/LinXtend/usr/bin/busybox2 tar czvf %2.tar $RARFILES").arg(sPath).arg(RName));

		delete msg0;
		msg0 = NULL;

		CargarBrowser(sPath); 	
	}



}

void ZLauncher::sendtoDevice()
{
  QString DESTINO;
  int dev1 = sPath.find( "/mmc/mmca1", 0 );
  if ( dev1 > -1 ) DESTINO="/ezxlocal/download/mystuff";
  else DESTINO="/mmc/mmca1";

  //qDebug(QString("DESTINO: " + DESTINO));


	if ( ActiveSel == "YES" )
	{
	  QStringList archivos;
	  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
	  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	  if ( val == "big" )
	  { 
		int k2 = Bbrowser->count(); for( int k=0; k < k2; k++ )	 
		{
		  if ( Bbrowser->itemChecked(k) ) {
			QString sapp = Bbrowser->item( k )->text();
			QString nnn = sapp; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { sapp.remove( sapp.length() -1, 1 ); }
			QString fName = sPath + "/" + sapp;
			int encontro = fName.find( "//" , 0 );
			if ( encontro > -1 ) { fName.remove( encontro,1); }
			archivos += QString(fName);
		  }
		}
	  }
	  else
	  {
		for( int k=0; k < browser->count(); k++ )	 
		{
		  if ( browser->itemChecked(k) ) {
			QString sapp = browser->item( k )->getSubItemText ( 2, 0, true );  
			QString nnn = sapp; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { sapp.remove( sapp.length() -1, 1 ); }
			QString fName = sPath + "/" + sapp;
			int encontro = fName.find( "//" , 0 );
			if ( encontro > -1 ) { fName.remove( encontro,1); }
			archivos += QString(fName);
			//qDebug(QString("Agregando: " + fName));
		  }
		}
	  }
	  if ( archivos.count() > 0 )
	  {
		system(QString("rm /tmp/clipboard"));
		system(QString("touch /tmp/clipboard"));
		int k2 = archivos.count(); for( int k=0; k < k2; k++ )	
		{
		  QString app = archivos[k];
		  QString FullName = app;
		  int encontro = FullName.find( "//" , 0 );
		  if ( encontro > -1 ) { FullName.remove( encontro,1); }
		  FullName = RealName(FullName);
		  if ( app != ".." ) {  
			  //qDebug(QString("PORTAPAPELES: " + FullName));
			  CopyPath=sPath;
			  system(QString("echo \"%1\" >> /tmp/clipboard").arg(FullName));
		  }  
		}
	  }

	}
	else
	{
	  QString app;
	  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), true);
	  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	  if ( val != "big" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
	  else { app = Bbrowser->currentItem()->text(); }
	  QString nnn = app; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { app.remove( app.length() -1, 1 ); }

	  QString FullName = sPath + "/" + app;
	  int encontro = FullName.find( "//" , 0 );
	  if ( encontro > -1 ) { FullName.remove( encontro,1); }
	  FullName = RealName(FullName);
	  if ( app != ".." ) {  
		  //qDebug(QString("PORTAPAPELES: " + FullName));
		  CopyPath=sPath;
		  system(QString("echo \"%1\" > /tmp/clipboard").arg(FullName));
	  }  
	} 


  QString folder; QFile entrada("/tmp/clipboard"); QTextStream stentrada(&entrada);
  if ( entrada.open(IO_ReadOnly | IO_Translate) )
  {
	while ( !stentrada.eof() )
    {           
	  QString line = stentrada.readLine();

	  if ( QFileInfo(line).isFile() ) 
	  {
		  PasteFiles(line, DESTINO, false);
	  }
	  else
	  {
		system(QString("find \"%1\" > /tmp/clipboard2").arg(line));
		QString folder; QFile entrada2("/tmp/clipboard2"); QTextStream stentrada2(&entrada2);
		if ( entrada2.open(IO_ReadOnly | IO_Translate) ) {
		  while ( !stentrada2.eof() ) {           
			QString line2 = stentrada2.readLine();
			QString destino = getFilePath(line2);
			int k = destino.find ( CopyPath, 0 ); 
			destino.remove( k, CopyPath.length() );
			destino = DESTINO + "/" + destino;
			if ( QFileInfo(line2).isFile() ) { PasteFiles(line2, destino, false); }
			else {
				QString carpeta = line2;
				int i = carpeta.find ( CopyPath, 0 );
				carpeta.remove( i, CopyPath.length() );
				QDir dd; if ( ! dd.exists(DESTINO + carpeta) ) { dd.mkdir(DESTINO + carpeta); }
			}
		  }
		}
	  }
	}
  }
}


void ZLauncher::applyRingtone()
{
  menuMain->hide();
  qApp->processEvents();
  QString app, fName;
  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), false);
  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
  if ( val != "big" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
  else { app = Bbrowser->currentItem()->text(); }
  QString nnn = app; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { app.remove( app.length() -1, 1 ); }
  fName = sPath + "/" + app;

  QString Eval = fName;
  int i = Eval.findRev ( "." ); Eval.remove ( 0, i +1 );

  if ( (Eval.lower()=="mp3") || (Eval.lower()=="wav") || (Eval.lower()=="wma") )
  {
  	applyElement(SETUP_THEME:: CALL_ALERT_LINE_1, fName, SETUP_THEME::TYPE_AUDIO );
  }
  else
  {
    ZMessageDlg* dlgw  = new ZMessageDlg ( APP_NAME, APPLY_ERROR,  ZMessageDlg::just_ok, 0 , this , "dlgw" , true , 0 );
    dlgw->exec();
    delete dlgw;
  }
}


void ZLauncher::applyWallpaper()
{
  menuMain->hide();
  qApp->processEvents();
  QString app, fName;
  QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), false);
  val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
  if ( val != "big" ) { app = browser->item ( browser->currentItem() )->getSubItemText ( 1, 0, true ); }
  else { app = Bbrowser->currentItem()->text(); }
  QString nnn = app; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { app.remove( app.length() -1, 1 ); }
  fName = sPath + "/" + app;

  QString Eval = fName;
  int i = Eval.findRev ( "." ); Eval.remove ( 0, i +1 );

  if ( (Eval.lower()=="jpg") || (Eval.lower()=="jpeg") || (Eval.lower()=="png") || (Eval.lower()=="gif") || (Eval.lower()=="bmp") )
  {
  	applyElement(SETUP_THEME::SLIDE_TONE, fName, SETUP_THEME::TYPE_IMAGE );
  }
  else
  {
    ZMessageDlg* dlgw  = new ZMessageDlg ( APP_NAME, APPLY_ERROR,  ZMessageDlg::just_ok, 0 , this , "dlgw" , true , 0 );
    dlgw->exec();
    delete dlgw;
  }
}


void ZLauncher::verMiniaturas()
{
	int numero;
	if ( ActiveSel == "YES" ) { numero=1; } else { numero=0; }
	QString app, fName;
	QString val; ZConfig ini(QString("%1/settings.cfg").arg(ProgramDir), false);
	val = ini.readEntry(QString("FM"), QString("IconStyle"), "");
	QString Pval = ini.readEntry(QString("FM"), QString("ShowPreview"), "");

	if ( Pval == "No" ) return;

	if ( val == "big" )
	{ 
	  
	  for( int k=0; k < Bbrowser->count(); k++ )	 
	  {
//		QPoint ubic = Bbrowser->item(k)->pos();
//		if ( ( ubic.x() < 240 ) && ( ubic.y() < 270 ) )
//		{
		  QString ssaa = Bbrowser->item(k)->text();;
//		  QString nnn = ssaa; nnn.remove( 0, nnn.length() -1 ); if ( nnn == " " ) { ssaa.remove( ssaa.length() -1, 1 ); }
//		  fName = sPath + "/" + ssaa;
		  QPixmap pixmap; QPixmap* p1;
		  QString Eval = fName;
		  int i = Eval.findRev ( "." ); Eval.remove ( 0, i +1 );

		  if ( (Eval.lower()=="jpg") || (Eval.lower()=="jpeg") || (Eval.lower()=="png") || (Eval.lower()=="gif") || (Eval.lower()=="bmp") )
		  {
			//system(QString("echo \"Archivo 1 = %1 - %2-%3\"\n").arg(Bbrowser->item(k)->text()).arg(ubic.x()).arg(ubic.y()));
			Eval = Bbrowser->item(k)->text();
			Eval.remove( 0, Eval.length() -2 );
			if (  Eval != " " )
			{
				pixmap.load( fName );				
				double alto = pixmap.height();
				double ancho = pixmap.width();
				if ( alto > ancho ) { double medio=alto/ancho; 
				  while ( alto > 48 ) { alto=alto-medio; ancho=ancho-1; }
				  while ( ancho > 48 ) { alto=alto-medio; ancho=ancho-1; }
				} else if ( ancho > alto ) { double medio=ancho/alto;
				  while ( ancho > 48 ) { alto=alto-1; ancho=ancho-medio; }
				  while ( alto > 48 ) { alto=alto-1; ancho=ancho-medio; }
				} else if ( alto==ancho ) {  
				  while ( alto > 48 ) { alto=alto-1; ancho=ancho-1; }
				  while ( ancho > 48 ) { alto=alto-1; ancho=ancho-1; }
				}
				if ( pixmap.height() > 48 ) { QImage image; image = pixmap.convertToImage(); 
				image = image.smoothScale( ancho, alto ); pixmap.convertFromImage(image); }
				QString app = Bbrowser->item(k)->text();
				//p1 = new QPixmap(pixmap);
//				Bbrowser->item(k)->setText(app + " ");
				//browser->item(k)->setSubItemShowed(1, 0, false);
				//browser->item(k)->setSubItemShowed(1, 0, false);
				//browser->item(k)->setSubItemShowed(1, 2, false);
				Bbrowser->item(k)->setPixmap(pixmap,false);
			}
		  }
//		}
	  }
	}

	else

	{
	  for( int k=0; k < browser->count(); k++ )	 
	  {
//		if ( browser->itemVisible(k) )
//		{
			fName = sPath + "/" + browser->item(k)->getSubItemText ( numero+1, 0, true );
			QPixmap pixmap; QPixmap* p1;
			QString Eval = fName;
			int i = Eval.findRev ( "." ); Eval.remove ( 0, i +1 );

		  if ( (Eval.lower()=="jpg") || (Eval.lower()=="jpeg") || (Eval.lower()=="png") || (Eval.lower()=="gif") || (Eval.lower()=="bmp") )
		  {
			//system(QString("echo \"Archivo 1 = %1\"\n").arg(browser->item(k)->getSubItemText( numero+1, 1, true )));
//			Eval =  browser->item(k)->getSubItemText( numero+1, 0, true );
//			Eval.remove( 0, Eval.length() -2 );
//			if (  Eval != " " )
//			{
				pixmap.load( fName );				
				double alto = pixmap.height();
				double ancho = pixmap.width();
				if ( alto > ancho ) { double medio=alto/ancho; 
				  while ( alto > ItemSize ) { alto=alto-medio; ancho=ancho-1; }
				  while ( ancho > ItemSize ) { alto=alto-medio; ancho=ancho-1; }
				} else if ( ancho > alto ) { double medio=ancho/alto;
				  while ( ancho > ItemSize ) { alto=alto-1; ancho=ancho-medio; }
				  while ( alto > ItemSize ) { alto=alto-1; ancho=ancho-medio; }
				} else if ( alto==ancho ) {  
				  while ( alto > ItemSize ) { alto=alto-1; ancho=ancho-1; }
				  while ( ancho > ItemSize ) { alto=alto-1; ancho=ancho-1; }
				}
				if ( pixmap.height() > ItemSize ) { QImage image; image = pixmap.convertToImage(); 
				image = image.smoothScale( ancho, alto ); pixmap.convertFromImage(image); }
				QString app = browser->item(k)->getSubItemText( numero+1, 0, true );
				//p1 = new QPixmap(pixmap);
//				browser->item(k)->setSubItem(numero+1, 0, app + " ", false, NULL);
				//browser->item(k)->setSubItemShowed(1, 0, false);
				//browser->item(k)->setSubItemShowed(1, 0, false);
				//browser->item(k)->setSubItemShowed(1, 2, false);
				browser->item(k)->setPixmap(numero, pixmap);
//			}
		  }
//		}
	  }
	}



}


