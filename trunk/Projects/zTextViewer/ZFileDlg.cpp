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
#include "texteditor.h"
#include <qpixmap.h>
#include <qlabel.h>
#include <ZApplication.h>
#include <ZSoftKey.h>
#include "resources.h"
#include <ZSingleSelectDlg.h>
#include <ZScrollView.h>
#include <ZNumPickerDlg.h>
#include <ZNoticeDlg.h>

#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <qpixmap.h>
#include <ezxres.h>

#include <ZScrollView.h>


QString ProgDir; QString Idioma; QString FStext;
int SelectProg=0;

extern TextEditor * pTextEditor;

//************************************************************************************************
//************************************** class ZFileOpenDialog ***********************************
//************************************************************************************************
ZFileOpenDialog::ZFileOpenDialog()
  :MyBaseDlg()
{
  
  QString ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = ProgDir + "/languages/en-us";
  ZConfig lang (Idioma,false); 

  this->setMainWidgetTitle("zTextViewer");

  QWidget *myWidget = new ZWidget ( this, NULL, 0);
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );
  myWidget->setFixedWidth(240);
  ZAppInfoArea* pAIA = new ZAppInfoArea(myWidget, NULL, 0);
  pAIA->setActArea(ZAppInfoArea::no_progress_meter);
  pAIA->appendText(ZAppInfoArea::activity, lang.readEntry(QString("TEXTVIEWER"), "IDS_OPEN", "Open"), "name");
  setAppInfoArea(pAIA);
  myVBoxLayout->addWidget ( pAIA, 1, 1 );

  fb = new FileBrowser("*", QString ( "%I%M" ), myWidget, 0);
  fb->setDir("/");
  QObject::connect(fb, SIGNAL(isFilePicked(bool)), SLOT(filePicked(bool)));

 // myVBoxLayout->addWidget ( fb, 1 );
  
  setContentWidget ( fb );
  
  ZSoftKey *softKey = new ZSoftKey ( NULL, this, this );
  softKey->setText ( ZSoftKey::LEFT, lang.readEntry(QString("TEXTVIEWER"), "IDS_OPEN", "Open"), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::RIGHT, lang.readEntry(QString("TEXTVIEWER"), "IDS_CANCEL", "Cancel"), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( reject() ) );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( accept() ) );
  setCSTWidget ( softKey );
}

ZFileOpenDialog::~ZFileOpenDialog()
{

}

void ZFileOpenDialog::keyPressEvent ( QKeyEvent* e )
{
  switch ( e->key() ) 
  {
    case Z6KEY_CENTER:
    {
      accept();
      break;
    }
    default:
      MyBaseDlg::keyPressEvent ( e );
  }   

}

void ZFileOpenDialog::accept()
{
  fileName = fb->getFileName();
  basePath = fb->getFilePath();
  if (fileName != "") { MyBaseDlg::accept(); }
  else {
    ZMessageDlg * dlg = new ZMessageDlg ( "Error", QString("Nombre de archivo vacio"), ZMessageDlg::just_ok, 0, this, "1", true, 0 );
    dlg->exec();
    delete dlg; dlg = NULL;
  }
}

//************************************************************************************************
//************************************** class ZFileSaveDialog ***********************************
//************************************************************************************************
ZFileSaveDialog::ZFileSaveDialog(const QString &curPath, const QString &originName)
  :MyBaseDlg()
{
  QWidget *myWidget = new QWidget ( this );
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );
  
  // label
  QLabel *myLabel = new QLabel ( QString ( "Nombre para guardar:" ), myWidget );
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
  //zmle->setCellHeight(22);
  zmle->setText(originName);
  
  myVBoxLayout->addWidget ( zmle, 1 );
  
  fb = new FileBrowser("*", QString ( "%I%M" ), myWidget, 0);
  fb->setDir(curPath);
  QObject::connect(fb, SIGNAL(isFilePicked(bool)), SLOT(filePicked(bool)));
  myVBoxLayout->addWidget ( fb, 1 );
  
  
  setContentWidget ( myWidget );
  
  ZSoftKey *softKey = new ZSoftKey ( NULL, this, this );
  softKey->setText ( ZSoftKey::LEFT, "Guardar", ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::RIGHT, "Cancelar", ( ZSoftKey::TEXT_PRIORITY ) 0 );
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
    ZMessageDlg *dlg = new ZMessageDlg ( "Error", "Ingrese un nombre para el archivo.", ZMessageDlg::just_ok, 0, this, "Acerca", true, 0 );
    dlg->setIcon(QPixmap("Dialog_Question_Mark.gif"));
    dlg->exec();
    delete dlg;
    dlg = NULL;  
  }
}



//************************************************************************************************
//*************************************** class ZSearch ******************************************
//************************************************************************************************
ZSearch::ZSearch(const QString &curPath, const QString &originName)
  :MyBaseDlg()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = ProgDir + "/languages/en-us";
  ZConfig lang (Idioma,false); 

  this->setMainWidgetTitle("zTextViewer");
  
  QWidget *myWidget = new ZWidget ( this, NULL, 0);
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );
  myWidget->setFixedWidth(240);
  ZAppInfoArea* pAIA = new ZAppInfoArea(myWidget, NULL, 0);
  pAIA->setActArea(ZAppInfoArea::no_progress_meter);
  pAIA->appendText(ZAppInfoArea::activity, lang.readEntry(QString("TEXTVIEWER"), "IDS_SAVE", ""), "name");
  setAppInfoArea(pAIA);
  myVBoxLayout->addWidget ( pAIA, 1, 1 );

    ZFormContainer* fc = new ZFormContainer(this);
    setContentWidget(fc);

    lineEdit = new ZLineEdit(this, NULL);
    lineEdit->setTitle(lang.readEntry(QString("TEXTVIEWER"), "IDS_ENTER_FILE_NAME", ""));
    lineEdit->setTitlePosition(ZLineEdit::TitleTop);
    lineEdit->setMaxLength(50);
	lineEdit->setText(originName);
    fc->addChild(lineEdit);

 	buttonDir = new myButton(this, "ZPressButton");
	buttonDir->setTitle(lang.readEntry(QString("TEXTVIEWER"), "IDS_LOCATION", ""));
	QString direc = curPath;
	if ( direc == "" ) direc = "/mmc/mmca1";
	if ( direc[direc.length()-1] == QChar('/') ) 
	  {	direc.remove( direc.length()-1, 1 ); }
    buttonDir->setText(direc); 
	FStext=direc;
	buttonDir->setPixmap(QPixmap(ProgDir+"/img/folder.png"));
    fc->addChild(buttonDir);
	

  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  softKey->setText ( ZSoftKey::RIGHT, lang.readEntry(QString("TEXTVIEWER"), "IDS_CANCEL", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::LEFT, lang.readEntry(QString("TEXTVIEWER"), "IDS_SAVE", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( slot_search() ) );


  this->setCSTWidget( softKey );

}


ZSearch::~ZSearch()
{
  
}

void ZSearch::slot_cancel()
{
  reject();
}


void ZSearch::slot_search()
{
  QString txt = lineEdit->text();
  if (txt != "")
  {
    fileName = txt;
    basePath = FStext;
    MyBaseDlg::accept();
  }
  else
  {
    ZMessageDlg *dlg = new ZMessageDlg ( "Error", "Ingrese un nombre para el archivo.", ZMessageDlg::just_ok, 0, this, "Acerca", true, 0 );
    dlg->setIcon(QPixmap("Dialog_Question_Mark.gif"));
    dlg->exec();
    delete dlg;
    dlg = NULL;  
  }
}


myButton::myButton(QWidget* parent, char const* name, ZSkinService::WidgetClsID clsId)
:ZPressButton( parent, NULL )
{
	
}

myButton::~myButton()
{

}

void myButton::keyPressEvent(QKeyEvent* k)
{
	if ( k->key() == Z6KEY_CENTER )
	{
	  ZFolderSelect2 *zf = new ZFolderSelect2(true, FStext);
	  if ( zf->exec() )
	  {
		QString fDestPath = zf->getFileName(); 
		int i = fDestPath.findRev ( "//" ); fDestPath.remove ( i, 1 );
		if ( ! fDestPath.isEmpty())
		{
			setText(fDestPath); 
			FStext=fDestPath;
		}
	  }
	  delete zf;
	  zf = NULL;
	}
	else
	{
      ZPressButton::keyPressEvent(k);
	}

}



//************************************************************************************************
//************************************** class ZFolderSelect2 ***********************************
//************************************************************************************************
ZFolderSelect2::ZFolderSelect2(bool onlyDirs, QString path)
  :MyBaseDlg()
{
  QString ProgDir =  QString ( qApp->argv() [0] );
  int i = ProgDir.findRev ( "/" );
  ProgDir.remove ( i+1, ProgDir.length() - i );

  ZConfig readlang("/ezxlocal/download/appwrite/setup/ezx_system.cfg", true);
  Idioma = ProgDir + "/languages/" + readlang.readEntry(QString("SYS_SYSTEM_SET"), QString("LanguageType"), "en-us");
  if ( ! QFileInfo(Idioma).isFile() ) Idioma = ProgDir + "/languages/en-us";
  ZConfig lang (Idioma,false); 

  this->setMainWidgetTitle("zFileManager");
  
  QWidget *myWidget = new ZWidget ( this, NULL, 0);
  QVBoxLayout *myVBoxLayout = new QVBoxLayout ( myWidget, 0 );
  myWidget->setFixedWidth(240);
  ZAppInfoArea* pAIA = new ZAppInfoArea(myWidget, NULL, 0);
  pAIA->setActArea(ZAppInfoArea::no_progress_meter);
  QPixmap pixmap; pixmap.load(ProgDir+"/img/folder.png");
  pAIA->appendIcon(ZAppInfoArea::activity,pixmap,"");
  pAIA->appendText(ZAppInfoArea::activity, lang.readEntry(QString("LANGUAGE"), "SEARCH_IN", ""), "name");
  setAppInfoArea(pAIA);
  myVBoxLayout->insertWidget (0, pAIA );

  fb = new FileBrowser2("*", onlyDirs, QString ( "%I%M" ), myWidget, 0);
  fb->setDir(path);
  QObject::connect(fb, SIGNAL(isFilePicked(bool)), SLOT(filePicked(bool)));

  
  fb->ProgramDir = ProgDir;
  setContentWidget ( fb );
  
  ZSoftKey *softKey = new ZSoftKey ( "CST_2A", this, this );
  
  softKey->setText ( ZSoftKey::RIGHT, lang.readEntry(QString("LANGUAGE"), "CANCEL", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setText ( ZSoftKey::LEFT, lang.readEntry(QString("LANGUAGE"), "CHOISE", ""), ( ZSoftKey::TEXT_PRIORITY ) 0 );
  softKey->setClickedSlot ( ZSoftKey::RIGHT, this, SLOT ( slot_cancel() ) );
  softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( accept() ) );
   
  this->setCSTWidget( softKey );

}

ZFolderSelect2::~ZFolderSelect2()
{
  //launcher->CreateSoftKeys(NULL);
}

void ZFolderSelect2::accept()
{
  fileName = fb->getFileName();
  basePath = fb->getFilePath();
  if (fileName != "") { MyBaseDlg::accept(); }
  else {
    ZMessageDlg * dlg = new ZMessageDlg ( "Error", QString("Filename is empty"), ZMessageDlg::just_ok, 0, this, "1", true, 0 );
    dlg->exec();
    delete dlg; dlg = NULL;
	
  }
}


void ZFolderSelect2::selectItem(int idx)
{

}

void ZFolderSelect2::slot_cancel()
{
  reject();
}

void ZFolderSelect2::slot_paste()
{
  accept();
}
