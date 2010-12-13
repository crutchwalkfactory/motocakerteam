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

  QString val; 
  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");

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
  ZConfig IDini(Idioma, false);
  QString IDval = IDini.readEntry(QString("COMMON"), QString("MENU_SELECT"), "");
  softKey->setText ( ZSoftKey::LEFT, IDval, ( ZSoftKey::TEXT_PRIORITY ) 0 );
  IDval = IDini.readEntry(QString("COMMON"), QString("MENU_CANCEL"), "");
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
