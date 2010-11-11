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
#include <ezxres.h>
#include <qpixmap.h>
#include <qtextcodec.h>
#include <qlabel.h>
#include <qfileinfo.h>
#include <ZApplication.h>
#include <ZSoftKey.h>
#include "ZConfig.h"
#include <string.h>
#include <ZScrollView.h>

QString Idioma;

//************************************************************************************************
//************************************** class ZFileOpenDialog ***********************************
//************************************************************************************************
ZFileOpenDialog::ZFileOpenDialog()
  :MyBaseDlg()
{
  QWidget *myWidget = new QWidget ( this );
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );

  fb = new FileBrowser("*", QString ( "%I%M" ), myWidget, 0, ( ZSkinService::WidgetClsID ) 4);
  fb->setDir("/");
  QObject::connect(fb, SIGNAL(isFilePicked(bool)), SLOT(filePicked(bool)));

  setMainWidgetTitle ( "Z6 Manager" );

  myVBoxLayout->addWidget ( fb, 1 );
  
  setContentWidget ( myWidget );
  
  ZSoftKey *softKey = new ZSoftKey ( NULL, this, this );

  QString ProgDir = QString ( qApp->argv() [0] ) ;
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = "ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/en-us";


  ZConfig IDini(Idioma, false);
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
  fileName = fb->getFileName();
  basePath = fb->getFilePath();
  MyBaseDlg::accept();
}

//************************************************************************************************
//************************************** class ZFileSaveDialog ***********************************
//************************************************************************************************
ZFileSaveDialog::ZFileSaveDialog()
  :MyBaseDlg()
{
  QWidget *myWidget = new QWidget ( this );
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );
  
  // label
  QLabel *myLabel = new QLabel ( QString ( "Filename to save:" ), myWidget );
  myVBoxLayout->addWidget ( myLabel, 0 );
  myLabel->setIndent ( 5 );
  myLabel->setAutoResize ( true );
    
  // zlineedit
  zmle = new ZMultiLineEdit(myWidget, true, 1, "ZMultiLineEdit");
  zmle->setEdited(false);
  zmle->setFixedHeight(myWidget->sizeHint().height());
  zmle->setInsertionMethod ( ( ZMultiLineEdit::InsertionMethod ) 3 );
  zmle->setFontPercent ( 0.7 );
  zmle->setSizePolicy ( QSizePolicy ( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
  zmle->setCellHeight(22);

  
  myVBoxLayout->addWidget ( zmle, 1 );
  
  fb = new FileBrowser("*", QString ( "%I%M" ), myWidget, 0, ( ZSkinService::WidgetClsID ) 4);
  fb->setDir("/");
  QObject::connect(fb, SIGNAL(isFilePicked(bool)), SLOT(filePicked(bool)));
  myVBoxLayout->addWidget ( fb, 1 );
  
  
  setContentWidget ( myWidget );
  
  ZSoftKey *softKey = new ZSoftKey ( NULL, this, this );
  ZConfig IDini("/tmp/Z6ManagerLang", false);
  QString IDval = IDini.readEntry(QString("Z6MANAGER"), QString("MENU_SELECT"), "");
  softKey->setText ( ZSoftKey::LEFT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  IDval = IDini.readEntry(QString("Z6MANAGER"), QString("MENU_CANCEL"), "");
  softKey->setText ( ZSoftKey::RIGHT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( reject() ) );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( accept() ) );
  setCSTWidget ( softKey );
}

ZFileSaveDialog::~ZFileSaveDialog()
{

}

void ZFileSaveDialog::keyPressEvent ( QKeyEvent* e )
{
  switch ( e->key() ) 
  {
    case Z6KEY_LEFT:
    case Z6KEY_RIGHT: 
    {
      zmle->setFocus();
      break;
    }
    default:
      MyBaseDlg::keyPressEvent ( e );
  }   

}

void ZFileSaveDialog::accept()
{
  // check multilineedit
  QString txt;
  txt = zmle->text();
  if ((txt != "")) // || (txt != QString::null))
  {
    fileName = txt;
    basePath = fb->getFilePath();
    MyBaseDlg::accept();
  }
  else
  {
    ZMessageDlg *dlg = new ZMessageDlg ( "Error", "Please enter a file name.", ZMessageDlg::just_ok, 0, this, "About", true, 0 );
    dlg->setIcon(QPixmap("Dialog_Question_Mark.gif"));
    dlg->exec();
    delete dlg;
    dlg = NULL;  
  }
}



//************************************************************************************************
//************************************** class ZIconSelect ***********************************
//************************************************************************************************
ZIconSelect::ZIconSelect()
  :MyBaseDlg()
{
  QString ProgDir = QString ( qApp->argv() [0] ) ;
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  setMainWidgetTitle ( "Z6 Manager" );

  QString val; 
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = "ezxlocal/download/mystuff/.system/LinXtend/usr/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = "/ezxlocal/download/mystuff/.system/LinXtend/usr/languages/en-us";

  ZConfig lang(Idioma, false);

  this->setMainWidgetTitle(lang.readEntry(QString("LANGUAGE"), "SELECTICON", ""));
  
  QWidget *myWidget = new QWidget ( this );
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );

  browser = new ZListBox ( QString ( "%I%M" ), this, 0);
  browser->setFixedWidth ( 240 );
  QPixmap p1;

  ZListBoxItem* Item = new ZListBoxItem ( browser, QString ( "%I%M" ) );
  Item->appendSubItem ( 1, QString ( ".." ), false, NULL );
  browser->insertItem ( Item, 0,true );
  int ItemSize = browser->itemHeight(0)-2 ; if ( ItemSize>20 ) ItemSize=20;
  browser->clear();

	RES_ICON_Reader iconReader;
	QStringList nombres, iconos;
	nombres.clear();
	nombres += "alarm_clock_std"; nombres += "bluetooth_std"; nombres += "browser_setup_std"; nombres += "browser_std";
	nombres += "calculator_std"; nombres += "calen_std"; nombres += "camera_std"; nombres += "certificate_mgr_std";
	nombres += "connections_std"; nombres += "ctact_std"; nombres += "dialing_service_std"; nombres += "dwnld_mgr_std";
	nombres += "email_std"; nombres += "fem_std"; nombres += "fixed_dial_std"; nombres += "fm_std";
	nombres += "games_std"; nombres += "go_to_url_std"; nombres += "help_std"; nombres += "location_std";
	nombres += "media_finder_std"; nombres += "media_player_std"; nombres += "java_std"; nombres += "im_std";
	nombres += "im_yahoo_std"; nombres += "images_std"; nombres += "fonts_std"; nombres += "lan_std";
	nombres += "moto_source_std"; nombres += "motosync_std"; nombres += "mp_radio_std";
	nombres += "msg_std"; nombres += "multimedia_std"; nombres += "music_library_std"; nombres += "music_std";
	nombres += "netwk_std"; nombres += "notepad_std"; nombres += "office_tools_std"; nombres += "pfile_std";
	nombres += "phone_setting_std"; nombres += "pictureflow_std"; nombres += "recnt_call_std"; nombres += "refresh_std";
	nombres += "screen_std"; nombres += "security_std"; nombres += "service_dial_std"; nombres += "settings_std";
	nombres += "sim_apps_std"; nombres += "stored_page_std"; nombres += "task_list_std"; nombres += "themes_std";
	nombres += "tweaks_std"; nombres += "utility_std"; nombres += "vid_camera_std";
	nombres += "video_std"; nombres += "voice_rec_std"; nombres += "web_access_std"; nombres += "web_history_std";
	nombres += "web_session_std"; nombres += "web_shortcuts_std"; nombres += "world_clock_std";
	nombres += "z6manager_std"; nombres += "z6tweaker_std";

	for ( int j=0 ; j < nombres.count(); ++j )
	{
		ZListBoxItem* listitem1 = new ZListBoxItem ( browser, QString ( "%I%M" ) );
		QString icono = nombres[j];
		p1=QPixmap(iconReader.getIcon(icono, false));
		if ( p1.height() > 19 ) { QImage image; image = p1.convertToImage(); 
		image = image.smoothScale(19, 19); p1.convertFromImage(image); }     
		listitem1->setPixmap ( 0, p1 );
		listitem1->appendSubItem ( 1, nombres[j], false, 0 );
		browser->insertItem ( listitem1,-1,true );
	}	


  connect(browser, SIGNAL(returnPressed(ZListBoxItem *)), SLOT(brClicked()));

  myVBoxLayout->addWidget ( browser, 1 );
  setContentWidget ( browser );

  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );

  QString IDval = lang.readEntry(QString("COMMON"), QString("MENU_SELECT"), "");
  softKey->setText ( ZSoftKey::LEFT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  IDval = lang.readEntry(QString("COMMON"), QString("MENU_CANCEL"), "");
  softKey->setText ( ZSoftKey::RIGHT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( reject() ) );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( brClicked() ) );
  setCSTWidget ( softKey );

}

ZIconSelect::~ZIconSelect()
{
  
}

QString ZIconSelect::texto(const char*xString)
{
	ZConfig IDini(Idioma, false);	
	QString	IDval = IDini.readEntry(QString("LANGUAGE"), QString("UNICODE"), "");
	if ( IDval == "1" ) { 
		QTextCodec* txtcodec = QTextCodec::codecForName("UTF-8");
		return txtcodec->toUnicode(xString); }
	else { return xString; }
}  

void ZIconSelect::accept()
{
/*  int index = browser->currentItem();
  if (index == -1) {return;}
  ZListBoxItem* listitem = browser->item ( index );
  QString app = listitem->getSubItemText ( 1, 0, true );  
*/
  MyBaseDlg::accept();

}


void ZIconSelect::selectItem(int idx)
{

}

void ZIconSelect::slot_cancel()
{
  reject();
}


void ZIconSelect::brClicked()
{

  int index = browser->currentItem();
  if (index == -1) {return;}
  ZListBoxItem* listitem = browser->item ( index );
  QString app = listitem->getSubItemText ( 1, 0, true );  

  basePath = app;
  MyBaseDlg::accept();


}

